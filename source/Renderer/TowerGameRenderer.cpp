#include "pch.h"
#include "TowerGameRenderer.h"

#include <Rendering\Renderers\R_LambertCam_Tex_Tran_Inst.h>
#include <Rendering\Renderers\Texture2DRenderer.h>
#include <Services\GameplaySystems.h>
#include <Services\RenderSystems.h>
#include <Weapons\ArrowSystem.h>
#include <Character\EnemySystem.h>

using namespace TowerGame;
using namespace Rendering;

TowerGameRenderer::TowerGameRenderer()
	: m_Shader_Tex_Trans_Inst{ Resources::GlobalShader(L"LambertCam_Tex_Tran_Inst.hlsl") }
	, m_Shader_Terrain{ Resources::Local(L"Terrain.hlsl") }
	, m_Il_V_PosNorUv_I_ModelMatrices{ InputLayout::FromTypes<V_PosNorUv, I_ModelMatrices>() }
	, m_Il_V_PosNorCol{ InputLayout::FromType<V_PosNorCol>() }
	, m_DepthStencilState_On{ true }
{
}

void TowerGameRenderer::OnCanvasResized(const App::ResizedEvent& event)
{
	m_SkyDomeRenderer.OnCanvasResized(event);
}

void TowerGameRenderer::PreRender()
{
	//Get
	const Camera& camera{ *Globals::pCamera };

	m_ShadowRenderer.BeginShadowMapRender();
	const Float4X4 viewProjection{ m_ShadowRenderer.GetLightViewProjection() };
	
	m_DepthStencilState_On.Activate();

	m_Il_V_PosNorUv_I_ModelMatrices.Activate();
	m_Shader_Tex_Trans_Inst.Activate<Shader::Function::Vertex>();
	m_CameraPosBuffer.Update(CB_CamPos{ camera.GetPosition() });
	m_CameraPosBuffer.Activate();

	GameplaySystems::GetEnemySystem().Render(viewProjection); //Render
}

void TowerGameRenderer::Render()
{
	const Camera& camera{ *Globals::pCamera };
	const Float4X4& viewProjection{ camera.GetViewProjection() };

	m_SkyDomeRenderer.Render(); //Render
	m_ShadowRenderer.BeginRender();

	m_DepthStencilState_On.Activate();
	m_Sampler.Activate();

	m_Shader_Terrain.Activate();
	m_Il_V_PosNorCol.Activate();
	m_CameraMatrixPosBuffer.Update({ camera });
	m_CameraMatrixPosBuffer.Activate();
	RenderSystems::GetTerrainRenderer().Render();

	m_Il_V_PosNorUv_I_ModelMatrices.Activate();
	m_Shader_Tex_Trans_Inst.Activate();
	m_CameraPosBuffer.Update(CB_CamPos{ camera.GetPosition() });
	m_CameraPosBuffer.Activate();
	GameplaySystems::GetEnemySystem().Render(viewProjection); //Render
	GameplaySystems::GetArrowSystem().Render(); //Render

	RenderSystems::GetTransformRenderer().Render();
	RenderSystems::GetSimpleRenderer().Render();
	RenderSystems::GetTexture2DRenderer().Render();
	RenderSystems::GetUnlitRenderer().Render();
	RenderSystems::GetTextureRenderer().Render();

	m_ShadowRenderer.EndRender();
}
