#pragma once
#include "Physics/CollisionDetection.h"
#include "Rendering/Structs/VertexTypes.h"

class TowerAppRenderer;

namespace MyEngine
{
	namespace Framework
	{
		class CoreServices;
	}
}

namespace MyEngine
{
	namespace Rendering
	{
		class Mesh;
	}
}

class Tower
{
public:
	explicit Tower(
		const Framework::CoreServices& services, const TowerAppRenderer& renderer,
		const Math::Float3& position, const Math::Float2& roofSize, float height);
	~Tower();

	bool IsColliding(const Math::Float3& begin, const Math::Float3& end) const;
	bool IsColliding(const Math::Float3& begin, const Math::Float3& end, Physics::CollisionDetection::Collision& collision) const;

private:
	Rendering::Mesh* m_pMesh{};
	Array<Math::Float3> m_Points;
	Array<Math::Float3> m_TriangleNormals;
	Array<int> m_Indices;

	void Generate(const Rendering::Gpu& gpu, 
		const Math::Float3& position, const Math::Float2& roofSize,
		float towerHeight);

	void AddPlane(
		const Math::Float3& right, const Math::Float3& up, const Math::Float3& rightUp,
		const Math::Float3& origin,
		const Math::Float3& color,
		Array<Rendering::V_PosNorCol>& vertices, int& verticesIdx,
		int& indicesIdx,
		int& triangleNormalIdx);

	/**
	 * \param width normalized
	 * \param depth normalized
	 */
	void AddRamp(
		const Math::Float3& width, const Math::Float3& depth,
		float rampWidth, float rampHeight, float rampDepth,
		const Math::Float3& position,
		const Math::Float3& color,
		Array<Rendering::V_PosNorCol>& vertices, int& verticesIdx,
		int& indicesIdx, int& triangleNormalIdx);
};

