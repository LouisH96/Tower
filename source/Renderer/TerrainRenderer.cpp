#include "pch.h"
#include "TerrainRenderer.h"

using namespace TowerGame;
using namespace Rendering;

TerrainRenderer::TerrainRenderer()
	: m_Shader{ Resources::Local(L"Terrain.hlsl") }
{
}

void TerrainRenderer::Start(PtrRangeConst<TerrainRenderComponent*> components)
{
	MeshData<Vertex, TOPOLOGY> allData{};
	MeshData<Vertex, TOPOLOGY> componentData{};

	for (unsigned iComp{ 0 }; iComp < components.count; ++iComp)
	{
		componentData.Clear();
		components[iComp]->GetDrawData(componentData);

		allData.StartShape();
		allData.Add(componentData);
	}

	m_MeshBuffers.CreateBuffers(allData);
}

void TerrainRenderer::Render_Internal()
{
	m_MeshBuffers.ActivateAndDraw();
}
