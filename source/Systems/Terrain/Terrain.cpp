#include "pch.h"
#include "Terrain.h"

#include <Rendering/State/Mesh.h>
#include <Rendering/Structs/VertexTypes.h>

using namespace TowerGame;

float Terrain::GetHeight(Float2 point) const
{
	point -= m_Position.Xz();
	return m_HeightMap.GetHeight(point);
}

void Terrain::GenerateMeshIn(StaticMeshSystem::MeshData& data)
{
	StaticMeshSystem::MeshData terrainMesh{};
	m_HeightMap.ToVertices(terrainMesh.Vertices, terrainMesh.Indices, m_Position);
	data.StartShape();
	data.Add(terrainMesh);
}

void Terrain::Init(const Float3& origin, const Float2& posSize, const Float2& negSize)
{
	m_Position = origin - Float3::FromXz(negSize);
	m_HeightMap = HeightMap{ 0, 1.1f, (posSize + negSize), negSize };
	FillHeightMap();
}

void Terrain::FillHeightMap()
{
	m_HeightMap.AddSinWaveX(40, 1.2f);
	m_HeightMap.AddSinWaveX(100, 1);
	m_HeightMap.AddSinWaveX(50, 1);

	m_HeightMap.AddCubeWaveX(50, 3);
	m_HeightMap.AddCubeWaveX(30, 1);

	m_HeightMap.AddSinWaveY(60, .25);
	m_HeightMap.AddSinWaveY(120, .9f);
	m_HeightMap.AddSinWaveY(30, .55f);
	m_HeightMap.AddSinWaveY(50, 1.75f);

	m_HeightMap.AddCubeWaveY(40, 3);

	m_HeightMap.Scale(.6f);

	m_HeightMap.SinDisplaceAlongX(40, 2);
	m_HeightMap.SinDisplaceAlongY(70, 3);
}