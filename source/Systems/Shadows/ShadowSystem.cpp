#include "pch.h"
#include "ShadowSystem.h"

#include <Transform\ViewMatrix.h>
#include <Transform\WorldMatrix.h>

using namespace TowerGame;
using namespace Rendering;

#undef near
#undef far

const Float3 ShadowSystem::m_LightDir{ Float3{.432709f,-.639439f,.635516f}.Normalized() };
const Float2 ShadowSystem::m_TextureSize{ 512 * 2 };

ShadowSystem::ShadowSystem()
	: m_Viewport{ m_TextureSize }
	, m_View{ ViewMatrix::From(m_LightDir) }
	, m_Sampler{ SamplerState::BorderMode::Value, SamplerState::BorderMode::Value, Float4{Float::MAX}, SamplerState::Filter::Linear, SamplerState::Comparison::Greater }
	, m_PrevForward{ 0.f }
{
	DepthStencilBuffer::Options options{};
	options.AsShaderResource = true;
	options.MultiSampling = DepthStencilBuffer::Options::MultiSampling::Disabled;
	for (unsigned iMap{ 0 }; iMap < NR_MAPS; ++iMap)
	{
		m_DepthStencil[iMap].Init(m_TextureSize, options);
		m_ShadowMap[iMap] = { m_DepthStencil[iMap].MakeShaderResourceView() };
	}
}

void ShadowSystem::Init(const Camera& camera)
{
	CalculatePlaneSize();
}

void ShadowSystem::Update()
{
	//Get Info
	Camera& camera{ CAMERA };

	//'Early' exit
	if (camera.GetForward().Dot(m_PrevForward) >= m_CosExtraAngle)
	{
		//Only update movement
		for (unsigned iSlice{ 0 }; iSlice < NR_SLICES; ++iSlice)
		{
			m_ViewProj[iSlice].SetRow3(
				-(m_View.AppliedToPoint(camera.GetPosition())
					+ m_Offsets[iSlice]
					) * m_Scales[iSlice]);
		}
		return;
	}

	//Store current forward
	m_PrevForward = camera.GetForward();

	//Prepare bounding calculation
	const Float4X4 transform{ camera.GetWorld() * m_View };
	const Float2 right{ transform[0][0], transform[1][0] };
	const Float2 up{ transform[0][1], transform[1][1] };
	const Float2 forward{ transform[0][2], transform[1][2] };

	//Bounding calculation
	const Float2 normalSize{ //normal is for frustum-plane at distance 1
		{ abs(right.x * m_PlaneSize.x)
		+ abs(up.x * m_PlaneSize.y)
		+ abs(forward.x * m_PlaneSize.z)
		},
		{ abs(right.y * m_PlaneSize.x)
		+ abs(up.y * m_PlaneSize.y)
		+ abs(forward.y * m_PlaneSize.z)
		}
	};
	const Float2 normalCenter{
		forward.x * m_PlaneCenterDepth,
		forward.y * m_PlaneCenterDepth };

	Vector2<Float2> minFactors;
	Vector2<Float2> maxFactors;
	for (unsigned iDim{ 0 }; iDim < 2; ++iDim)
	{
		const bool boolMin{ normalCenter[iDim] >= normalSize[iDim] };
		minFactors[iDim] = { Vector2<bool>{boolMin, !boolMin} };
		minFactors[iDim] *= Float::Max(normalCenter[iDim] - normalSize[iDim], -m_PlaneRadius);

		const bool boolMax{ normalCenter[iDim] <= -normalSize[iDim] };
		maxFactors[iDim] = { Vector2<bool>{boolMax, !boolMax} };
		maxFactors[iDim] *= Float::Min(normalCenter[iDim] + normalSize[iDim], m_PlaneRadius);
	}

	//Calculate ViewProjection for each Slice/Map
	float sliceBegin{ camera.GetNear() };
	for (unsigned iSlice{ 0 }; iSlice < NR_SLICES; ++iSlice)
	{
		//Prepare
		const Float2 caps{ sliceBegin, SLICE_END[iSlice] };
		const Float2 min{ minFactors.x.Dot(caps), minFactors.y.Dot(caps) };
		const Float2 max{ maxFactors.x.Dot(caps), maxFactors.y.Dot(caps) };
		const Float2 size{ max - min };
		const Float2 center{ min + size / 2.f };

		//Calculate and set ViewProjection
		Float4X4& viewProj{ m_ViewProj[iSlice] };
		viewProj = m_View;

		m_Scales[iSlice] = { 2.f / size.x, 2.f / size.y, 1.f / (m_PlaneRadius * caps.y + SHADOW_HEIGHT) };
		m_Offsets[iSlice] = Float3{ center.x, center.y, -SHADOW_HEIGHT };
		const Float3 movement{
			-(viewProj.AppliedToPoint(camera.GetPosition())
			+ m_Offsets[iSlice])
		};
		viewProj.SetRow3(movement);
		viewProj.GetCol0() *= m_Scales[iSlice].x;
		viewProj.GetCol1() *= m_Scales[iSlice].y;
		viewProj.GetCol2() *= m_Scales[iSlice].z;

		//End
		sliceBegin = caps.y;
	}
}

