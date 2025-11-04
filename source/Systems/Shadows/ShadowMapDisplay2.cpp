#include "pch.h"
#include "ShadowMapDisplay2.h"

#include <App\Win32\Window.h>
#include <TowerApp.h>
#include <Transform\ViewMatrix.h>
#include <Transform\WorldMatrix.h>
#include <Ui\Elements\Box.h>
#include <Ui\Elements\Extender.h>
#include <Ui\Elements\Image.h>
#include <Ui\Elements\Margin.h>
#include <Ui\UiSystem.h>

using namespace TowerGame;

#undef min
#undef max

void ShadowMapDisplay2::Init(ShadowSystem2& shadowMap)
{
	//Ui
	Margin* pMargin{ new Margin(10) };
	UI.AddChild(
		pMargin, { 1,1 }, { 1,1 });

	Extender* pExtender{ new Extender(SizeDef::Pixels(100,100)) };
	pMargin->AddChild(pExtender);

	unsigned textureId{
		UI_IMAGE.AddTexture(
			shadowMap.GetDepthStencilBuffer(1).MakeShaderResourceView()) };

	Ui::Image* pImage{ new Ui::Image(textureId) };
	pExtender->AddChild(pImage);

	InitEyeRect();
	InitMapRect();

	//
	const CollisionSystem& collisions{ SYSTEMS.Collisions };
	const List<ModelCollidable>& models{ collisions.Models.Models };
	for (unsigned iModel{ 0 }; iModel < models.GetSize(); ++iModel)
	{
		const ModelCollidable& model{ models[iModel] };
		const List<ModelCollidable::Instance>& instances{ model.Instances };

		for (unsigned iInst{ 0 }; iInst < model.Instances.GetSize(); ++iInst)
		{
			const ModelCollidable::Instance& instance{ model.Instances[iInst] };
			for (unsigned iPoint{ 2 }; iPoint < model.Points.GetSize(); iPoint += 3)
			{
				Float3 points[3]{
					model.Points[iPoint - 2],
					model.Points[iPoint - 1],
					model.Points[iPoint - 0]
				};
				for (unsigned iCorner{ 0 }; iCorner < 3; ++iCorner)
					points[iCorner] = instance.World.AppliedToPoint(points[iCorner]);

				m_Models.Add(points[0], points[1]);
				m_Models.Add(points[1], points[2]);
				m_Models.Add(points[2], points[0]);
			}
		}
	}
}

unsigned iSlice{ 1 };
void ShadowMapDisplay2::Update()
{
	//
	const ShadowSystem2& shadows{ SYSTEMS.Shadows2 };

	//Setup
	m_Matrix = WorldMatrix::GetIdentity();
	DrawEyeRect();
	DrawMapRect();

	m_EyeRectLines.Clear();
	m_EyeRectColors.Clear();
	m_MapRectLines.Clear();
	m_MapRectColors.Clear();

	EyeFrustum eye{ CalcEyeFrustum() };

	//EyeRect
	AddEyeLines(eye, m_EyeRectLines, m_EyeRectColors);
	Float4X4 eyeRectMatrix{ SYSTEMS.Shadows2.GetView() };
	eyeRectMatrix *= WorldMatrix::Translation(-(eye.Origin * eyeRectMatrix));
	TransformLines(m_EyeRectLines, eyeRectMatrix);

	//TEST
	AddEyeLines(eye, m_MapRectLines, m_MapRectColors);

	if (KEYBOARD.IsPressed(Keyboard::ScanCodeToVk(KeyCode::N)))
		iSlice = (iSlice + 1) % 3;

	Float4X4 matrix{ shadows.GetViewProj(iSlice) };
	TransformLines(m_MapRectLines, matrix);

	//
	eyeRectMatrix = WorldMatrix::Scale(Float3{ 1.f / 100 });
	TransformLines(m_EyeRectLines, eyeRectMatrix);

	for (unsigned i{ 0 }; i < m_MapRectLines.GetSize(); ++i)
	{
		m_MapRectLines[i] /= m_MapRectLines[i].w;
	}

	//Draw
	DrawEyeRectLines(m_EyeRectLines, m_EyeRectColors);
	DrawMapRectLines(m_MapRectLines, m_MapRectColors);
}

