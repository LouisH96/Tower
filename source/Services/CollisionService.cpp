#include "pch.h"
#include "CollisionService.h"

bool MeshCollidable::IsColliding(const Float3& begin, const Float3& end) const
{
	Physics::CollisionDetection::Collision collision;
	return Physics::CollisionDetection::Detect(begin, end, Points, TriangleNormals, Indices, collision);
}

bool MeshCollidable::IsColliding(const Float3& begin, const Float3& end,
	Physics::CollisionDetection::Collision& collision) const
{
	return Physics::CollisionDetection::Detect(begin, end, Points, TriangleNormals, Indices, collision);
}

bool MeshCollidable::IsColliding(const Sphere& sphere, Float3& sphereToHitPlane) const
{
	return Physics::CollisionDetection::Detect(sphere, Points, TriangleNormals, Indices, sphereToHitPlane);
}

Enemy* EnemiesCollidable::IsColliding(const Float3& begin, const Float3& end) const
{
	for (unsigned i = 0; i < pEnemies->GetSize(); i++)
		if (IsColliding(begin, end, (*pEnemies)[i]))
			return &(*pEnemies)[i];
	return nullptr;
}

bool EnemiesCollidable::IsColliding(const Float3& begin, const Float3& end, const Enemy& enemy) const
{
	constexpr float cylinderRadius = .75f;
	const float cylinderBot{ enemy.GetTransform().Position.y };
	const float cylinderTop{ cylinderBot + Enemy::HEIGHT };

	//is in cylinder height?
	if (begin.y <= end.y)
	{
		if (!HasOverlap(cylinderBot, cylinderTop, begin.y, end.y))
			return false;
	}
	else
	{
		if (!HasOverlap(cylinderBot, cylinderTop, end.y, begin.y))
			return false;
	}

	//cylinder horizontal check
	const float distanceSq{ Float2{Float2{end.x, end.z} - enemy.GetTransform().Position.Xz() }.LengthSq() };
	if (distanceSq > cylinderRadius * cylinderRadius)
		return false;

	const Float3 localBegin{ enemy.GetTransform().WorldToLocal(begin) };
	const Float3 localEnd{ enemy.GetTransform().WorldToLocal(end) };

	Physics::CollisionDetection::Collision collision;
	return Physics::CollisionDetection::Detect(localBegin, localEnd, Points, TriangleNormals, collision);
}

bool EnemiesCollidable::HasOverlap(float aMin, float aMax, float bMin, float bMax)
{
	if (aMin < bMin)
		return aMax >= bMin;
	return aMin <= bMax;
}
