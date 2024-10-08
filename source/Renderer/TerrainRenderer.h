#pragma once

#include <Rendering\Mesh\MeshData.h>
#include <Rendering\Mesh\MeshBuffers.h>
#include <Rendering\State\Shader.h>

namespace TowerGame
{
class TerrainRenderComponent
{
public:
	static constexpr ModelTopology TOPOLOGY{ ModelTopology::TriangleListIdx };
	using Vertex = Rendering::V_PosNorCol;

	virtual void GetDrawData(MeshData<Vertex, TOPOLOGY>& meshData) const = 0;
};

class TerrainRenderer
{
public:
	static constexpr ModelTopology TOPOLOGY{ ModelTopology::TriangleListIdx };
	using Vertex = Rendering::V_PosNorCol;

	TerrainRenderer();

	void Start(PtrRangeConst<TerrainRenderComponent*> components);

	template<Rendering::Shader::Function::Flag F = Rendering::Shader::Function::Both, bool unsetOthers = true>
	void Render();

private:
	MeshBuffers<Vertex, TOPOLOGY> m_MeshBuffers;
	Rendering::Shader m_Shader;

	void Render_Internal();
};

template<Rendering::Shader::Function::Flag F, bool UnsetOthers>
inline void TerrainRenderer::Render()
{
	m_Shader.Activate<F, UnsetOthers>();
	Render_Internal();
}
}

