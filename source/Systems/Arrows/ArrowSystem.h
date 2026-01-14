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
#include "Tracing/TracingSystem.h"

namespace TowerGame
{
class ArrowSystem
{
public:
	ArrowSystem();
	~ArrowSystem() = default;

	void Update();
	void Render(bool hideCharging = false);
	void RenderTracers();

	int Spawn();
	void Launch(int arrowIdx, const Float3& launchedPosition);
	void SetArrowTransform(int arrowIdx, const Float4X4& newArrowWorld);
	void SetArrowTransform(int arrowIdx, const Transform& newArrowWorld);

	static constexpr float LAUNCH_SPEED{ 25 };

private:
	using Vertex = Rendering::V_PosNorUv;
	using Instance = Rendering::I_ModelMatrix;

	static constexpr float ARROW_FINISHED = 2000;

	Rendering::Texture m_Texture;
	Rendering::InstanceList<Vertex, Instance> m_Instances;

	List<Float3> m_Velocities;
	List<Float3> m_LaunchedPosition;
	unsigned m_IsCharging{ 0 };

	TracingSystem m_Tracing{};

	static bool IsArrowFinished(const Float3& arrowVelocity);
	void SetArrowFinished(unsigned idx);
};
}
