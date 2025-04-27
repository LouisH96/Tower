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
