#pragma once
#include <exception>
#include <string>

class ChiliException : public std::exception //inherit from std::exception
{
public:
	ChiliException(int line, const char* file) noexcept; //Constructor with no exceps
	const char* what() const noexcept override; //override from std::exception. the const qualifier (after function name) means member variables cannot be changed inside the function unless they are marked 'mutable'
	virtual const char* GetType() const noexcept; //Gets the type of exception
	int GetLine() const noexcept;	//Get line exception occurred
	const std::string& GetFile() const noexcept;	//Get file exception occurred
	std::string GetOriginString() const noexcept;	//Formats the exception type + file + line
private:
	int line;			//Line number exception was thrown from
	std::string file;	//File exception was thrown from
protected:
	mutable std::string whatBuffer; //string that lives past the What() function call so when char* is passed back it still refers to living memory, otherwise it would point to empty space
};

