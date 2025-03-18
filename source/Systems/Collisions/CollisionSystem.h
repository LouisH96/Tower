#pragma once
#include <Physics/CollisionDetection.h>

#include <Geometry\Shapes\Line.h>
#include <Systems\Enemies\Enemy.h>

namespace TowerGame
{
struct MeshCollidable
{
	MeshCollidable() = default;
	~MeshCollidable() = default;

	MeshCollidable(const MeshCollidable& other) = delete;
	MeshCollidable(MeshCollidable&& other) noexcept = default;
	MeshCollidable& operator=(const MeshCollidable& other) = delete;
	MeshCollidable& operator=(MeshCollidable&& other) noexcept = default;

	bool IsColliding(const Float3& begin, const Float3& end) const;
	bool IsColliding(const Float3& begin, const Float3& end, Physics::CollisionDetection::Collision& collision) const;
	bool IsColliding(const Sphere& sphere, Float3& sphereToHitPlane) const;

	Array<Float3> Points;
	Array<Float3> TriangleNormals;
	Array<int> Indices;
};

struct ModelCollidable
{
	struct Instance
	{
		Float4X4 World;
		Float4X4 WorldInverse;
	};

	Float3 Size;
	Array<Float3> Points;
	Array<Float3> TriangleNormals;

	List<Instance> Instances{};

	bool IsColliding(const Ray& ray,
		Physics::CollisionDetection::Collision& collision) const;

	const Instance* GetBoundsCollision(const Ray& ray) const;
};

struct ModelCollidables
{
	List<ModelCollidable> Models{};

	bool GetBoundsCollision(
		const Ray& ray, 
		const ModelCollidable*& pModel, const ModelCollidable::Instance*& pInstance) const;

	bool IsColliding(const Ray& ray,
		Physics::CollisionDetection::Collision& collision) const;

	void RenderDebugBoundingBoxes();
};

struct EnemiesCollidable
{
	struct Vertex
	{
		Float3 Point;
		Int4 BoneIndices;
		Float4 BoneWeights;
	};

	Enemy* IsColliding(const Float3& begin, const Float3& end) const;

	Array<Enemy>* pEnemies{};
	Array<Vertex> Vertices;

private:
	bool IsColliding(const Float3& begin, const Float3& end, const Enemy& enemy) const;
	static bool HasOverlap(float aMin, float aMax, float bMin, float bMax);
};

struct CollisionSystem
{
	MeshCollidable Tower;
	EnemiesCollidable Enemies;
	ModelCollidables Models;
};
}
