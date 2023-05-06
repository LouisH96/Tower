#pragma once
#include "DataStructures/GridArray.h"
#include <Rendering/Structs/VertexTypes.h>

class HeightMap
{
public:
	//---| Constructor/Destructor |---
	explicit HeightMap(Math::Int2 nrElements, float initHeight, const Math::Float2& size);

	void ApplyWaveX(float wavePeriod, float waveMagnitude);
	void ApplyWaveY(float wavePeriod, float waveMagnitude);
	void ApplyWave(float wavePeriod, float waveMagnitude);
	void Scale(float scale);
	void Divide(float scale);

	void ToVertices(Array<Rendering::V_PosNorCol>& vertices, Array<Math::Float3>& triangleNormals, Array<int>& indices, const Math::Float3& origin) const;
	Math::Float2 GetCellSize() const;

private:
	GridArray<float> m_Grid;
	Math::Float2 m_Size;
};

