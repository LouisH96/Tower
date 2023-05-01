#include "pch.h"
#include "Bow.h"

#include <Framework/Resources.h>
#include <Io/Fbx/FbxClass.h>
#include <Rendering/State/Mesh.h>
#include <Rendering/State/Texture.h>
#include <Rendering/Structs/VertexTypes.h>
#include "TowerAppRenderer.h"
#include <Game/Globals.h>
#include <App/Wrappers/Win32/Mouse.h>
#include <Math/Quaternion.h>

Bow::Bow(Rendering::Gpu& gpu)
{
	using namespace Rendering;
	using namespace DirectX;
	using namespace Math;

	m_LocalTransform.Position = Float3{ .5f, 0, 1.2f };

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
	for (int i = 0; i < m_BowData.size(); i++)
		delete m_BowData[i].pTransform;
	delete m_pArrowMesh;
	delete m_pBowMesh;
	delete m_pTexture;
}

void Bow::Update(const Game::Transform& cameraTransform, Rendering::R_LambertCam_Tex_Transform& renderer)
{
	using namespace DirectX;

	m_WorldTransform = cameraTransform.MakeChildTransform(m_LocalTransform);

	if (Globals::pMouse->IsLeftBtnReleased())
	{
		m_BowData.push_back(BowData{
			new Game::Transform(m_WorldTransform), m_WorldTransform.Rotation.GetForward() * 10
			});
		const BowData& bowData{ m_BowData[m_BowData.size() - 1] };
		renderer.AddEntry(*m_pArrowMesh, *m_pTexture, *bowData.pTransform);
	}

	constexpr float maxSpeed = .1;
	constexpr float gravity = -9.81;
	for (int i = 0; i < m_BowData.size(); i++)
	{
		m_BowData[i].Velocity.y += gravity * Globals::DeltaTime;
		m_BowData[i].pTransform->Position += m_BowData[i].Velocity * Globals::DeltaTime;
		m_BowData[i].pTransform->Rotation = Math::Quaternion::FromForward(m_BowData[i].Velocity.Normalized());
	}
}

void Bow::Register(Rendering::R_LambertCam_Tex_Transform& renderer)
{
	renderer.AddEntry(*m_pBowMesh, *m_pTexture, m_WorldTransform);
}
