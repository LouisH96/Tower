#pragma once

#include <Rendering\Mesh\MeshBuffers.h>
#include <Rendering\Mesh\MeshData.h>

namespace TowerGame
{
class TerrainRenderer
{
public:
	static constexpr ModelTopology TOPOLOGY{ ModelTopology::TriangleListIdx };
	using Vertex = Rendering::V_PosNorCol;
	using MeshData = MeshData<Vertex, TOPOLOGY>;

	TerrainRenderer() = default;

	void Init(MeshData& data);
	void Render();

private:
	MeshBuffers<Vertex, TOPOLOGY> m_MeshBuffers;
};
}

