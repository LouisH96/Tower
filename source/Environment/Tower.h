#pragma once
#include "Rendering/Structs/VertexTypes.h"
#include <Renderer\TerrainRenderer.h>

struct MeshCollidable;

namespace MyEngine
{
	namespace Rendering
	{
		class Mesh;
	}
}

class Tower
	: public TowerGame::TerrainRenderComponent
{
public:
	explicit Tower(const Float3& position, const Float2& roofSize, float height);
	~Tower() = default;

	void LinkGameplay();

private:
	List<Rendering::V_PosNorCol> m_Vertices;
	Float3 m_Position;
	Float2 m_RoofSize;
	float m_Height;

	void Generate(MeshCollidable& collidable,
		const Float3& position, const Float2& roofSize,
		float towerHeight);

	void AddPlane(
		MeshCollidable& collidable,
		const Float3& right, const Float3& up, const Float3& rightUp,
		const Float3& origin,
		const Float3& color,
		List<Rendering::V_PosNorCol>& vertices, int& verticesIdx,
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
		List<Rendering::V_PosNorCol>& vertices, int& verticesIdx,
		int& indicesIdx, int& triangleNormalIdx);

	// Inherited via TerrainRenderComponent
	void GetDrawData(MeshData<Vertex, TOPOLOGY>& meshData) const override;
};