void ShadowMapDisplay2::InitEyeRect()
{
	constexpr float heigthPerc{ .3f };
	constexpr float margin{ 20.f };
	const Float2 screenSize{ Float2{WINDOW.GetClientSize()} };

	m_EyeRectSize.y = screenSize.y * heigthPerc;
	m_EyeRectOrigin.y = screenSize.y * (1 - heigthPerc) - margin;

	m_EyeRectOrigin.x = margin;
	m_EyeRectSize.x = m_EyeRectSize.y;
}

void ShadowMapDisplay2::InitMapRect()
{
	constexpr float heightPerc{ .7f };
	const Float2 screenSize{ Float2{WINDOW.GetClientSize()} };

	m_MapRectSize.y = screenSize.y * heightPerc;
	m_MapRectOrigin.y = (screenSize.y - m_MapRectSize.y) / 2;

	m_MapRectSize.x = m_MapRectSize.y;
	m_MapRectOrigin.x = (screenSize.x - m_MapRectSize.x) / 2;
}

void ShadowMapDisplay2::DrawEyeRect()
{
	const Float3 color{ Color::Black };
	DrawEyeRectLine({ -1,-1 }, { -1, 1 }, color);
	DrawEyeRectLine({ -1,1 }, { 1, 1 }, color);
	DrawEyeRectLine({ 1,1 }, { 1, -1 }, color);
	DrawEyeRectLine({ 1,-1 }, { -1, -1 }, color);
}

void ShadowMapDisplay2::DrawMapRect()
{
	const Float3 color{ Color::Black };
	DrawMapRectLine({ -1,-1 }, { -1, 1 }, color);
	DrawMapRectLine({ -1,1 }, { 1, 1 }, color);
	DrawMapRectLine({ 1,1 }, { 1, -1 }, color);
	DrawMapRectLine({ 1,-1 }, { -1, -1 }, color);
}

void ShadowMapDisplay2::DrawEyeRectLine(Float2 from, Float2 to, const Float3& color)
{
	from += 1;
	to += 1;

	from /= 2;
	to /= 2;

	from *= m_EyeRectSize;
	to *= m_EyeRectSize;

	from += m_EyeRectOrigin;
	to += m_EyeRectOrigin;

	DebugRenderer::DrawLine(from, to, color);
}

void ShadowMapDisplay2::DrawMapRectLine(Float2 from, Float2 to, const Float3& color)
{
	from += 1;
	to += 1;

	from /= 2;
	to /= 2;

	from *= m_MapRectSize;
	to *= m_MapRectSize;

	from += m_MapRectOrigin;
	to += m_MapRectOrigin;

	DebugRenderer::DrawLine(from, to, color);
}

void ShadowMapDisplay2::DrawEyeRectLines(const List<Float4>& linePoints, const List<Float3>& lineColors)
{
	const Float3* pColor{ lineColors.GetData() };
	for (unsigned i{ 1 }; i < linePoints.GetSize(); i += 2)
		DrawEyeRectLine(linePoints[i - 1].Xy(), linePoints[i].Xy(), *pColor++);
}

void ShadowMapDisplay2::DrawMapRectLines(const List<Float4>& linePoints, const List<Float3>& lineColors)
{
	const Float3* pColor{ lineColors.GetData() };
	for (unsigned i{ 1 }; i < linePoints.GetSize(); i += 2)
		DrawMapRectLine(linePoints[i - 1].Xy(), linePoints[i].Xy(), *pColor++);
}

