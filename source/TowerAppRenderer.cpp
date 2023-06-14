#include "pch.h"
#include "TowerAppRenderer.h"

#include <App/FpsControl.h>
#include <Debug/Rendering/DebugRenderer.h>
#include <Framework/CoreServices.h>
#include <Game/Camera/FpsCameraController.h>
#include <Generation/Shapes/ArrowGenerator.h>
#include <Rendering/Canvas.h>
#include <Rendering/FpsDisplay.h>
#include <Rendering/Renderers/R_LambertCam_Tex_Transform.h>

#include "Framework/Resources.h"
#include "Game/Camera/Camera.h"
#include "Rendering/Image.h"
#include "Rendering/Renderers/Texture2DRenderer.h"

using namespace Math;
using namespace DirectX;
using namespace Rendering;

TowerAppRenderer::TowerAppRenderer(const Framework::CoreServices& services)
	: m_Window{ services.Window }
	, m_Canvas{ services.Canvas }
	, m_FpsDisplay{ services.Canvas, services.Window }
	, m_pUnlitRenderer{ RendererFactory::CreateUnlitRenderer(false) }
	, m_pSimpleRenderer(RendererFactory::CreateSimpleRenderer())
	, m_pTransformRenderer{ new R_LambertCam_Tex_Transform() }
	, m_pTerrainRenderer{ new TerrainRenderer(false) }
	, m_pTextureRenderer{ new TextureRenderer(Framework::Resources::GlobalShader(L"Font_Inst.hlsl")) }
	, m_pTexture2DRenderer{ new Texture2DRenderer() }
{
	DebugRenderer::Init(*Globals::pGpu);
	services.FpsControl.SetFpsDisplay(m_FpsDisplay);

	CreateArrows();
}

void TowerAppRenderer::Release() const
{
	delete m_pTexture2DRenderer;
	delete m_pTextureRenderer;
	delete m_pTerrainRenderer;
	delete m_pTransformRenderer;
	delete m_pSimpleRenderer;
	delete m_pUnlitRenderer;
	DebugRenderer::Release();
}

void TowerAppRenderer::Render(const Game::FpsCameraController& cameraController)
{
	const Float3& cameraPosition{ cameraController.GetPosition() };
	const Float4X4& viewProjection{ cameraController.GetCamera().GetViewProjection() };

	m_Canvas.BeginPaint();
	m_pTerrainRenderer->Render(cameraPosition, viewProjection);
	m_pTransformRenderer->Render(cameraPosition, viewProjection);
	m_pSimpleRenderer->Render(cameraPosition, viewProjection);
	m_pTexture2DRenderer->Render(cameraController.GetCamera());
	m_pUnlitRenderer->Render(cameraPosition, viewProjection);
	m_pTextureRenderer->Render(cameraPosition, viewProjection);
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
