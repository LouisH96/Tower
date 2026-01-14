#pragma once

namespace TowerGame
{
template<typename T>
class OffsetList
{
public:
	OffsetList(unsigned capacity = 0, unsigned initialOffset = 0);

	const T& Get(unsigned idx) const { return m_List[idx - m_Offset]; }
	T& Get(unsigned idx) { return m_List[idx - m_Offset]; }
	bool IsEmpty() const { return m_List.IsEmpty(); }

	unsigned Add(const T& object);
	T& AddEmpty();
	void Remove(unsigned idx);

	//Local
	const T& GetLocal(unsigned localIdx) const { return m_List[localIdx]; }
	T& GetLocal(unsigned localIdx) { return m_List[localIdx]; }
	unsigned GetLocalSize() const { return m_List.GetSize(); }
	void RemoveLocal(unsigned localIdx);

private:
	unsigned m_Offset{ 0 };
	List<T> m_List;
};
template<typename T>
inline OffsetList<T>::OffsetList(unsigned capacity, unsigned initialOffset)
	: m_Offset{ initialOffset }
	, m_List{ capacity }
{
}
template<typename T>
inline unsigned OffsetList<T>::Add(const T& object)
{
	m_List.Add(object);
	return m_List.GetSize() - 1;
}
template<typename T>
inline T& OffsetList<T>::AddEmpty()
{
	return m_List.AddEmpty();
}
template<typename T>
inline void OffsetList<T>::Remove(unsigned idx)
{
	idx -= m_Offset;
	RemoveLocal(idx);
}
template<typename T>
inline void OffsetList<T>::RemoveLocal(unsigned localIdx)
{
	if (localIdx > 0)
	{
		m_List[localIdx].Invalidate();
		return;
	}

	unsigned iInvalid{ 1 };
	while (true)
	{
		if (iInvalid >= m_List.GetSize()
			|| m_List[iInvalid].IsValid())
		{
			m_List.ShrinkFromBegin(iInvalid);
			break;
		}
		++iInvalid;
	}
	m_Offset += iInvalid;
}
}