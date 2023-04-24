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

struct TowerAppRenderData
{
	Math::Float3 CameraPosition;
	DirectX::XMMATRIX CameraProjection;
};

class TowerAppRenderer
{
public:

	TowerAppRenderer(const Framework::CoreServices& services);
	~TowerAppRenderer() = default;
	void Release() const;
	void Render(const Game::FpsCameraController& cameraController);

	Rendering::ConstantBuffer<Rendering::CB_CamMatPos>& GetCameraConstantBuffer() { return m_CameraConstantBuffer; }
	Rendering::ConstantBuffer<Rendering::CB_ModelBuffer>& GetModelConstantBuffer() { return m_ModelConstantBuffer; }
	Rendering::Gpu& GetGpu() const { return m_Gpu; }

private:
	using SimpleRenderer = Rendering::RendererFactory::SimpleRenderer;

	App::Wrappers::Win32::Window& m_Window;
	Rendering::Gpu& m_Gpu;
	Rendering::Canvas& m_Canvas;
	Rendering::FpsDisplay m_FpsDisplay;

	Rendering::RendererFactory::UnlitRenderer* m_pUnlitRenderer{};
	SimpleRenderer* m_pSimpleRenderer{};

	Bow m_Bow;

	//Custom rendering
	Rendering::BlendState m_BlendState;
	Rendering::RasterizerState m_RasterizerState;
	Rendering::SamplerState m_Sampler;
	Rendering::InputLayout m_InputLayout;
	Rendering::Shader m_Shader;
	Rendering::ConstantBuffer<Rendering::CB_CamMatPos> m_CameraConstantBuffer;
	Rendering::ConstantBuffer<Rendering::CB_ModelBuffer> m_ModelConstantBuffer;

	void CreateArrows() const;

	void BeginCustomRender(const Game::FpsCameraController& cameraController);
};

