#include "pch.h"
#include "ShadowRenderer.h"

using namespace TowerGame;

const Float3 ShadowRenderer::m_LightDir{ { Float3{-1,-1,1}.Normalized() } };

ShadowRenderer::ShadowRenderer()
{
	m_DepthStencil.Init({ 500,500 });
}

void ShadowRenderer::Render()
{
}

