#pragma once
#include "CleanWin.h"
#include <vector>

class DxgiInfoManager
{
public:
	DxgiInfoManager();
	~DxgiInfoManager();
	DxgiInfoManager(const DxgiInfoManager&) = delete; //no copy constructor
	DxgiInfoManager& operator=(const DxgiInfoManager&) = delete; //no = operator
	void Set() noexcept;
	std::vector <std::string > GetMessages() const;
private:
	unsigned long long next = 0u;
	struct IDXGIInfoQueue* pDxgiInfoQueue = nullptr;

};

