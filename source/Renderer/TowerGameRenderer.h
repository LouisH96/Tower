#pragma once

#include "SkyDomeRenderer.h"

namespace TowerGame
{

class TowerGameRenderer
{
public:
	TowerGameRenderer();

	void Render();

private:
	SkyDomeRenderer m_SkyDomeRenderer;
};

}
