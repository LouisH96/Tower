#include "pch.h"
#include "Character.h"

#include "TowerAppRenderer.h"
#include "Framework/CoreServices.h"

Character::Character(const Framework::CoreServices& services)
	: m_CameraController{ services.Camera, services.Window.GetKeyboard(), services.Window.GetMouse() }
	, m_Bow{ services.Gpu }
{
	m_CameraController.SetPosition({ 0,1,0 });
}

void Character::Register(const Terrain& terrain)
{
	m_Bow.SetTerrain(terrain);
}

void Character::Register(const TowerAppRenderer& renderer)
{
	m_Bow.Register(renderer.GetTransformRenderer());
}

void Character::Update()
{
	const Math::Float2 movement{ Globals::pKeyboard->GetWasdInput(Globals::DeltaTime * 5) };
	m_CameraController.MoveRelative({ movement.x, 0, movement.y });

	m_CameraController.Update();
	m_Bow.Update(m_CameraController.GetTransform());
}
