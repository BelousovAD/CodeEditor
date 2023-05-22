#pragma once
#include "wx/treebase.h"

class TreeItemData : public wxTreeItemData {
public:
	TreeItemData()
		: wxTreeItemData() {
	}

	TreeItemData(const size_t& index)
		: m_index(index) {
	}

	~TreeItemData() {
	}

	const size_t getIndex() {
		return m_index;
	}

	void setIndex(const size_t& index) {
		m_index = index;
	}
private:
	size_t m_index = 0;
};