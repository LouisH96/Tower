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
	explicit Tower(const Framework::CoreServices& services);
	~Tower();

	void Register(const TowerAppRenderer& renderer) const;

	bool IsColliding(const Math::Float3& begin, const Math::Float3& end) const;
	bool IsColliding(const Math::Float3& begin, const Math::Float3& end, Physics::CollisionDetection::Collision& collision) const;

private:
	Rendering::Mesh* m_pMesh{};
	Array<Math::Float3> m_Points;
	Array<Math::Float3> m_TriangleNormals;
	Array<int> m_Indices;

	void Generate(const Rendering::Gpu& gpu);

	void AddPlane(
		const Math::Float3& right, const Math::Float3& up,
		const Math::Float3& origin,
		const Math::Float3& color,
		Array<Rendering::V_PosNorCol>& vertices, int& verticesIdx,
		int& indicesIdx,
		int& triangleNormalIdx);
};

