#include "pch.h"
#include "Enemy.h"

#include <Systems\Arrows\ArrowSystem.h>
#include <Systems\Bow\Bow.h>
#include <Systems\Terrain\Terrain.h>

#include <Transform\WorldMatrix.h>
#include <TowerApp.h>

using namespace TowerGame;

Float2 Enemy::FullAnimationMovement{};

Enemy::Enemy()
	: m_State{ State::Running }
{
}

Enemy::Enemy(const Animations::Animation& animation, const Float3& initPos)
	: m_Animator{ animation }
	, m_World{ initPos, {} }
	, m_State{ State::Running }
{
}

void Enemy::Update(
	const Float2& target,
	const Animations::Animation& animation)
{
	if (m_State == State::Dead)
		return;

	if (m_State == State::Running)
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
	else if (m_State == State::Falling)
	{
		UpdateFall();
		for (int i = 0; i < m_Arrows.size(); i++)
			SYSTEMS.Arrows.SetArrowTransform(m_Arrows[i].arrowIdx, Transform::LocalToWorld(m_Arrows[i].Local, m_World));
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
	m_State = State::Falling;
}

Float3 Enemy::ToAnimationSpace(const Float3& worldSpace) const
{
	Float3 local{ m_World.WorldToLocal(worldSpace) };
	local.AddXz(m_RootPos);
	return local;
}

void Enemy::UpdateMove(const Float2& target, const Float2& movement)
{
	constexpr float minTargetDistanceSq = 6 * 6;

	if (m_World.Position.Xz().DistanceSq(target) <= minTargetDistanceSq)
	{
		m_State = State::Completed;
		return;
	}

	m_World.MoveRelativeXz(movement);
	m_World.Position.y = SYSTEMS.Terrain.GetHeight(m_World.Position.Xz());
}

void Enemy::UpdateFall()
{
	const Quaternion rotation{ Quaternion::FromAxis(m_FallOverAxis, 150 * Constants::TO_RAD * Globals::DeltaTime) };
	m_World.Rotation.RotateBy(rotation);

	const Float3 head{ m_World.LocalToWorld({0, 1.8f,0}) };
	const float terrainHeight{ SYSTEMS.Terrain.GetHeight(head.Xz()) };

	if (head.y <= terrainHeight)
	{
		m_State = State::Dead;
	}
}
