#pragma once

namespace TowerGame
{
class PauseMenu
{
public:

	void Toggle();
	void SetOpen(bool setOpen);
	void Open();
	void Close();

private:
	bool m_IsPaused{ false };
};
}