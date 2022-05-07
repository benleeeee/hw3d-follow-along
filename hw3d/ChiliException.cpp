#include "ChiliException.h"
#include <sstream>

ChiliException::ChiliException(int line, const char* file) noexcept
	:
	line(line),	//member initialiser list, initialise int line & string file
	file(file)
{

}

const char* ChiliException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl	//"<type>
		<< GetOriginString();		// <exception string>"

	//Store output in member variable buffer, or else it deletes at end of func
	whatBuffer = oss.str();
	return whatBuffer.c_str();	
}

const char* ChiliException::GetType() const noexcept
{
	return "Chili Exception";
}

int ChiliException::GetLine() const noexcept
{
	return line;
}

const std::string& ChiliException::GetFile() const noexcept
{
	return file;
}

std::string ChiliException::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[File] " << file << std::endl
		<< "[Line] " << line;
	return oss.str();
}