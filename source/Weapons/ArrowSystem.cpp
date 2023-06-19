#include "pch.h"
#include "ArrowSystem.h"

#include "Character/EnemySystem.h"
#include "Environment/Terrain.h"
#include "Framework/Resources.h"
#include "Game/WorldMatrix.h"
#include "Game/Camera/Camera.h"
#include "Generation/PlaneGeneration.h"
#include "Io/Fbx/FbxClass.h"
#include "Services/CollisionService.h"
#include "Services/GameplaySystems.h"

using namespace Rendering;

ArrowSystem::ArrowSystem()
	: m_InputLayout{ InputLayout::FromTypes<Vertex, Instance>() }
	, m_Shader{ Resources::GlobalShader(L"LambertCam_Tex_Tran_Inst.hlsl") }
	, m_Texture{ Resources::Local(L"Texture_01.png") }
{
	//VERTICES
	const std::wstring arrowMeshPath{ Resources::Local(L"SM_Arrow_01.fbx") };
	Io::Fbx::FbxClass arrowFbxModel{ arrowMeshPath };
	Io::Fbx::FbxClass::Geometry& arrowGeom = arrowFbxModel.GetGeometries()[0];
	Array<V_PosNorUv> arrowVertices{ arrowGeom.Points.GetSize() };
	for (int i = 0; i < arrowGeom.Points.GetSize(); i++)
		arrowVertices[i] = V_PosNorUv{ arrowGeom.Points[i] * 0.01f, arrowGeom.Normals[i], arrowGeom.Uvs[i] };

	m_Instances = InstanceList<Vertex, Instance>{
		arrowVertices.GetData(), arrowVertices.GetSizeU()
	};
}

void ArrowSystem::Update()
{
	for (int i = 0; i < m_Velocities.GetSize(); i++)
	{
		//IS ALREADY FINISHED ?
		Float3& velocity{ m_Velocities[i] };
		Float4X4& world{ m_Instances[i].model };
		if (IsArrowFinished(velocity))
		{
			m_Instances[i].modelViewProj = world * Globals::pCamera->GetViewProjection();
			continue;
		}

		//IS UNDER TERRAIN ?
		const Float3 position{ WorldMatrix::GetPosition(world) };
		if (GameplaySystems::GetTerrain().GetHeight(position.Xz()) >= position.y)
		{
			SetArrowFinished(velocity);
			continue;
		}

		//UPDATE WORLD-MATRIX
		velocity.y += -9.81f * Globals::DeltaTime;
		const Float3 newPosition{ position + velocity * Globals::DeltaTime };
		WorldMatrix::SetRotation(world, velocity.Normalized());
		WorldMatrix::SetPosition(world, newPosition);
		m_Instances[i].modelViewProj = world * Globals::pCamera->GetViewProjection();

		//IS IN TOWER?
		const CollisionService& collisions{ GameplaySystems::GetCollisionService() };
		if (collisions.Tower.IsColliding(position, newPosition))
		{
			SetArrowFinished(velocity);
			continue;
		}

		//IS IN ENEMY?
		Enemy* pEnemy{ collisions.Enemies.IsColliding(position, newPosition) };
		if (pEnemy)
		{
			GameplaySystems::GetEnemySystem().OnCollision(Transform{ newPosition, Quaternion{world} }, i, *pEnemy);
			SetArrowFinished(velocity);
			continue;
		}
	}
}

void ArrowSystem::Spawn(const Transform& bowTransform)
{
	const Float4X4 world{ bowTransform.AsMatrix() };
	const Float4X4 worldViewProjection{ world * Globals::pCamera->GetViewProjection() };
	m_Instances.Add({ world, worldViewProjection });
	m_Velocities.Add(WorldMatrix::GetForward(world) * 20);
}

void ArrowSystem::SetArrowTransform(int arrowIdx, const Transform& newArrowWorld)
{
	const Float4X4 world{ newArrowWorld.AsMatrix() };
	const Float4X4 worldViewProjection{ world * Globals::pCamera->GetViewProjection() };
	m_Instances[arrowIdx] = { world, worldViewProjection };
}

bool ArrowSystem::IsArrowFinished(const Float3& arrowVelocity)
{
	return arrowVelocity.x == ARROW_FINISHED;
}

void ArrowSystem::SetArrowFinished(Float3& arrowVelocity)
{
	arrowVelocity.x = ARROW_FINISHED;
}

void ArrowSystem::Render()
{
	m_BlendState.Activate();
	m_RasterizerState.Activate();
	m_Sampler.Activate();
	m_DepthStencilState.Activate();
	m_InputLayout.Activate();
	m_Shader.Activate();
	m_Texture.Activate();
	m_CameraBuffer.Update(CB_CamPos{ Globals::pCamera->GetPosition() });
	m_CameraBuffer.Activate();
	m_Instances.Draw();
}
