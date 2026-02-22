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
	: m_Shader_Entity{ Resources::Local(L"Entity.hlsl") }
	, m_Shader_Weapon{ Resources::Local(L"Weapon.hlsl") }
	, m_Shader_Terrain{ Resources::Local(L"Terrain.hlsl") }
	, m_Shader_StaticMesh{ Resources::Local(L"StaticMesh.hlsl") }
	, m_Shader_Tracer{ Resources::Local(L"Tracer.hlsl") }
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

		//Entity
		m_Shader_Entity.Activate<Shader::Function::Vertex>();
		m_Il_V_PosNorUv_I_ModelMatrix.Activate();
		m_BonesBuffer.Activate(3);
		SYSTEMS.Arrows.Render(true);
		SYSTEMS.Enemies
			.Render<Shader::Function::Vertex>(m_BonesBuffer); //Render

		m_Shader_Terrain.Activate<Shader::Function::Vertex>();
		m_Il_V_PosNorCol.Activate();
		SYSTEMS.StaticMeshes.Render();

		SYSTEMS.StaticModels.Render();
	}
}

void TowerGameRenderer::Render()
{
	const Camera& camera{ *Globals::pCamera };
	const Float4X4& viewProjection{ camera.GetViewProjection() };

	m_Culling_Back.Activate();

	SYSTEMS.Skydome.Render(); //Render
	SYSTEMS.Shadows.BeginRender();

	m_DepthStencilState_On.Activate();
	m_CameraMatrixPosBuffer.Update(CB_CamMatPosFor{ camera.GetPosition(), camera.GetForward(), viewProjection });
	m_CameraMatrixPosBuffer.Activate();

	//Terrain
	m_Shader_Terrain.Activate();
	m_Il_V_PosNorCol.Activate();
	SYSTEMS.StaticMeshes.Render();

	//Entity
	m_Sampler.Activate();
	m_Il_V_PosNorUv_I_ModelMatrix.Activate();
	m_Shader_Entity.Activate();
	m_BonesBuffer.Activate(3);
	SYSTEMS.Arrows.Render();
	SYSTEMS.Enemies.Render(m_BonesBuffer);

	m_CameraMatrixPosBuffer.Update(CB_CamMatPosFor{ camera.GetPosition(), camera.GetForward(), viewProjection });
	m_CameraMatrixPosBuffer.Activate();

	//StaticMesh
	m_Shader_StaticMesh.Activate();
	m_Il_V_PosNorUv.Activate();
	SYSTEMS.StaticModels.Render();

	//Weapon
	const Bow& bow{ SYSTEMS.Bow };
	m_Shader_Weapon.Activate();
	m_Il_V_PosNorUvSkin.Activate();
	m_ModelBuffer.Update(CB_ModelBuffer{ bow.GetWorldTransform().AsMatrix() });
	m_ModelBuffer.Activate(1);
	m_BonesBuffer.Update(bow.GetBones());
	SYSTEMS.Bow.Render();

	//Other
	SYSTEMS.pSimpleRenderer->Render();

	SYSTEMS.Shadows.EndRender();

	//---| Transparency |---
	m_Culling_None.Activate();
	m_DepthStencilState_Off.Activate();

	//Arrow Tracers
	PrimitiveTopology::Activate(ModelTopology::TriangleListIdx);
	m_Il_V_PosCol4Uv.Activate();
	m_Shader_Tracer.Activate();
	SYSTEMS.Arrows.RenderTracers();
}
