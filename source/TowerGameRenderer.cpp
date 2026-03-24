#include "pch.h"
#include "TowerGameRenderer.h"

#include <Rendering\Renderers\R_LambertCam_Tex_Tran_Inst.h>
#include <Rendering\Renderers\Texture2DRenderer.h>
#include <Rendering\State\RasterizerState.h>

#include <Systems\Arrows\ArrowSystem.h>
#include <Systems\Bow\Bow.h>
#include <Systems\Enemies\EnemySystem.h>
#include <TowerApp.h>

using namespace TowerGame;
using namespace Rendering;

TowerGameRenderer::TowerGameRenderer()
	: m_Shader_Object_Inst{ Resources::Local(L"S_Object_Inst.hlsl") }
	, m_Shader_Weapon{ Resources::Local(L"S_Weapon.hlsl") }
	, m_Shader_Terrain{ Resources::Local(L"S_Terrain.hlsl") }
	, m_Shader_Object{ Resources::Local(L"S_StaticMesh.hlsl") }
	, m_Shader_Tracer{ Resources::Local(L"S_Tracer.hlsl") }
	, m_Il_V_PosCol{ InputLayout::FromType<V_PosCol>() }
	, m_Il_V_PosNorUv_I_ModelMatrix{ InputLayout::FromTypes<V_PosNorUv, I_ModelMatrix>() }
	, m_Il_V_PosNorCol{ InputLayout::FromType<V_PosNorCol>() }
	, m_Il_V_PosNorUv{ InputLayout::FromType<V_PosNorUv>() }
	, m_Il_V_PosNorUvSkin{ InputLayout::FromType<V_PosNorUvSkin>() }
	, m_Il_V_PosColUv{ InputLayout::FromType<V_PosColUv>() }
	, m_Il_V_PosCol4Uv{ InputLayout::FromType<V_PosCol4Uv>() }
	, m_Culling_Back{ RasterizerState::Culling::Back }
	, m_Culling_Front{ RasterizerState::Culling::Front }
	, m_Culling_None{ RasterizerState::Culling::None }
	, m_DepthStencilState_On{ true }
	, m_DepthStencilState_Off{ false }
	, m_BonesBuffer{ BONES_BUFFER_SIZE }
{
}

void TowerGameRenderer::OnCanvasResized(const App::ResizedEvent& event)
{
	SYSTEMS.Skydome.OnCanvasResized(event);
}

void TowerGameRenderer::PreRender()
{
	//Get
	static constexpr Shader::Function::Flag VertexShader{ Shader::Function::Vertex };
	const Camera& camera{ *Globals::pCamera };
	const ShadowSystem& shadows{ SYSTEMS.Shadows };

	m_Culling_Front.Activate();
	m_DepthStencilState_On.Activate();

	SYSTEMS.Shadows.BeginCreateAny();

	for (unsigned iShadow{ 0 }; iShadow < shadows.NR_MAPS; ++iShadow)
	{
		SYSTEMS.Shadows.BeginCreate(iShadow);

		const Float4X4 viewProjection{ shadows.GetViewProj(iShadow) };
		m_CameraMatrixPosBuffer.Update(CB_CamMatPosFor{ camera.GetPosition(), camera.GetForward(), viewProjection });
		m_CameraMatrixPosBuffer.Activate();

		//Terrain
		m_Shader_Terrain.Activate<Shader::Function::Vertex>();
		m_Il_V_PosNorCol.Activate();
		SYSTEMS.StaticMeshes.Render();

		//Object_Instances
		m_Shader_Object_Inst.Activate<VertexShader>();
		m_Il_V_PosNorUv_I_ModelMatrix.Activate();
		SYSTEMS.Arrows.Render(true);

		//Objects
		m_Shader_Object.Activate<Shader::Function::Vertex>();
		m_Il_V_PosNorUv.Activate();
		SYSTEMS.StaticModels.Render();

		//Enemies
		//	Contains it's own InputLayout & Shader
		m_BonesBuffer.Activate(3);
		SYSTEMS.Enemies.Render<VertexShader>(m_BonesBuffer);
	}
}

void TowerGameRenderer::Render()
{
	const Camera& camera{ *Globals::pCamera };
	const Float4X4& viewProjection{ camera.GetViewProjection() };

	m_Culling_Back.Activate();
	m_Sampler.Activate();

	//Skydome
	SYSTEMS.Skydome.Render();

	//Begin Shadowed
	SYSTEMS.Shadows.BeginRender();
	m_DepthStencilState_On.Activate();

	//Set Camera
	m_CameraMatrixPosBuffer.Update(CB_CamMatPosFor{ camera.GetPosition(), camera.GetForward(), viewProjection });
	m_CameraMatrixPosBuffer.Activate();

	//Terrain
	m_Shader_Terrain.Activate();
	m_Il_V_PosNorCol.Activate();
	SYSTEMS.StaticMeshes.Render();

	//Object Instances
	m_Shader_Object_Inst.Activate();
	m_Shader_Object_Inst.Activate();
	m_Il_V_PosNorUv_I_ModelMatrix.Activate();
	SYSTEMS.Arrows.Render();

	//Objects
	m_Shader_Object.Activate();
	m_Il_V_PosNorUv.Activate();
	SYSTEMS.StaticModels.Render();

	//Enemies
	m_BonesBuffer.Activate(3);
	SYSTEMS.Enemies.Render(m_BonesBuffer);

	//Weapon
	const Bow& bow{ SYSTEMS.Bow };
	m_Shader_Weapon.Activate();
	m_Il_V_PosNorUvSkin.Activate();
	m_ModelBuffer.Update(CB_ModelBuffer{ bow.GetWorldTransform().AsMatrix() });
	m_ModelBuffer.Activate(1);
	m_BonesBuffer.Update(bow.GetBones());
	SYSTEMS.Bow.Render();

	//End Shadowed
	SYSTEMS.Shadows.EndRender();

	//Other
	SYSTEMS.pSimpleRenderer->Render();

	//---| Transparency |---
	m_Culling_None.Activate();
	m_DepthStencilState_Off.Activate();

	//Arrow Tracers
	PrimitiveTopology::Activate(ModelTopology::TriangleListIdx);
	m_Il_V_PosCol4Uv.Activate();
	m_Shader_Tracer.Activate();
	SYSTEMS.Arrows.RenderTracers();
}
