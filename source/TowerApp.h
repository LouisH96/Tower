#pragma once
#include <Game/Camera/Camera.h>

#include "TowerAppRenderer.h"

namespace MyEngine
{
	namespace Framework
	{
		struct AppServices;
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
	explicit TowerApp(const Framework::CoreServices& services);
	void Release();
	void Update();
	void Render();

private:
	App::Wrappers::Win32::Window& m_Window;
	Rendering::Gpu& m_Gpu;
	Rendering::Canvas& m_Canvas;
	Game::Camera m_Camera;
	TowerAppRenderer* m_pRenderer{};
};

