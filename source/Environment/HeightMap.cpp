#include "pch.h"
#include "HeightMap.h"

HeightMap::HeightMap(Int2 nrElements, float initHeight, const Float2& size)
	: m_Grid{ nrElements, initHeight }
	, m_Size{ size }
{
}

void HeightMap::AddSinWaveX(float wavePeriod, float waveMagnitude)
{
	const Float2 cellSize{ GetCellSize() };
	for (int iRow = 0, idx = 0; iRow < m_Grid.GetNrRows(); iRow++)
		for (int iCol = 0; iCol < m_Grid.GetNrCols(); iCol++, idx++)
			m_Grid.Get(idx) += SinFunction(wavePeriod, waveMagnitude, iCol * cellSize.x);
}

void HeightMap::AddSinWaveY(float wavePeriod, float waveMagnitude)
{
	const Float2 cellSize{ GetCellSize() };
	for (int iRow = 0, idx = 0; iRow < m_Grid.GetNrRows(); iRow++)
		for (int iCol = 0; iCol < m_Grid.GetNrCols(); iCol++, idx++)
			m_Grid.Get(idx) += SinFunction(wavePeriod, waveMagnitude, iRow * cellSize.y);
}

void HeightMap::AddSinWave(float wavePeriod, float waveMagnitude)
{
	const Float2 cellSize{ GetCellSize() };
	int idx = 0;
	for (int iRow = 0; iRow < m_Grid.GetNrRows(); iRow++)
	{
		for (int iCol = 0; iCol < m_Grid.GetNrCols(); iCol++)
		{
			const float xPeriodTime{ iCol * cellSize.x / wavePeriod };
			const float xAngle{ xPeriodTime * Constants::PI2 };
			const float xHeight{ sinf(xAngle) * waveMagnitude };

			const float yPeriodTime{ iRow * cellSize.y / wavePeriod };
			const float yAngle{ yPeriodTime * Constants::PI2 };
			const float yHeight{ sinf(yAngle) * waveMagnitude };

			m_Grid.Get(idx) += (xHeight + yHeight) / 2;
			idx++;
		}
	}
}

void HeightMap::Scale(float scale)
{
	for (int i = 0; i < m_Grid.GetNrElements(); i++)
		m_Grid.Get(i) *= scale;
}

void HeightMap::Divide(float scale)
{
	for (int i = 0; i < m_Grid.GetNrElements(); i++)
		m_Grid.Get(i) /= scale;
}

void HeightMap::AddCubeWaveX(float period, float magnitude)
{
	const float cellWidth{ GetCellWidth() };
	for (int iRow = 0, idx = 0; iRow < m_Grid.GetNrRows(); iRow++)
		for (int iCol = 0; iCol < m_Grid.GetNrCols(); iCol++, idx++)
			m_Grid.Get(idx) += CubeFunction(period, magnitude, iCol * cellWidth);
}

void HeightMap::AddCubeWaveY(float period, float magnitude)
{
	const float cellHeight{ GetCellHeight() };
	for (int iCol = 0, idx = 0; iCol < m_Grid.GetNrCols(); iCol++)
		for (int iRow = 0; iRow < m_Grid.GetNrRows(); iRow++, idx++)
			m_Grid.Get({ iCol, iRow }) += CubeFunction(period, magnitude, iRow * cellHeight);
}

void HeightMap::CubeDisplaceAlongX(float period, float magnitude)
{
	const float cellHeight{ GetCellHeight() };
	const float cellWidth{ GetCellWidth() };

	const GridArray<float> copy{m_Grid};

	for (int iRow = 0; iRow < m_Grid.GetNrRows(); iRow++)
	{
		const int displacement{ Float::Round(CubeFunction(period, magnitude, iRow * cellHeight) / cellWidth) };
		for (int iCol = 0; iCol < m_Grid.GetNrCols(); iCol++)
		{
			int prevCol{ iCol - displacement };
			if (prevCol < 0) prevCol += m_Grid.GetNrCols();
			if (prevCol >= m_Grid.GetNrCols()) prevCol -= m_Grid.GetNrCols();
			m_Grid.Set({ iCol, iRow }, copy.Get({ prevCol, iRow }));
		}
	}
}

void HeightMap::SinDisplaceAlongX(float period, float magnitude)
{
	const Float2 cellSize{ GetCellSize() };
	const GridArray<float> copy{m_Grid};
	for (int iRow = 0; iRow < m_Grid.GetNrRows(); iRow++)
	{
		const int displacement{ Float::Round(SinFunction(period, magnitude, iRow * cellSize.y) / cellSize.x) };
		for (int iCol = 0; iCol < m_Grid.GetNrCols(); iCol++)
		{
			int prevCol{ iCol - displacement };
			if (prevCol < 0) prevCol = 0;
			if (prevCol >= m_Grid.GetNrCols()) prevCol = m_Grid.GetNrCols() - 1;
			m_Grid.Set({ iCol, iRow }, copy.Get({ prevCol, iRow }));
		}
	}
}

