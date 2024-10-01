#pragma once

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

	//InputLayouts
	Rendering::InputLayout m_Il_V_PosNorUv_I_ModelMatrices;

	//Shaders
	Rendering::Shader m_Shader_Tex_Trans_Inst;

	//DepthStencilStates
	Rendering::DepthStencilState m_DepthStencilState_On;

	//Renderers
	ShadowRenderer m_ShadowRenderer;
	SkyDomeRenderer m_SkyDomeRenderer;
};

}
