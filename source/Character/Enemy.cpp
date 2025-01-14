#include "pch.h"
#include "Enemy.h"

#include <Environment/Terrain.h>
#include <Services/GameplaySystems.h>
#include <Weapons/Bow.h>

#include "Weapons/ArrowSystem.h"
#include <Transform\WorldMatrix.h>

using namespace TowerGame;

constexpr float MOVING = 2000;
constexpr float DEAD = 2001;

Float2 Enemy::FullAnimationMovement{};

Enemy::Enemy()
	: m_FallOverAxis{ MOVING }
{
}

Enemy::Enemy(const Animations::Animation& animation, const Float3& initPos)
	: m_Animator{ animation }
	, m_World{ initPos, {} }
	, m_FallOverAxis{ MOVING }
{
}

void Enemy::Update(
	const Float2& target,
	const Animations::Animation& animation)
{
	if (m_FallOverAxis.x < DEAD)
	{
		if (m_FallOverAxis.x == MOVING)
		{
			const unsigned nrLoops{ m_Animator.ProgressTime(animation) };

			const Float2 newRootPos{ m_Animator.GetRootXz() };
			const Float2 rootMotion{
				(newRootPos - m_RootPos)
				+ FullAnimationMovement * nrLoops
			};

			m_RootPos = newRootPos;
			UpdateMove(target, rootMotion);
		}
		else
		{
			UpdateFall();
			for (int i = 0; i < m_Arrows.size(); i++)
				GameplaySystems::GetArrowSystem().SetArrowTransform(m_Arrows[i].arrowIdx, Transform::LocalToWorld(m_Arrows[i].Local, m_World));
		}
	}
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

void Enemy::UpdateMove(const Float2& target, const Float2& movement)
{
	constexpr float minTargetDistanceSq = 6 * 6;

	if (m_World.Position.Xz().DistanceSq(target) <= minTargetDistanceSq)
		return;

	m_World.MoveRelativeXz(movement);
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
