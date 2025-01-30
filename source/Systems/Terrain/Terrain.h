#pragma once
#include "HeightMap.h"
#include <Systems\Terrain\TerrainRenderer.h>

namespace MyEngine
{
namespace Rendering
{
class Mesh;
}
}

namespace TowerGame
{
class Terrain
{
public:
	Terrain() = default;
	~Terrain() = default;
	void Init(const Float3& position, const Float2& size);

	Terrain(const Terrain& other) = delete;
	Terrain(Terrain&& other) noexcept = delete;
	Terrain& operator=(const Terrain& other) = delete;
	Terrain& operator=(Terrain&& other) noexcept = delete;

	float GetHeight(const Float2& point) const; //xz
	const Float2& GetSize() const { return m_HeightMap.GetSize(); }

	void GenerateMeshIn(TerrainRenderer::MeshData& data);

private:
	HeightMap m_HeightMap;
	Float3 m_Position;

	static HeightMap MakeHeightMap(const Float2& worldSize, const Int2& nrPoints);
};
}