void HeightMap::SinDisplaceAlongY(float period, float magnitude)
{
	const Float2 cellSize{ GetCellSize() };
	const GridArray<float> copy{m_Grid};
	for (int iCol = 0; iCol < m_Grid.GetNrCols(); iCol++)
	{
		const int displacement{ Float::Round(SinFunction(period, magnitude, iCol * cellSize.x) / cellSize.y) };
		for (int iRow = 0; iRow < m_Grid.GetNrRows(); iRow++)
		{
			int prevRow{ iRow - displacement };
			if (prevRow < 0) prevRow = 0;
			if (prevRow >= m_Grid.GetNrRows()) prevRow = m_Grid.GetNrRows() - 1;
			m_Grid.Set({ iCol, iRow }, copy.Get({ iCol, prevRow }));
		}
	}
}

void HeightMap::ToVertices(Array<Rendering::V_PosNorCol>& vertices, Array<Float3>& triangleNormals, Array<int>& indices, const
	Float3& origin) const
{
	vertices = { m_Grid.GetNrElements() };

	//Positions
	const Float2 cellSize{ GetCellSize() };
	int idx = 0;
	for (int iRow = 0; iRow < m_Grid.GetNrRows(); iRow++)
	{
		for (int iCol = 0; iCol < m_Grid.GetNrCols(); iCol++)
		{
			vertices[idx].Pos = origin + Float3{ iCol* cellSize.x, m_Grid.Get(idx), iRow* cellSize.y };
			vertices[idx].Color = Float3{ 242 / 255.f,209 / 255.f,107 / 255.f };
			idx++;
		}
	}

	//Normals
	triangleNormals = { (m_Grid.GetNrCols() - 1) * (m_Grid.GetNrRows() - 1) * 2 };
	int triangleIdx = 0;
	idx = 0;
	for (int iRow = 0; iRow < m_Grid.GetNrRows() - 1; iRow++)
	{
		for (int iCol = 0; iCol < m_Grid.GetNrCols() - 1; iCol++)
		{
			const int idxBotLeft = idx;
			const int idxBotRight = idx + 1;
			const int idxTopLeft = idx + m_Grid.GetNrCols();
			const int idxTopRight = idxTopLeft + 1;
			const Float3& botLeft{ vertices[idxBotLeft].Pos };
			const Float3& botRight{ vertices[idxBotRight].Pos };
			const Float3& topLeft{ vertices[idxTopLeft].Pos };
			const Float3& topRight{ vertices[idxTopRight].Pos };

			const Float3 diagonal{ topRight - botLeft };
			const Float3 horizontal{ botRight - botLeft };
			const Float3 vertical{ topLeft - botLeft };

			const Float3 normal1{ diagonal.Cross(horizontal).Normalized() };
			const Float3 normal2{ vertical.Cross(diagonal).Normalized() };
			triangleNormals[triangleIdx++] = normal1;
			triangleNormals[triangleIdx++] = normal2;

			vertices[idxBotLeft].Normal += normal1 + normal2;
			vertices[idxBotRight].Normal += normal1;
			vertices[idxTopLeft].Normal += normal2;
			vertices[idxTopRight].Normal += normal1 + normal2;

			idx++;
		}
		idx++;
	}
	//can optimize because nr of normals is known
	for (int i = 0; i < vertices.GetSize(); i++)
		vertices[i].Normal.Normalize();

	//Indices
	indices = { (m_Grid.GetNrCols() - 1) * (m_Grid.GetNrRows() - 1) * 2 * 3 };
	idx = 0;
	int iIndex = 0;
	for (int iRow = 0; iRow < m_Grid.GetNrRows() - 1; iRow++)
	{
		for (int iCol = 0; iCol < m_Grid.GetNrCols() - 1; iCol++)
		{
			const int botLeft = idx;
			const int botRight = idx + 1;
			const int topLeft = idx + m_Grid.GetNrCols();
			const int topRight = topLeft + 1;

			//Triangle Left
			indices[iIndex++] = botLeft;
			indices[iIndex++] = topRight;
			indices[iIndex++] = botRight;

			//Triangle Right
			indices[iIndex++] = botLeft;
			indices[iIndex++] = topLeft;
			indices[iIndex++] = topRight;

			idx++;
		}
		idx++;
	}
}

Float2 HeightMap::GetCellSize() const
{
	return { GetCellWidth(), GetCellHeight() };
}

float HeightMap::GetCellWidth() const
{
	return m_Size.x / (m_Grid.GetNrCols() - 1);
}

float HeightMap::GetCellHeight() const
{
	return m_Size.y / (m_Grid.GetNrRows() - 1);
}

float HeightMap::CubeFunction(float period, float magnitude, float t) const
{
	constexpr float scaleX{ 4 }; //constant value is to choose pointiness
	const float scaleY{ 1.f / (scaleX * scaleX * scaleX) * magnitude };
	t /= period;
	t = fmod(t, 2);
	if (t > 1)
		t = 1 - (t - 1);
	t *= scaleX;
	return scaleY * t * t * t;
}

float HeightMap::SinFunction(float period, float magnitude, float t)
{
	t /= period;
	t *= Constants::PI2;
	return sinf(t) * magnitude;
}
