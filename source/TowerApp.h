#pragma once
#include <Game/Camera/FpsCameraController.h>
#include "TowerAppRenderer.h"
#include "Environment/Terrain.h"

namespace MyEngine
{
	namespace Framework
	{
		struct AppServices;
	}
	namespace App
	{
		class FpsControl;
		namespace Win32
		{
			class Window;
		}
	}
	namespace Rendering
	{
		class Gpu;
		class Canvas;
	}
}

class TowerApp
{
public:
	explicit TowerApp(const Framework::CoreServices& services);
	void Release();
	void Update();
	void Render();

private:
	App::Win32::Window& m_Window;
	Rendering::Gpu& m_Gpu;
	Rendering::Canvas& m_Canvas;
	Game::FpsCameraController m_CameraController;
	TowerAppRenderer m_Renderer;

	Bow m_Bow;
	Terrain m_Terrain;
};
