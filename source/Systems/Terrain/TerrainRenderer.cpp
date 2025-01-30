#include "pch.h"
#include "TerrainRenderer.h"

using namespace TowerGame;
using namespace Rendering;

void TerrainRenderer::Init(MeshData& data)
{
	m_MeshBuffers.CreateBuffers(data);
}

void TerrainRenderer::Render()
{
	m_MeshBuffers.ActivateAndDraw();
}