void ShadowMapDisplay2::AddBoundingBox(
	const EyeFrustum& eye,
	List<Float4>& linePoints, List<Float3>& lineColors)
{
	//Numbers
	Float2 min{ Float::MAX };
	Float2 max{ -Float::MAX };
	const Float4 rightPerp{ -eye.RightFlatNorm.y, eye.RightFlatNorm.x,0,0 };

	for (unsigned iPlane{ 0 }; iPlane < 2; ++iPlane)
	{
		const Float4* pCorner{ iPlane == 0 ? eye.Near : eye.Far };
		for (unsigned iCorner{ 0 }; iCorner < 4; ++iCorner, ++pCorner)
		{
			const Float2 pos{
				(*pCorner / pCorner->w).Dot(eye.RightFlatNorm),
				(*pCorner / pCorner->w).Dot(rightPerp)
			};
			if (pos.x < min.x) min.x = pos.x;
			if (pos.x > max.x) max.x = pos.x;
			if (pos.y < min.y) min.y = pos.y;
			if (pos.y > max.y) max.y = pos.y;
		}
	}

	//Points
	const Float4 corners[]{
		eye.RightFlatNorm * min.x + rightPerp * min.y + Float4{0,0,0,1},
		eye.RightFlatNorm * min.x + rightPerp * max.y + Float4{0,0,0,1},
		eye.RightFlatNorm * max.x + rightPerp * max.y + Float4{0,0,0,1},
		eye.RightFlatNorm * max.x + rightPerp * min.y + Float4{0,0,0,1},
	};

	for (unsigned i{ 0 }; i < 4; ++i)
	{
		linePoints.Add(corners[i]);
		linePoints.Add(corners[(i + 1) % 4]);
		lineColors.Add(Color::Yellow);
	}

	//TEMP
	min = Float2{ Float::MAX };
	max = Float2{ -Float::MAX };
	for (unsigned i{ 0 }; i < 4; ++i)
	{
		if (corners[i].x < min.x) min.x = corners[i].x;
		if (corners[i].x > max.x) max.x = corners[i].x;
		if (corners[i].y < min.y) min.y = corners[i].y;
		if (corners[i].y > max.y) max.y = corners[i].y;
	}
	const Float2 translation{ -(min + max) / 2 };
	const Float2 scale{ max - min };

	Float4X4 matrix{ Float4X4::GetIdentity() };
	matrix *= WorldMatrix::Translation({ translation.x, translation.y, 0 });
	matrix *= WorldMatrix::Scale({ 2.f / scale.x, 2.f / scale.y, 1 });

	m_Matrix *= matrix;
}

void ShadowMapDisplay2::Try(EyeFrustum& eye)
{
	//Get
	const Camera& camera{ CAMERA };

	const Float4X4& view{ SYSTEMS.Shadows2.GetView() };
	constexpr unsigned NR_SLICES{ 2 };
	const float sliceDepth[]{
		eye.NearDepth + eye.SliceEnds[0],
		eye.FarDepth
	};

	Float4X4 matrices[NR_SLICES];

	//Use ViewProj[0] temporary
	matrices[0] = camera.GetWorld() * view;
	const Float2 right2{ view.AppliedToVector(camera.GetRight()).Xy().Normalized() };
	const Float2 forward2{ -right2.y, right2.x };

	//Find bounds in light-space
	Float3 min2{ 0, 0, -20 };
	Float3 max2{ 0 };
	Float3 camCorner{
			camera.GetTanHalfFov() * camera.GetAspectRatio(),
			camera.GetTanHalfFov(),
			1
	};
	for (unsigned iCol{ 0 }; iCol < 2; ++iCol, camCorner.x *= -1)
		for (unsigned iRow{ 0 }; iRow < 2; iRow++, camCorner.y *= -1)
		{
			Float3 point{ matrices[0].AppliedToVector(camCorner) };
			point = {
				right2.Dot(point.Xy()),
				forward2.Dot(point.Xy()),
				point.z
			};

			if (point.x < min2.x) min2.x = point.x;
			if (point.x > max2.x) max2.x = point.x;
			if (point.y < min2.y) min2.y = point.y;
			if (point.y > max2.y) max2.y = point.y;
			if (point.z > max2.z) max2.z = point.z;
		}

	//Make ViewProj matrices
	const Float3 eyePos2{ matrices[0].GetRow3().Xyz() };
	const Float3 toCenterBot{
		right2 * (min2.x + max2.x) / 2.f +
		forward2 * min2.y, min2.z
	}; //from Eye (in light space)
	const Float3 size{
		abs(max2.x - min2.x),
		abs(max2.y - min2.y),
		abs(max2.z - min2.z)
	};

	for (unsigned iSlice{ 0 }; iSlice < NR_SLICES; ++iSlice)
	{
		//Transform to lightSpace
		matrices[iSlice] = view;

		//Move to bottom of box around slice
		matrices[iSlice] *= WorldMatrix::Translation(Float3{
			  -(eyePos2 + toCenterBot * sliceDepth[iSlice])
			});

		//Rotate
		matrices[iSlice] *= Float4X4{
			{right2.x, right2.y, 0, 0},
			{-right2.y, right2.x, 0, 0},
			{0,0,1,0},
			{0,0,0,1}
		};
		//Scale
		matrices[iSlice] *= Float4X4{
			{2.f / (size.x * sliceDepth[iSlice]), 0, 0, 0},
			{0, 2.f / (size.y * sliceDepth[iSlice]), 0, -1}, //also y - 1 to center
			{0, 0, 1.f / (size.z * sliceDepth[iSlice]), 0},
			{0, 0, 0, 1}
		};
	}
	TransformLines(m_MapRectLines, matrices[1]);
}

