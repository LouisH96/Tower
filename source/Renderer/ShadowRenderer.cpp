#include "pch.h"
#include "ShadowRenderer.h"

#include <Rendering\Canvas.h>
#include <Rendering\State\Shader.h>
#include <Services\RenderSystems.h>
#include <Transform\ViewMatrix.h>

#undef near
#undef far

using namespace TowerGame;
using namespace Rendering;

const Float3 ShadowRenderer::m_LightDir{ Float3{0,-1,1}.Normalized() };
const Float2 ShadowRenderer::m_TextureSize{ 512 };

ShadowRenderer::ShadowRenderer()
	: m_Viewport{ m_TextureSize }
	, m_View{ ViewMatrix::From(m_LightDir) }
	, m_Projection{ MakeProjectionMatrix() }
{
	m_DepthStencil.Init(m_TextureSize, true);
	m_ShadowMap = { m_DepthStencil.MakeShaderResourceView() };


}

void ShadowRenderer::BeginShadowMapRender()
{
	m_DepthStencil.Clear();
	ID3D11RenderTargetView* renderTargets[1]
	{
		nullptr
	};
	Globals::pGpu->GetContext().OMSetRenderTargets(1, renderTargets, m_DepthStencil.GetView());
	m_Viewport.Activate();

	m_LightBufferData.ViewProjection = m_View * m_Projection;
}

void ShadowRenderer::BeginRender()
{
	m_ShadowMap.Activate(1);
	m_LightBuffer.Update(m_LightBufferData);
	m_LightBuffer.Activate(1);
}

void ShadowRenderer::EndRender()
{
	Texture::Unset(1);
}

void ShadowRenderer::SetTargetPos(const Float3& pos)
{
	ViewMatrix::SetPosition(m_View, pos - m_LightDir * 4);
}

Float4X4 ShadowRenderer::MakeProjectionMatrix()
{
	const float near{ .01f };
	const float far{ 15 };
	return {
		{1,0,0,0},
		{0,1,0,0},
		{0,0, far / (far - near), -(far * near) / (far - near)},
		{0,0,1,0}
	};
}

