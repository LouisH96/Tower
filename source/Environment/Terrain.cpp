#include "pch.h"
#include "Terrain.h"

#include <Rendering/State/Mesh.h>
#include <Rendering/Structs/VertexTypes.h>
#include "HeightMap.h"
#include "../TowerAppRenderer.h"
#include "../Services/TowerAppServices.h"


Terrain::Terrain(TowerAppServices& services, const Math::Float3& position, const Math::Float2& size)
	: m_Size{ size }
{
	FromHeightMap(services, position);
	services.Renderer.GetTerrainRenderer().AddEntry(*m_pMesh);
}

Terrain::~Terrain()
{
	delete m_pMesh;
}

void Terrain::FromHeightMap(TowerAppServices& services, const Math::Float3& position)
{
	MeshCollidable& collidable{ services.Collision.Terrain };
	Array<Rendering::V_PosNorCol> vertices{ 0 };
	HeightMap heightMap{ {300,300}, 0, m_Size };

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

	heightMap.ToVertices(vertices, collidable.TriangleNormals, collidable.Indices, position);
	m_pMesh = Rendering::Mesh::Create(vertices, collidable.Indices);

	collidable.Points = { vertices.GetSize() };
	for (int i = 0; i < collidable.Points.GetSize(); i++)
		collidable.Points[i] = vertices[i].Pos;
}
