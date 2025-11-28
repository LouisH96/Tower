#include "pch.h"
#include "TracingSystem.h"

#include <Transform\WorldMatrix.h>

using namespace TowerGame;

TracingSystem::TracingSystem()
	: m_Vertices{}
{
}

void TracingSystem::Add(const Float3& origin, const Float2& forwardXz, const Float2& force, const Float4X4& transform)
{
	TraceData data{};
	data.PrevTrans = transform;
	data.PrevSliceIdx = AddVertices(2);
	data.CurrSliceIdx = data.PrevSliceIdx + NR_SIDES;
	m_Tracers.Add(data);

	SetVertices(data.PrevSliceIdx, transform);
	ConnectSlices(data.PrevSliceIdx, data.CurrSliceIdx);
}

void TracingSystem::Update(unsigned idx, const Float4X4& transform, const Float3& velocity)
{
	TraceData& data{ m_Tracers[idx] };

	const Float3 prevUp{ WorldMatrix::GetUp(data.PrevTrans) };
	const float velDot{ abs(velocity.Dot(prevUp)) };
	const float forwardDor{ abs(WorldMatrix::GetForward(transform).Dot(prevUp)) };

	data.AngleDistSum += velDot * forwardDor;

	if (data.AngleDistSum > m_AngleDistInterval)
	{
		data.PrevSliceIdx = data.CurrSliceIdx;
		data.CurrSliceIdx = AddVertices();
		data.AngleDistSum = 0;
		SetVertices(data.CurrSliceIdx, transform);
		ConnectSlices(data.PrevSliceIdx, data.CurrSliceIdx);
		data.PrevTrans = transform;
		//DebugRenderer::AddRay(WorldMatrix::GetPosition(transform), WorldMatrix::GetUp(transform) * 5, Color::Blue);
	}
	else
	{
		SetVertices(data.CurrSliceIdx, transform);
	}
}

void TracingSystem::Render()
{
	List<Vertex>& vertices{ m_DrawData.GetVertices() };
	List<int>& indices{ m_DrawData.GetIndices() };

	vertices.Clear();
	for (unsigned iVertex{ 0 }; iVertex < m_Vertices.GetEndIdx(); ++iVertex)
	{
		Vertex vertex{ m_Vertices.Get(iVertex) };
		if (!vertex.IsValid())
			vertex.Pos = {};
		vertices.Add(vertex);
	}

	indices.Clear();
	for (unsigned iIndex{ m_Indices.GetFirstIdx() }; iIndex < m_Indices.GetEndIdx(); ++iIndex)
	{
		const Index& index{ m_Indices.Get(iIndex) };
		if (index.IsValid())
			indices.Add(index.Index);
	}

	m_DrawData.Draw();
}

unsigned TracingSystem::AddVertices(unsigned nrSlices)
{
	Vertex vertex{};
	vertex.Color = Color::Red;
	return m_Vertices.AddContinuous(vertex, NR_SIDES * nrSlices);
}

void TracingSystem::SetVertices(unsigned iVertex, const Float4X4& transform)
{
	const Float3 right{ WorldMatrix::GetRight(transform) * THICKNESS };
	const Float3 up{ WorldMatrix::GetUp(transform) * THICKNESS };
	const Float3 pos{ WorldMatrix::GetPosition(transform) };

	//Vertices
	Vertex* pVertex{ &m_Vertices.Get(iVertex) };

	for (unsigned iSide{ 0 }; iSide < NR_SIDES; ++iSide)
	{
		const float angle{ Constants::PI2 / NR_SIDES * iSide };
		const float cos{ cosf(angle) };
		const float sin{ sinf(angle) };

		pVertex->Pos = pos + right * cos + up * sin;
		++pVertex;
	}
}

void TracingSystem::ConnectSlices(unsigned iFirstSlice, unsigned iSecSlice)
{
	for (unsigned iSide{ 0 }; iSide < NR_SIDES; ++iSide)
	{
		const unsigned right{ iSide };
		const unsigned left{ (right + 1) % NR_SIDES };
		const unsigned bot{ iFirstSlice };
		const unsigned top{ iSecSlice };

		const unsigned rb{ right + bot };
		const unsigned rt{ right + top };
		const unsigned lb{ left + bot };
		const unsigned lt{ left + top };

		m_Indices.AddContinuous({ rb }, { lb }, { lt });
		m_Indices.AddContinuous({ rb }, { lt }, { rt });
	}
}