Float4X4 ShadowMapDisplay2::Try2(EyeFrustum& eye)
{
	//
	const Camera& camera{ CAMERA };
	const Float4X4& view{ SYSTEMS.Shadows2.GetView() };

	//
	Float4X4 matrix{ Float4X4::GetIdentity() };
	matrix *= view;
	matrix *= WorldMatrix::Translation(-eye.Origin * view);

	//
	Float3 min{};
	Float3 max{};
	for (unsigned iCorner{ 0 }; iCorner < 4; ++iCorner)
	{
		const Float4& point{ eye.Far[iCorner] };
		if (point.x < min.x) min.x = point.x;
		if (point.x > max.x) max.x = point.x;
		if (point.y < min.y) min.y = point.y;
		if (point.y > max.y) max.y = point.y;
		if (point.z < min.z) min.z = point.z;
		if (point.z > max.z) max.z = point.z;
	}
	Float3 size{ max - min };
	size.x = abs(size.x);
	size.y = abs(size.y);

	matrix *= WorldMatrix::Translation({ 0, 0, -min.z });
	matrix *= WorldMatrix::Scale(Float3{ 2.f / size.x, 2.f / size.y, 1 });

	matrix *= Float4X4{
		{1,0,0,0},
		{0,1,0,0},
		{0,0,1,0},
		{0,0,1,0}
	};
	/*                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        matrix *= Float4X4{
		{max.z, 0, 0, -min.x},
		{0,1,0,0},
		{0,0,1,0},
		{0,0,0,1}
	};*/

	return matrix;
}

