#pragma once
#include <DataStructures\InvalidateList.h>
#include <DataStructures\OffsetList.h>
#include <DataStructures\StackArray.h>
#include <Rendering\Buffers\IdxBuffer.h>
#include <Rendering\Buffers\InvalidateBuffer.h>
#include <Rendering\Structs\VertexTypes.h>

namespace TowerGame
{
class TracingSystem
{
public:
	TracingSystem();

	void Add(const Float3& origin, const Float2& forwardXz, const Float2& force, const Float4X4& transform);

	void UpdateHead(unsigned idx, const Float4X4& transform, const Float3& velocity);
	void UpdateTracers();

	void Render();

private:
	static constexpr unsigned NR_SIDES{ 2 * 2 }; //Needs to be even
	static constexpr unsigned NR_SIDE_TRIANGLES{ 2 };
	static constexpr float SLICE_TIME_INTERVAL{ .05f };
	static constexpr float THICKNESS{ .04f };
	static constexpr float THICKNESS_GAIN{ 10.75f };
	static constexpr float LIFETIME{ .15f };

	static constexpr float GRAY_COLOR{ .8f };
	static constexpr float DELAY{ .10f };
	static constexpr float ALPHA_MAX{ .3f };
	static constexpr float ALPHA_GAIN{ ALPHA_MAX / 2.5f };

	using Vertex = Rendering::V_PosCol4Uv;

	struct Index {
		unsigned Index;

		bool IsValid() const { return Index != static_cast<unsigned>(-1); }
		void Invalidate() { Index = static_cast<unsigned>(-1); }
	};
	struct TraceSlice {
		Float3 Center;
		unsigned IVertex;
		float Time;
	};
	struct TraceData {
		float SpawnTime{};
		List<TraceSlice> Slices{};

		bool IsValid() const { return SpawnTime != -1; }
		void Invalidate() { SpawnTime = -1; }
	};

	StackArray<float, NR_SIDES * 2> m_SideCosSin{};

	OffsetList<TraceData> m_Tracers{};
	unsigned m_ArrowIdxOffset{ 0 };

	InvalidateBuffer<Vertex> m_Vertices;
	IdxBuffer m_Indices;

	unsigned AddVertices(unsigned nrSlices = 1);
	void SetSlicePositions(TraceSlice& slice, const Float4X4& transform);
	void CopyVerticesTo(unsigned iSource, unsigned iTarget);
	void ConnectSlices(unsigned iFirstSlice, unsigned iSecSlice, int*& pIndices);

	void UpdateSizeAndAlpha(TraceData& tracer);
	bool ProgressTail(TraceData& tracer); //returns false if trace needs to be removed
};
}