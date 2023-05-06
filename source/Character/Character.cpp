#include "pch.h"
#include "Character.h"

#include "../TowerAppRenderer.h"
#include "../Environment/Tower.h"
#include "../Environment/Terrain.h"
#include "Framework/CoreServices.h"
#include "Physics/CollisionDetection.h"

using namespace Math;
using namespace Physics;

Character::Character(const Framework::CoreServices& services, const Math::Float3& position)
	: m_CameraController{ services.Camera, services.Window.GetKeyboard(), services.Window.GetMouse() }
	, m_Bow{ services.Gpu }
{
	m_CameraController.SetPosition(position);
}

void Character::Register(const Terrain& terrain)
{
	m_Bow.Register(terrain);
	m_pTerrain = &terrain;
}

void Character::Register(const Tower& tower)
{
	m_Bow.Register(tower);
	m_pTower = &tower;
}

void Character::Register(const TowerAppRenderer& renderer)
{
	m_Bow.Register(renderer.GetTransformRenderer());
}

void Character::Update()
{
	const Float3 oldHead{ m_CameraController.GetCameraPosition() };
	const Float2 movement{ Globals::pKeyboard->GetWasdInput(Globals::DeltaTime * 5) };
	m_CameraController.MoveRelative({ movement.x, -9.81f * Globals::DeltaTime, movement.y });
	const Float3 newPos{ m_CameraController.GetCameraPosition() };

	//ground collision
	constexpr float height{ 1.8f };
	const Float3 head{ oldHead };
	const Float3 feet{ newPos - Float3{0, height, 0} };
	CollisionDetection::Collision collision{};
	if (m_pTower->IsColliding(head, feet, collision) || m_pTerrain->IsColliding(head, feet, collision))
		m_CameraController.SetPositionY(collision.position.y + height);
	else if (feet.y < -1)
		m_CameraController.SetPositionY(-1 + height);

	//cam & bow
	m_CameraController.Update();
	m_Bow.Update(m_CameraController.GetTransform());
}