ShadowMapDisplay2::EyeFrustum ShadowMapDisplay2::CalcEyeFrustum()
{
	//Get
	const Camera& camera{ CAMERA };
	const float ar{ camera.GetAspectRatio() };
	const float camTan{ camera.GetTanHalfFov() };
	const ShadowSystem2& shadowSystem{ SYSTEMS.Shadows2 };

	//Frustum
	EyeFrustum f{};
	f.Right = Float4{ camera.GetRight(), 0 };
	f.Up = Float4{ camera.GetUp(), 0 };
	f.Forward = Float4{ camera.GetForward(), 0 };

	f.RightFlat = f.Right.Xy();
	f.UpFlat = f.Up.Xy();
	f.ForwardFlat = f.Forward.Xy();

	f.RightFlatNorm = f.RightFlat.Normalized();
	f.UpFlatNorm = f.UpFlat.Normalized();
	f.ForwardFlatNorm = f.ForwardFlat.Normalized();

	f.NearDepth = camera.GetNear();
	f.FarDepth = camera.GetFar();
	f.SliceEnds.Clear();
	for (unsigned i{ 0 }; i < shadowSystem.NR_SLICES; ++i)
		f.SliceEnds.Add(shadowSystem.SLICE_END[i]);

	f.Origin = Float4{ camera.GetPosition(), 1 };

	const Float4X4 cornerMatrixBase{
		Float4{camTan * ar, 0,0,0},
		Float4{0,camTan, 0,0},
		Float4{0,0,1,0},
		Float4{0,0,0,1}
	};
	const Float4X4 eyeMatrix{
		Float4{f.Right.x, f.Up.x, f.Forward.x, f.Origin.x},
		Float4{f.Right.y, f.Up.y, f.Forward.y, f.Origin.y},
		Float4{f.Right.z, f.Up.z, f.Forward.z, f.Origin.z},
		Float4{0,0,0,1}
	};

	Float4X4 cornerMatrix{ cornerMatrixBase * WorldMatrix::Scale(Float3{f.NearDepth}) };
	f.Near[0] = Float4{ (cornerMatrix * eyeMatrix).AppliedToPoint({ -1,-1,1 }),1 };
	f.Near[1] = Float4{ (cornerMatrix * eyeMatrix).AppliedToPoint({ -1,1,1 }),1 };
	f.Near[2] = Float4{ (cornerMatrix * eyeMatrix).AppliedToPoint({ 1,1,1 }),1 };
	f.Near[3] = Float4{ (cornerMatrix * eyeMatrix).AppliedToPoint({ 1,-1,1 }),1 };

	cornerMatrix = cornerMatrixBase * WorldMatrix::Scale(Float3{ f.FarDepth });
	f.Far[0] = Float4{ (cornerMatrix * eyeMatrix).AppliedToPoint({ -1,-1,1 }),1 };
	f.Far[1] = Float4{ (cornerMatrix * eyeMatrix).AppliedToPoint({ -1,1,1 }), 1 };
	f.Far[2] = Float4{ (cornerMatrix * eyeMatrix).AppliedToPoint({ 1,1,1 }), 1 };
	f.Far[3] = Float4{ (cornerMatrix * eyeMatrix).AppliedToPoint({ 1,-1,1 }), 1 };

	for (unsigned i{ 0 }; i < 4; ++i)
		f.EdgeDir[i] = (f.Near[i] - f.Origin).Normalized();

	for (unsigned iSlice{ 0 }; iSlice < shadowSystem.NR_SLICES; ++iSlice)
	{
		cornerMatrix = cornerMatrixBase * WorldMatrix::Scale(Float3{ shadowSystem.SLICE_END[iSlice] });
		f.Slices.Add(Float4{ (cornerMatrix * eyeMatrix).AppliedToPoint({ -1,-1,1 }),1 });
		f.Slices.Add(Float4{ (cornerMatrix * eyeMatrix).AppliedToPoint({ -1,1,1 }),1 });
		f.Slices.Add(Float4{ (cornerMatrix * eyeMatrix).AppliedToPoint({ 1,1,1  }),1 });
		f.Slices.Add(Float4{ (cornerMatrix * eyeMatrix).AppliedToPoint({ 1,-1,1 }),1 });
	}

	return f;
}

void ShadowMapDisplay2::Apply(const Float4X4& matrix, EyeFrustum& eye)
{
	eye.Origin *= matrix;
	for (unsigned i{ 0 }; i < 4; ++i)
	{
		eye.Near[i] *= matrix;
		eye.Far[i] *= matrix;
		eye.EdgeDir[i] *= matrix;
	}
	for (unsigned i{ 0 }; i < eye.Slices.GetSize(); ++i)
		eye.Slices[i] *= matrix;

	eye.Right *= matrix;
	eye.Up *= matrix;
	eye.Forward *= matrix;
	eye.RightFlat = eye.Right.Xy();
	eye.UpFlat = eye.Up.Xy();
	eye.ForwardFlat = eye.Forward.Xy();
	eye.RightFlatNorm = eye.RightFlat.Normalized();
	eye.UpFlatNorm = eye.UpFlat.Normalized();
	eye.ForwardFlatNorm = eye.ForwardFlat.Normalized();
}

