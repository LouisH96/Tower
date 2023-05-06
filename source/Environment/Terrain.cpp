#include "pch.h"
#include "Terrain.h"

#include <Framework/Resources.h>
#include <Rendering/Structs/VertexTypes.h>
#include <Io/Fbx/FbxClass.h>
#include <Rendering/State/Mesh.h>
#include <Rendering/State/Texture.h>

#include "../TowerAppRenderer.h"
#include <Rendering/Renderers/R_LambertLight_Tex.h>
#include "HeightMap.h"
#include "Physics/CollisionDetection.h"

Terrain::Terrain(Rendering::Gpu& gpu)
{
	FromHeightMap(gpu);
}

Terrain::~Terrain()
{
	delete m_pMesh;
}

void Terrain::Register(const TowerAppRenderer& renderer) const
{
	renderer.GetTerrainRenderer().AddEntry(*m_pMesh);
}

bool Terrain::IsColliding(const Math::Float3& begin, const Math::Float3& end) const
{
	Physics::CollisionDetection::Collision collision;
	return Physics::CollisionDetection::Detect(begin, end, m_Points, m_TriangleNormals, m_Indices, collision);
}

void Terrain::FromHeightMap(const Rendering::Gpu& gpu)
{
	Array<Rendering::V_PosNorCol> vertices{ 0 };
	HeightMap heightMap{ {30,30}, 0, {30,30}};
	heightMap.ApplyWave(5, .1);
	heightMap.ApplyWaveX(10, .15);
	heightMap.ApplyWaveY(7, .15);
	heightMap.ToVertices(vertices, m_TriangleNormals, m_Indices);
	m_pMesh = Rendering::Mesh::Create(gpu, vertices, m_Indices);

	m_Points = { vertices.GetSize() };
	for (int i = 0; i < m_Points.GetSize(); i++)
		m_Points[i] = vertices[i].Pos;
}
