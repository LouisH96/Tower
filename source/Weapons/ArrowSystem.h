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
#include "Rendering/Structs/InstanceTypes.h"
#include "Rendering/Structs/VertexTypes.h"

class ArrowSystem
{
public:
	ArrowSystem();
	~ArrowSystem() = default;

	void Update();
	void Render();

	int Spawn();
	void Launch(int arrowIdx);
	void SetArrowTransform(int arrowIdx, const Transform& newArrowWorld);

private:
	using Vertex = Rendering::V_PosNorUv;
	using Instance = Rendering::I_ModelMatrices;

	static constexpr float ARROW_FINISHED = 2000;

	Rendering::Texture m_Texture;

	Rendering::InstanceList<Vertex, Instance> m_Instances;

	List<Float3> m_Velocities;

	static bool IsArrowFinished(const Float3& arrowVelocity);
	static void SetArrowFinished(Float3& arrowVelocity);
};
