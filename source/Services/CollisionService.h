#pragma once
#include "Physics/CollisionDetection.h"

struct MeshCollidable
{
	MeshCollidable() = default;
	~MeshCollidable() = default;

	MeshCollidable(const MeshCollidable& other) = delete;
	MeshCollidable(MeshCollidable&& other) noexcept = default;
	MeshCollidable& operator=(const MeshCollidable& other) = delete;
	MeshCollidable& operator=(MeshCollidable&& other) noexcept = default;

	bool IsColliding(const Math::Float3& begin, const Math::Float3& end) const;
	bool IsColliding(const Math::Float3& begin, const Math::Float3& end, Physics::CollisionDetection::Collision& collision) const;

	Array<Math::Float3> Points;
	Array<Math::Float3> TriangleNormals;
	Array<int> Indices;
};

struct CollisionService
{
	MeshCollidable Tower;
	MeshCollidable Terrain;
};

