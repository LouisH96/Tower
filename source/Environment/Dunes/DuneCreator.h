#pragma once

namespace MyEngine
{
	namespace Rendering
	{
		class Image;
	}
}

class TowerAppRenderer;

class DuneCreator
{
public:
	explicit DuneCreator(TowerAppRenderer& renderer);
	~DuneCreator();

private:
	TowerAppRenderer& m_Renderer;
	Rendering::Image* m_pImage;
};
