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

TowerAppRenderer::TowerAppRenderer(const Framework::AppServices& appServices)
	: m_Window{ appServices.Window }
	, m_Gpu{ appServices.Gpu }
	, m_Canvas{ appServices.Canvas }
	, m_FpsDisplay{ appServices.Gpu, appServices.Canvas, appServices.Window}
	, m_Camera{ appServices.Camera }
	, m_pUnlitRenderer{ Rendering::RendererFactory::CreateUnlitRenderer(appServices.Gpu, appServices.Camera, false) }
	, m_pSimpleRenderer(Rendering::RendererFactory::CreateSimpleRenderer(appServices.Gpu, appServices.Camera))
	, m_pTransformRenderer(new TransformRenderer(appServices.Gpu, appServices.Camera))
{
	DebugRenderer::Init(m_Gpu, m_Camera);
	appServices.FpsControl.SetFpsDisplay(m_FpsDisplay);

	//CreateCube();
	CreateArrows();
	CreateBow();
}

void TowerAppRenderer::Release() const
{
	delete m_pBowTransform;
	delete m_pBowTexture;
	delete m_pBowMesh;

	delete m_pTransformRenderer;
	delete m_pSimpleRenderer;
	delete m_pUnlitRenderer;
	DebugRenderer::Release();
}

void TowerAppRenderer::Render()
{
	m_pBowTransform->Position.z += 1.f * Game::GameGlobals::GetDeltaTime();

	m_Canvas.BeginPaint();
	m_pTransformRenderer->Render();
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

void TowerAppRenderer::CreateBow()
{
	//MESH
	const std::wstring meshPath{ Framework::Resources::GetLocalResourcePath(L"SM_Prop_Sled_02.fbx") };
	Io::Fbx::FbxClass fbxModel{ meshPath };
	Io::Fbx::FbxClass::Geometry& geom = fbxModel.GetGeometries()[0];

	Array<TransformRenderer::Vertex> vertices{ geom.Points.GetSize()};
	for (int i = 0; i < geom.Points.GetSize(); i++)
		vertices[i] = TransformRenderer::Vertex{ geom.Points[i] * 0.01f, geom.Normals[i], geom.Uvs[i] };

	m_pBowMesh = Rendering::Mesh::Create(m_Gpu, vertices);

	//TEXTURE
	const std::wstring texturePath{ Framework::Resources::GetLocalResourcePath(L"PolygonSnow_Texture_01_A.png") };
	m_pBowTexture = new Rendering::Texture(m_Gpu, texturePath);

	//TRANSFORM
	m_pBowTransform = new Game::Transform();

	//ADD ENTRY²
	m_pTransformRenderer->AddEntry(*m_pBowMesh, *m_pBowTexture, *m_pBowTransform);
}
