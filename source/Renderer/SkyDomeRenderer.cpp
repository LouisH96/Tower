#include "pch.h"
#include "SkyDomeRenderer.h"

using namespace TowerGame;
using namespace Rendering;

SkyDomeRenderer::SkyDomeRenderer()
	: m_Shader{ Resources::Local(L"SkyDome.hlsl") }
	, m_InputLayout{ InputLayout::FromType<Vertex>() }
	, m_DepthStencil{ false }
	, m_Texture{ Resources::Local(L"Sunset.jpg") }
{
	InitVertexBuffer();
}

void SkyDomeRenderer::Render()
{
	m_DepthStencil.Activate();
	m_Shader.Activate();
	m_InputLayout.Activate();

	m_Texture.Activate();
	m_Sampler.Activate();

	m_VertexBuffer.Activate(0);
	m_VertexBuffer.Draw();
}

void SkyDomeRenderer::InitVertexBuffer()
{
	constexpr unsigned nrVertices{ 6 };
	Vertex vertices[nrVertices];

	vertices[0].pos = { -1,-1 }; //left bot
	vertices[0].uv = { 0,1 };
	vertices[1].pos = { -1,1 }; //left top
	vertices[1].uv = { 0,0 };
	vertices[2].pos = { 1,1 }; //right top
	vertices[2].uv = { 1,0 };

	vertices[3] = vertices[0];
	vertices[4] = vertices[2];
	vertices[5].pos = { 1,-1 }; //right bot
	vertices[5].uv = { 1,1 };

	m_VertexBuffer = Buffer<Vertex>{ PtrRangeConst<Vertex>{vertices, nrVertices}, false };
}
