#include "pch.h"
#include "TowerAppRenderer.h"

#include <App/FpsControl.h>
#include <Rendering/FpsDisplay.h>
#include <Rendering/Canvas.h>
#include <Debug/DebugRenderer.h>
#include <Math/Cube.h>
#include <Generation/Shapes/ArrowGenerator.h>
#include <Rendering/Renderers/R_LambertCam_Tex_Transform.h>
#include <Framework/CoreServices.h>
#include <Game/Camera/FpsCameraController.h>

using namespace Math;
using namespace DirectX;
using namespace Rendering;

TowerAppRenderer::TowerAppRenderer(const Framework::CoreServices& services)
	: m_Window{ services.GetWindow() }
	, m_Gpu{ services.GetGpu() }
	, m_Canvas{ services.GetCanvas() }
	, m_FpsDisplay{ services.GetGpu(), services.GetCanvas(), services.GetWindow() }
	, m_pUnlitRenderer{ Rendering::RendererFactory::CreateUnlitRenderer(services.GetGpu(), false) }
	, m_pSimpleRenderer(Rendering::RendererFactory::CreateSimpleRenderer(services.GetGpu()))
	, m_pTransformRenderer{ new R_LambertCam_Tex_Transform(m_Gpu) }
{
	DebugRenderer::Init(m_Gpu);
	services.GetFpsControl().SetFpsDisplay(m_FpsDisplay);

	CreateArrows();
}

void TowerAppRenderer::Release() const
{
	delete m_pTransformRenderer;
	delete m_pSimpleRenderer;
	delete m_pUnlitRenderer;
	DebugRenderer::Release();
}

void TowerAppRenderer::Render(const Game::FpsCameraController& cameraController)
{
	const Math::Float3& cameraPosition{ cameraController.GetCameraPosition() };
	const XMMATRIX viewProjection{ cameraController.GetViewProjectionMatrix() };

	m_Canvas.BeginPaint();
	m_pTransformRenderer->Render(cameraPosition, viewProjection);
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
