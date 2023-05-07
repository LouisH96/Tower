#include "pch.h"
#include "Enemy.h"

#include "Physics/CollisionDetection.h"
#include "../Environment/Terrain.h"

using namespace Math;

Enemy::Enemy(const Math::Float3& initPos)
	: m_World{ initPos, {} }
{
}

void Enemy::Update(const Terrain& terrain, const Math::Float2& target, float maxMovement)
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
	if (terrain.IsColliding(head, feet, collision))
		m_World.Position.y = collision.position.y;
}
