/******************************************************************************************
*	Chili DirectX Framework Version 16.10.01											  *
*	Surface.h																			  *
*	Copyright 2016 PlanetChili <http://www.planetchili.net>								  *
*																						  *
*	This file is part of The Chili DirectX Framework.									  *
*																						  *
*	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
*	it under the terms of the GNU General Public License as published by				  *
*	the Free Software Foundation, either version 3 of the License, or					  *
*	(at your option) any later version.													  *
*																						  *
*	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
*	GNU General Public License for more details.										  *
*																						  *
*	You should have received a copy of the GNU General Public License					  *
*	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
******************************************************************************************/
#pragma once
#include "CleanWin.h"
#include "ChiliException.h"
#include <string>
#include <assert.h>
#include <memory>


class Surface
{
public:
	class Colour
	{
	public:
		unsigned int dword;	//Bitmask 4bytes (32bit) long, one byte per channel for rgba (xrgb)
	public:
		constexpr Colour() noexcept : dword()			//Default constructor
		{}
		constexpr Colour(const Colour& col) noexcept	//Copy constructor
			:
			dword( col.dword)
		{}
		constexpr Colour(unsigned int dw) noexcept		//Int based constructor
			:
			dword(dw)
		{}
		constexpr Colour(unsigned char x, unsigned char r, unsigned char g, unsigned char b) noexcept
			:
			dword( (x << 24u) | (r << 16u) | (g << 8u) | b)	//Bitmasking xrgb component constructor
		{}
		constexpr Colour( unsigned char r, unsigned char g, unsigned char b ) noexcept
			:
			dword( (r << 16u) | (g << 8u) | b )				//Bitmasking rgb component constructor
		{}
		constexpr Colour( Colour& col, unsigned char x ) noexcept
			:
			dword( x << 24u | col.dword )					//Bitmasking x channel onto copy constructor
		{}
		Colour& operator =( Colour colour ) noexcept		//Ref returned assign (=) operator
		{
			dword = colour.dword;
			return *this;
		}
		constexpr unsigned char GetX() const noexcept
		{
			return dword >> 24u;
		}
		constexpr unsigned char GetA() const noexcept
		{
			return GetX();
		}
		constexpr unsigned char GetR() const noexcept
		{
			return (dword >> 16u) & 0xFFu;
		}
		constexpr unsigned char GetG() const noexcept
		{
			return (dword >> 8u) & 0xFFu;
		}
		constexpr unsigned char GetB() const noexcept
		{
			return dword & 0xFFu;
		}
		void SetX( unsigned char x ) noexcept
		{
			dword = (dword & 0xFFFFFFu) | (x << 24u);
		}
		void SetA( unsigned char a ) noexcept
		{
			SetX( a );
		}
		void SetR( unsigned char r ) noexcept
		{
			dword = (dword & 0xFF00FFFFu) | (r << 16u);
		}
		void SetG( unsigned char g ) noexcept
		{
			dword = (dword & 0xFFFF00FFu) | (g << 8u);
		}
		void SetB( unsigned char b ) noexcept
		{
			dword = (dword & 0xFFFFFF00u) | b;
		}
	};
	//-----------------------------------------------------------

public:
	class Exception : public ChiliException
	{
	public:
		Exception( int line, const char* file, std::string note ) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		const std::string& GetNote() const noexcept;
	private:
		std::string note;
	};
	//-----------------------------------------------------------

public:
	Surface( unsigned int width, unsigned int height ) noexcept;
	Surface( Surface&& source ) noexcept;
	Surface( Surface& ) = delete;
	Surface& operator=( Surface&& donor ) noexcept;
	Surface& operator=( const Surface& ) = delete;
	~Surface();
	void Clear( Colour fillValue ) noexcept;
	void PutPixel( unsigned int x, unsigned int y, Colour c ) noexcept(!IS_DEBUG);
	Colour GetPixel( unsigned int x, unsigned int y ) const noexcept(!IS_DEBUG);
	unsigned int GetWidth() const noexcept;
	unsigned int GetHeight() const noexcept;
	Colour* GetBufferPtr() noexcept;
	const Colour* GetBufferPtr() const noexcept;
	const Colour* GetBufferPtrConst() const noexcept;
	static Surface FromFile( const std::string& name );
	void Save( const std::string& filename ) const;
	void Copy( const Surface& src ) noexcept(!IS_DEBUG);
private:
	Surface( unsigned int width, unsigned int height, std::unique_ptr<Colour[]> pBufferParam ) noexcept;
private:
	std::unique_ptr<Colour[]> pBuffer; //Array of colours to make surface
	unsigned int width;
	unsigned int height;

};

