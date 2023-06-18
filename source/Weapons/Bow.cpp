#include "pch.h"
#include "Bow.h"

#include <App/Win32/Mouse.h>
#include <Framework/Resources.h>
#include <Game/Globals.h>
#include <Io/Fbx/FbxClass.h>
#include <Math/Quaternion.h>
#include <Rendering/State/Mesh.h>
#include <Rendering/State/Texture.h>
#include <Rendering/Structs/VertexTypes.h>
#include <Character/EnemySystem.h>
#include <Rendering/Renderers/R_LambertCam_Tex_Transform.h>
#include <Services/CollisionService.h>
#include <Services/GameplaySystems.h>
#include <Services/RenderSystems.h>

Bow::Bow()
{
	using namespace Rendering;
	using namespace DirectX;
	using namespace Math;

	m_LocalTransform.Position = Float3{ .5f, -.1f, 1.0f };
	m_LocalTransform.Rotation = Quaternion::FromAxis({ 1,0,0 }, -25 * Constants::TO_RAD);

	//BOW-MESH
	const std::wstring meshPath{ Resources::Local(L"Rigged_Bow_Testing.fbx") };
	Io::Fbx::FbxClass fbxModel{ meshPath };
	Io::Fbx::FbxClass::Geometry& geom = fbxModel.GetGeometries()[0];

	Array<V_PosNorUv> vertices{ geom.Points.GetSize() };
	for (int i = 0; i < geom.Points.GetSize(); i++)
		vertices[i] = V_PosNorUv{ geom.Points[i] * 0.01f, geom.Normals[i], geom.Uvs[i] };

	m_pBowMesh = Mesh::Create(vertices);

	//TEXTURE
	const std::wstring texturePath{ Resources::Local(L"Texture_01.png") };
	m_pTexture = new Texture(texturePath);
}

Bow::~Bow()
{
	delete m_pBowMesh;
	delete m_pTexture;
}

void Bow::Update(const Transform& cameraTransform)
{
	if (Globals::pMouse->GetScroll() != 0)
		m_LocalTransform.Rotation.RotateBy({ 1,0,0 }, Globals::pMouse->GetScroll() * 3 * Constants::TO_RAD);

	m_WorldTransform = Transform::LocalToWorld(m_LocalTransform, cameraTransform);

	if (Globals::pMouse->IsLeftBtnReleased())
	{
		m_ArrowData.push_back(ArrowData{
			Transform{m_WorldTransform}, m_WorldTransform.Rotation.GetForward() * 20
			});
	}

	RenderSystems::GetArrowRenderer().SetSize(m_ArrowData.size());
	for (int i = 0; i < m_ArrowData.size(); i++)
	{
		UpdateArrow(i, m_ArrowData[i]);
		RenderSystems::GetArrowRenderer().UpdateData(i, m_ArrowData[i].Transform);
	}
}

void Bow::LinkRenderers()
{
	RenderSystems::GetTransformRenderer().AddEntry(*m_pBowMesh, *m_pTexture, m_WorldTransform);
}

void Bow::SetArrowTransform(int idx, const Transform& transform)
{
	m_ArrowData[idx].Transform = transform;
}

void Bow::UpdateArrow(int arrowIdx, ArrowData& arrowData) const
{
	constexpr float gravity = -9.81f;

	if (arrowData.Transform.Position.y <= -1) return;
	if (arrowData.Velocity.x == 2000) return;

	const Math::Float3 oldPos{ arrowData.Transform.Position };
	arrowData.Velocity.y += gravity * Globals::DeltaTime;
	arrowData.Transform.Position += arrowData.Velocity * Globals::DeltaTime;
	arrowData.Transform.Rotation = Math::Quaternion::FromForward(arrowData.Velocity.Normalized());

	CollisionService& collisions{ GameplaySystems::GetCollisionService() };

	if (collisions.Tower.IsColliding(oldPos, arrowData.Transform.Position))
	{
		arrowData.Velocity.x = 2000;
		return;
	}
	Enemy* pHitEnemy{ collisions.Enemies.IsColliding(oldPos, arrowData.Transform.Position) };
	if (pHitEnemy)
	{
		GameplaySystems::GetEnemySystem().OnCollision(arrowData.Transform, arrowIdx, *pHitEnemy);
		arrowData.Velocity.x = 2000;
		return;
	}
	if (GameplaySystems::GetTerrain().GetHeight(arrowData.Transform.Position.Xz()) > arrowData.Transform.Position.y)
	{
		arrowData.Velocity.x = 2000;
		return;
	}
}
