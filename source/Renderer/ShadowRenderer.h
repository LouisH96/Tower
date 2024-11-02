#pragma once

#include <Rendering\State\DepthStencilBuffer.h>
#include <Rendering\State\Viewport.h>

namespace TowerGame
{
	class ShadowRenderer
	{
	public:
		ShadowRenderer();

		void BeginShadowMapRender();
		void BeginRender();
		void EndRender();

		Rendering::DepthStencilBuffer& GetDepthStencilBuffer() { return m_DepthStencil; }

		void SetTargetPos(const Float3& pos);

		const Float4X4& GetLightViewProjection() const { return m_LightBufferData.ViewProjection; }

	private:
		struct LightBuffer
		{
			Float4X4 ViewProjection;
		};

		static const Float3 m_LightDir;
		static const Float2 m_TextureSize;
		
		static Float4X4 MakeProjectionMatrix();

		Rendering::DepthStencilBuffer m_DepthStencil;
		Rendering::Viewport m_Viewport;
		Rendering::Texture m_ShadowMap;
		Rendering::ConstantBuffer<LightBuffer> m_LightBuffer;
		Float4X4 m_View;
		Float4X4 m_Projection;
		LightBuffer m_LightBufferData;
	};
}
