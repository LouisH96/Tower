#include "pch.h"
#include "ShadowMapController.h"

#include <Generation\RectGenerator.h>
#include <Services\GameplaySystems.h>
#include <Services\RenderSystems.h>
#include <Character\Character.h>
#include <DataStructures\Adders\ArrayAdder.h>
#include <Rendering\State\Mesh.h>
#include <Rendering\State\Texture.h>
#include <Rendering\State\DepthStencilBuffer.h>
#include <Renderer\ShadowRenderer.h>

#include <d3d11.h>

using namespace TowerGame;
using namespace Rendering;

ShadowMapController::ShadowMapController()
{
}

void ShadowMapController::Start(ShadowRenderer& shadowMapRenderer)
{
	Texture* pTexture{ MakeTexture(shadowMapRenderer) };
	InitDemoQuad(pTexture);
}

void ShadowMapController::InitDemoQuad(Texture* pTexture)
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
	vertices[2].Uv = { 1,0 };
	vertices[3].Uv = { 1,1 };
	Generator::GenerateIndices(ArrayAdder<int>{indices}, 0);

	Mesh* pMesh{ Mesh::Create<Vertex>(vertices, indices) };

	RenderSystems::GetTextureRenderer().AddMesh(pMesh, pTexture);
}

Texture* ShadowMapController::MakeTexture(ShadowRenderer& renderer)
{
	//Get
	DepthStencilBuffer& dsBuffer{ renderer.GetDepthStencilBuffer() };
	ID3D11DepthStencilView* pDsView{ dsBuffer.GetView() };

	CD3D11_DEPTH_STENCIL_VIEW_DESC dsViewDesc;
	pDsView->GetDesc(&dsViewDesc);

	ID3D11Resource* dsResource;
	pDsView->GetResource(&dsResource);

	//Make ShaderResourceViewDesc
	D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc{};
	resourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	resourceViewDesc.Texture2D.MipLevels = 1;

	//Make ShaderResourceView
	ID3D11ShaderResourceView* pShaderResourceView;
	const HRESULT result{
		Globals::pGpu->GetDevice().CreateShaderResourceView(dsResource, &resourceViewDesc, &pShaderResourceView)
	};
	SAFE_RELEASE(dsResource);

	if (FAILED(result)) {
		Logger::PrintError("[ShadowMapController::MakeTexture]");
		SAFE_RELEASE(pShaderResourceView);
	}

	//Make Texture
	Texture* pTexture{ new Texture(pShaderResourceView) };

	return pTexture;
}