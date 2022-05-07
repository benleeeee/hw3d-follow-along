#pragma once
#include "CleanWin.h"
#include <unordered_map>

class WindowsMessageMap
{
public:
	WindowsMessageMap();		//Constructor
	std::string operator()(DWORD msg, LPARAM lp, WPARAM wp) const;	//operator overloading - 
																	//overloads when inst of class is called like a function
																	//e.g.	WindowsMessageMap myMap;
																	//		myMap();

private:
	std::unordered_map<DWORD, std::string> map;
};

