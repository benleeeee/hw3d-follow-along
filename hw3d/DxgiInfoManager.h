#pragma once
#include "CleanWin.h"
#include <wrl.h>
#include <vector>
#include <dxgidebug.h>

class DxgiInfoManager
{
public:
	DxgiInfoManager();
	~DxgiInfoManager() = default;
	DxgiInfoManager(const DxgiInfoManager&) = delete; //no copy constructor
	DxgiInfoManager& operator=(const DxgiInfoManager&) = delete; //no = operator
	void Set() noexcept;
	std::vector <std::string > GetMessages() const;
private:
	unsigned long long next = 0u;
	Microsoft::WRL::ComPtr<IDXGIInfoQueue> pDxgiInfoQueue;
};

