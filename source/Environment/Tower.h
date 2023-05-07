#pragma once
#include "Physics/CollisionDetection.h"
#include "Rendering/Structs/VertexTypes.h"

struct MeshCollidable;
struct TowerAppServices;
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
		TowerAppServices& services,
		const Math::Float3& position, const Math::Float2& roofSize, float height);
	~Tower();

private:
	Rendering::Mesh* m_pMesh{};

	void Generate(const Rendering::Gpu& gpu, MeshCollidable& collidable,
		const Math::Float3& position, const Math::Float2& roofSize,
		float towerHeight);

	void AddPlane(
		MeshCollidable& collidable,
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
		MeshCollidable& collidable,
		const Math::Float3& width, const Math::Float3& depth,
		float rampWidth, float rampHeight, float rampDepth,
		const Math::Float3& position,
		const Math::Float3& color,
		Array<Rendering::V_PosNorCol>& vertices, int& verticesIdx,
		int& indicesIdx, int& triangleNormalIdx);
};

