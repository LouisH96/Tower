#include "pch.h"
#include "CharacterMovement.h"

#include <Physics\Collisions\PointTriangleCollision.h>
#include <Systems\Collisions\CollisionSystem.h>
#include <TowerApp.h>
#include <Transform\WorldMatrix.h>

//#define MY_ASSERT
//#define MY_PRINT

using namespace TowerGame;
using namespace Physics;

const float CharacterMovement::BOUNCE_COS{ cosf(BOUNCE_ANGLE) };
const float CharacterMovement::BOUNCE_SIN{ sinf(BOUNCE_ANGLE) };

void CharacterMovement::DoMovement(
	Float3& center,
	Float3 direction, float distance,
	Float3& velocity,
	Result& result)
{
	const Float3 initDirection{ direction };

#ifdef MY_PRINT
	Logger::Print("");
	Logger::Print("---| DoMovement |---");
#endif

	unsigned step{ 0 };
	while (step < 10)
	{
#ifdef MY_PRINT
		Logger::Print("Step", step);
		Logger::Print("Pos", center);
		Logger::Print("Dir", direction);
		Logger::Print("Dist", distance);
#endif

		const CubeAA globalBounds{
			Sphere::GetGlobalBounds(center, FULL_RADIUS, direction, distance) };

		if (!DoMovementStep(center, direction, distance, velocity, globalBounds, result))
			break;

		if (direction.Dot(initDirection) <= 0)
		{
#ifdef MY_PRINT
			Logger::Print("Direction DOT initDirection < 0",
				direction.Dot(initDirection));
#endif
			break;
		}
		++step;
	}
#ifdef MY_PRINT
	if (step > 5)
		Logger::Print("Steps", step);
#endif

	//Terrain
	const float terrainHeight{ SYSTEMS.Terrain.GetHeight({center.x, center.z})
		+ FULL_RADIUS };
	if (center.y <= terrainHeight)
	{
		center.y = terrainHeight;
		velocity.y = 0;
		result.lowestSphereHit = -1.f;
	}
}

bool CharacterMovement::DoMovementStep(
	Float3& center, Float3& forward, float& distance,
	Float3& velocity,
	const CubeAA& bounds,
	Result& movementResult)
{
	//---| Static Check |---
	const StaticCheckResult result{
		DoStaticCheck(center, forward,
			bounds)
	};

	switch (result.Type)
	{
	case StaticCheckResult::Type::CoreHit:

#ifdef MY_PRINT
		Logger::Warning("CoreHit");
#endif
		//Todo: MoveOut
		break;
	case StaticCheckResult::Type::MarginHit:

		Float3 hitDir{
			result.HitPoint - center };
		hitDir.Normalize();

#ifdef MY_PRINT
		Logger::Print("MarginHit");
		Logger::Print("HitDir", hitDir);
#endif
		FindBounceDir(hitDir, forward, velocity, movementResult);

#ifdef MY_PRINT
		Logger::Print("Bounce", forward);
#endif
		return true;
	}

#ifdef MY_PRINT
	Logger::Print("NoStaticCollision");
#endif

	//---| Move Check |---
	return DoMoveCheck(
		center, forward, distance, velocity, bounds,
		movementResult);
}

CharacterMovement::StaticCheckResult CharacterMovement::DoStaticCheck(
	const Float3& center, const Float3& forward,
	const CubeAA& globalBounds)
{
	//Sources
	const List<ModelCollidable>& models{ SYSTEMS.Collisions.Models.Models };

	//Local Variables
	StaticCheckResult result{};
	result.DistanceSq = nextafterf(FULL_RADIUS_SQ, Float::MAX);
	unsigned iModelHit{};
	unsigned iInstHit{};

	for (unsigned iModel{ 0 }; iModel < models.GetSize(); ++iModel)
	{
		const ModelCollidable& model{ models[iModel] };
		for (unsigned iInst{ 0 }; iInst < model.Instances.GetSize(); ++iInst)
		{
			const ModelCollidable::Instance& instance{ model.Instances[iInst] };

			if (!globalBounds.Overlaps(instance.WorldBounds))
				continue;

			const Float3 localCenter{ WorldMatrix::TransformedPoint(instance.WorldInverse, center) };
			const Float3 localForward{ WorldMatrix::RotatedPoint(instance.WorldInverse, forward) };

			if (CharacterMovement::DoStaticCheck(
				localCenter, localForward,
				CORE_RADIUS_SQ, FULL_RADIUS_SQ,
				{ model.Points },
				result))
			{
				iModelHit = iModel;
				iInstHit = iInst;
			}
		}
	}

	if (result.DistanceSq > FULL_RADIUS_SQ)
	{
		result.Type = StaticCheckResult::Type::NoHit;
	}
	else
	{
		if (result.DistanceSq > CORE_RADIUS_SQ)
			result.Type = StaticCheckResult::Type::MarginHit;
		else
			result.Type = StaticCheckResult::Type::CoreHit;

		WorldMatrix::TransformPoint(
			models[iModelHit].Instances[iInstHit].World,
			result.HitPoint);
	}

	return result;
}

