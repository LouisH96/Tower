#pragma once
#include <DataStructures/GridArray.h>
#include <Rendering/Structs/VertexTypes.h>

namespace TowerGame
{
class HeightMap
{
public:
	//---| Constructor/Destructor |---
	HeightMap() = default;
	explicit HeightMap(float initHeight, float pointsPerSize, const Float2& size, const Float2& offset = Float2{ 0 });

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

	void ToVertices(List<Rendering::V_PosNorCol>& vertices, List<int>& indices, Float3 origin) const;
	Float2 GetCellSize() const;
	float GetCellWidth() const;
	float GetCellHeight() const;
	Float2 GetSize() const { return m_CellSize * m_Grid.GetSize(); }

	float GetHeight(Float2 point) const;

private:
	GridArray<float> m_Grid;
	Float2 m_OriginOffset; //Local (within the map), thus map starts at 0,0 anyway. used only for generation
	Float2 m_CellSize;

	float CubeFunction(float period, float magnitude, float t) const;
	static float SinFunction(float period, float magnitude, float t);
};
}
