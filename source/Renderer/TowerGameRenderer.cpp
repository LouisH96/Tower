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
	: m_Shader_Entity{ Resources::Local(L"Entity.hlsl") }
	, m_Shader_Terrain{ Resources::Local(L"Terrain.hlsl") }
	, m_Il_V_PosNorUv_I_ModelMatrix{ InputLayout::FromTypes<V_PosNorUv, I_ModelMatrix>() }
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

	//Entity
	m_Shader_Entity.Activate<Shader::Function::Vertex>();
	m_CameraMatrixPosBuffer.Update(CB_CamMatPos{ Float3{}, viewProjection });
	m_CameraMatrixPosBuffer.Activate();
	m_Il_V_PosNorUv_I_ModelMatrix.Activate();
	GameplaySystems::GetArrowSystem().Render(true);
	GameplaySystems::GetEnemySystem().Render(); //Render

	m_Shader_Terrain.Activate<Shader::Function::Vertex>();
	m_Il_V_PosNorCol.Activate();
	RenderSystems::GetTerrainRenderer().Render();
}

void TowerGameRenderer::Render()
{
	const Camera& camera{ *Globals::pCamera };
	const Float4X4& viewProjection{ camera.GetViewProjection() };

	m_SkyDomeRenderer.Render(); //Render
	m_ShadowRenderer.BeginRender();

	m_DepthStencilState_On.Activate();

	//Terrain
	m_Shader_Terrain.Activate();
	m_Il_V_PosNorCol.Activate();
	m_CameraMatrixPosBuffer.Update({ camera });
	m_CameraMatrixPosBuffer.Activate();
	RenderSystems::GetTerrainRenderer().Render();

	//Entity
	m_Sampler.Activate();
	m_Il_V_PosNorUv_I_ModelMatrix.Activate();
	m_Shader_Entity.Activate();
	m_CameraMatrixPosBuffer.Activate();
	GameplaySystems::GetArrowSystem().Render();
	GameplaySystems::GetEnemySystem().Render();

	//Other
	RenderSystems::GetTransformRenderer().Render();
	RenderSystems::GetSimpleRenderer().Render();
	RenderSystems::GetTexture2DRenderer().Render();
	RenderSystems::GetUnlitRenderer().Render();
	RenderSystems::GetTextureRenderer().Render();

	m_ShadowRenderer.EndRender();
}
