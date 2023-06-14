#pragma once

namespace MyEngine
{
	namespace Rendering
	{
		class Texture;
		class Image;
	}
}

class TowerAppRenderer;

class DuneCreator
{
public:
	explicit DuneCreator(TowerAppRenderer& renderer);
	~DuneCreator();

	void Update();

private:
	TowerAppRenderer& m_Renderer;
	Rendering::Image* m_pImage;
	Rendering::Texture* m_pTexture;
};
