#include "pch.h"
#include "TowerGameRenderer.h"

#include <Rendering\Renderers\R_LambertCam_Tex_Tran_Inst.h>
#include <Rendering\Renderers\Texture2DRenderer.h>

#include <Systems\Arrows\ArrowSystem.h>
#include <Systems\Bow\Bow.h>
#include <Systems\Enemies\EnemySystem.h>
#include <TowerApp.h>

using namespace TowerGame;
using namespace Rendering;

TowerGameRenderer::TowerGameRenderer()
	: m_Shader_Entity{ Resources::Local(L"Entity.hlsl") }
	, m_Shader_Weapon{ Resources::Local(L"Weapon.hlsl") }
	, m_Shader_Terrain{ Resources::Local(L"Terrain.hlsl") }
	, m_Shader_StaticMesh{ Resources::Local(L"StaticMesh.hlsl") }
	, m_Il_V_PosNorUv_I_ModelMatrix{ InputLayout::FromTypes<V_PosNorUv, I_ModelMatrix>() }
	, m_Il_V_PosNorCol{ InputLayout::FromType<V_PosNorCol>() }
	, m_Il_V_PosNorUv{ InputLayout::FromType<V_PosNorUv>() }
	, m_Il_V_PosNorUvSkin{ InputLayout::FromType<V_PosNorUvSkin>() }
	, m_DepthStencilState_On{ true }
	, m_BonesBuffer{ BONES_BUFFER_SIZE }
{
}

void TowerGameRenderer::OnCanvasResized(const App::ResizedEvent& event)
{
	SYSTEMS.Skydome.OnCanvasResized(event);
}

void TowerGameRenderer::PreRender()
{
	//Get
	const Camera& camera{ *Globals::pCamera };

	SYSTEMS.Shadows.BeginShadowMapRender();
	const Float4X4 viewProjection{ SYSTEMS.Shadows.GetLightViewProjection() };

	m_DepthStencilState_On.Activate();

	//Entity
	m_Shader_Entity.Activate<Shader::Function::Vertex>();
	m_CameraMatrixPosBuffer.Update(CB_CamMatPos{ Float3{}, viewProjection });
	m_CameraMatrixPosBuffer.Activate();
	m_Il_V_PosNorUv_I_ModelMatrix.Activate();
	m_BonesBuffer.Activate(3);
	SYSTEMS.Arrows.Render(true);
	SYSTEMS.Enemies
		.Render<Shader::Function::Vertex>(m_BonesBuffer); //Render

	m_Shader_Terrain.Activate<Shader::Function::Vertex>();
	m_Il_V_PosNorCol.Activate();
	SYSTEMS.StaticMeshes.Render();

	SYSTEMS.StaticModels.Render();
}

void TowerGameRenderer::Render()
{
	const Camera& camera{ *Globals::pCamera };
	const Float4X4& viewProjection{ camera.GetViewProjection() };

	SYSTEMS.Skydome.Render(); //Render
	SYSTEMS.Shadows.BeginRender();

	m_DepthStencilState_On.Activate();

	//Terrain
	m_Shader_Terrain.Activate();
	m_Il_V_PosNorCol.Activate();
	m_CameraMatrixPosBuffer.Update({ camera });
	m_CameraMatrixPosBuffer.Activate();
	SYSTEMS.StaticMeshes.Render();

	//Entity
	m_Sampler.Activate();
	m_Il_V_PosNorUv_I_ModelMatrix.Activate();
	m_Shader_Entity.Activate();
	m_CameraMatrixPosBuffer.Activate();
	m_BonesBuffer.Activate(3);
	SYSTEMS.Arrows.Render();
	SYSTEMS.Enemies.Render(m_BonesBuffer);

	//StaticMesh
	m_Shader_StaticMesh.Activate();
	m_Il_V_PosNorUv.Activate();
	SYSTEMS.StaticModels.Render();

	//Weapon
	const Bow& bow{ SYSTEMS.Bow };
	m_Shader_Weapon.Activate();
	m_Il_V_PosNorUvSkin.Activate();
	m_ModelBuffer.Update(CB_ModelBuffer{ bow.GetWorldTransform().AsMatrix() });
	m_ModelBuffer.Activate(1);
	m_BonesBuffer.Update(bow.GetBones());
	SYSTEMS.Bow.Render();

	//Other
	SYSTEMS.pSimpleRenderer->Render();

	SYSTEMS.Shadows.EndRender();
}
