#include "pch.h"
#include "ArrowRenderer.h"

#include "Framework/Resources.h"
#include "Game/Camera/Camera.h"
#include "Generation/PlaneGeneration.h"
#include "Io/Fbx/FbxClass.h"

using namespace Rendering;

ArrowRenderer::ArrowRenderer()
	: m_InputLayout{ InputLayout::FromType<Instance>() }
	, m_Shader{ Resources::GlobalShader(L"LambertCam_Tex_Tran_Inst.hlsl") }
	, m_Texture{ Resources::Local(L"Texture_01.png") }
{
	//VERTICES
	const std::wstring arrowMeshPath{ Resources::Local(L"SM_Arrow_01.fbx") };
	Io::Fbx::FbxClass arrowFbxModel{ arrowMeshPath };
	Io::Fbx::FbxClass::Geometry& arrowGeom = arrowFbxModel.GetGeometries()[0];
	Array<V_PosNorUv> arrowVertices{ arrowGeom.Points.GetSize() };
	for (int i = 0; i < arrowGeom.Points.GetSize(); i++)
		arrowVertices[i] = V_PosNorUv{ arrowGeom.Points[i] * 0.01f, arrowGeom.Normals[i], arrowGeom.Uvs[i] };

	m_Instances = InstanceList<Vertex, Instance>{
		arrowVertices.GetData(), arrowVertices.GetSizeU()
	};
}

void ArrowRenderer::SetSize(int size)
{
	m_Instances.EnsureCapacity(size);
	m_Instances.SetCount(size);
}

void ArrowRenderer::UpdateData(int idx, const Transform& transform, const Camera& camera)
{
	const Float4X4 model{ transform.AsMatrix() };
	const Float4X4 modelViewProj{ model * camera.GetViewProjection() };
	m_Instances[idx] = Instance{
		model,
		modelViewProj };
}

void ArrowRenderer::Render(const Camera& camera)
{
	m_BlendState.Activate();
	m_RasterizerState.Activate();
	m_Sampler.Activate();
	m_DepthStencilState.Activate();
	m_InputLayout.Activate();
	m_Shader.Activate();
	m_Texture.Activate();
	m_CameraBuffer.Update(CB_CamPos{ camera.GetPosition() });
	m_CameraBuffer.Activate();
	m_Instances.Draw();
}
