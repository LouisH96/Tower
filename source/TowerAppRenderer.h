#pragma once
#include <Rendering/FpsDisplay.h>
#include <Rendering/Renderers/RendererFactory.h>

#include "Bow.h"

namespace MyEngine
{
	namespace Framework
	{
		class CoreServices;
	}

	namespace Game
	{
		class FpsCameraController;
		class Camera;
		class FocusPointCamera;
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
		class R_LambertLight_Tex;
		class R_LambertCam_Tex;
		class R_LambertCam_Tex_Transform;
		class FpsDisplay;
		class Gpu;
		class Canvas;
	}
}

class TowerAppRenderer
{
public:
	using TransformRenderer = Rendering::R_LambertCam_Tex_Transform;
	using TerrainRenderer = Rendering::R_LambertLight_Tex;

	TowerAppRenderer(const Framework::CoreServices& services);
	~TowerAppRenderer() = default;
	void Release() const;
	void Render(const Game::FpsCameraController& cameraController);

	Rendering::Gpu& GetGpu() const { return m_Gpu; }
	TransformRenderer& GetTransformRenderer() const { return *m_pTransformRenderer; }
	TerrainRenderer& GetTerrainRenderer() const { return *m_pTerrainRenderer; }

private:
	using SimpleRenderer = Rendering::RendererFactory::SimpleRenderer;

	App::Win32::Window& m_Window;
	Rendering::Gpu& m_Gpu;
	Rendering::Canvas& m_Canvas;
	Rendering::FpsDisplay m_FpsDisplay;

	Rendering::RendererFactory::UnlitRenderer* m_pUnlitRenderer{};
	SimpleRenderer* m_pSimpleRenderer{};
	TransformRenderer* m_pTransformRenderer{};
	TerrainRenderer* m_pTerrainRenderer{};

	void CreateArrows() const;
};

