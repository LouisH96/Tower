#include "pch.h"
#include "HeightMap.h"

using namespace TowerGame;

HeightMap::HeightMap(float initHeight, float pointsPerSize, const Float2& size, const Float2& offset)
	: m_Grid{ size * pointsPerSize, initHeight }
	, m_OriginOffset{ offset }
{
	m_CellSize = size / m_Grid.GetSize();
}

void HeightMap::AddSinWaveX(float wavePeriod, float waveMagnitude)
{
	const Float2 cellSize{ GetCellSize() };
	for (int iRow = 0, idx = 0; iRow < m_Grid.GetNrRows(); iRow++)
		for (int iCol = 0; iCol < m_Grid.GetNrCols(); iCol++, idx++)
			m_Grid.Get(idx) += SinFunction(wavePeriod, waveMagnitude, iCol * cellSize.x - m_OriginOffset.x);
}

void HeightMap::AddSinWaveY(float wavePeriod, float waveMagnitude)
{
	const Float2 cellSize{ GetCellSize() };
	for (int iRow = 0, idx = 0; iRow < m_Grid.GetNrRows(); iRow++)
		for (int iCol = 0; iCol < m_Grid.GetNrCols(); iCol++, idx++)
			m_Grid.Get(idx) += SinFunction(wavePeriod, waveMagnitude, iRow * cellSize.y - m_OriginOffset.y);
}

