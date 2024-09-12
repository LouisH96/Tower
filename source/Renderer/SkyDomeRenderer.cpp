#include "pch.h"
#include "SkyDomeRenderer.h"
#include <Framework\Globals.h>

using namespace TowerGame;
using namespace Rendering;

SkyDomeRenderer::SkyDomeRenderer()
	: m_Shader{ Resources::Local(L"SkyDome.hlsl") }
	, m_InputLayout{ InputLayout::FromType<Vertex>() }
	, m_DepthStencil{ false }
	//, m_Texture{ Resources::Local(L"SkyDome1.jpg") }
	//, m_Texture{ Resources::Local(L"SkyDome2.jpg") }
	, m_Texture{ Resources::Local(L"SkyDome3.jpg") }
{
	InitVertexBuffer();
	InitPanelBuffer();
}

void SkyDomeRenderer::Render()
{
	m_DepthStencil.Activate();
	m_Shader.Activate();
	m_InputLayout.Activate();

	m_Texture.Activate();
	m_Sampler.Activate();

	m_PanelBuffer.Activate(1);

	m_VertexBuffer.Activate(0);
	m_VertexBuffer.Draw();
}

void SkyDomeRenderer::InitVertexBuffer()
{
	constexpr unsigned nrVertices{ 6 };
	Vertex vertices[nrVertices];

	vertices[0].pos = { -1,-1 }; //left bot
	vertices[1].pos = { -1,1 }; //left top
	vertices[2].pos = { 1,1 }; //right top

	vertices[3] = vertices[0];
	vertices[4] = vertices[2];
	vertices[5].pos = { 1,-1 }; //right bot

	m_VertexBuffer = Buffer<Vertex>{ PtrRangeConst<Vertex>{vertices, nrVertices}, false };
}

void SkyDomeRenderer::InitPanelBuffer()
{
	PanelBuffer buffer{};

	float windowWidth{ Globals::pCamera->GetAspectRatio() };
	float windowHeight{ 1 };
	const float windowRadius{ sqrtf(1 + windowWidth * windowWidth) };

	windowWidth /= windowRadius;
	windowHeight /= windowRadius;

	buffer.PanelSize.x = windowWidth;
	buffer.PanelSize.y = windowHeight;

	buffer.MaxFov = 60.f / 90.f;

	m_PanelBuffer.Update(buffer);
}
