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
	, m_Il_V_PosNorUv_I_ModelMatrices{ InputLayout::FromTypes<V_PosNorUv, I_ModelMatrices>() }
	, m_DepthStencilState_On{ true }
{
}

void TowerGameRenderer::OnCanvasResized(const App::ResizedEvent& event)
{
	m_SkyDomeRenderer.OnCanvasResized(event);
}

void TowerGameRenderer::PreRender()
{
	const Camera& camera{ *Globals::pCamera };
	const Float4X4& viewProjection{ camera.GetViewProjection() };
	m_DepthStencilState_On.Activate();
	m_Il_V_PosNorUv_I_ModelMatrices.Activate();
	m_Shader_Tex_Trans_Inst.Activate<Shader::Function::Vertex>();
	m_CameraPosBuffer.Update(CB_CamPos{ camera.GetPosition() });
	m_CameraPosBuffer.Activate();

	m_ShadowRenderer.PrepareRendering();
	GameplaySystems::GetEnemySystem().Render(viewProjection); //Render
	GameplaySystems::GetArrowSystem().Render();

	RenderSystems::GetTerrainRenderer().Render<Shader::Function::Vertex>();
	RenderSystems::GetTransformRenderer().Render<Shader::Function::Vertex>();
}

void TowerGameRenderer::Render()
{
	const Camera& camera{ *Globals::pCamera };
	const Float4X4& viewProjection{ camera.GetViewProjection() };

	m_SkyDomeRenderer.Render(); //Render

	m_DepthStencilState_On.Activate();
	m_Il_V_PosNorUv_I_ModelMatrices.Activate();
	m_Shader_Tex_Trans_Inst.Activate();
	m_CameraPosBuffer.Update(CB_CamPos{ camera.GetPosition() });
	m_CameraPosBuffer.Activate();
	GameplaySystems::GetEnemySystem().Render(viewProjection); //Render
	GameplaySystems::GetArrowSystem().Render(); //Render

	RenderSystems::GetTerrainRenderer().Render();
	RenderSystems::GetTransformRenderer().Render();
	RenderSystems::GetSimpleRenderer().Render();
	RenderSystems::GetTexture2DRenderer().Render();
	RenderSystems::GetUnlitRenderer().Render();
	RenderSystems::GetTextureRenderer().Render();
}
