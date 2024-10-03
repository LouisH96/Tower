#pragma once

#include <Rendering\State\DepthStencilBuffer.h>
#include <Rendering\State\Viewport.h>

namespace TowerGame
{
	class ShadowRenderer
	{
	public:
		ShadowRenderer();

		void PrepareRendering();

		Rendering::DepthStencilBuffer& GetDepthStencilBuffer() { return m_DepthStencil; }

		void SetTargetPos(const Float3& pos);

		Float4X4 MakeViewProjection() const { return m_View * m_Projection; }

	private:
		static const Float3 m_LightDir;
		static const Float2 m_TextureSize;
		
		static Float4X4 MakeProjectionMatrix();

		Rendering::DepthStencilBuffer m_DepthStencil;
		Rendering::Viewport m_Viewport;
		Float4X4 m_View;
		Float4X4 m_Projection;
	};
}
