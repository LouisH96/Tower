#pragma once
#include "SkyDomeRenderer.h"
#include <Rendering\State\DepthStencilBuffer.h>

namespace TowerGame
{

class TowerGameRenderer
{
public:

	TowerGameRenderer();

	void OnCanvasResized(const App::ResizedEvent& event);

	void PreRender();
	void Render();

	Rendering::DepthStencilBuffer& GetDsBufferShadow() { return m_DepthStencilBuffer_Shadow; }

private:

	//CamBuffers
	Rendering::ConstantBuffer<Rendering::CB_CamPos> m_CameraPosBuffer;

	//InputLayouts
	Rendering::InputLayout m_Il_V_PosNorUv_I_ModelMatrices;

	//Shaders
	Rendering::Shader m_Shader_Tex_Trans_Inst;

	//DepthStencilStates
	Rendering::DepthStencilState m_DepthStencilState_On;

	//DepthStencilBuffers
	Rendering::DepthStencilBuffer m_DepthStencilBuffer_Shadow;

	//Renderers
	//ShadowRenderer m_ShadowRenderer;
	SkyDomeRenderer m_SkyDomeRenderer;
};

}
