#pragma once

#include <Rendering\State\SamplerState.h>

#include "ShadowRenderer.h"
#include "SkyDomeRenderer.h"

namespace TowerGame
{

class TowerGameRenderer
{
public:
	TowerGameRenderer();

	void OnCanvasResized(const App::ResizedEvent& event);

	void PreRender();
	void Render();

	ShadowRenderer& GetShadowRenderer() { return m_ShadowRenderer; }
	const ShadowRenderer& GetShadowRenderer() const { return m_ShadowRenderer; }

private:
	//CamBuffers
	Rendering::ConstantBuffer<Rendering::CB_CamPos> m_CameraPosBuffer;
	Rendering::ConstantBuffer<Rendering::CB_CamMatPos> m_CameraMatrixPosBuffer;

	//InputLayouts
	Rendering::InputLayout m_Il_V_PosNorUv_I_ModelMatrices;
	Rendering::InputLayout m_Il_V_PosNorCol;

	//Shaders
	Rendering::Shader m_Shader_Tex_Trans_Inst;
	Rendering::Shader m_Shader_Terrain;

	//DepthStencilStates
	Rendering::DepthStencilState m_DepthStencilState_On;

	//Samplers
	Rendering::SamplerState m_Sampler;

	//Renderers
	ShadowRenderer m_ShadowRenderer;
	SkyDomeRenderer m_SkyDomeRenderer;
};

}
