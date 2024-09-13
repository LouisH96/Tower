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
	, m_Texture{ Resources::Local(L"SkyDome2.jpg") }
	//, m_Texture{ Resources::Local(L"SkyDome3.jpg") }
{
	InitVertexBuffer();
	InitDomeBuffer();
}

void SkyDomeRenderer::Render()
{
	UpdateDomeBufferCameraData();

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

void SkyDomeRenderer::InitDomeBuffer()
{
	const float aspectRatio{ Globals::pCamera->GetAspectRatio() };
	const float cameraFovX = 45 * Constants::TO_RAD;

	m_DomeBuffer.FovTan.x = tanf(cameraFovX);
	m_DomeBuffer.FovTan.y = m_DomeBuffer.FovTan.x / aspectRatio;
}

void SkyDomeRenderer::UpdateDomeBufferCameraData()
{
	const Float3 cameraForward{ Globals::pCamera->GetForward() };
	const float cameraPitch{ asinf(cameraForward.y) };
	float cameraYaw{ atan2f(cameraForward.z, cameraForward.x) };

	m_DomeBuffer.PitchCos = cosf(cameraPitch);
	m_DomeBuffer.PitchSin = cameraForward.y;
	m_DomeBuffer.Yaw = cameraYaw;

	m_PanelBuffer.Update(m_DomeBuffer);
}