void ShadowMapDisplay2::AddSurroundLines(
	const EyeFrustum& eye, float dist, const Float3& color,
	List<Float4>& linePoints, List<Float3>& lineColors)
{
	for (unsigned i{ 0 }; i < 4; ++i)
	{
		const Float4 from{ eye.Near[i]
		+ (eye.Far[i] - eye.Near[i]) * dist };

		const unsigned nextI{ (i + 1) % 4 };
		const Float4 to{ eye.Near[nextI]
		+ (eye.Far[nextI] - eye.Near[nextI]) * dist };

		linePoints.Add(from, to);
		lineColors.Add(color);
	}
}

void ShadowMapDisplay2::AddSurroundLines(const Float4* pPoints, const Float3& color, List<Float4>& linePoints, List<Float3>& lineColors)
{
	for (unsigned i{ 0 }; i < 4; ++i)
	{
		const Float4& from{ pPoints[i] };
		const Float4& to{ pPoints[(i + 1) % 4] };
		linePoints.Add(from, to);
		lineColors.Add(color);
	}

}

void ShadowMapDisplay2::AddDepthLines(
	const EyeFrustum& eye, float fromDist, float toDist, const Float3& color,
	List<Float4>& linePoints, List<Float3>& lineColors)
{
	for (unsigned i{ 0 }; i < 4; ++i)
	{
		const Float4 from{ eye.Near[i]
		+ (eye.Far[i] - eye.Near[i]) * fromDist };
		const Float4 to{ eye.Near[i]
		+ (eye.Far[i] - eye.Near[i]) * toDist };

		linePoints.Add(from, to);
		lineColors.Add(color);
	}
}

void ShadowMapDisplay2::AddDepthLines(const Float4* pFrom, const Float4* pTo, const Float3& color, List<Float4>& linePoints, List<Float3>& lineColors)
{
	for (unsigned i{ 0 }; i < 4; ++i)
	{
		const Float4& from{ pFrom[i] };
		const Float4& to{ pTo[i] };
		linePoints.Add(from, to);
		lineColors.Add(color);
	}
}

void ShadowMapDisplay2::AddEyeLines(
	const EyeFrustum& eye,
	List<Float4>& linePoints, List<Float3>& lineColors)
{
	List<const Float4*> slicePoints{};
	slicePoints.Add(&eye.Near[0]);
	for (unsigned i{ 0 }; i < eye.Slices.GetSize(); i += 4)
		slicePoints.Add(&eye.Slices[i]);
	slicePoints.Add(&eye.Far[0]);

	const Float3 colors[]{
		Color::Red,
		Color::Green,
		Color::Blue,
		Color::White,
		Color::Yellow,
	};

	for (unsigned i{ 0 }; i < slicePoints.GetSize(); ++i)
	{
		AddSurroundLines(slicePoints[i], colors[i],
			linePoints, lineColors);
		if (i < slicePoints.GetSize() - 1)
			AddDepthLines(slicePoints[i], slicePoints[i + 1], colors[i],
				linePoints, lineColors);
	}
}

void ShadowMapDisplay2::TransformLines(List<Float4>& linePoints, const Float4X4& matrix)
{
	for (unsigned i{ 0 }; i < linePoints.GetSize(); ++i)
		linePoints[i] = linePoints[i] * matrix;
}

void ShadowMapDisplay2::AddLines(const List<Float3>& lines, const Float3& color, List<Float4>& linesDestination, List<Float3>& colorsDestination)
{
	for (unsigned i{ 1 }; i < lines.GetSize(); i += 2)
	{
		linesDestination.Add(
			Float4{ lines[i - 1],1 },
			Float4{ lines[i], 1 });
		colorsDestination.Add(color);
	}
}
