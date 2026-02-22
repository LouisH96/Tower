#pragma once

#include <Math\Matrices.h>
#include <Rendering\State\DepthStencilBuffer.h>
#include <Rendering\State\Viewport.h>

namespace TowerGame
{
class ShadowSystem
{
public:
	static constexpr unsigned NR_MAPS{ 3 };
	static const Float3 m_LightDir;
	static constexpr unsigned NR_SLICES{ 3 };
	static constexpr float SLICE_END[]{
		5.f,15.f,50.f
	};

	ShadowSystem();

	void Init(const Camera& camera);
	void Update();

	void BeginCreateAny();
	void BeginCreate(unsigned iMap);

	void BeginRender();
	void EndRender();

	Rendering::DepthStencilBuffer& GetDepthStencilBuffer(unsigned iMap) { return m_DepthStencil[iMap]; }
	const Float4X4& GetView() const { return m_View; }
	const Float4X4& GetViewProj(unsigned iMap) const { return m_ViewProj[iMap]; }
	const Float3& GetPrevForward() const { return m_PrevForward; }

private:
	struct LightBuffer {
		Float4X4 ViewProjection[NR_MAPS];
		Float3 Forward;
		float Padding[13];
	};

	//Constant Values
	static constexpr float SHADOW_HEIGHT{ 20.f };
	static const Float2 m_TextureSize;
	Float4X4 m_View;
	Float3 m_PlaneSize; //Half the size of the view-plane at distance 1
	float m_PlaneRadius{};
	float m_PlaneCenterDepth{};
	float m_CosExtraAngle{ std::cosf(5.f * Constants::TO_RAD) };

	//DirectX
	Rendering::DepthStencilBuffer m_DepthStencil[NR_MAPS];
	Rendering::Texture m_ShadowMap[NR_MAPS];
	Rendering::Viewport m_Viewport;
	Rendering::ConstantBuffer<LightBuffer> m_LightBuffer;
	Rendering::SamplerState m_Sampler;

	//Matrices
	Float4X4 m_ViewProj[NR_MAPS];
	Float3 m_Offsets[NR_SLICES];
	Float3 m_Scales[NR_SLICES];
	Float3 m_PrevForward;

	void CalculatePlaneSize();
};
}