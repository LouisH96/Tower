#pragma once
#include <Game/Camera/FpsCameraController.h>

#include "Character.h"
#include "TowerAppRenderer.h"
#include "Environment/Terrain.h"
#include "Environment/Tower.h"

namespace MyEngine
{
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
	TowerAppRenderer m_Renderer;

	Character m_Character;
	Terrain m_Terrain;
	Tower m_Tower;
};
