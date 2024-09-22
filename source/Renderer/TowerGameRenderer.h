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
	using CameraPosBuffer = Rendering::CB_CamPos;
	Rendering::ConstantBuffer<CameraPosBuffer> m_CameraPosBuffer;

	//InputLayouts
	Rendering::InputLayout m_Il_Tex_Trans_Inst;

	//Shaders
	Rendering::Shader m_Shader_Tex_Trans_Inst;

	//Renderers
	ShadowRenderer m_ShadowRenderer;
	SkyDomeRenderer m_SkyDomeRenderer;
};

}
