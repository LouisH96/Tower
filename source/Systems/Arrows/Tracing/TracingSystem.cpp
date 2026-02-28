#include "pch.h"
#include "TracingSystem.h"

#include <Math\Math.h>
#include <Transform\WorldMatrix.h>

using namespace TowerGame;

TracingSystem::TracingSystem()
	: m_Vertices{}
	, m_Indices{ 50 * NR_SIDES * NR_SIDE_TRIANGLES * Triangle::NR_POINTS }
{
	for (unsigned iSide{ 0 }; iSide < NR_SIDES; ++iSide)
	{
		const float angle{ Constants::PI2 / NR_SIDES * iSide };
		m_SideCosSin[iSide * 2 + 0] = cosf(angle);
		m_SideCosSin[iSide * 2 + 1] = sinf(angle);
	}
}

void TracingSystem::Add(const Float3& origin, const Float2& forwardXz, const Float2& force, const Float4X4& transform)
{
	TraceData data{};
	data.SpawnTime = Globals::Time + DELAY;

	m_Tracers.Add(data);
}

void TracingSystem::UpdateHead(unsigned idx, const Float4X4& transform, const Float3& velocity)
{
	//Get
	TraceData& data{ m_Tracers.Get(idx) };

	if (Globals::Time < data.SpawnTime)
		return;

	//Add Slices?
	if (data.Slices.GetSize() < 2)
	{
		TraceSlice& slice{ data.Slices.AddEmpty() };
		slice.IVertex = AddVertices();
		slice.Time = Globals::Time;
		SetSlicePositions(slice, transform);
		return;
	}

	//Move Head
	TraceSlice* pCurrSlice{ &data.Slices.Last() };
	pCurrSlice->Time = Globals::Time;
	SetSlicePositions(*pCurrSlice, transform);

	//New Slice?
	TraceSlice* pPrevSlice{ &data.Slices.GetReverse(1) };
	if (Globals::Time > pPrevSlice->Time + SLICE_TIME_INTERVAL)
	{
		TraceSlice& newSlice{ data.Slices.AddEmpty() };
		newSlice.Time = Globals::Time;
		newSlice.IVertex = AddVertices();
		pCurrSlice = &data.Slices.GetReverse(1);
		CopyVerticesTo(pCurrSlice->IVertex, newSlice.IVertex);
		newSlice.Center = pCurrSlice->Center;
	}
}

void TracingSystem::UpdateTracers()
{
	for (unsigned iTrace{ 0 }; iTrace < m_Tracers.GetLocalSize(); ++iTrace)
	{
		TraceData& trace{ m_Tracers.GetLocal(iTrace) };

		if (!trace.IsValid())
			continue;
		if (trace.SpawnTime + DELAY > Globals::Time)
			continue;

		if (!ProgressTail(trace))
		{
			m_Tracers.RemoveLocal(iTrace);
			iTrace--;
			continue;
		}
		UpdateSizeAndAlpha(trace);
	}
}

void TracingSystem::Render()
{
	if (m_Tracers.IsEmpty())
		return;

	//Count Indices
	unsigned nrIndices{ 0 };
	for (unsigned iTrace{ 0 }; iTrace < m_Tracers.GetLocalSize(); ++iTrace)
	{
		const TraceData& tracer{ m_Tracers.GetLocal(iTrace) };
		if (tracer.Slices.IsEmpty())
			continue;
		const unsigned nrSections{ m_Tracers.GetLocal(iTrace).Slices.GetSize() - 1 };
		nrIndices += nrSections * NR_SIDES * NR_SIDE_TRIANGLES * Triangle::NR_POINTS;
	}
	m_Indices.EnsureCapacityNoCopy(nrIndices, true);

	//Create Indices on GPU
	int* pIndex{ m_Indices.BeginCopyData() };
	for (unsigned iTrace{ 0 }; iTrace < m_Tracers.GetLocalSize(); ++iTrace)
	{
		const TraceData& trace{ m_Tracers.GetLocal(iTrace) };

		for (unsigned iSlice{ 1 }; iSlice < trace.Slices.GetSize(); ++iSlice)
		{
			const unsigned iPrev{ trace.Slices[iSlice - 1].IVertex };
			const unsigned iCurr{ trace.Slices[iSlice].IVertex };
			ConnectSlices(iPrev, iCurr, pIndex);
		}
	}

	//Activate & Draw
	m_Indices.EndCopyData();
	m_Indices.Activate();
	m_Vertices.ActivateFromStart();
	m_Indices.Draw(nrIndices);
}

