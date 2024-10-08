#include "pch.h"
#include "Terrain.h"

#include <Environment/HeightMap.h>
#include <Rendering/State/Mesh.h>
#include <Rendering/Structs/VertexTypes.h>
#include <Services/RenderSystems.h>

float Terrain::GetHeight(const Float2& point) const
{
	return m_HeightMap.GetHeight(point);
}

Terrain::Terrain(const Float3& position, const Float2& size)
	: m_HeightMap{ MakeHeightMap(size, {300,300}) }
	, m_Position{ position }
{
}

HeightMap Terrain::MakeHeightMap(const Float2& worldSize, const Int2& nrPoints)
{
	HeightMap heightMap{ nrPoints, 0, worldSize };

	heightMap.AddSinWaveX(40, 1.2f);
	heightMap.AddSinWaveX(100, 1);
	heightMap.AddSinWaveX(50, 1);

	heightMap.AddCubeWaveX(50, 3);
	heightMap.AddCubeWaveX(30, 1);

	heightMap.AddSinWaveY(60, .25);
	heightMap.AddSinWaveY(120, .9f);
	heightMap.AddSinWaveY(30, .55f);
	heightMap.AddSinWaveY(50, 1.75f);

	heightMap.AddCubeWaveY(40, 3);

	heightMap.Scale(.6f);

	heightMap.SinDisplaceAlongX(40, 2);
	heightMap.SinDisplaceAlongY(70, 3);

	return heightMap;
}

void Terrain::GetDrawData(MeshData<Vertex, TOPOLOGY>& meshData) const
{
	m_HeightMap.ToVertices(meshData.Vertices, meshData.Indices, m_Position);
}