#pragma region OldUpdate
//Old Update which includes rotating & stretching to fill the map
/*

void ShadowSystem2::Update()
{
	//Get Info
	const Camera& camera{ CAMERA };

	if (camera.GetForward().Dot(m_PrevForward) >= m_CosExtraAngle * Constants::TO_RAD)
		return;
	else
		m_PrevForward = camera.GetForward();

	//Use ViewProj[0] temporary
	m_ViewProj[0] = camera.GetWorld() * m_View;
	const Float2 right{ m_View.AppliedToVector(camera.GetRight()).Xy().Normalized() };
	const Float2 forward{ -right.y, right.x };

	//Find bounds in light-space
	Float3 min{ Float::MAX, Float::MAX, -SHADOW_HEIGHT };
	Float3 max{ -Float::MAX };

	const float extraAngle{ std::acosf(m_CosExtraAngle) };
	const float depth{ cos(camera.GetHalfFov() - extraAngle * Constants::TO_RAD) / cos(camera.GetHalfFov()) };
	Float3 camCorner{
		tan(camera.GetHalfFov() + extraAngle * Constants::TO_RAD) * depth * camera.GetAspectRatio(),
		tan(camera.GetHalfFov() + extraAngle * Constants::TO_RAD) * depth,
		1
	};

	for (unsigned iCol{ 0 }; iCol < 2; ++iCol, camCorner.x *= -1)
		for (unsigned iRow{ 0 }; iRow < 2; ++iRow, camCorner.y *= -1)
		{
			Float3 point{ m_ViewProj[0].AppliedToVector(camCorner) };
			point = {
				right.Dot(point.Xy()),
				forward.Dot(point.Xy()),
				point.z
			};

			if (point.x < min.x) min.x = point.x;
			if (point.x > max.x) max.x = point.x;
			if (point.y < min.y) min.y = point.y;
			if (point.y > max.y) max.y = point.y;
			if (point.z > max.z) max.z = point.z;
		}

	//Make ViewProj matrices
	const Float3 eyePos{ m_ViewProj[0].GetRow3().Xyz() };

	const Vector3<Float2> minBounds{
		{(min.x > 0) * min.x, (min.x < 0) * min.x},
		{(min.y > 0) * min.y, (min.y < 0) * min.y},
		{(min.z > 0) * min.z, (min.z < 0) * min.z}
	};
	const Vector3<Float2> maxBounds{
		{(max.x < 0) * max.x, (max.x > 0) * max.x},
		{(max.y < 0) * max.y, (max.y > 0) * max.y},
		{(max.z < 0) * max.z, (max.z > 0) * max.z}
	};

	float sliceBegin{ camera.GetNear() };
	for (unsigned iSlice{ 0 }; iSlice < NR_SLICES; ++iSlice)
	{
		//
		const float& sliceEnd{ SLICE_END[iSlice] };
		const Float2 caps{ sliceBegin, sliceEnd };

		//Box
		min.x = minBounds.x.Dot(caps);
		min.y = minBounds.y.Dot(caps);
		min.z = minBounds.z.Dot(caps);
		max.x = maxBounds.x.Dot(caps);
		max.y = maxBounds.y.Dot(caps);
		max.z = maxBounds.z.Dot(caps);

		//Transform to lightSpace
		m_ViewProj[iSlice] = m_View;

		//Move to middle of the bottom of the box around slice
		const Float3 toBoxBottom{
			right * ((min.x + max.x) / 2) +
			forward * min.y,
			min.z
		};
		m_ViewProj[iSlice] *= WorldMatrix::Translation(Float3{
			  -(eyePos + toBoxBottom)
			});

		//Rotate
		m_ViewProj[iSlice] *= Float4X4{
			{right.x, right.y, 0, 0},
			{-right.y, right.x, 0, 0},
			{0,0,1,0},
			{0,0,0,1}
		};
		//Scale
		const Float3 size{
			abs(max.x - min.x),
			abs(max.y - min.y),
			abs(max.z - min.z)
		};
		m_ViewProj[iSlice] *= Float4X4{
			{2.f / (size.x), 0, 0, 0},
			{0, 2.f / (size.y), 0, -1}, //also y - 1 to center
			{0, 0, 1.f / (size.z), 0},
			{0, 0, 0, 1}
		};

		//End
		sliceBegin = SLICE_END[iSlice];
	}
}*/
#pragma endregion

