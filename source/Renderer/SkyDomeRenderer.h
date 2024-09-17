#pragma once

#include <Rendering\State\Shader.h>
#include <Rendering\State\InputLayout.h>
#include <Rendering\Buffers\Buffer.h>
#include <Rendering\State\SamplerState.h>

namespace TowerGame
{
class SkyDomeRenderer
{
public:
	SkyDomeRenderer();

	void OnCanvasResized(const App::ResizedEvent& event);

	void Render();

private:
	using Vertex = Rendering::V_Pos2;
	static constexpr ModelTopology TOPOLOGY{ ModelTopology::TriangleList };

	struct DomeBuffer
	{
		Float2 FovTan;
		float PitchCos;
		float PitchSin;

		float Yaw; //radians
		Float3 Padding;
	} m_DomeBuffer{};

	Rendering::Shader m_Shader;
	Rendering::InputLayout m_InputLayout;
	Rendering::Buffer<Vertex> m_VertexBuffer;
	Rendering::DepthStencilState m_DepthStencil;
	Rendering::Texture m_Texture;
	Rendering::SamplerState m_Sampler;
	Rendering::ConstantBuffer<DomeBuffer> m_PanelBuffer;

	void InitVertexBuffer();
	void InitDomeBuffer();

	void UpdateDomeBufferCameraData();
};
}

