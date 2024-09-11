#pragma once
#include "Services/GameplaySystems.h"
#include "Services/RenderSystems.h"
#include <Renderer\TowerGameRenderer.h>

class TowerApp
{
public:
	explicit TowerApp();
	~TowerApp();

	void OnCanvasResized(const App::ResizedEvent& event);

	void EarlyUpdate();
	void Update();
	void Render();
	void RenderUi();

private:
	GameplaySystems m_Gameplay;
	RenderSystems m_Rendering;
	TowerGame::TowerGameRenderer m_Renderer;

	void InitGameplay();
	void LinkGameplay();
	void InitRendering();
	void LinkRendering();

	void CreateArrows();
};
