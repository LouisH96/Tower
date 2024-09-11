#include "pch.h"
#include "TowerGameRenderer.h"

using namespace TowerGame;

TowerGameRenderer::TowerGameRenderer()
{
}

void TowerGameRenderer::Render()
{
	m_SkyDomeRenderer.Render();
}
