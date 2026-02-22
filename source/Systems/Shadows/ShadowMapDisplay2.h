#pragma once
#include "ShadowSystem.h"
#include <Framework\Systems\System.h>

#undef near
#undef far

namespace TowerGame
{
class ShadowMapDisplay2
	: public ISystem
{
public:
	void Init(ShadowSystem& shadowMap);

	const Float4X4& GetMatrix() const { return m_Matrix; }
	void SetMatrix(const Float4X4& matrix) { m_Matrix = matrix; }

	void OnCanvasResized(const App::ResizedEvent& event) override {};
	void EarlyUpdate() override {};
	void Update() override;
	void PreRender() override {};
	void Render() override {};

private:
	struct EyeFrustum
	{
		//LeftBot -> LeftTop -> RightTop -> RightBot
		Float4 Origin;
		Float4 Near[4];
		Float4 Far[4];
		Float4 EdgeDir[4];
		List<Float4> Slices{};

		Float4 Right;
		Float4 Up;
		Float4 Forward;

		Float4 RightFlat;
		Float4 UpFlat;
		Float4 ForwardFlat;

		Float4 RightFlatNorm;
		Float4 UpFlatNorm;
		Float4 ForwardFlatNorm;

		List<float> SliceEnds{};
		float NearDepth;
		float FarDepth; //Starting from Eye
	};

	Float2 m_EyeRectOrigin;
	Float2 m_EyeRectSize;
	List<Float4> m_EyeRectLines{};
	List<Float3> m_EyeRectColors{};

	Float2 m_MapRectOrigin;
	Float2 m_MapRectSize;
	List<Float4> m_MapRectLines{};
	List<Float3> m_MapRectColors{};

	List<Float3> m_Models{};

	Float4X4 m_Matrix;

	//Frame
	void InitEyeRect();
	void InitMapRect();

	void DrawEyeRect();
	void DrawMapRect();
	void DrawEyeRectLine(Float2 from, Float2 to, const Float3& color);
	void DrawMapRectLine(Float2 from, Float2 to, const Float3& color);

	void DrawEyeRectLines(const List<Float4>& linePoints, const List<Float3>& lineColors);
	void DrawMapRectLines(const List<Float4>& linePoints, const List<Float3>& lineColors);

	//App
	void AddBoundingBox(
		const EyeFrustum& eye,
		List<Float4>& linePoints, List<Float3>& lineColors);

	void Try(EyeFrustum& eye);
	Float4X4 Try2(EyeFrustum& eye);

	//Fustum
	EyeFrustum CalcEyeFrustum();
	static void Apply(const Float4X4& matrix, EyeFrustum& eye);

	//LinePoints
	void AddSurroundLines(
		const EyeFrustum& eye, float dist, const Float3& color,
		List<Float4>& linePoints, List<Float3>& lineColors);
	void AddSurroundLines(
		const Float4* pPoints, const Float3& color,
		List<Float4>& linePoints, List<Float3>& lineColors);
	void AddDepthLines(
		const EyeFrustum& eye, float fromDist, float toDist, const Float3& color,
		List<Float4>& linePoints, List<Float3>& lineColors);
	void AddDepthLines(
		const Float4* pFrom, const Float4* pTo, const Float3& color,
		List<Float4>& linePoints, List<Float3>& lineColors);
	void AddEyeLines(
		const EyeFrustum& eye,
		List<Float4>& linePoints, List<Float3>& lineColors);
	void TransformLines(
		List<Float4>& linePoints, const Float4X4& matrix);
	void AddLines(
		const List<Float3>& lines, const Float3& color,
		List<Float4>& linesDestination, List<Float3>& colorsDestination);

	};
}