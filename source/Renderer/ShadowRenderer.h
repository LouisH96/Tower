#pragma once

#include <Rendering\State\DepthStencilBuffer.h>

namespace TowerGame
{
	class ShadowRenderer
	{
	public:
		ShadowRenderer();

		void Render();

		Rendering::DepthStencilBuffer& GetDepthStencilBuffer() { return m_DepthStencil; }

	private:
		static const Float3 m_LightDir;

		Rendering::DepthStencilBuffer m_DepthStencil;
	};
}
