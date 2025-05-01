#pragma once

#include <Geometry\Shapes\CubeAA.h>
#include <Geometry\Shapes\Line.h>
#include <Physics\CollisionDetection.h>
#include <Physics\Collisions\SphereTriangleCollision.h>
#include <Systems\Enemies\Enemy.h>

namespace TowerGame
{
struct ModelCollidable
{
	struct Instance
	{
		Float4X4 World;
		Float4X4 WorldInverse;

		CubeAA WorldBounds;
	};

	Float3 Size;
	List<Float3> Points;
	List<Float3> TriangleNormals;

	List<Instance> Instances{};

	bool IsColliding(const Ray& ray,
		Physics::CollisionDetection::Collision& collision) const;

	bool IsColliding(const Sphere& sphere,
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

	bool IsColliding(const Sphere& sphere,
		Physics::CollisionDetection::Collision& collision) const;
	
	void RenderDebugBoundingBoxes();
	void RenderDebugGlobalBoundingBoxes();
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
	EnemiesCollidable Enemies;
	ModelCollidables Models;
};
}
