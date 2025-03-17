#pragma once
#include <Rendering\Structs\VertexTypes.h>
#include <Systems\StaticMeshes\StaticMeshSystem.h>
#include <Systems\StaticMeshes\StaticModelSystem.h>

namespace MyEngine
{
	namespace Rendering
	{
		class Mesh;
	}
}

namespace TowerGame
{
struct MeshCollidable;

class Tower
{
public:
	struct Desc {
		Float3 Position;
		Float2 RoofSize;
		float Height;
	};

public:
	Tower() = default;
	void Init(Desc& desc);

	~Tower() = default;

	void GenerateMeshIn(
		StaticMeshSystem::MeshData& staticMeshData,
		MeshCollidable& collidable);

	void AddModelsIn(
		StaticModelSystem::InitData& staticModelData);

private:
	List<Rendering::V_PosNorCol> m_Vertices;
	Float3 m_Position;
	Float2 m_RoofSize;
	float m_Height{};

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
};
}
