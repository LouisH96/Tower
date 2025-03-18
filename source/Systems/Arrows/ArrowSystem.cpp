#include "pch.h"
#include "ArrowSystem.h"

#include "Camera/Camera.h"
#include "Generation/PlaneGeneration.h"
#include "Io/Fbx/FbxClass.h"
#include "Transform/WorldMatrix.h"
#include <Geometry\Shapes\Ray.h>
#include <Physics\CollisionDetection.h>
#include <Systems\Bow\Bow.h>
#include <Systems\Collisions\CollisionSystem.h>
#include <Systems\Enemies\EnemySystem.h>
#include <Systems\Terrain\Terrain.h>
#include <TowerApp.h>

using namespace TowerGame;
using namespace Rendering;
using namespace Physics;

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
		if (SYSTEMS.Terrain.GetHeight(position.Xz()) >= position.y)
		{
			SetArrowFinished(velocity);
			continue;
		}

		//UPDATE WORLD-MATRIX
		velocity.y += -9.81f * Globals::DeltaTime;

		float movementAmount{};
		const Float3 movementDirection{ velocity.Normalized(movementAmount) };
		movementAmount *= Globals::DeltaTime;

		const Float3 newPosition{ position + movementDirection * movementAmount };

		WorldMatrix::SetRotation(world, movementDirection);
		WorldMatrix::SetPosition(world, newPosition);
		//m_Instances[i].modelViewProj = world * Globals::pCamera->GetViewProjection();

		//COLLISIONS
		const CollisionSystem& collisions{ SYSTEMS.Collisions };
		const Ray arrowMovement{ position, movementDirection, movementAmount };

		//IS IN ENEMY?
		Enemy* pEnemy{ collisions.Enemies.IsColliding(position, newPosition) };
		if (pEnemy)
		{
			SYSTEMS.Enemies.OnCollision(Transform{ newPosition, Quaternion{world} }, i, *pEnemy);
			SetArrowFinished(velocity);

			const Float3& launchedPosition{ m_LaunchedPosition[i] };
			const Float3& hitPosition{ newPosition };
			const float travelDistance{ launchedPosition.Distance(hitPosition) };
			const unsigned score{ static_cast<unsigned>(travelDistance * 10) };
			SYSTEMS.Ui.AddScore(score);
			continue;
		}

		//IS IN TOWER?
		const ModelCollidable* pHitModel{};
		const ModelCollidable::Instance* pHitInstance{};
		CollisionDetection::Collision collision{};
		if (collisions.Models.IsColliding(arrowMovement, collision))
		{
			SetArrowFinished(velocity);
			continue;
		}

		if (collisions.Tower.IsColliding(position, newPosition))
		{
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

void ArrowSystem::SetArrowTransform(int arrowIdx, const Float4X4& newArrowWorld)
{
	m_Instances[arrowIdx] = { newArrowWorld };
}

void ArrowSystem::SetArrowTransform(int arrowIdx, const Transform& newArrowWorld)
{
	const Float4X4 world{ newArrowWorld.AsMatrix() };
	const Float4X4 worldViewProjection{ world * Globals::pCamera->GetViewProjection() };
	m_Instances[arrowIdx] = { world };
}

void ArrowSystem::Launch(int arrowIdx, const Float3& launchedPosition)
{
	const Instance& instance{ m_Instances[arrowIdx] };
	m_Velocities[arrowIdx] = WorldMatrix::GetForward(instance.model) * LAUNCH_SPEED;
	m_LaunchedPosition.Add(launchedPosition);
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