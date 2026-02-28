#pragma once

#include <Geometry\Shapes\CubeAA.h>
#include <Geometry\Shapes\Line.h>
#include <Physics\CollisionDetection.h>
#include <Physics\Collisions\SphereTriangleCollision.h>

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

struct CollisionSystem
{
	ModelCollidables Models;
};
}
