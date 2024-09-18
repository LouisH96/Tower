#include "pch.h"
#include "ShadowRenderer.h"

#include <Services\RenderSystems.h>
#include <Rendering\State\Shader.h>

using namespace TowerGame;
using namespace Rendering;

const Float3 ShadowRenderer::m_LightDir{ { Float3{-1,-1,1}.Normalized() } };

ShadowRenderer::ShadowRenderer()
{
	m_DepthStencil.Init({ 500,500 });
}

void ShadowRenderer::Render()
{
	m_DepthStencil.Clear();
	ID3D11RenderTargetView* renderTargets[1]
	{
		nullptr
	};
	Globals::pGpu->GetContext().OMSetRenderTargets(1, renderTargets, m_DepthStencil.GetView());
	RenderSystems::GetTextureRenderer().Render<Shader::Function::Vertex>();
	RenderSystems::GetTerrainRenderer().Render<Shader::Function::Vertex>();
}

