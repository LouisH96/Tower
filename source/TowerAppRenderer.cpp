#include "pch.h"
#include "TowerAppRenderer.h"

#include <App/FpsControl.h>
#include <Rendering/FpsDisplay.h>
#include <Rendering/Canvas.h>
#include <Debug/DebugRenderer.h>
#include <Math/Cube.h>
#include <Generation/Shapes/ArrowGenerator.h>
#include <Rendering/Renderers/R_LambertCam_Tex_Transform.h>
#include <Rendering/Renderers/R_LambertCam_Tex.h>
#include <Framework/CoreServices.h>
#include <Game/Camera/FpsCameraController.h>
#include <Rendering/Renderers/R_LambertLight_Tex.h>

using namespace Math;
using namespace DirectX;
using namespace Rendering;

TowerAppRenderer::TowerAppRenderer(const Framework::CoreServices& services)
	: m_Window{ services.Window }
	, m_Gpu{ services.Gpu }
	, m_Canvas{ services.Canvas }
	, m_FpsDisplay{ services.Gpu, services.Canvas, services.Window }
	, m_pUnlitRenderer{RendererFactory::CreateUnlitRenderer(services.Gpu, false) }
	, m_pSimpleRenderer(RendererFactory::CreateSimpleRenderer(services.Gpu))
	, m_pTransformRenderer{ new R_LambertCam_Tex_Transform(m_Gpu) }
	, m_pTerrainRenderer{new TerrainRenderer(m_Gpu, false)}
{
	DebugRenderer::Init(m_Gpu);
	services.FpsControl.SetFpsDisplay(m_FpsDisplay);

	CreateArrows();
}

void TowerAppRenderer::Release() const
{
	delete m_pTerrainRenderer;
	delete m_pTransformRenderer;
	delete m_pSimpleRenderer;
	delete m_pUnlitRenderer;
	DebugRenderer::Release();
}

void TowerAppRenderer::Render(const Game::FpsCameraController& cameraController)
{
	const Float3& cameraPosition{ cameraController.GetCameraPosition() };
	const XMMATRIX viewProjection{ cameraController.GetViewProjectionMatrix() };

	m_Canvas.BeginPaint();
	m_pTerrainRenderer->Render(cameraPosition, viewProjection);
	m_pTransformRenderer->Render(cameraPosition, viewProjection);
	m_pSimpleRenderer->Render(cameraPosition, viewProjection);
	m_pUnlitRenderer->Render(cameraPosition, viewProjection);
	DebugRenderer::Render(cameraPosition, viewProjection);
	m_FpsDisplay.Render();
	m_Canvas.Present();
}

void TowerAppRenderer::CreateArrows() const
{
	Array<V_PosColNorm> pivotVertices{};
	Array<int> pivotIndices{};
	Generation::ArrowGenerator::CreatePivotArrows(pivotVertices, pivotIndices, 16, { 0,0,0 });
	m_pSimpleRenderer->AddMesh(pivotVertices, pivotIndices);
}
