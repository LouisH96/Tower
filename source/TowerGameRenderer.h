#pragma once

#include <Rendering\State\SamplerState.h>

#include <Systems\StaticMeshes\StaticMeshSystem.h>
#include <Systems\Skydome\SkyDomeSystem.h>

namespace TowerGame
{
class TowerGameRenderer
{
public:

	static constexpr unsigned BONES_BUFFER_SIZE{ 1024 };

	TowerGameRenderer();

	void OnCanvasResized(const App::ResizedEvent& event);

	void PreRender();
	void Render();

private:
	//CamBuffers
	Rendering::ConstantBuffer<Rendering::CB_CamMatPosFor> m_CameraMatrixPosBuffer;
	Rendering::ConstantBuffer<Rendering::CB_ModelBuffer> m_ModelBuffer;
	Rendering::ConstantBuffer<Float4X4> m_BonesBuffer;

	//InputLayouts
	Rendering::InputLayout m_Il_V_PosCol;
	Rendering::InputLayout m_Il_V_PosNorUv;
	Rendering::InputLayout m_Il_V_PosNorUvSkin;
	Rendering::InputLayout m_Il_V_PosNorUv_I_ModelMatrix;
	Rendering::InputLayout m_Il_V_PosNorCol;
	Rendering::InputLayout m_Il_V_PosColUv;
	Rendering::InputLayout m_Il_V_PosCol4Uv;

	//RasterizerStates
	Rendering::RasterizerState m_Culling_Back;
	Rendering::RasterizerState m_Culling_Front;
	Rendering::RasterizerState m_Culling_None;

	//Shaders
	Rendering::Shader m_Shader_Terrain;
	Rendering::Shader m_Shader_Entity;
	Rendering::Shader m_Shader_Weapon;
	Rendering::Shader m_Shader_StaticMesh;
	Rendering::Shader m_Shader_Tracer;

	//DepthStencilStates
	Rendering::DepthStencilState m_DepthStencilState_On;
	Rendering::DepthStencilState m_DepthStencilState_Off;

	//Samplers
	Rendering::SamplerState m_Sampler;
};
}
