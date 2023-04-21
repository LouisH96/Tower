#pragma once
#include <Rendering/FpsDisplay.h>
#include <Rendering/Renderers/RendererFactory.h>

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
		class FpsDisplay;
		class Gpu;
		class Canvas;
	}
}

class TowerAppRenderer
{
public:
	TowerAppRenderer(App::Wrappers::Win32::Window& window, Rendering::Gpu& gpu, Rendering::Canvas& canvas,
		App::FpsControl& fpsControl, Game::Camera& camera);
	~TowerAppRenderer() = default;
	void Release() const;
	void Render();

private:
	using SimpleRenderer = Rendering::RendererFactory::SimpleRenderer;
	App::Wrappers::Win32::Window& m_Window;
	Rendering::Gpu& m_Gpu;
	Rendering::Canvas& m_Canvas;
	Rendering::FpsDisplay m_FpsDisplay;
	Game::Camera& m_Camera;

	Rendering::RendererFactory::UnlitRenderer* m_pUnlitRenderer{};
	SimpleRenderer* m_pSimpleRenderer{};

	void CreateCube() const;
	void CreateArrows() const;
};