bool CharacterMovement::DoStaticCheck(
	//Character
	const Float3& center, const Float3& direction,
	float coreRadiusSq, float fullRadiusSq,
	//Object
	PtrRangeConst<Float3> points,
	//Result
	StaticCheckResult& result)
{
	//
	const float initDistSq{ result.DistanceSq };

	//Loop
	for (unsigned iPoint{ 2 }, iTriangle{ 0 };
		iPoint < points.count;
		iPoint += 3, iTriangle++)
	{
		//Get triangle
		const Float3* pTriangle{ &points[iPoint - 2] };

		DoStaticCheck(
			center, direction,
			coreRadiusSq, fullRadiusSq,
			pTriangle,
			result);
	}
	return result.DistanceSq < initDistSq;
}

void CharacterMovement::DoStaticCheck(
	const Float3& center, const Float3& direction,
	float coreRadiusSq, float fullRadiusSq,
	const Float3* pTriangle,
	StaticCheckResult& result)
{
	//Get closest point to triangle
	const Float3 triangleClosest{
		PointTriangleCollision::Closest(
			center, pTriangle) };

	//Assert result
#ifdef MY_ASSERT
	if (triangleClosest.HasNan())
		Logger::Warning("[CharacterMovement::DoStaticCheck] closest has nan");
#endif

	//Get squaed distance to closest
	const float triangleDistSq{ triangleClosest.DistanceSq(center) };

	//Compare
	if (triangleDistSq >= result.DistanceSq)
		return;

	/*
		If hit is outside coreRadius,
			only detect if character isn't running away.
		Maybe replace 0 by a variable (cos of an angle).
	*/
	if (triangleDistSq > coreRadiusSq)
		if (direction.Dot(triangleClosest - center) < 0)
			return;

	//Hit! Triangle is closest (for now)
	result.DistanceSq = triangleDistSq;
	result.HitPoint = triangleClosest;
}

bool CharacterMovement::DoMoveCheck(
	Float3& center, Float3& forward, float& distance,
	Float3& velocity,
	const CubeAA& bounds,
	Result& movementResult)
{
	//Sources
	const List<ModelCollidable>& models{ SYSTEMS.Collisions.Models.Models };

	//Local Variables
	MoveCheckResult result{};
	result.T = nextafterf(distance, Float::MAX);
	Sphere localSphere{ {}, FULL_RADIUS };
	unsigned iModelHit{};
	unsigned iInstHit{};
	const float initT{ result.T };

	//Loop
	for (unsigned iModel{ 0 }; iModel < models.GetSize(); ++iModel)
	{
		const ModelCollidable& model{ models[iModel] };
		for (unsigned iInst{ 0 }; iInst < model.Instances.GetSize(); ++iInst)
		{
			const ModelCollidable::Instance& instance{ model.Instances[iInst] };

			if (!bounds.Overlaps(instance.WorldBounds))
				continue;

			localSphere.SetCenter(WorldMatrix::TransformedPoint(instance.WorldInverse, center));
			const Float3 localForward{ WorldMatrix::RotatedPoint(instance.WorldInverse, forward) };

			if (SphereTriangleCollision::ContinueDetecting(
				localSphere, localForward,
				{ model.Points }, { model.TriangleNormals },
				result))
			{
				iModelHit = iModel;
				iInstHit = iInst;
			}
		}
	}

	if (result.T < initT)
	{
#ifdef MY_PRINT
		if (result.T < 0)
			Logger::Warning("T negative", result.T);
		else if (result.T > 0)
			Logger::Print("T Positive", result.T);
		else
			Logger::Print("T Zero", result.T);
#endif

		center += forward * result.T;
		distance -= result.T;

#ifdef MY_PRINT
		Logger::Print("NewPos", center);
#endif

		if (distance > 0)
		{
			WorldMatrix::RotatePoint(models[iModelHit].Instances[iInstHit].World, result.HitDirection);
			result.HitDirection.Normalize();

			FindBounceDir(result.HitDirection, forward, velocity, movementResult);

#ifdef MY_PRINT
			Logger::Print("HitDir", result.HitDirection);
			Logger::Print("NewDir", forward);
#endif
			return true;
		}
		else
		{
			//#ifdef MY_PRINT
			Logger::Print("New Distance is neg.", distance);
			Logger::Print("Neg. Distance returns false");
			//#endif
			return false;
		}
	}
	else
	{
		center += forward * distance;

#ifdef MY_PRINT
		Logger::Print("No Hit");
		Logger::Print("NewPos", center);
		Logger::Print("Not Hit returns false");
#endif
		return false;
	}
}

void CharacterMovement::FindBounceDir(
	const Float3& hitDir, Float3& forward,
	Float3& velocity,
	Result& result)
{
#ifdef MY_ASSERT
	if (abs(forward.Length() - 1) > .01f)
		Logger::Warning("Forward not normalized");
	if (abs(hitDir.Length() - 1) > .01f)
		Logger::Warning("HitDit not normalized");
#endif

	result.lowestSphereHit = Float::Min(
		result.lowestSphereHit,
		hitDir.y);

	forward -= hitDir * forward.Dot(hitDir);
	velocity -= hitDir * velocity.Dot(hitDir);
	if (forward.LengthSq() == 0)
	{
		forward = -hitDir;
		return;
	}
	forward.Normalize();

	forward *= BOUNCE_COS;
	forward -= hitDir * BOUNCE_SIN;
}

void CharacterMovement::StaticCheckResult::SetType(
	float coreRadiusSq, float fullRadiusSq)
{
	if (DistanceSq > fullRadiusSq)
		Type = Type::NoHit;
	else if (DistanceSq > coreRadiusSq)
		Type = Type::MarginHit;
	else
		Type = Type::CoreHit;
}

#undef MY_ASSERT
#undef MY_PRINT