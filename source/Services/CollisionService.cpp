#include "pch.h"
#include "CollisionService.h"

bool MeshCollidable::IsColliding(const Math::Float3& begin, const Math::Float3& end) const
{
	Physics::CollisionDetection::Collision collision;
	return Physics::CollisionDetection::Detect(begin, end, Points, TriangleNormals, Indices, collision);
}

bool MeshCollidable::IsColliding(const Math::Float3& begin, const Math::Float3& end,
	Physics::CollisionDetection::Collision& collision) const
{
	return Physics::CollisionDetection::Detect(begin, end, Points, TriangleNormals, Indices, collision);
}
