#include "pch.h"
#include "HeightMap.h"

HeightMap::HeightMap(Math::Int2 nrElements, float initHeight, const Math::Float2& size)
	: m_Grid{ nrElements, initHeight }
	, m_Size{ size }
{
}

void HeightMap::ApplyWaveX(float wavePeriod, float waveMagnitude)
{
	const Math::Float2 cellSize{ GetCellSize() };
	int idx = 0;
	for (int iRow = 0; iRow < m_Grid.GetNrRows(); iRow++)
	{
		for (int iCol = 0; iCol < m_Grid.GetNrCols(); iCol++)
		{
			const float periodTime{ iCol * cellSize.x / wavePeriod };
			const float angle{ periodTime * Math::Constants::PI2 };
			const float height{ sinf(angle) * waveMagnitude };

			m_Grid.Get(idx) += height;
			idx++;
		}
	}
}

void HeightMap::ApplyWaveY(float wavePeriod, float waveMagnitude)
{
	const Math::Float2 cellSize{ GetCellSize() };
	int idx = 0;
	for (int iRow = 0; iRow < m_Grid.GetNrRows(); iRow++)
	{
		for (int iCol = 0; iCol < m_Grid.GetNrCols(); iCol++)
		{
			const float periodTime{ iRow * cellSize.y / wavePeriod };
			const float angle{ periodTime * Math::Constants::PI2 };
			const float height{ sinf(angle) * waveMagnitude };

			m_Grid.Get(idx) += height;
			idx++;
		}
	}
}

void HeightMap::ApplyWave(float wavePeriod, float waveMagnitude)
{
	const Math::Float2 cellSize{ GetCellSize() };
	int idx = 0;
	for (int iRow = 0; iRow < m_Grid.GetNrRows(); iRow++)
	{
		for (int iCol = 0; iCol < m_Grid.GetNrCols(); iCol++)
		{
			const float xPeriodTime{ iCol * cellSize.x / wavePeriod };
			const float xAngle{ xPeriodTime * Math::Constants::PI2 };
			const float xHeight{ sinf(xAngle) * waveMagnitude };

			const float yPeriodTime{ iRow * cellSize.y / wavePeriod };
			const float yAngle{ yPeriodTime * Math::Constants::PI2 };
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

void HeightMap::ToVertices(Array<Rendering::V_PosNorCol>& vertices, Array<Math::Float3>& triangleNormals, Array<int>& indices) const
{
	vertices = { m_Grid.GetNrElements() };

	//Positions
	const Math::Float2 cellSize{ GetCellSize() };
	int idx = 0;
	for (int iRow = 0; iRow < m_Grid.GetNrRows(); iRow++)
	{
		for (int iCol = 0; iCol < m_Grid.GetNrCols(); iCol++)
		{
			vertices[idx].Pos = Math::Float3{ iCol * cellSize.x, m_Grid.Get(idx), iRow * cellSize.y };
			vertices[idx].Color = Math::Float3{ 242 / 255.f,209 / 255.f,107 / 255.f };
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
			const Math::Float3& botLeft{ vertices[idxBotLeft].Pos };
			const Math::Float3& botRight{ vertices[idxBotRight].Pos };
			const Math::Float3& topLeft{ vertices[idxTopLeft].Pos };
			const Math::Float3& topRight{ vertices[idxTopRight].Pos };

			const Math::Float3 diagonal{ topRight - botLeft };
			const Math::Float3 horizontal{ botRight - botLeft };
			const Math::Float3 vertical{ topLeft - botLeft };

			const Math::Float3 normal1{ diagonal.Cross(horizontal).Normalized() };
			const Math::Float3 normal2{ vertical.Cross(diagonal).Normalized() };
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

Math::Float2 HeightMap::GetCellSize() const
{
	return { m_Size.x / (m_Grid.GetNrCols() - 1), m_Size.y / (m_Grid.GetNrRows() - 1) };
}
