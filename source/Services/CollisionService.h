#pragma once
#include "Physics/CollisionDetection.h"
#include "../Character/Enemy.h"

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
	bool IsColliding(const Sphere& sphere, Float3& sphereToHitPlane) const;

	Array<Math::Float3> Points;
	Array<Math::Float3> TriangleNormals;
	Array<int> Indices;
};

struct EnemiesCollidable
{
	Enemy* IsColliding(const Math::Float3& begin, const Math::Float3& end) const;

	Array<Enemy>* pEnemies{};

	Array<Math::Float3> Points;
	Array<Math::Float3> TriangleNormals;

private:
	bool IsColliding(const Math::Float3& begin, const Math::Float3& end, const Enemy& enemy) const;
	static bool HasOverlap(float aMin, float aMax, float bMin, float bMax);
};
struct CollisionService
{
	MeshCollidable Tower;
	EnemiesCollidable Enemies;
};

