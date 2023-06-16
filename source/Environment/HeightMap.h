#pragma once
#include "DataStructures/GridArray.h"
#include <Rendering/Structs/VertexTypes.h>

class HeightMap
{
public:
	//---| Constructor/Destructor |---
	explicit HeightMap(Int2 nrElements, float initHeight, const Float2& size);

	void AddSinWaveX(float wavePeriod, float waveMagnitude);
	void AddSinWaveY(float wavePeriod, float waveMagnitude);
	void AddSinWave(float wavePeriod, float waveMagnitude);

	void AddCubeWaveX(float period, float magnitude);
	void AddCubeWaveY(float period, float magnitude);

	void CubeDisplaceAlongX(float period, float magnitude);
	void SinDisplaceAlongX(float period, float magnitude);
	void SinDisplaceAlongY(float period, float magnitude);

	void Scale(float scale);
	void Divide(float scale);

	void ToVertices(Array<Rendering::V_PosNorCol>& vertices, Array<int>& indices, const Float3& origin) const;
	Float2 GetCellSize() const;
	float GetCellWidth() const;
	float GetCellHeight() const;
	const Float2& GetSize() const { return m_Size; }

	float GetHeight(const Float2& point) const;

private:
	GridArray<float> m_Grid;
	Float2 m_Size;

	float CubeFunction(float period, float magnitude, float t) const;
	static float SinFunction(float period, float magnitude, float t);
};

