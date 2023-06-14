#pragma once
#include <Rendering/FpsDisplay.h>
#include <Rendering/Renderers/RendererFactory.h>

#include "Weapons/Bow.h"
#include "Rendering/Renderers/R_LambertLight_Col.h"

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
		class Texture2DRenderer;
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
	using TerrainRenderer = Rendering::R_LambertLight_Col;

	TowerAppRenderer(const Framework::CoreServices& services);
	~TowerAppRenderer() = default;
	void Release() const;
	void Render(const Game::FpsCameraController& cameraController);

	TransformRenderer& GetTransformRenderer() const { return *m_pTransformRenderer; }
	TerrainRenderer& GetTerrainRenderer() const { return *m_pTerrainRenderer; }
	Rendering::Texture2DRenderer& GetTexture2DRenderer() const { return *m_pTexture2DRenderer; }

private:
	using SimpleRenderer = Rendering::RendererFactory::SimpleRenderer;
	using TextureRenderer = Rendering::TextureRenderer<Rendering::V_PosUv, Rendering::CB_CamMat>;

	App::Win32::Window& m_Window;
	Rendering::Canvas& m_Canvas;
	Rendering::FpsDisplay m_FpsDisplay;

	Rendering::RendererFactory::UnlitRenderer* m_pUnlitRenderer{};
	SimpleRenderer* m_pSimpleRenderer{};
	TransformRenderer* m_pTransformRenderer{};
	TerrainRenderer* m_pTerrainRenderer{};
	TextureRenderer* m_pTextureRenderer{};
	Rendering::Texture2DRenderer* m_pTexture2DRenderer{};

	void CreateArrows() const;
};

