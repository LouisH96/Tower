#include "pch.h"
#include "Character.h"

#include "../TowerAppRenderer.h"
#include "../Environment/Tower.h"
#include "../Environment/Terrain.h"
#include "../Services/TowerAppServices.h"
#include "Framework/CoreServices.h"
#include "Physics/CollisionDetection.h"

using namespace Math;
using namespace Physics;

Character::Character(const Framework::CoreServices& services, const Math::Float3& position)
	: m_CameraController{ services.Camera }
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

void Character::Register(TowerAppRenderer& renderer)
{
	m_Bow.Register(renderer.GetTransformRenderer());
	renderer.SetCharacter(*this);
}

void Character::Update(const TowerAppServices& services)
{
	const Float3 oldHead{ m_CameraController.GetPosition() };
	const Float2 movement{ Globals::pKeyboard->GetWasdInput(Globals::DeltaTime * 5) };
	m_CameraController.MoveRelative({ movement.x, -9.81f * Globals::DeltaTime, movement.y });
	const Float3 newPos{ m_CameraController.GetPosition() };

	//ground collision
	constexpr float height{ 1.8f };
	const Float3 head{ oldHead };
	const Float3 feet{ newPos - Float3{0, height, 0} };
	CollisionDetection::Collision collision{};
	if (services.Collision.Tower.IsColliding(head, feet, collision))
		m_CameraController.SetPositionY(collision.position.y + height);
	else
	{
		const float terrainHeight{ Terrain::Get().GetHeight(feet.Xz()) };
		if (feet.y < terrainHeight)
			m_CameraController.SetPositionY(terrainHeight + height);
	}

	Float3 overlap;
	if (services.Collision.Tower.IsColliding(Sphere{ newPos, .1f }, overlap))
		m_CameraController.SetPosition(m_CameraController.GetPosition() - overlap);

	//cam & bow
	m_CameraController.Update();
	m_Bow.Update(services, m_CameraController.GetTransform());
}

void Character::Render(const Camera& camera)
{
	m_Bow.Render(camera);
}
