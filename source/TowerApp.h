#pragma once
#include "TowerAppRenderer.h"

namespace MyEngine
{
	namespace Framework
	{
		struct AppServices;
	}

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
	explicit TowerApp(const Framework::AppServices& appData);
	void Release();
	void Update();
	void Render();

private:
	App::Wrappers::Win32::Window& m_Window;
	Rendering::Gpu& m_Gpu;
	Rendering::Canvas& m_Canvas;
	TowerAppRenderer m_Renderer;
};

