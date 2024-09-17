#include "pch.h"
#include "TowerGameRenderer.h"

#include <Rendering\Renderers\R_LambertCam_Tex_Tran_Inst.h>
#include <Rendering\Renderers\Texture2DRenderer.h>
#include <Services\GameplaySystems.h>
#include <Services\RenderSystems.h>
#include <Weapons\ArrowSystem.h>

using namespace TowerGame;

TowerGameRenderer::TowerGameRenderer()
{
}

void TowerGameRenderer::Render()
{
	m_SkyDomeRenderer.Render();

	RenderSystems::GetTerrainRenderer().Render();
	RenderSystems::GetTransformRenderer().Render();
	RenderSystems::GetSimpleRenderer().Render();
	RenderSystems::GetTexture2DRenderer().Render();
	RenderSystems::GetUnlitRenderer().Render();
	RenderSystems::GetTextureRenderer().Render();
	RenderSystems::GetInstanceTransformRenderer().Render();
	GameplaySystems::GetArrowSystem().Render();
}
