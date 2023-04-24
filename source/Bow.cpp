#include "pch.h"
#include "Bow.h"

#include <Framework/Resources.h>
#include <Io/Fbx/FbxClass.h>
#include <Rendering/State/Mesh.h>
#include <Rendering/State/Texture.h>
#include <Rendering/Structs/VertexTypes.h>

#include "TowerAppRenderer.h"

Bow::Bow(Rendering::Gpu& gpu)
{
	using namespace Rendering;
	using namespace DirectX;

	m_World = XMMatrixTranslation(-.5, 0, 1.2);
	m_InverseTranspose = XMMatrixInverse(nullptr, XMMatrixTranspose(m_World));

	//BOW-MESH
	const std::wstring meshPath{ Framework::Resources::GetLocalResourcePath(L"Rigged_Bow_Testing.fbx") };
	Io::Fbx::FbxClass fbxModel{ meshPath };
	Io::Fbx::FbxClass::Geometry& geom = fbxModel.GetGeometries()[0];

	Array<V_PosNorUv> vertices{ geom.Points.GetSize() };
	for (int i = 0; i < geom.Points.GetSize(); i++)
		vertices[i] = V_PosNorUv{ geom.Points[i] * 0.01f, geom.Normals[i], geom.Uvs[i] };

	m_pBowMesh = Mesh::Create(gpu, vertices);

	//ARROW-MESH
	const std::wstring arrowMeshPath{ Framework::Resources::GetLocalResourcePath(L"SM_Arrow_01.fbx") };
	Io::Fbx::FbxClass arrowFbxModel{ arrowMeshPath };
	Io::Fbx::FbxClass::Geometry& arrowGeom = arrowFbxModel.GetGeometries()[0];

	Array<V_PosNorUv> arrowVertices{ arrowGeom.Points.GetSize() };
	for (int i = 0; i < arrowGeom.Points.GetSize(); i++)
		arrowVertices[i] = V_PosNorUv{ arrowGeom.Points[i] * 0.01f, arrowGeom.Normals[i], arrowGeom.Uvs[i] };

	m_pArrowMesh = Mesh::Create(gpu, arrowVertices);

	//TEXTURE
	const std::wstring texturePath{ Framework::Resources::GetLocalResourcePath(L"Texture_01.png") };
	m_pTexture = new Texture(gpu, texturePath);
}

Bow::~Bow()
{
	delete m_pArrowMesh;
	delete m_pBowMesh;
	delete m_pTexture;
}

void Bow::Render(TowerAppRenderer& renderer, const TowerAppRenderData& data)
{
	renderer.GetCameraConstantBuffer().Update(renderer.GetGpu(), Rendering::CB_CamMatPos{ data.CameraPosition, GetWorld() * data.CameraProjection });
	renderer.GetCameraConstantBuffer().Activate(renderer.GetGpu());

	renderer.GetModelConstantBuffer().Update(renderer.GetGpu(), Rendering::CB_ModelBuffer{ GetInverseTranspose() });
	renderer.GetModelConstantBuffer().Activate(renderer.GetGpu(), 1);

	m_pTexture->Activate(renderer.GetGpu());
	m_pBowMesh->Activate(renderer.GetGpu());
	m_pBowMesh->Draw(renderer.GetGpu());

	m_pArrowMesh->Activate(renderer.GetGpu());
	m_pArrowMesh->Draw(renderer.GetGpu());


	m_World = DirectX::XMMatrixTranslation(.5, 0, 1.2);
	m_InverseTranspose = XMMatrixInverse(nullptr, XMMatrixTranspose(m_World));
	renderer.GetCameraConstantBuffer().Update(renderer.GetGpu(), Rendering::CB_CamMatPos{ data.CameraPosition, GetWorld() * data.CameraProjection });
	renderer.GetModelConstantBuffer().Update(renderer.GetGpu(), Rendering::CB_ModelBuffer{ GetInverseTranspose() });
	m_pArrowMesh->Draw(renderer.GetGpu());
	m_World = DirectX::XMMatrixTranslation(-.5, 0, 1.2);
	m_InverseTranspose = XMMatrixInverse(nullptr, XMMatrixTranspose(m_World));
}
