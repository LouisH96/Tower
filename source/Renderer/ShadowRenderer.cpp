#include "pch.h"
#include "ShadowRenderer.h"

#include <Rendering\Canvas.h>
#include <Rendering\State\Shader.h>
#include <Services\RenderSystems.h>
#include <Transform\ViewMatrix.h>
#include <Character\Character.h>

#undef near
#undef far

using namespace TowerGame;
using namespace Rendering;

const Float3 ShadowRenderer::m_LightDir{ Float3{.432709f,-.639439f,.635516f}.Normalized() };
const Float2 ShadowRenderer::m_TextureSize{ 512 * 2 };

ShadowRenderer::ShadowRenderer()
	: m_Viewport{ m_TextureSize }
	, m_View{ ViewMatrix::From(m_LightDir) }
	, m_Projection{ MakeProjectionMatrix() }
	, m_Sampler{ SamplerState::BorderMode::Value, SamplerState::BorderMode::Value, Float4{Float::MAX}, SamplerState::Filter::Linear, SamplerState::Comparison::Greater }
{
	m_DepthStencil.Init(m_TextureSize, true);
	m_ShadowMap = { m_DepthStencil.MakeShaderResourceView() };
}

void ShadowRenderer::InitShadow(const Character& character)
{
	m_PrevYaw = GetYaw(character);
	m_PrevForward = GetForward(character);
}

void ShadowRenderer::MoveShadow(const Character& character)
{
	const float yaw{ GetYaw(character) };

	if (abs(yaw - m_PrevYaw) >= YAW_UPDATE_INTERVAL)
	{
		m_PrevYaw = yaw;
		m_PrevForward = GetForward(character);
	}

	const Float3 lightPos{
		character.GetFeetPosition()
		+ m_PrevForward * (SHADOW_AREA_SIZE - 1)
		- m_LightDir * LIGHT_TARGET_DISTANCE
	};

	ViewMatrix::SetPosition(m_View, lightPos);
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
	m_LightBufferData.Forward = m_LightDir;
}

void ShadowRenderer::BeginRender()
{
	m_ShadowMap.Activate(1);
	m_LightBuffer.Update(m_LightBufferData);
	m_LightBuffer.Activate(2);
	m_Sampler.Activate(1);
}

void ShadowRenderer::EndRender()
{
	Texture::Unset(1);
}

Float4X4 ShadowRenderer::MakeProjectionMatrix()
{
	const float near{ .01f };
	const float far{
		LIGHT_TARGET_DISTANCE + 20
	};
	return {
		{1.f / SHADOW_AREA_SIZE,0,0,0},
		{0, 1.f / SHADOW_AREA_SIZE,0,0},
		{0,0, 1 / (far - near), -near / (far - near)},
		{0,0,0,1}
	};
}

float ShadowRenderer::GetYaw(const Character& character)
{
	const Camera& camera{ character.GetCameraController().GetCamera() };
	const Float3& forward{ camera.GetForward() };
	return atan2f(forward.z, forward.x);
}

Float3 ShadowRenderer::GetForward(const Character& character)
{
	const Camera& camera{ character.GetCameraController().GetCamera() };
	return camera.GetForwardXz().Normalized();
}

