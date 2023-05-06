#include "pch.h"
#include "Character.h"

#include "TowerAppRenderer.h"
#include "Framework/CoreServices.h"
#include "Physics/CollisionDetection.h"

using namespace Math;
using namespace Physics;

Character::Character(const Framework::CoreServices& services)
	: m_CameraController{ services.Camera, services.Window.GetKeyboard(), services.Window.GetMouse() }
	, m_Bow{ services.Gpu }
{
	m_CameraController.SetPosition({ 0,1,0 });
}

void Character::Register(const Terrain& terrain)
{
	m_Bow.SetTerrain(terrain);
	m_pTerrain = &terrain;
}

void Character::Register(const TowerAppRenderer& renderer)
{
	m_Bow.Register(renderer.GetTransformRenderer());
}

void Character::Update()
{
	const Float2 movement{ Globals::pKeyboard->GetWasdInput(Globals::DeltaTime * 5) };
	m_CameraController.MoveRelative({ movement.x, -9.81f * Globals::DeltaTime, movement.y });

	//ground collision
	constexpr float height{ 1.8f };
	const Float3 head{ m_CameraController.GetCameraPosition() };
	const Float3 feet{ head - Float3{0, height, 0} };
	CollisionDetection::Collision collision{};
	if (m_pTerrain->IsColliding(head, feet, collision))
		m_CameraController.SetPositionY(collision.position.y + height);
	else if (feet.y < -1)
		m_CameraController.SetPositionY(-1 + height);

	//cam & bow
	m_CameraController.Update();
	m_Bow.Update(m_CameraController.GetTransform());
}
