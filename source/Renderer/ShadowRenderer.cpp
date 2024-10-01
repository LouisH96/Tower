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
	m_DepthStencil.Init(Globals::pCanvas->GetSize(), true);
}

void ShadowRenderer::Render()
{
	m_DepthStencil.Clear();
	ID3D11RenderTargetView* renderTargets[1]
	{
		nullptr
	};
	Globals::pGpu->GetContext().OMSetRenderTargets(1, renderTargets, m_DepthStencil.GetView());
	RenderSystems::GetTerrainRenderer().Render<Shader::Function::Vertex>();
	RenderSystems::GetTransformRenderer().Render<Shader::Function::Vertex>();
}

void ShadowRenderer::OnCanvasResized(const App::ResizedEvent& event)
{
	m_DepthStencil.Update(event.NewSize, true);
}

