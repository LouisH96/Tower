#pragma once
#include "Rendering/DrawData/InstanceList.h"
#include "Rendering/State/BlendState.h"
#include "Rendering/State/ConstantBuffer.h"
#include "Rendering/State/DepthStencilState.h"
#include "Rendering/State/InputLayout.h"
#include "Rendering/State/RasterizerState.h"
#include "Rendering/State/SamplerState.h"
#include "Rendering/State/Shader.h"
#include "Rendering/State/Texture.h"
#include "Rendering/Structs/ConstantBufferTypes.h"
#include "Rendering/Structs/Instances.h"
#include "Rendering/Structs/VertexTypes.h"

class ArrowSystem
{
public:
	ArrowSystem();
	~ArrowSystem() = default;

	void Update();
	void Render();

	void Spawn(const Transform& bowTransform);
	void SetArrowTransform(int arrowIdx, const Transform& newArrowWorld);

private:
	using Vertex = Rendering::V_PosNorUv;
	using Instance = Rendering::I_ModelMatrices;

	static constexpr float ARROW_FINISHED = 2000;

	Rendering::BlendState m_BlendState;
	Rendering::RasterizerState m_RasterizerState;
	Rendering::SamplerState m_Sampler;
	Rendering::DepthStencilState m_DepthStencilState;

	Rendering::InputLayout m_InputLayout;
	Rendering::Shader m_Shader;
	Rendering::Texture m_Texture;

	Rendering::ConstantBuffer<Rendering::CB_CamPos> m_CameraBuffer;
	Rendering::InstanceList<Vertex, Instance> m_Instances;

	List<Float3> m_Velocities;

	static bool IsArrowFinished(const Float3& arrowVelocity);
	static void SetArrowFinished(Float3& arrowVelocity);
};
