#include "pch.h"
#include "CollisionSystem.h"

#include <Animations\AnimationUtils.h>
#include <Geometry\Shapes\Triangle.h>

using namespace TowerGame;
using namespace Physics;

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

	const Float3 localBegin{ enemy.ToAnimationSpace(begin) };
	const Float3 localEnd{ enemy.ToAnimationSpace(end) };

	Physics::CollisionDetection::Collision collision;

	const Array<Float4X4>& bones{ enemy.GetAnimator().GetBones() };

	for (
		unsigned iVertex{ 2 };
		iVertex < Vertices.GetSize();
		iVertex += 3)
	{
		const Vertex& v0{ Vertices[iVertex - 2] };
		const Vertex& v1{ Vertices[iVertex - 1] };
		const Vertex& v2{ Vertices[iVertex] };

		const Float3 pb0{ AnimationUtils::Blend(v0.Point, v0.BoneIndices, v0.BoneWeights, bones) };
		const Float3 pb1{ AnimationUtils::Blend(v1.Point, v1.BoneIndices, v1.BoneWeights, bones) };
		const Float3 pb2{ AnimationUtils::Blend(v2.Point, v2.BoneIndices, v2.BoneWeights, bones) };
		const Float3 tbNormal{ Triangle::FindNormal(pb0,pb1,pb2) };

		CollisionDetection::Collision collision{};
		if (Physics::CollisionDetection::IsLineInTriangle(
			localBegin, localEnd,
			pb0, pb1, pb2,
			tbNormal,
			collision))
		{
			return true;
		}
	}
	return false;
}

bool EnemiesCollidable::HasOverlap(float aMin, float aMax, float bMin, float bMax)
{
	if (aMin < bMin)
		return aMax >= bMin;
	return aMin <= bMax;
}
