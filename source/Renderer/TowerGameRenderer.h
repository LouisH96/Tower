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
	ShadowRenderer m_ShadowRenderer;
	SkyDomeRenderer m_SkyDomeRenderer;
};

}
