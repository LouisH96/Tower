#pragma once
#include "TowerGameRenderer.h"
#include <Systems\Systems.h>


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
	Systems& GetSystems() { return m_Systems; }

private:
	TowerGameRenderer m_Renderer;
	Systems m_Systems;
};
}

#define APP (*TowerApp::pApp)
#define NEW_UI (*Globals::pUi)
#define SYSTEMS (APP.GetSystems())
