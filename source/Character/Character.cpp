#include "pch.h"
#include "Character.h"

#include <Environment/Terrain.h>
#include <Physics/CollisionDetection.h>
#include <Services/CollisionService.h>
#include <Services/GameplaySystems.h>

Character::Character(const Float3& position)
	: m_CameraController{ *Globals::pCamera }
{
	m_CameraController.SetPosition(position);
}

void Character::Update()
{
	const Float3 oldHead{ m_CameraController.GetPosition() };
	const Float2 movement{ Globals::pKeyboard->GetWasdInput(Globals::DeltaTime * 5) };
	m_CameraController.MoveRelative({ movement.x, -9.81f * Globals::DeltaTime, movement.y });
	const Float3 newPos{ m_CameraController.GetPosition() };

	//ground collision
	constexpr float height{ 1.8f };
	const Float3 head{ oldHead };
	const Float3 feet{ newPos - Float3{0, height, 0} };
	Physics::CollisionDetection::Collision collision{};
	if (GameplaySystems::GetCollisionService().Tower.IsColliding(head, feet, collision))
		m_CameraController.SetPositionY(collision.position.y + height);
	else
	{
		const float terrainHeight{ GameplaySystems::GetTerrain().GetHeight(feet.Xz()) };
		if (feet.y < terrainHeight)
			m_CameraController.SetPositionY(terrainHeight + height);
	}

	Float3 overlap;
	if (GameplaySystems::GetCollisionService().Tower.IsColliding(Sphere{ newPos, .1f }, overlap))
		m_CameraController.SetPosition(m_CameraController.GetPosition() - overlap);

	//cam & bow
	m_CameraController.Update();
}
