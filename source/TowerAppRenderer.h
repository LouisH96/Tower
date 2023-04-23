#pragma once
#include <Rendering/FpsDisplay.h>
#include <Rendering/Renderers/RendererFactory.h>

namespace MyEngine
{
	namespace Framework
	{
		class CoreServices;
	}

	namespace Game
	{
		class Camera;
		class FocusPointCamera;
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
		class R_LambertCam_Tex_Transform;
		class FpsDisplay;
		class Gpu;
		class Canvas;
	}
}

class TowerAppRenderer
{
public:
	TowerAppRenderer(const Framework::CoreServices& services, Game::Camera& camera);
	~TowerAppRenderer();
	void Release() const;
	void Render();

private:
	using SimpleRenderer = Rendering::RendererFactory::SimpleRenderer;
	using TransformRenderer = Rendering::R_LambertCam_Tex_Transform;

	App::Wrappers::Win32::Window& m_Window;
	Rendering::Gpu& m_Gpu;
	Rendering::Canvas& m_Canvas;
	Rendering::FpsDisplay m_FpsDisplay;
	Game::Camera& m_Camera;

	Rendering::RendererFactory::UnlitRenderer* m_pUnlitRenderer{};
	SimpleRenderer* m_pSimpleRenderer{};
	TransformRenderer* m_pTransformRenderer{};

	Rendering::Mesh* m_pBowMesh{};
	Rendering::Texture* m_pBowTexture{};
	Game::Transform* m_pBowTransform{};

	void CreateCube() const;
	void CreateArrows() const;
	void CreateBow();
};

