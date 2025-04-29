#pragma once
#include <Geometry\Shapes\Sphere.h>
#include <Physics\Collisions\SphereTriangleCollision.h>

namespace TowerGame
{

class CharacterMovement
{
public:
	static constexpr float FULL_RADIUS{ .6f };

	static void DoMovement(
		Float3& center,
		Float3 direction, float distance,
		Float3& velocity);

private:
	using MoveCheckResult = MyEngine::Physics::SphereTriangleCollision::MovingSphereCollision;
	static constexpr float MARGIN_RADIUS{ .3f };
	static constexpr float FULL_RADIUS_SQ{ FULL_RADIUS * FULL_RADIUS };
	static constexpr float CORE_RADIUS_SQ{
		(FULL_RADIUS - MARGIN_RADIUS)
		* (FULL_RADIUS - MARGIN_RADIUS) };

	static constexpr float BOUNCE_ANGLE{ 5.f * Constants::TO_RAD };
	static const float BOUNCE_COS;
	static const float BOUNCE_SIN;

	//---| Step |---
	static bool DoMovementStep(
		Float3& center, Float3& forward, float& distance,
		Float3& velocity,
		const CubeAA& bounds);

	//---| Static Check |---
	/*
	* Data local to Object
	*/
	struct StaticCheckResult {
		enum class Type {
			NoHit, CoreHit, MarginHit
		};

		Float3 HitPoint;
		float DistanceSq;
		Type Type;

		void SetType(float coreRadiusSq, float fullRadiusSq);
	};
	/*
		Forward doesn't have to be normalized
	*/
	static StaticCheckResult DoStaticCheck(
		const Float3& center, const Float3& forward,
		const CubeAA& globalBounds);
	/*
	* Result should be an already active struct.
	* Direction doesn't have to be normalized.
	* Result.Type will not be set.
	*/
	static bool DoStaticCheck(
		//Character
		const Float3& center, const Float3& direction,
		float coreRadiusSq, float fullRadiusSq,
		//Object
		PtrRangeConst<Float3> points,
		//Result
		StaticCheckResult& result);
	static void DoStaticCheck(
		//Character
		const Float3& center, const Float3& direction,
		float coreRadiusSq, float fullRadiusSq,
		//Object
		const Float3* pTriangle,
		//Result
		StaticCheckResult& result);

	//---| Move Check |---
	static bool DoMoveCheck(
		Float3& center, Float3& forward, float& distance,
		Float3& velocity,
		const CubeAA& bounds);
	 
	//---| Other |---
	static void FindBounceDir(
		const Float3& hitDir, Float3& forward,
		Float3& velocity);
};
}