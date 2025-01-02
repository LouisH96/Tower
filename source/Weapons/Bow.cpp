
#include "pch.h"
#include "Bow.h"

#include <App/Win32/Mouse.h>
#include <Framework/Resources.h>
#include <Framework/Globals.h>
#include <Io/Fbx/FbxClass.h>
#include <Math/Quaternion.h>
#include <Rendering/Renderers/R_LambertCam_Tex_Transform.h>
#include <Rendering/State/Mesh.h>
#include <Rendering/State/Texture.h>
#include <Rendering/Structs/VertexTypes.h>
#include <Services/GameplaySystems.h>
#include <Services/RenderSystems.h>

#include "ArrowSystem.h"

using namespace TowerGame;
using namespace Animations;

Bow::Bow()
{
	using namespace Rendering;
	using namespace DirectX;

	m_LocalTransform.Position = Float3{ .5f, -.1f, 1.0f };
	m_LocalTransform.Rotation = Quaternion::FromAxis({ 1,0,0 }, -25 * Constants::TO_RAD);

	//BOW-MESH
	const std::wstring meshPath{ Resources::Local(L"Rigged_Bow_Testing.fbx") };
	Io::Fbx::FbxClass fbxModel{ meshPath, .01f };
	Io::Fbx::FbxClass::Geometry& geom = fbxModel.GetGeometries()[0];

	Array<Vertex> vertices{ geom.Points.GetSize() };
	for (unsigned i = 0; i < geom.Points.GetSize(); i++)
	{
		Vertex& vertex{ vertices[i] };
		vertex.Pos = geom.Points[i];
		vertex.Normal = geom.Normals[i];
		vertex.Uv = geom.Uvs[i];

		const Io::Fbx::FbxClass::BlendData& blendData{ geom.Weights[i] };
		vertex.BoneIds = blendData.IndicesAsInt4();
		vertex.BoneWeights = blendData.WeightsAsFloat4();
	}

	MeshData<Vertex, TOPOLOGY> meshData{ };
	meshData.Vertices.Add(vertices);
	m_Buffers.CreateBuffers(meshData);

	//TEXTURE
	const std::wstring texturePath{ Resources::Local(L"Texture_01.png") };
	m_Texture = Texture{ texturePath };

	//ANIMATION
	m_Animation = Animation{ fbxModel, fbxModel.GetAnimations().First() };
	m_Animator = { m_Animation };
}

void Bow::Update(const Transform& cameraTransform)
{
	if (Globals::pMouse->GetScroll() != 0)
		m_LocalTransform.Rotation.RotateBy({ 1,0,0 }, Globals::pMouse->GetScroll() * 3 * Constants::TO_RAD);

	m_WorldTransform = Transform::LocalToWorld(m_LocalTransform, cameraTransform);

	if (Globals::pMouse->IsLeftBtnPressed())
	{
		m_ArrowIdx = GameplaySystems::GetArrowSystem().Spawn();
	}
	else if (Globals::pMouse->IsLeftBtnReleased())
	{
		GameplaySystems::GetArrowSystem().Launch(m_ArrowIdx);
		m_ArrowIdx = -1;
	}

	if (m_ArrowIdx >= 0)
	{
		GameplaySystems::GetArrowSystem().SetArrowTransform(m_ArrowIdx, m_WorldTransform);
	}

	m_Animator.ProgressTime(m_Animation);
}

void Bow::Render()
{
	m_Texture.Activate();
	m_Buffers.ActivateAndDraw();
}

void Bow::LinkRenderers()
{
	//RenderSystems::GetTransformRenderer().AddEntry(*m_pBowMesh, *m_pTexture, m_WorldTransform);
}
