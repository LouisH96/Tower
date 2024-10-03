#include "pch.h"
#include "ShadowRenderer.h"

#include <Services\RenderSystems.h>
#include <Rendering\State\Shader.h>
#include <Rendering\Canvas.h>

using namespace TowerGame;
using namespace Rendering;

const Float3 ShadowRenderer::m_LightDir{ { Float3{-1,-1,1}.Normalized() } };

ShadowRenderer::ShadowRenderer()
{
	m_DepthStencil.Init({600,600}, true);
}

void ShadowRenderer::PrepareRendering()
{
	m_DepthStencil.Clear();
	ID3D11RenderTargetView* renderTargets[1]
	{
		nullptr
	};
	Globals::pGpu->GetContext().OMSetRenderTargets(1, renderTargets, m_DepthStencil.GetView());
}

