#include "pch.h"
#include "SkyDomeRenderer.h"

using namespace TowerGame;
using namespace Rendering;

SkyDomeRenderer::SkyDomeRenderer()
	: m_Shader{ Resources::Local(L"SkyDome.hlsl") }
	, m_InputLayout{ InputLayout::FromType<Vertex>() }
	, m_DepthStencil{ false }
{
	InitVertexBuffer();
}

void SkyDomeRenderer::Render()
{
	m_DepthStencil.Activate();
	m_Shader.Activate();
	m_InputLayout.Activate();
	m_VertexBuffer.Activate(0);
	m_VertexBuffer.Draw();
}

void SkyDomeRenderer::InitVertexBuffer()
{
	constexpr unsigned nrVertices{ 6 };
	Vertex vertices[nrVertices];

	vertices[0].pos = { -1,-1 };
	vertices[1].pos = { -1,1 };
	vertices[2].pos = { 1,1 };

	vertices[3] = vertices[0];
	vertices[4] = vertices[2];
	vertices[5].pos = { 1,-1 };

	m_VertexBuffer = Buffer<Vertex>{ PtrRangeConst<Vertex>{vertices, nrVertices}, false };
}
