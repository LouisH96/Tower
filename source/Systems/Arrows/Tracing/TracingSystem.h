#pragma once
#include <Rendering\Structs\VertexTypes.h>
#include <DataStructures\InvalidateList.h>

namespace TowerGame
{
class TracingSystem
{
public:
	TracingSystem();

	void Add(const Float3& origin, const Float2& forwardXz, const Float2& force, const Float4X4& transform);
	void Update(unsigned idx, const Float4X4& transform, const Float3& velocity);

	void Render();

private:
	static constexpr unsigned NR_SIDES{ 3 };
	static constexpr unsigned NR_SIDE_TRIANGLES{ 2 };
	static constexpr float THICKNESS{ .1f };

	struct Index {
		unsigned Index;

		bool IsValid() const { return Index != static_cast<unsigned>(-1); }
		void Invalidate() { Index = static_cast<unsigned>(-1); }
	};

	const float m_AngleDistInterval{ .4f }; //Change in arrow_angle * velocity (if reached spawn a new point)
	using Vertex = Rendering::V_PosCol;

	struct TraceData {
		Float4X4 PrevTrans;
		unsigned PrevSliceIdx{}; //Index of first vertex
		unsigned CurrSliceIdx{};
		float AngleDistSum{}; //Change in arrow angle * velocity
	};
	List<TraceData> m_Tracers{};

	InvalidateList<Vertex> m_Vertices;
	InvalidateList<Index> m_Indices;

	Rendering::VertexIdxList<Vertex> m_DrawData;

	unsigned AddVertices(unsigned nrSlices = 1);
	void SetVertices(unsigned iVertex, const Float4X4& transform);
	void ConnectSlices(unsigned iFirstSlice, unsigned iSecSlice);
};
}