unsigned TracingSystem::AddVertices(unsigned nrSlices)
{
	Vertex vertex{};
	vertex.Color = Float4{ Float3{GRAY_COLOR}, 0 };

	return m_Vertices.GetCpuData().AddContinuous(vertex, NR_SIDES * nrSlices);
}

void TracingSystem::SetSlicePositions(TraceSlice& slice, const Float4X4& transform)
{
	const Float3 right{ WorldMatrix::GetRight(transform) * THICKNESS };
	const Float3 up{ WorldMatrix::GetUp(transform) * THICKNESS };
	const Float3 pos{ WorldMatrix::GetPosition(transform) };
	slice.Center = pos;

	//Vertices
	Vertex* pVertex{ &m_Vertices.Get(slice.IVertex) };

	for (unsigned iSide{ 0 }; iSide < NR_SIDES; ++iSide)
	{
		pVertex->Pos = pos
			+ right * m_SideCosSin[iSide * 2 + 0]
			+ up * m_SideCosSin[iSide * 2 + 1];
		pVertex->Uv.y = static_cast<float>(iSide % 2);
		++pVertex;
	}
}

void TracingSystem::CopyVerticesTo(unsigned iSource, unsigned iTarget)
{
	InvalidateList<Vertex>& vertices{ m_Vertices.GetCpuData() };
	std::copy(&vertices.Get(iSource), &vertices.Get(iSource + NR_SIDES), &vertices.Get(iTarget));
}

void TracingSystem::ConnectSlices(unsigned iFirstSlice, unsigned iSecSlice, int*& pIndices)
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

		*pIndices++ = rb;
		*pIndices++ = lb;
		*pIndices++ = lt;

		*pIndices++ = rb;
		*pIndices++ = lt;
		*pIndices++ = rt;
	}
}

void TracingSystem::UpdateSizeAndAlpha(TraceData& tracer)
{
	const float prevAlpha{ m_Vertices[tracer.Slices.First().IVertex].Color.w };
	const float newAlpha{ Float::Min(prevAlpha + ALPHA_GAIN * Globals::DeltaTime, ALPHA_MAX) };

	for (unsigned iSlice{ 0 }; iSlice < tracer.Slices.GetSize(); ++iSlice)
	{
		TraceSlice& slice{ tracer.Slices[iSlice] };
		for (unsigned iVertex{ slice.IVertex }; iVertex < slice.IVertex + NR_SIDES; ++iVertex)
		{
			Vertex& vertex{ m_Vertices[iVertex] };
			Float3 diff{ vertex.Pos - slice.Center };
			diff *= 1 + THICKNESS_GAIN * Globals::DeltaTime;
			vertex.Pos = slice.Center + diff;
			vertex.Color.w = newAlpha;
		}
	}
}

bool TracingSystem::ProgressTail(TraceData& trace)
{
	List<TraceSlice>& slices{ trace.Slices };
	const float tailTime{ Globals::Time - LIFETIME };

	unsigned iSlice{ 1 };
	for (; iSlice < slices.GetSize(); ++iSlice)
	{
		TraceSlice& slice{ slices[iSlice] };

		if (slice.Time <= tailTime)
			continue;

		//Move previous slice
		TraceSlice& prevSlice{ slices[iSlice - 1] };
		const float alpha{ Math::Unlerp(prevSlice.Time, slice.Time, tailTime) };
		const Float3 diff{
			(slice.Center -
			prevSlice.Center) * alpha };

		for (unsigned iVertex{ prevSlice.IVertex }; iVertex < prevSlice.IVertex + NR_SIDES; ++iVertex)
		{
			Vertex& vertex{ m_Vertices[iVertex] };
			vertex.Pos += diff;
		}
		prevSlice.Time = tailTime;
		prevSlice.Center += diff;
		iSlice--;
		break;
	}

	//-- Remove slices & vertices
	const unsigned slicesToRemove{ Uint::Min(iSlice, slices.GetSize()) };

	//remove vertices of slices that will be removed
	for (unsigned iRemoveSlice{ 0 }; iRemoveSlice < slicesToRemove; ++iRemoveSlice)
	{
		TraceSlice& removeSlice{ trace.Slices[iRemoveSlice] };
		for (unsigned iRemoveVertex{ removeSlice.IVertex }; iRemoveVertex < removeSlice.IVertex + NR_SIDES; ++iRemoveVertex)
			m_Vertices.Remove(iRemoveVertex);
	}

	//remove slices
	trace.Slices.ShrinkFromBegin(slicesToRemove);
	if (trace.Slices.IsEmpty())
		return false;
	return true;
}
