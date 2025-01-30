#pragma once

#include <Geometry\ModelTopology.h>
#include <Rendering\Mesh\MeshBuffers.h>
#include <Rendering\Mesh\MeshData.h>

namespace TowerGame
{
class StaticMeshSystem
{
public:
	static constexpr ModelTopology TOPOLOGY{ ModelTopology::TriangleListIdx };
	using Vertex = Rendering::V_PosNorCol;
	using MeshData = MeshData<Vertex, TOPOLOGY>;

	StaticMeshSystem() = default;

	void Init(MeshData& data);
	void Render();

private:
	MeshBuffers<Vertex, TOPOLOGY> m_MeshBuffers;
};
}