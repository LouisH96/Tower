#include "pch.h"
#include "Terrain.h"

#include <Framework/Resources.h>
#include <Rendering/Structs/VertexTypes.h>
#include <Io/Fbx/FbxClass.h>
#include <Rendering/State/Mesh.h>
#include <Rendering/State/Texture.h>

#include "../TowerAppRenderer.h"
#include <Rendering/Renderers/R_LambertLight_Tex.h>

Terrain::Terrain(Rendering::Gpu& gpu)
{
	//BOW-MESH
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

Terrain::~Terrain()
{
	delete m_pTexture;
	delete m_pMesh;
}

void Terrain::Register(const TowerAppRenderer& renderer) const
{
	renderer.GetTerrainRenderer().AddEntry(*m_pMesh, *m_pTexture);
}
