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

class ArrowRenderer
{
public:
	ArrowRenderer();
	~ArrowRenderer() = default;

	void SetSize(int size);
	void UpdateData(int idx, const Transform& transform, const Camera& camera);
	void Render(const Camera& camera);

private:
	using Vertex = Rendering::V_PosNorUv;
	using Instance = Rendering::I_ModelMatrices;

	Rendering::BlendState m_BlendState;
	Rendering::RasterizerState m_RasterizerState;
	Rendering::SamplerState m_Sampler;
	Rendering::DepthStencilState m_DepthStencilState;

	Rendering::InputLayout m_InputLayout;
	Rendering::Shader m_Shader;
	Rendering::Texture m_Texture;

	Rendering::ConstantBuffer<Rendering::CB_CamPos> m_CameraBuffer;
	Rendering::InstanceList<Vertex, Instance> m_Instances;
};
