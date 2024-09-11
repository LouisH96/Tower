#pragma once

#include <Rendering\State\Shader.h>
#include <Rendering\State\InputLayout.h>
#include <Rendering\Buffers\Buffer.h>

namespace TowerGame
{
class SkyDomeRenderer
{
public:
	SkyDomeRenderer();

	void Render();

private:
	using Vertex = Rendering::V_Pos2;
	static constexpr ModelTopology TOPOLOGY{ ModelTopology::TriangleList };

	Rendering::Shader m_Shader;
	Rendering::InputLayout m_InputLayout;
	Rendering::Buffer<Vertex> m_VertexBuffer;
	Rendering::DepthStencilState m_DepthStencil;

	void InitVertexBuffer();
};
}

