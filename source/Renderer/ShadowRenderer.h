#pragma once

namespace TowerGame
{
	class ShadowRenderer
	{
	public:
		ShadowRenderer();

		void Render();

	private:

		static const Float3 m_LightDir;

	};
}
