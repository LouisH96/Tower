#include "pch.h"
#include "Tower.h"

#include <Rendering\State\Mesh.h>
#include <Rendering\Structs\VertexTypes.h>
#include <Systems\Collisions\CollisionSystem.h>
#include <Transform\WorldMatrix.h>

using namespace TowerGame;
using namespace Rendering;

void Tower::Init(Desc& desc)
{
	m_Position = desc.Position;
	m_RoofSize = desc.RoofSize;
	m_Height = desc.Height;
}

void Tower::AddModelsIn(StaticModelSystem::InitData& staticModelData)
{
	using TextureGroup = StaticModelSystem::InitData::TextureGroup;
	using ModelGroup = StaticModelSystem::InitData::ModelGroup;

	m_Position.z -= 1;
	m_Position.y += 3;

	struct Orientation
	{
		Float3 Pos;
		Float3 Eulers;
	};
	struct Model {
		std::wstring Path;
		Float3 Scale{ 1, 1, 1 };
		List<Orientation> Instances;
	};
	struct Texture {
		std::wstring Path;
		List<Model> Models;
	};

	//Input
	List<Texture> data{};

	Texture& adventure{ data.AddEmpty() };
	adventure.Path = L"PolyAdventureTexture_01.png";

	Model& village3{ adventure.Models.AddEmpty() };
	village3.Path = L"SM_Bld_Village_03.fbx";
	village3.Scale = { 1,2,1 };
	village3.Instances.Add({
		{ -3.96f, -1.02f, -2.09f },
		{0.f, -180.f, 0.f } });

	Model& village5{ adventure.Models.AddEmpty() };
	village5.Path = L"SM_Bld_Village_05.fbx";
	village5.Instances.Add({
		{3.23f, 3.06f, -0.2500038f},
		{0,-90,0} });

	Model& village6{ adventure.Models.AddEmpty() };
	village6.Path = L"SM_Bld_Village_06.fbx";
	village6.Scale = { 1,1,1.05373f };
	village6.Instances.Add({
		{1.649994f, 0, -1.099998f},
		{ 0,-180,0 } });

	Model& drapes{ adventure.Models.AddEmpty() };
	drapes.Path = L"SM_Bld_Village_WindowDrapes_01.fbx";
	drapes.Scale = Float3{ 0.97691f, 1.11162f, 1.34364f };
	drapes.Instances.Add({
		{5.18f, 2.72f, -1.900002f},
		{ 0, 91, 0 } });
	drapes.Instances.Add({
		{1.167f, 6.054f, -0.2f},
		{4.886f,-88.835f, 0.f } });

	Model& stall{ adventure.Models.AddEmpty() };
	stall.Path = L"SM_Bld_Stall_Cover_02.fbx";
	stall.Instances.Add({
		{3.729973f, 0, -5.740005f},
		{0,-180,0} });

	Model& cart{ adventure.Models.AddEmpty() };
	cart.Path = L"SM_Prop_Cart_03.fbx";
	cart.Instances.Add({
		{ -0.53f, -0.1f, -5.735f },
		{5.608f, -101.065f, 3.665f} });

	Model& crate{ adventure.Models.AddEmpty() };
	crate.Path = L"SM_Prop_Crate_01.fbx";
	crate.Scale = Float3{ 1.27147f };
	crate.Instances.Add({
		{4.109985f,3.23f,-3.210007f},
		{0,-167.389f,0} });
	crate.Instances.Add({
		{-1.407001f,3.21f,-1.306f},
		{0,-162.879f,0} });

	Texture& knights{ data.AddEmpty() };
	knights.Path = L"Texture_01.png";

	Model& stairs{ knights.Models.AddEmpty() };
	stairs.Path = L"SM_Bld_Rockwall_Stairs_01.fbx";
	stairs.Scale = Float3{ 1.1679f };
	stairs.Instances.Add({
		{-1.339999f, 2.26f, -2.930031f},
		{0,90,0} });
	stairs.Instances.Add({
		{0.1199989f, 0, 3.029999f},
		{0,90,0} });

	Model& trapDoor{ knights.Models.AddEmpty() };
	trapDoor.Path = L"SM_Prop_Trapdoor_01.fbx";
	trapDoor.Instances.Add({
		{-3.159f, 5.532f, -1.124f},
		{} });

	Model& dirtFloor{ knights.Models.AddEmpty() };
	dirtFloor.Path = L"SM_Env_Path_Dirt_02.fbx";
	dirtFloor.Scale = Float3{ 1.1591f };
	dirtFloor.Instances.Add({
		{-2.28f, 5.52f, -0.38f},
		{} });

	Texture& western{ data.AddEmpty() };
	western.Path = L"PolygonWestern_Texture_01_A.png";

	Model& ladder{ western.Models.AddEmpty() };
	ladder.Path = L"SM_Prop_Ladder_01.fbx";
	ladder.Scale = { 1.2917f, 1.390386f, 1.29171f };
	ladder.Instances.Add({
		{-2.606f, 5.569f, -1.077f},
		{78.157f, 87.524f, 1.905f} });

	//Add to InitData
	for (unsigned iTexture{ 0 }; iTexture < data.GetSize(); ++iTexture)
	{
		Texture& textureData{ data[iTexture] };

		TextureGroup& texture{ staticModelData.TextureGroups.AddEmpty() };
		texture.Path = Resources::Local(textureData.Path);

		for (unsigned iModel{ 0 }; iModel < textureData.Models.GetSize(); ++iModel)
		{
			Model& modelData{ textureData.Models[iModel] };

			ModelGroup& model{ texture.ModelGroups.AddEmpty() };
			model.Path = Resources::Local(modelData.Path);
			model.Scale = modelData.Scale;

			for (unsigned iInstance{ 0 }; iInstance < modelData.Instances.GetSize(); ++iInstance)
			{
				Orientation& instanceData{ modelData.Instances[iInstance] };
				
				//Data came from Unity scene.
					//Quaternion there cames from (eulers): z->x->y
				Quaternion quat{ Quaternion::FromAxis(
					{ 0,0,1 }, instanceData.Eulers.z * Constants::TO_RAD
				) };
				quat.RotateBy({ 1,0,0 }, instanceData.Eulers.x * Constants::TO_RAD);
				quat.RotateBy({ 0,1,0 }, instanceData.Eulers.y * Constants::TO_RAD);
				quat.Normalize();

				model.Matrices.Add(WorldMatrix::FromPosAndQuat(
					instanceData.Pos + m_Position,
					quat
				));
			}
		}
	}
}
