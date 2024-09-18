#pragma once

#include <Rendering\State\DepthStencilBuffer.h>

namespace TowerGame
{
	class ShadowRenderer
	{
	public:
		ShadowRenderer();

		void Render();

	private:
		static const Float3 m_LightDir;

		Rendering::DepthStencilBuffer m_DepthStencil;
	};
}