void ShadowSystem::BeginCreateAny()
{
	m_Viewport.Activate();
}

void ShadowSystem::BeginCreate(unsigned iMap)
{
	m_DepthStencil[iMap].Clear();
	ID3D11RenderTargetView* renderTargets[1]
	{
		nullptr
	};
	Globals::pGpu->GetContext().OMSetRenderTargets(1, renderTargets, m_DepthStencil[iMap].GetView());
}

void ShadowSystem::BeginRender()
{
	LightBuffer buffer{};
	buffer.Forward = m_LightDir;
	for (unsigned iMap{ 0 }; iMap < NR_MAPS; ++iMap)
		buffer.ViewProjection[iMap] = m_ViewProj[iMap];

	//Todo
	m_ShadowMap[0].Activate(1);
	m_ShadowMap[1].Activate(2);
	m_ShadowMap[2].Activate(3);
	m_LightBuffer.Update(buffer);
	m_LightBuffer.Activate(2);
	m_Sampler.Activate(1);
}

void ShadowSystem::EndRender()
{
	for (unsigned i{ 0 }; i < NR_MAPS; ++i)
		Texture::Unset(i + 1);
}

void ShadowSystem::CalculatePlaneSize()
{
	//Calculate the bounding box around the view plane at distance 1,
	//	taking into account the allowed movement (ExtraAngle)
	// 
	// note: this is still slightly flawed, in some situations 
	// corners of the view-plane can get out of these bounds.
	// For example when looking straight of the light and moving horizontally.
	// These calculations work perfectly when looking parallel to the ground.
	// Only when looking up or down there's a (small) chance a corner gets slightly out.

	const float tan{ CAMERA.GetTanHalfFov() };
	const float ar{ CAMERA.GetAspectRatio() };
	const float cosE{ m_CosExtraAngle };
	const float sinE{ std::sqrtf(1 - m_CosExtraAngle * m_CosExtraAngle) };

	m_PlaneSize.x = Float::Max(tan * ar, tan * ar * cosE + sinE);
	m_PlaneSize.y = Float::Max(tan, tan * cosE + sinE);

	const float zMin{ Float::Min(-tan * ar * sinE + cosE, -tan * sinE + cosE) };
	const float zMax{ Float::Max(tan * sinE + cosE, tan * ar * sinE + cosE, 1) };
	m_PlaneSize.z = zMax - zMin;
	m_PlaneCenterDepth = m_PlaneSize.z / 2.f + zMin;

	m_PlaneRadius = std::sqrtf(tan * tan + tan * tan * ar * ar + 1);
}
