#include "pch.h"
#include "Enemy.h"

#include <Environment/Terrain.h>
#include <Services/GameplaySystems.h>
#include <Weapons/Bow.h>

#include "Weapons/ArrowSystem.h"

using namespace TowerGame;

constexpr float MOVING = 2000;
constexpr float DEAD = 2001;

Enemy::Enemy()
	: m_FallOverAxis{ MOVING }
{
}

Enemy::Enemy(const Float3& initPos)
	: m_World{ initPos, {} }
	, m_FallOverAxis{ MOVING }
{
}

void Enemy::Update(const Float2& target, float maxMovement)
{
	if (m_FallOverAxis.x < DEAD)
	{
		if (m_FallOverAxis.x == MOVING)
			UpdateMove(target, maxMovement);
		else
			UpdateFall();
	}
	for (int i = 0; i < m_Arrows.size(); i++)
		GameplaySystems::GetArrowSystem().SetArrowTransform(m_Arrows[i].arrowIdx, Transform::LocalToWorld(m_Arrows[i].Local, m_World));
}

void Enemy::HitByArrow(const Transform& worldArrowTransform, int arrowIdx)
{
	m_Arrows.push_back(
		{
			arrowIdx,
			worldArrowTransform
		});
	m_Arrows[m_Arrows.size() - 1].Local.SetRelativeTo(m_World);

	//fall over
	m_FallOverAxis =
		Float3{ 0,1,0 }.Cross(worldArrowTransform.Rotation.GetForward()).Normalized();
}

void Enemy::UpdateMove(const Float2& target, float maxMovement)
{
	constexpr float minTargetDistanceSq = 6 * 6;

	const Float2 toTarget{ Float2{target - m_World.Position.Xz()} };
	const float toTargetLengthSq{ toTarget.LengthSq() };
	if (toTargetLengthSq > minTargetDistanceSq)
	{
		const Float2 movement{ toTarget * (maxMovement / sqrt(toTargetLengthSq)) };
		m_World.Position += Float3::FromXz(movement);
	}

	m_World.Position.y = GameplaySystems::GetTerrain().GetHeight(m_World.Position.Xz());
}

void Enemy::UpdateFall()
{
	const Quaternion rotation{ Quaternion::FromAxis(m_FallOverAxis, 150 * Constants::TO_RAD * Globals::DeltaTime) };
	m_World.Rotation.RotateBy(rotation);

	const Float3 head{ m_World.LocalToWorld({0, 1.8f,0}) };
	const float terrainHeight{ GameplaySystems::GetTerrain().GetHeight(head.Xz()) };

	if (head.y <= terrainHeight)
		m_FallOverAxis.x = DEAD;
}
