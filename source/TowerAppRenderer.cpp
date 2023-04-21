#include "pch.h"
#include "TowerAppRenderer.h"

#include <App/FpsControl.h>
#include <Rendering/FpsDisplay.h>
#include <Rendering/Canvas.h>
#include <Debug/DebugRenderer.h>
#include <Math/Cube.h>
#include <Generation/Shapes.h>
#include <Generation/Shapes/ArrowGenerator.h>

TowerAppRenderer::TowerAppRenderer(App::Wrappers::Win32::Window& window, Rendering::Gpu& gpu,
	Rendering::Canvas& canvas, App::FpsControl& fpsControl, Game::Camera& camera)
	: m_Window{ window }
	, m_Gpu{ gpu }
	, m_Canvas{ canvas }
	, m_FpsDisplay{ gpu, canvas, window }
	, m_Camera{ camera }
	, m_pUnlitRenderer{ Rendering::RendererFactory::CreateUnlitRenderer(gpu, camera, false) }
	, m_pSimpleRenderer(Rendering::RendererFactory::CreateSimpleRenderer(gpu, camera))
{
	DebugRenderer::Init(m_Gpu, m_Camera);
	fpsControl.SetFpsDisplay(m_FpsDisplay);

	CreateCube();
	CreateArrows();
}

void TowerAppRenderer::Release() const
{
	delete m_pSimpleRenderer;
	delete m_pUnlitRenderer;
	DebugRenderer::Release();
}

void TowerAppRenderer::Render()
{
	m_Canvas.BeginPaint();
	m_pSimpleRenderer->Render();
	m_pUnlitRenderer->Render();
	DebugRenderer::Render();
	m_FpsDisplay.Render();
	m_Canvas.Present();
}

void TowerAppRenderer::CreateCube() const
{
	using namespace Math;
	Array<Float3> positions{};
	Array<Float3> normals{};
	Array<int> indices{};
	const Cube cube{ {0,0,0},1.f };
	Generation::Shapes::GenerateCubeBuffers(cube, positions, normals, indices);
	Array<Rendering::RendererFactory::UnlitRenderer::VertexType> vertices{ positions.GetSize() };
	for (int i = 0; i < vertices.GetSize(); i++)
	{
		Float3 color;
		if (i / 8 == 0) color = { 1,0,0 };
		else if (i / 8 == 1) color = { 0,1,0 };
		else color = { 0,0,1 };
		vertices[i] = { {positions[i].x, positions[i].y, positions[i].z }, color };
	}
	m_pUnlitRenderer->AddMesh(vertices, indices);
}

void TowerAppRenderer::CreateArrows() const
{
	Array<Rendering::V_PosColNorm> pivotVertices{};
	Array<int> pivotIndices{};
	Generation::ArrowGenerator::CreatePivotArrows(pivotVertices, pivotIndices, 16, { 0,0,0 });
	m_pSimpleRenderer->AddMesh(pivotVertices, pivotIndices);

}