void HeightMap::AddSinWave(float wavePeriod, float waveMagnitude)
{
	const Float2 cellSize{ GetCellSize() };
	int idx = 0;
	for (int iRow = 0; iRow < m_Grid.GetNrRows(); iRow++)
	{
		for (int iCol = 0; iCol < m_Grid.GetNrCols(); iCol++)
		{
			const float xPeriodTime{ (iCol * cellSize.x - m_OriginOffset.x) / wavePeriod };
			const float xAngle{ xPeriodTime * Constants::PI2 };
			const float xHeight{ sinf(xAngle) * waveMagnitude };

			const float yPeriodTime{ (iRow * cellSize.y - m_OriginOffset.y) / wavePeriod };
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
			m_Grid.Get(idx) += CubeFunction(period, magnitude, iCol * cellWidth - m_OriginOffset.x);
}

void HeightMap::AddCubeWaveY(float period, float magnitude)
{
	const float cellHeight{ GetCellHeight() };
	for (int iCol = 0, idx = 0; iCol < m_Grid.GetNrCols(); iCol++)
		for (int iRow = 0; iRow < m_Grid.GetNrRows(); iRow++, idx++)
			m_Grid.Get({ iCol, iRow }) += CubeFunction(period, magnitude, iRow * cellHeight - m_OriginOffset.y);
}

void HeightMap::CubeDisplaceAlongX(float period, float magnitude)
{
	const Float2 cellSize{ GetCellSize() };
	const GridArray<float> copy{ m_Grid };
	for (int iRow = 0; iRow < m_Grid.GetNrRows(); iRow++)
	{
		const int displacement{ Int::Round(CubeFunction(period, magnitude, iRow * cellSize.y - m_OriginOffset.y) / cellSize.x) };
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
	const GridArray<float> copy{ m_Grid };
	for (int iRow = 0; iRow < m_Grid.GetNrRows(); iRow++)
	{
		const float displacement{ SinFunction(period, magnitude, iRow * cellSize.y - m_OriginOffset.y) / cellSize.x };
		const int lerpA{ Int::Floor(displacement) };
		const int lerpB{ Int::Ceil(displacement) };
		const float lerpAlpha{ displacement - lerpA };

		for (int iCol = 0; iCol < m_Grid.GetNrCols(); iCol++)
		{
			const int lerpACol{ Int::Clamped(iCol + lerpA, 0, m_Grid.GetNrCols() - 1) };
			const int lerpBCol{ Int::Clamped(iCol + lerpB, 0, m_Grid.GetNrCols() - 1) };
			const float lerpAValue{ copy.Get({lerpACol, iRow}) };
			const float lerpBValue{ copy.Get({lerpBCol, iRow}) };
			const float lerped{ Float::Lerp(lerpAlpha, lerpAValue, lerpBValue) };
			m_Grid.Set({ iCol, iRow }, lerped);
		}
	}
}

void HeightMap::SinDisplaceAlongY(float period, float magnitude)
{
	const Float2 cellSize{ GetCellSize() };
	const GridArray<float> copy{ m_Grid };
	for (int iCol = 0; iCol < m_Grid.GetNrCols(); iCol++)
	{
		const float displacement{ SinFunction(period, magnitude, iCol * cellSize.x - m_OriginOffset.x) / cellSize.y };
		const int lerpA{ Int::Floor(displacement) };
		const int lerpB{ Int::Ceil(displacement) };
		const float lerpAlpha{ displacement - lerpA };
		for (int iRow = 0; iRow < m_Grid.GetNrRows(); iRow++)
		{
			const int lerpARow{ Int::Clamped(iRow + lerpA, 0, m_Grid.GetNrRows() - 1) };
			const int lerpBRow{ Int::Clamped(iRow + lerpB, 0, m_Grid.GetNrRows() - 1) };
			const float lerpAValue{ copy.Get({iCol, lerpARow}) };
			const float lerpBValue{ copy.Get({iCol, lerpBRow}) };
			const float lerped{ Float::Lerp(lerpAlpha, lerpAValue, lerpBValue) };
			m_Grid.Set({ iCol, iRow }, lerped);
		}
	}
}

void HeightMap::ToVertices(List<Rendering::V_PosNorCol>& vertices, List<int>& indices, Float3 origin) const
{
	vertices.EnsureIncrease(m_Grid.GetNrElements());

	//Positions
	const Float2 cellSize{ GetCellSize() };
	int idx = 0;
	for (int iRow = 0; iRow < m_Grid.GetNrRows(); iRow++)
	{
		for (int iCol = 0; iCol < m_Grid.GetNrCols(); iCol++)
		{
			Rendering::V_PosNorCol vertex{};
			vertex.Pos = origin + Float3{ iCol * cellSize.x, m_Grid.Get(idx), iRow * cellSize.y };
			vertex.Color = Float3{ 242 / 255.f,209 / 255.f,107 / 255.f };
			vertices.Add(vertex);
			idx++;
		}
	}

	//Normals
	int triangleIdx = 0;
	idx = 0;
	for (int iRow = 0; iRow < m_Grid.GetNrRows() - 1; iRow++)
	{
		for (int iCol = 0; iCol < m_Grid.GetNrCols() - 1; iCol++)
		{
			const int idxBotLeft{ idx };
			const int idxBotRight{ idx + 1 };
			const int idxTopLeft{ idx + m_Grid.GetNrCols() };
			const int idxTopRight{ idxTopLeft + 1 };
			Float3 botLeft{ 0, vertices[idxBotLeft].Pos.y, 0 };
			Float3 botRight{ cellSize.x, vertices[idxBotRight].Pos.y, 0 };
			Float3 topLeft{ 0, vertices[idxTopLeft].Pos.y, cellSize.y };
			Float3 topRight{ cellSize.x, vertices[idxTopRight].Pos.y, cellSize.y };

			const Float3 horizontal{ botRight - botLeft };
			const Float3 vertical{ topLeft - botLeft };

			if ((iRow + iCol) % 2)
			{
				const Float3 diagonal{ topRight - botLeft };

				const Float3 normal1{ diagonal.Cross(horizontal).Normalized() };
				const Float3 normal2{ vertical.Cross(diagonal).Normalized() };

				vertices[idxBotLeft].Normal += normal1 + normal2;
				vertices[idxBotRight].Normal += normal1;
				vertices[idxTopLeft].Normal += normal2;
				vertices[idxTopRight].Normal += normal1 + normal2;
			}
			else
			{
				const Float3 diagonal{ botRight - topLeft };

				const Float3 normal1{ vertical.Cross(horizontal).Normalized() };
				const Float3 normal2{ (topRight - topLeft).Cross(diagonal).Normalized()};

				vertices[idxBotLeft].Normal += normal1;
				vertices[idxBotRight].Normal += normal1 + normal2;
				vertices[idxTopLeft].Normal += normal1 + normal2;
				vertices[idxTopRight].Normal += normal2;
			}

			idx++;
		}
		idx++;
	}
	//todo: can be optimized because nr of normals is known
	for (unsigned i = 0; i < vertices.GetSize(); i++)
		vertices[i].Normal.Normalize();

	//Indices
	indices.EnsureIncrease((m_Grid.GetNrCols() - 1) * (m_Grid.GetNrRows() - 1) * 2 * 3);
	idx = 0;
	for (int iRow = 0; iRow < m_Grid.GetNrRows() - 1; iRow++)
	{
		for (int iCol = 0; iCol < m_Grid.GetNrCols() - 1; iCol++)
		{
			const int botLeft{ idx };
			const int botRight{ idx + 1 };
			const int topLeft{ idx + m_Grid.GetNrCols() };
			const int topRight{ topLeft + 1 };

			if ((iRow + iCol) % 2)
			{
				indices.Add(botLeft, topRight, botRight);
				indices.Add(botLeft, topLeft, topRight);
			}
			else
			{
				indices.Add(botLeft, topLeft, botRight);
				indices.Add(topLeft, topRight, botRight);
			}

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
	return m_CellSize.x;
}

float HeightMap::GetCellHeight() const
{
	return m_CellSize.y;
}

float HeightMap::GetHeight(Float2 point) const
{
	constexpr float fallback{ -1 };
	const Float2 pointCoord{
		point.x / GetCellWidth(),
		point.y / GetCellHeight()
	};
	const Int2 leftBotCoord{
		Int::Floor(pointCoord.x),
		Int::Floor(pointCoord.y)
	};
	if (leftBotCoord.x < 0 || leftBotCoord.y < 0)  return fallback;
	const Int2 rightTopCoord{ leftBotCoord.x + 1,leftBotCoord.y + 1 };
	if (rightTopCoord.x >= m_Grid.GetNrCols() || rightTopCoord.y >= m_Grid.GetNrRows()) return fallback;

	//barycentric coordinates
	const Float2 pointLocal{ pointCoord - leftBotCoord };
	const float crossDiagonal{ pointLocal.Cross({1,1}) };
	if (crossDiagonal < 0)
	{
		//left-top triangle
		const float weightLeftTop{ -crossDiagonal };
		const float weightRightTop{ pointLocal.Cross({0,1}) };
		const float weightLeftBot{ Float2{pointLocal.x, pointLocal.y - 1}.Cross({1,0}) };
		const float heightLeftBot{ m_Grid.Get(leftBotCoord) };
		const float heightLeftTop{ m_Grid.Get({leftBotCoord.x, leftBotCoord.y + 1}) };
		const float heightTopRight{ m_Grid.Get({leftBotCoord.x + 1, leftBotCoord.y + 1}) };
		return heightLeftBot * weightLeftBot + heightLeftTop * weightLeftTop + heightTopRight * weightRightTop;
	}
	//right-bot triangle
	const float weightRightBot{ crossDiagonal };
	const float weightRightTop{ Float2{1,0}.Cross(pointLocal) };
	const float weightLeftBot{ Float2{0,1}.Cross({pointLocal.x - 1, pointLocal.y}) };
	const float heightRightBot{ m_Grid.Get({leftBotCoord.x + 1, leftBotCoord.y}) };
	const float heightRightTop{ m_Grid.Get(rightTopCoord) };
	const float heightLeftBot{ m_Grid.Get(leftBotCoord) };
	return heightLeftBot * weightLeftBot + heightRightTop * weightRightTop + heightRightBot * weightRightBot;
}

float HeightMap::CubeFunction(float period, float magnitude, float t) const
{
	constexpr float scaleX{ 4 }; //constant value is to choose pointiness
	const float scaleY{ 1.f / (scaleX * scaleX * scaleX) * magnitude };
	t = fabs(t);
	t /= period;
	t = fmod(t, 2.f);
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
