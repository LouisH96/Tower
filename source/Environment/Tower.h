#pragma once
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
		const Float3& position, const Float2& roofSize, float height);
	~Tower();

private:
	Rendering::Mesh* m_pMesh{};

	void Generate(MeshCollidable& collidable,
		const Float3& position, const Float2& roofSize,
		float towerHeight);

	void AddPlane(
		MeshCollidable& collidable,
		const Float3& right, const Float3& up, const Float3& rightUp,
		const Float3& origin,
		const Float3& color,
		Array<Rendering::V_PosNorCol>& vertices, int& verticesIdx,
		int& indicesIdx,
		int& triangleNormalIdx);

	/**
	 * \param width normalized
	 * \param depth normalized
	 */
	void AddRamp(
		MeshCollidable& collidable,
		const Float3& width, const Float3& depth,
		float rampWidth, float rampHeight, float rampDepth,
		const Float3& position,
		const Float3& color,
		Array<Rendering::V_PosNorCol>& vertices, int& verticesIdx,
		int& indicesIdx, int& triangleNormalIdx);
};

