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

private:
	ShadowRenderer m_ShadowRenderer;
	SkyDomeRenderer m_SkyDomeRenderer;
};

}
