#pragma once
#include "Services/GameplaySystems.h"
#include "Services/RenderSystems.h"

namespace MyEngine
{
	namespace App
	{
		class FpsControl;
	}
	namespace Framework
	{
		class CoreServices;
	}
}

class TowerApp
{
public:
	explicit TowerApp(const Framework::CoreServices& coreServices);
	void Release();
	void EarlyUpdate();
	void Update();
	void Render();

private:
	GameplaySystems m_Gameplay;
	RenderSystems m_Rendering;

	void InitGameplay();
	void LinkGameplay();
	void InitRendering(App::FpsControl& fpsControl);
	void LinkRendering();

	void CreateArrows();
};
