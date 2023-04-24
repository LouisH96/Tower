#include "pch.h"
#include "TowerAppRenderer.h"

#include <App/FpsControl.h>
#include <Rendering/FpsDisplay.h>
#include <Rendering/Canvas.h>
#include <Debug/DebugRenderer.h>
#include <Framework/BasicAppFrame.h>
#include <Math/Cube.h>
#include <Generation/Shapes.h>
#include <Generation/Shapes/ArrowGenerator.h>
#include <Framework/Resources.h>
#include <Io/Fbx/FbxClass.h>
#include <Rendering/Renderers/R_LambertCam_Tex_Transform.h>
#include <Game/Transform.h>
#include <Framework/CoreServices.h>
#include <Game/Camera/FpsCameraController.h>

using namespace DirectX;
using namespace Rendering;

TowerAppRenderer::TowerAppRenderer(const Framework::CoreServices& services)
	: m_Window{ services.GetWindow() }
	, m_Gpu{ services.GetGpu() }
	, m_Canvas{ services.GetCanvas() }
	, m_FpsDisplay{ services.GetGpu(), services.GetCanvas(), services.GetWindow() }
	, m_pUnlitRenderer{ Rendering::RendererFactory::CreateUnlitRenderer(services.GetGpu(), false) }
	, m_pSimpleRenderer(Rendering::RendererFactory::CreateSimpleRenderer(services.GetGpu()))
	, m_BlendState(services.GetGpu())
	, m_RasterizerState{ services.GetGpu() }
	, m_Sampler{ services.GetGpu() }
	, m_Shader{ services.GetGpu(), Framework::Resources::GetGlobalShaderPath(L"LambertCam_Tex_Trans.hlsl") }
	, m_InputLayout{ services.GetGpu(), V_PosColNorm::ELEMENTS, V_PosColNorm::NR_ELEMENTS }
	, m_CameraConstantBuffer{ services.GetGpu() }
	, m_Bow{ services.GetGpu() }
	, m_ModelConstantBuffer{ m_Gpu }
{
	DebugRenderer::Init(m_Gpu);
	services.GetFpsControl().SetFpsDisplay(m_FpsDisplay);

	CreateArrows();
}

void TowerAppRenderer::Release() const
{
	delete m_pSimpleRenderer;
	delete m_pUnlitRenderer;
	DebugRenderer::Release();
}

void TowerAppRenderer::Render(const Game::FpsCameraController& cameraController)
{
	const Float3& cameraPosition{ cameraController.GetCameraPosition() };
	const XMMATRIX viewProjection{ cameraController.GetViewProjectionMatrix() };

	m_Canvas.BeginPaint();
	BeginCustomRender(cameraController);
	m_pSimpleRenderer->Render(cameraPosition, viewProjection);
	m_pUnlitRenderer->Render(cameraPosition, viewProjection);
	DebugRenderer::Render(cameraPosition, viewProjection);
	m_FpsDisplay.Render();
	m_Canvas.Present();
}

void TowerAppRenderer::CreateArrows() const
{
	Array<Rendering::V_PosColNorm> pivotVertices{};
	Array<int> pivotIndices{};
	Generation::ArrowGenerator::CreatePivotArrows(pivotVertices, pivotIndices, 16, { 0,0,0 });
	m_pSimpleRenderer->AddMesh(pivotVertices, pivotIndices);
}

void TowerAppRenderer::BeginCustomRender(const Game::FpsCameraController& cameraController)
{
	m_Sampler.ActivatePs(m_Gpu);
	m_RasterizerState.Activate(m_Gpu);
	m_InputLayout.Activate(m_Gpu);
	m_BlendState.Activate(m_Gpu);
	m_Shader.Activate();

	const TowerAppRenderData data
	{
		cameraController.GetCameraPosition(),
		cameraController.GetProjectionMatrix()
	};
	m_Bow.Render(*this, data);
}
