#include "pch.h"
#include "Character.h"

#include <Geometry/Shapes/Sphere.h>
#include <Physics/CollisionDetection.h>
#include <Systems\Collisions\CollisionSystem.h>
#include <Systems\Terrain\Terrain.h>
#include <TowerApp.h>

using namespace TowerGame;

Character::Character()
	: m_MoveDownKey{ Keyboard::ScanCodeToVk(ScanCode::S) }
	, m_MoveUpKey{ Keyboard::ScanCodeToVk(ScanCode::W) }
	, m_MoveLeftKey{ Keyboard::ScanCodeToVk(ScanCode::A) }
	, m_MoveRightKey{ Keyboard::ScanCodeToVk(ScanCode::D) }
{
}

void Character::Init(const Float3& position)
{
	m_CameraController = FpsCameraController{ *Globals::pCamera };
	m_CameraController.SetPosition(position);
}

void Character::Update()
{
	const Float3 oldHead{ m_CameraController.GetPosition() };
	const Float2 movement{ Globals::pKeyboard->GetArrowInput(
			m_MoveLeftKey, m_MoveRightKey, m_MoveUpKey, m_MoveDownKey,
			Globals::DeltaTime * 5) };
	m_CameraController.MoveRelative({ movement.x, -9.81f * Globals::DeltaTime, movement.y });
	const Float3 newPos{ m_CameraController.GetPosition() };

	//ground collision
	const Float3 head{ oldHead };
	const Float3 feet{ GetFeetPosition() };
	Physics::CollisionDetection::Collision collision{};
	if (SYSTEMS.Collisions.Tower.IsColliding(head, feet, collision))
		m_CameraController.SetPositionY(collision.position.y + HEIGHT);
	else
	{
		const float terrainHeight{ SYSTEMS.Terrain.GetHeight(feet.Xz()) };
		if (feet.y < terrainHeight)
			m_CameraController.SetPositionY(terrainHeight + HEIGHT);
	}

	Float3 overlap;
	if (SYSTEMS.Collisions.Tower.IsColliding(Sphere{ newPos, .1f }, overlap))
		m_CameraController.SetPosition(m_CameraController.GetPosition() - overlap);

	//cam & bow
	m_CameraController.Update();
}

Float3 Character::GetFeetPosition() const
{
	Float3 pos{ GetPosition() };
	pos.y -= HEIGHT;
	return pos;
}
