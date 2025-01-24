#pragma once
#include "Services/GameplaySystems.h"
#include "Services/RenderSystems.h"
#include <Renderer\TowerGameRenderer.h>

namespace TowerGame
{
class TowerApp
{
public:
	static TowerApp* pApp;

	explicit TowerApp();
	~TowerApp();

	void OnCanvasResized(const App::ResizedEvent& event);

	void EarlyUpdate();
	void Update();
	void PreRender();
	void Render();
	void RenderUi();

	const TowerGame::TowerGameRenderer& GetRenderer() const { return m_Renderer; }

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
}

#define APP (*TowerApp::pApp)
#define NEW_UI (*Globals::pUi)
