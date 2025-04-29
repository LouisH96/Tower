#include "pch.h"
#include "Character.h"

#include "CharacterMovement.h"
#include <Geometry\Shapes\Sphere.h>
#include <Physics\CollisionDetection.h>
#include <Physics\Collisions\SphereTriangleCollision.h>
#include <Systems\Collisions\CollisionSystem.h>
#include <Systems\Terrain\Terrain.h>
#include <TowerApp.h>
#include <Transform\WorldMatrix.h>

using namespace TowerGame;

const float Character::JUMP_VELOCITY{ sqrtf(JUMP_HEIGHT * 2 * 9.81f) };

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
	//Get Input
	const Float2 movementInput{ Globals::pKeyboard->GetArrowInput(
			m_MoveLeftKey, m_MoveRightKey, m_MoveUpKey, m_MoveDownKey,
			Globals::DeltaTime * SPEED) };

	const Float3 inputDir{
		m_CameraController.GetRelativeMovement({ movementInput.x, 0, movementInput.y }) };

	m_Velocity.y -= 9.81f * Globals::DeltaTime;
	m_Velocity.x = inputDir.x;
	m_Velocity.z = inputDir.z;

	if (KEYBOARD.IsDown(KeyCodes::SHIFT_LEFT))
		m_Velocity.y = SPEED;
	else if (KEYBOARD.IsDown(KeyCodes::CTRL_LEFT))
		m_Velocity.y = -SPEED;

	const Float3 desiredMovement{
		m_Velocity.x,
		m_Velocity.y * Globals::DeltaTime,
		m_Velocity.z };

	//
	const float initVelocityY{ m_Velocity.y };
	const Float3 oldPos{ GetFeetPosition() + Float3{0,CharacterMovement::FULL_RADIUS,0} };

	Float3 spherePos{ oldPos };
	Float3 direction{ desiredMovement };
	float distance{};

	if (direction.LengthSq() > 0)
	{
		direction.Normalize(distance);
		CharacterMovement::DoMovement(
			spherePos,
			direction, distance, m_Velocity);
	}
	const Float3 movedAmount{ spherePos - oldPos };

	if(m_Velocity.y > initVelocityY 
		&& KEYBOARD.IsPressed(KeyCodes::SPACE))
		m_Velocity.y += JUMP_VELOCITY;

	//cam & bow
	m_CameraController.Move(movedAmount);
	m_CameraController.Update();
}

Float3 Character::GetFeetPosition() const
{
	Float3 pos{ GetPosition() };
	pos.y -= HEIGHT;
	return pos;
}

