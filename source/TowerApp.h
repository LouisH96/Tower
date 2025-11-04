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
	TowerSystems& GetSystems() { return m_Systems; }

private:
	TowerGameRenderer m_Renderer;
	TowerSystems m_Systems;
};
}

#define APP (*TowerApp::pApp)
#define NEW_UI (*Globals::pUi)
#define SYSTEMS (APP.GetSystems())
