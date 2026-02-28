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
	const float dt{ Globals::DeltaTime };

	for (unsigned iArrow = 0; iArrow < m_Velocities.GetSize(); iArrow++)
	{
		//IS ALREADY FINISHED ?
		Float3& velocity{ m_Velocities[iArrow] };
		Float4X4& world{ m_Instances[iArrow].model };
		if (IsArrowFinished(velocity))
			continue;

		//IS UNDER TERRAIN ?
		const Float3 position{ WorldMatrix::GetPosition(world) };
		if (SYSTEMS.Terrain.GetHeight(position.Xz()) >= position.y)
		{
			SetArrowFinished(iArrow);
			continue;
		}

		//UPDATE WORLD-MATRIX
		velocity.y += Constants::GRAVITY / 2 * dt;

		float movementAmount{};
		const Float3 movementDirection{ velocity.Normalized(movementAmount) };
		movementAmount *= dt;
		velocity.y += Constants::GRAVITY / 2 * dt;

		const Float3 newPosition{ position + movementDirection * movementAmount };

		WorldMatrix::SetRotation(world, movementDirection);
		WorldMatrix::SetPosition(world, newPosition);
		//m_Instances[i].modelViewProj = world * Globals::pCamera->GetViewProjection();

		//COLLISIONS
		const CollisionSystem& collisions{ SYSTEMS.Collisions };
		const Ray arrowMovement{ position, movementDirection, movementAmount };

		//IS IN ENEMY?
		EnemySystem::Enemy* pEnemy{ SYSTEMS.Enemies.IsColliding({position, newPosition}) };
		if (pEnemy)
		{
			SYSTEMS.Enemies.OnCollision(Transform{ newPosition, Quaternion{world} }, iArrow, *pEnemy);
			SetArrowFinished(iArrow);

			const Float3& launchedPosition{ m_LaunchedPosition[iArrow] };
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
			SetArrowFinished(iArrow);
			continue;
		}

		//Tracer
		m_Tracing.UpdateHead(iArrow, world, velocity);
	}

	m_Tracing.UpdateTracers();
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
	const Float3 forward{ WorldMatrix::GetForward(instance.model) };
	const Float4X4& transform{ instance.model };
	const Float3 position{ transform.GetRow3().Xyz() };

	m_Velocities[arrowIdx] = forward * LAUNCH_SPEED;
	m_LaunchedPosition.Add(position);
	m_IsCharging = 0;

	//Tracer
	const Float2 forwardXz{ forward.Xz().Normalized() };
	const float upAngle{ asinf(forward.y) };
	const Float2 force{ cosf(upAngle) * LAUNCH_SPEED, forward.y * LAUNCH_SPEED };
	m_Tracing.Add(position, forwardXz, force, transform);
}

bool ArrowSystem::IsArrowFinished(const Float3& arrowVelocity)
{
	return arrowVelocity.x == ARROW_FINISHED;
}

void ArrowSystem::SetArrowFinished(unsigned idx)
{
	m_Velocities[idx].x = ARROW_FINISHED;
}

void ArrowSystem::Render(bool hideCharging)
{
	m_Texture.Activate();
	m_Instances.Draw(m_Instances.GetSize() -
		(hideCharging ? m_IsCharging : 0));
}

void ArrowSystem::RenderTracers()
{
	m_Tracing.Render();
}
