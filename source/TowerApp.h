#pragma once
#include "TowerGameRenderer.h"
#include <Systems\TowerSystems.h>

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

	TowerGameRenderer& GetRenderer() { return m_Renderer; }
	TowerSystems& GetTowerSystems() { return m_TowerSystems; }

private:
	TowerGameRenderer m_Renderer;
	TowerSystems m_TowerSystems;
};
}

#define APP (*TowerApp::pApp)
#define NEW_UI (*Globals::pUi)
#define SYSTEMS (APP.GetTowerSystems())
