#include "pch.h"
#include "ShadowMapController.h"

#include <Generation\RectGenerator.h>
#include <Services\GameplaySystems.h>
#include <Services\RenderSystems.h>
#include <Character\Character.h>
#include <DataStructures\Adders\ArrayAdder.h>
#include <Rendering\State\Mesh.h>
#include <Rendering\State\Texture.h>

using namespace TowerGame;

ShadowMapController::ShadowMapController()
{
}

void ShadowMapController::Start()
{
	InitDemoQuad();
}

void ShadowMapController::InitDemoQuad()
{
	using namespace Rendering;

	using Renderer = RenderSystems::TextureRenderer;
	using Vertex = Renderer::VertexType;
	static constexpr ModelTopology Topology{ ModelTopology::TriangleListIdx };
	using Generator = RectGenerator<Topology>;

	const Float3& characterPos{ GameplaySystems::GetCharacter().GetPosition() };
	const Float3 planePos{ characterPos + Float3{ 0, 1, 3 } };

	const RectFloat rect{ {}, Float2{1,1} };

	Array<int> indices{ Generator::GetNrIndices() };
	Array<Vertex> vertices{ Generator::GetNrVertices() };
	auto combinator = [&planePos](const Float2& pos)
		{
			return Vertex{ Float3{
					planePos.x + pos.x,
					planePos.y + pos.y,
					planePos.z} };
		};

	Generator::GenerateVertices(combinator, ArrayAdder<Vertex>{vertices}, rect);
	vertices[0].Uv = { 0,1 };
	vertices[1].Uv = { 0,0 };
	vertices[2].Uv = { 1, 0 };
	vertices[3].Uv = { 1,1 };
	Generator::GenerateIndices(ArrayAdder<int>{indices}, 0);

	RenderSystems::GetTextureRenderer().AddMesh(vertices, indices, Resources::Local(L"PolygonFantasyKingdom_Texture_01_A.png"));
}
