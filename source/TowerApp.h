#pragma once
#include "TowerAppRenderer.h"

namespace MyEngine
{
	namespace Game
	{
		class Camera;
	}

	namespace App
	{
		class FpsControl;
		namespace Wrappers
		{
			namespace Win32
			{
				class Window;
			}
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
	TowerApp(App::Wrappers::Win32::Window& window, Rendering::Gpu& gpu, Rendering::Canvas& canvas,
		Game::Camera& camera, App::FpsControl& fpsControl);
	void Release();
	void Update();

private:
	App::Wrappers::Win32::Window& m_Window;
	Rendering::Gpu& m_Gpu;
	Rendering::Canvas& m_Canvas;
	TowerAppRenderer m_Renderer;
};

