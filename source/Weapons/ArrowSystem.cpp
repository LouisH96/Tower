#include "pch.h"
#include "ArrowSystem.h"

#include "Bow.h"
#include "Character/EnemySystem.h"
#include "Environment/Terrain.h"
#include "Framework/Resources.h"
#include "Camera/Camera.h"
#include "Generation/PlaneGeneration.h"
#include "Io/Fbx/FbxClass.h"
#include "Services/CollisionService.h"
#include "Services/GameplaySystems.h"
#include "Transform/WorldMatrix.h"

using namespace TowerGame;
using namespace Rendering;

ArrowSystem::ArrowSystem()
	: m_Texture{ Resources::Local(L"Texture_01.png") }
{
	//VERTICES
	const std::wstring arrowMeshPath{ Resources::Local(L"SM_Arrow_01.fbx") };
	Io::Fbx::FbxClass arrowFbxModel{ arrowMeshPath, 1.f };
	Io::Fbx::FbxClass::Geometry& arrowGeom = arrowFbxModel.GetGeometries()[0];
	Array<V_PosNorUv> arrowVertices{ arrowGeom.Points.GetSize() };
	for (unsigned i = 0; i < arrowGeom.Points.GetSize(); i++)
		arrowVertices[i] = V_PosNorUv{ arrowGeom.Points[i] * 0.01f, arrowGeom.Normals[i], arrowGeom.Uvs[i] };

	m_Instances = InstanceList<Vertex, Instance>{
		arrowVertices.GetData(), arrowVertices.GetSize()
	};
}

void ArrowSystem::Update()
{
	for (unsigned i = 0; i < m_Velocities.GetSize(); i++)
	{
		//IS ALREADY FINISHED ?
		Float3& velocity{ m_Velocities[i] };
		Float4X4& world{ m_Instances[i].model };
		if (IsArrowFinished(velocity))
		{
			//m_Instances[i].modelViewProj = world * Globals::pCamera->GetViewProjection();
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
		//m_Instances[i].modelViewProj = world * Globals::pCamera->GetViewProjection();

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

int ArrowSystem::Spawn()
{
	m_Instances.Add({  });
	m_Velocities.Add({ ARROW_FINISHED, 0, 0 });
	m_IsCharging = 1;
	return m_Velocities.GetSize() - 1;
}

void ArrowSystem::SetArrowTransform(int arrowIdx, const Transform& newArrowWorld)
{
	const Float4X4 world{ newArrowWorld.AsMatrix() };
	const Float4X4 worldViewProjection{ world * Globals::pCamera->GetViewProjection() };
	m_Instances[arrowIdx] = { world };
}

void ArrowSystem::Launch(int arrowIdx)
{
	const Instance& instance{ m_Instances[arrowIdx] };
	m_Velocities[arrowIdx] = WorldMatrix::GetForward(instance.model) * 20;
	m_IsCharging = 0;
}

bool ArrowSystem::IsArrowFinished(const Float3& arrowVelocity)
{
	return arrowVelocity.x == ARROW_FINISHED;
}

void ArrowSystem::SetArrowFinished(Float3& arrowVelocity)
{
	arrowVelocity.x = ARROW_FINISHED;
}

void ArrowSystem::Render(bool hideCharging)
{
	m_Texture.Activate();
	m_Instances.Draw(m_Instances.GetSize() - 
		(hideCharging ? m_IsCharging : 0));
}