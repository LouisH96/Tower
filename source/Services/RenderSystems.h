#pragma once
#include "Rendering/Renderers/RendererFactory.h"
#include "Rendering/Renderers/R_LambertCam_Tex_Transform.h"
#include "Rendering/Renderers/R_LambertLight_Col.h"
#include "Rendering/Renderers/TextureRenderer.h"
#include "Rendering/Structs/ConstantBufferTypes.h"
#include "Rendering/Structs/VertexTypes.h"
#include <Renderer\TerrainRenderer.h>

class ArrowSystem;

namespace MyEngine
{
namespace Rendering
{
class R_LambertCam_Tex_Tran_Inst;
class FpsDisplay;
class Texture2DRenderer;
}
}

class RenderSystems
{
public:
	using SimpleRenderer = Rendering::RendererFactory::SimpleRenderer;
	using TextureRenderer = Rendering::TextureRenderer<Rendering::V_PosUv, Rendering::CB_CamMat>;
	using TransformRenderer = Rendering::R_LambertCam_Tex_Transform;

	static SimpleRenderer& GetSimpleRenderer() { return *m_pStatic->pSimpleRenderer; }
	static TransformRenderer& GetTransformRenderer() { return *m_pStatic->pTransformRenderer; }
	static TowerGame::TerrainRenderer& GetTerrainRenderer() { return *m_pStatic->pTerrainRenderer; }
	static TextureRenderer& GetTextureRenderer() { return *m_pStatic->pTextureRenderer; }
	static Rendering::RendererFactory::UnlitRenderer& GetUnlitRenderer() { return *m_pStatic->pUnlitRenderer; }
	static Rendering::Texture2DRenderer& GetTexture2DRenderer() { return *m_pStatic->pTexture2DRenderer; }

	RenderSystems();

	SimpleRenderer* pSimpleRenderer{};
	TransformRenderer* pTransformRenderer{};
	TowerGame::TerrainRenderer* pTerrainRenderer{};
	TextureRenderer* pTextureRenderer{};
	Rendering::RendererFactory::UnlitRenderer* pUnlitRenderer{};
	Rendering::Texture2DRenderer* pTexture2DRenderer{};

private:
	static const RenderSystems* m_pStatic;
};
