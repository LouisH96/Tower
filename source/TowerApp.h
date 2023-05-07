#pragma once
#include <Game/Camera/FpsCameraController.h>

#include "TowerAppRenderer.h"
#include "Services/TowerAppServices.h"

class EnemySystem;
class Character;

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
	explicit TowerApp(const Framework::CoreServices& coreServices);
	void Release();
	void Update();
	void Render();

private:
	App::Win32::Window& m_Window;
	Rendering::Gpu& m_Gpu;
	Rendering::Canvas& m_Canvas;
	TowerAppServices m_Services;

	Character* m_pCharacter;
	Terrain* m_pTerrain;
	Tower* m_pTower;
	EnemySystem* m_pEnemySystem;
};
