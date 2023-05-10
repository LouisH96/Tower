#include "pch.h"
#include "Enemy.h"

#include "../Services/TowerAppServices.h"
#include "Physics/CollisionDetection.h"

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

void Enemy::Update(
	const TowerAppServices& services,
	const Float2& target, float maxMovement)
{
	if (m_FallOverAxis.x < DEAD)
	{
		if (m_FallOverAxis.x == MOVING)
			UpdateMove(services, target, maxMovement);
		else
			UpdateFall(services);
	}
}

void Enemy::UpdateArrows() const
{
	for (int i = 0; i < m_Arrows.size(); i++)
		*m_Arrows[i].pWorld = Game::Transform::LocalToWorld(m_Arrows[i].Local, m_World);
}

void Enemy::HitByArrow(Game::Transform& worldArrowTransform)
{
	m_Arrows.push_back(
		{
			&worldArrowTransform,
			worldArrowTransform
		});
	m_Arrows[m_Arrows.size() - 1].Local.SetRelativeTo(m_World);

	//fall over
	m_FallOverAxis =
		Float3{ 0,1,0 }.Cross(worldArrowTransform.Rotation.GetForward()).Normalized();
}

void Enemy::UpdateMove(
	const TowerAppServices& services,
	const Float2& target, float maxMovement)
{
	constexpr float minTargetDistanceSq = 6 * 6;

	const Float2 toTarget{ Float2{target - m_World.Position.Xz()} };
	const float toTargetLengthSq{ toTarget.LengthSq() };
	if (toTargetLengthSq > minTargetDistanceSq)
	{
		const Float2 movement{ toTarget * (maxMovement / sqrt(toTargetLengthSq)) };
		m_World.Position += Float3::FromXz(movement);
	}

	Float3 feet{ m_World.Position };
	feet.y = -1000;
	Float3 head{ m_World.Position };
	head.y += 2;

	Physics::CollisionDetection::Collision collision{};
	if (services.Collision.Terrain.IsColliding(head, feet, collision))
		m_World.Position.y = collision.position.y;
}

void Enemy::UpdateFall(const TowerAppServices& services)
{
	const Quaternion rotation{ Quaternion::FromAxis(m_FallOverAxis, 150 * Constants::TO_RAD * Globals::DeltaTime) };
	m_World.Rotation.RotateBy(rotation);

	const Float3 head{ 0, 1.8f,0 };
	constexpr float headRadius{ .1f };

	const Float3 headFront{ m_World.LocalToWorld(head + Float3{0,0,headRadius}) };
	const Float3 headBack{ m_World.LocalToWorld(head + Float3{0,0,-headRadius}) };
	if (services.Collision.Terrain.IsColliding(headFront, headBack))
		m_FallOverAxis.x = DEAD;

	const Float3 headLeft{ m_World.LocalToWorld(head + Float3{-headRadius, 0,0 }) };
	const Float3 headRight{ m_World.LocalToWorld(head + Float3{headRadius, 0, 0}) };
	if (services.Collision.Terrain.IsColliding(headLeft, headRight))
		m_FallOverAxis.x = DEAD;
}
