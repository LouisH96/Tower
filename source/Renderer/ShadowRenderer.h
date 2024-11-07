#pragma once

#include <Rendering\State\DepthStencilBuffer.h>
#include <Rendering\State\Viewport.h>

namespace TowerGame
{
	class Character;

	class ShadowRenderer
	{
	public:
		ShadowRenderer();

		void MoveShadow(const Character& character);
		void BeginShadowMapRender();
		void BeginRender();
		void EndRender();

		Rendering::DepthStencilBuffer& GetDepthStencilBuffer() { return m_DepthStencil; }

		const Float4X4& GetLightViewProjection() const { return m_LightBufferData.ViewProjection; }

	private:
		struct LightBuffer
		{
			Float4X4 ViewProjection;
		};

		static constexpr float SHADOW_AREA_SIZE{ 8 };
		static constexpr float LIGHT_TARGET_DISTANCE{ SHADOW_AREA_SIZE };
		static const Float3 m_LightDir;
		static const Float2 m_TextureSize;
		static Float4X4 MakeProjectionMatrix();

		Rendering::DepthStencilBuffer m_DepthStencil;
		Rendering::Viewport m_Viewport;
		Rendering::Texture m_ShadowMap;
		Rendering::ConstantBuffer<LightBuffer> m_LightBuffer;
		Rendering::SamplerState m_Sampler;
		Float4X4 m_View;
		Float4X4 m_Projection;
		LightBuffer m_LightBufferData;
	};
}
