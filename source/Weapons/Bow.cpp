#include "pch.h"
#include "Bow.h"

#include <App/Win32/Mouse.h>s
#include <Framework/Resources.h>
#include <Game/Globals.h>
#include <Io/Fbx/FbxClass.h>
#include <Math/Quaternion.h>
#include <Rendering/State/Mesh.h>
#include <Rendering/State/Texture.h>
#include <Rendering/Structs/VertexTypes.h>
#include "../TowerAppRenderer.h"
#include "../Environment/Tower.h"
#include "../Services/TowerAppServices.h"

Bow::Bow(Rendering::Gpu& gpu)
{
	using namespace Rendering;
	using namespace DirectX;
	using namespace Math;

	m_LocalTransform.Position = Float3{ .5f, -.1f, 1.0f };
	m_LocalTransform.Rotation = Quaternion::FromAxis({ 1,0,0 }, -25 * Constants::TO_RAD);

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
	for (int i = 0; i < m_ArrowData.size(); i++)
		delete m_ArrowData[i].pTransform;
	delete m_pArrowMesh;
	delete m_pBowMesh;
	delete m_pTexture;
}

void Bow::Update(const TowerAppServices& services, const Game::Transform& cameraTransform)
{
	if (Globals::pMouse->GetScroll() != 0)
		m_LocalTransform.Rotation.RotateBy({ 1,0,0 }, Globals::pMouse->GetScroll() * 3 * Math::Constants::TO_RAD);

	m_WorldTransform = cameraTransform.MakeChildTransform(m_LocalTransform);

	if (Globals::pMouse->IsLeftBtnReleased())
	{
		m_ArrowData.push_back(ArrowData{
			new Game::Transform(m_WorldTransform), m_WorldTransform.Rotation.GetForward() * 10
			});
		const ArrowData& bowData{ m_ArrowData[m_ArrowData.size() - 1] };
		m_pRenderer->AddEntry(*m_pArrowMesh, *m_pTexture, *bowData.pTransform);
	}

	for (int i = 0; i < m_ArrowData.size(); i++)
		UpdateArrow(services, m_ArrowData[i]);
}

void Bow::Register(Rendering::R_LambertCam_Tex_Transform& renderer)
{
	m_pRenderer = &renderer;
	renderer.AddEntry(*m_pBowMesh, *m_pTexture, m_WorldTransform);
}

void Bow::Register(const Terrain& terrain)
{
	m_pTerrain = &terrain;
}

void Bow::Register(const Tower& tower)
{
	m_pTower = &tower;
}

void Bow::UpdateArrow(const TowerAppServices& services, ArrowData& arrowData) const
{
	constexpr float maxSpeed = .1;
	constexpr float gravity = -9.81;

	if (arrowData.pTransform->Position.y <= -1) return;
	if (arrowData.Velocity.x == 2000) return;

	const Math::Float3 oldPos{ arrowData.pTransform->Position };
	arrowData.Velocity.y += gravity * Globals::DeltaTime;
	arrowData.pTransform->Position += arrowData.Velocity * Globals::DeltaTime;
	arrowData.pTransform->Rotation = Math::Quaternion::FromForward(arrowData.Velocity.Normalized());

	if (services.Collision.Tower.IsColliding(oldPos, arrowData.pTransform->Position) 
		|| services.Collision.Terrain.IsColliding(oldPos, arrowData.pTransform->Position))
		arrowData.Velocity.x = 2000;
}
