#include "pch.h"
#include "Terrain.h"

#include <Rendering/Structs/VertexTypes.h>
#include <Rendering/State/Mesh.h>

#include "../TowerAppRenderer.h"
#include "HeightMap.h"
#include "../Services/TowerAppServices.h"
#include "Framework/CoreServices.h"


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
	HeightMap heightMap{ {30,30}, 0, m_Size };
	heightMap.ApplyWave(5, .2);
	heightMap.ApplyWaveX(10, 1.25);
	heightMap.ApplyWaveY(7, 1.05);
	heightMap.ApplyWaveY(30, .25);
	heightMap.ToVertices(vertices, collidable.TriangleNormals, collidable.Indices, position);
	m_pMesh = Rendering::Mesh::Create(vertices, collidable.Indices);

	collidable.Points = { vertices.GetSize() };
	for (int i = 0; i < collidable.Points.GetSize(); i++)
		collidable.Points[i] = vertices[i].Pos;
}
