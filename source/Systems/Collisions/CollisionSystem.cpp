#include "pch.h"
#include "CollisionSystem.h"

#include <Animations\AnimationUtils.h>
#include <Geometry\Shapes\Cube.h>
#include <Geometry\Shapes\Sphere.h>
#include <Geometry\Shapes\Triangle.h>
#include <Physics\Collisions\LineCubeCollision.h>
#include <Physics\Collisions\PointTriangleCollision.h>
#include <Physics\Collisions\SphereAabCollision.h>
#include <Physics\Collisions\SphereTriangleCollision.h>
#include <Transform\WorldMatrix.h>

using namespace TowerGame;
using namespace Physics;

bool ModelCollidables::GetBoundsCollision(
	const Ray& ray, const ModelCollidable*& pModel, const ModelCollidable::Instance*& pInstance) const
{
	for (unsigned iModel{ 0 }; iModel < Models.GetSize(); ++iModel)
	{
		const ModelCollidable& model{ Models[iModel] };
		pInstance = model.GetBoundsCollision(ray);
		if (pInstance)
		{
			pModel = &model;
			return true;
		}
	}
	return false;
}

bool ModelCollidables::IsColliding(const Ray& ray,
	CollisionDetection::Collision& collision) const
{
	for (unsigned iModel{ 0 }; iModel < Models.GetSize(); ++iModel)
		if (Models[iModel].IsColliding(ray, collision))
			return true;
	return false;
}

bool ModelCollidables::IsColliding(
	const Sphere& sphere,
	Physics::CollisionDetection::Collision& collision) const
{
	for (unsigned iModel{ 0 }; iModel < Models.GetSize(); ++iModel)
	{
		if (Models[iModel].IsColliding(sphere, collision))
		{
			return true;
		}
	}
	return false;
}

void ModelCollidables::RenderDebugBoundingBoxes()
{
	for (unsigned iModel{ 0 }; iModel < Models.GetSize(); ++iModel)
	{
		const ModelCollidable& model{ Models[iModel] };
		for (unsigned iInst{ 0 }; iInst < model.Instances.GetSize(); ++iInst)
		{
			Float4X4 world{ model.Instances[iInst].World };
			Cube cube{ Transform{world}, model.Size };

			DebugRenderer::DrawCube(cube, Color::Functions::GetColor(iModel));
		}
	}
}

void ModelCollidables::RenderDebugGlobalBoundingBoxes()
{
	for (unsigned iModel{ 0 }; iModel < Models.GetSize(); ++iModel)
	{
		const ModelCollidable& model{ Models[iModel] };
		for (unsigned iInst{ 0 }; iInst < model.Instances.GetSize(); ++iInst)
		{
			DebugRenderer::DrawCube(
				model.Instances[iInst].WorldBounds,
				Color::Functions::GetColor(iModel));
		}
	}
}

bool ModelCollidable::IsColliding(const Ray& ray,
	CollisionDetection::Collision& collision) const
{
	Ray localRay{};
	localRay.Length = ray.Length;

	for (unsigned iInst{ 0 }; iInst < Instances.GetSize(); ++iInst)
	{
		localRay.Origin = ray.Origin;
		localRay.Direction = ray.Direction;

		const Instance& instance{ Instances[iInst] };

		WorldMatrix::TransformPoint(instance.WorldInverse, localRay.Origin);
		WorldMatrix::RotatePoint(instance.WorldInverse, localRay.Direction);
		const Float3 invDirection{ localRay.Direction.Inversed() };

		//Bounding Box
		if (!LineCubeCollision::Detect(localRay, invDirection, Size))
			continue;

		//Mesh
		if (CollisionDetection::Detect(localRay, Points, TriangleNormals, collision))
			return true;
	}
	return false;
}

bool ModelCollidable::IsColliding(const Sphere& sphere,
	Physics::CollisionDetection::Collision& collision) const
{
	Sphere localSphere{ sphere };

	for (unsigned iInst{ 0 }; iInst < Instances.GetSize(); ++iInst)
	{
		const Instance& instance{ Instances[iInst] };

		localSphere.SetCenter(
			WorldMatrix::TransformedPoint(instance.WorldInverse, sphere.GetCenter())
		);

		if (!SphereAabCollision::Detect(localSphere, Size))
			continue;

		Float3 closest;
		if (SphereTriangleCollision::Detect(localSphere, Points, closest))
			return true;
	}
	return false;
}

const ModelCollidable::Instance* ModelCollidable::GetBoundsCollision(const Ray& ray) const
{
	for (unsigned iInst{ 0 }; iInst < Instances.GetSize(); ++iInst)
	{
		Ray localRay{ ray };
		const Instance& instance{ Instances[iInst] };
		WorldMatrix::TransformPoint(instance.WorldInverse, localRay.Origin);
		WorldMatrix::RotatePoint(instance.WorldInverse, localRay.Direction);
		const Float3 invDirection{ localRay.Direction.Inversed() };

		if (LineCubeCollision::Detect(localRay, invDirection, Size))
			return &Instances[iInst];
	}
	return nullptr;
}
