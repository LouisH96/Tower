#include "pch.h"
#include "Terrain.h"

#include <Framework/Resources.h>
#include <Rendering/Structs/VertexTypes.h>
#include <Io/Fbx/FbxClass.h>
#include <Rendering/State/Mesh.h>
#include <Rendering/State/Texture.h>

#include "../TowerAppRenderer.h"
#include <Rendering/Renderers/R_LambertLight_Tex.h>
#include "../HeightMap.h"

Terrain::Terrain(Rendering::Gpu& gpu)
{
	//FromMesh(gpu);
	FromHeightMap(gpu);
}

Terrain::~Terrain()
{
	delete m_pTexture;
	delete m_pMesh;
}

void Terrain::Register(const TowerAppRenderer& renderer) const
{
	//renderer.GetTerrainRenderer().AddEntry(*m_pMesh, *m_pTexture);
	renderer.GetTerrainRenderer().AddEntry(*m_pMesh);
}

void Terrain::FromMesh(const Rendering::Gpu& gpu)
{
	//MESH
	const std::wstring meshPath{ Framework::Resources::GetLocalResourcePath(L"SM_Env_Ground_Farm_Flat_01.fbx") };
	Io::Fbx::FbxClass fbxModel{ meshPath };
	Io::Fbx::FbxClass::Geometry& geom = fbxModel.GetGeometries()[0];

	Array<Rendering::V_PosNorUv> vertices{ geom.Points.GetSize() };
	for (int i = 0; i < geom.Points.GetSize(); i++)
		vertices[i] = Rendering::V_PosNorUv{ (geom.Points[i] * 0.05f).Scaled({3,1,3}) + Math::Float3{0,-2,0}, geom.Normals[i], geom.Uvs[i] };

	m_pMesh = Rendering::Mesh::Create(gpu, vertices);

	//TEXTURE
	const std::wstring texturePath{ Framework::Resources::GetLocalResourcePath(L"PFK_Texture_Ground_Grass_01.png") };
	m_pTexture = new Rendering::Texture(gpu, texturePath);
}

void Terrain::FromHeightMap(const Rendering::Gpu& gpu)
{
	Array<Rendering::V_PosNorCol> vertices{ 0 };
	Array<int> indices{ 0 };
	HeightMap heightMap{ {30,30}, 0, {30,30}};
	heightMap.ApplyWave(5, .1);
	heightMap.ApplyWaveX(10, .15);
	heightMap.ApplyWaveY(7, .15);
	heightMap.ToVertices(vertices, indices);

	m_pMesh = Rendering::Mesh::Create(gpu, vertices, indices);
}
