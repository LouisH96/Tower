#pragma once
#include "HeightMap.h"
#include <Renderer\TerrainRenderer.h>

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
	: public TowerGame::TerrainRenderComponent
{
public:
	explicit Terrain(const Float3& position, const Float2& size);
	~Terrain() = default;
	Terrain(const Terrain& other) = delete;
	Terrain(Terrain&& other) noexcept = delete;
	Terrain& operator=(const Terrain& other) = delete;
	Terrain& operator=(Terrain&& other) noexcept = delete;

	float GetHeight(const Float2& point) const; //xz
	const Float2& GetSize() const { return m_HeightMap.GetSize(); }

private:
	HeightMap m_HeightMap;
	Float3 m_Position;

	static HeightMap MakeHeightMap(const Float2& worldSize, const Int2& nrPoints);

	// Inherited via TerrainRenderComponent
	void GetDrawData(MeshData<Vertex, TOPOLOGY>& meshData) const override;
};
}
