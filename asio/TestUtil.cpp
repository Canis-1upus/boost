#include <iostream>

#include "TestUtil.hpp"

#define TAB_SPACE	"\t      "

void FEATURE(const std::string& message)
{
	std::cout << TAB_SPACE <<  "FEATURE : " << message << std::endl;
}
void SCENARIO(const std::string& message)
{
	std::cout << TAB_SPACE << "SCENARIO :" << message << std::endl;
}
void GIVEN(const std::string& message)
{
	std::cout << TAB_SPACE << "GIVEN :   " << message << std::endl;
}
void WHEN(const std::string& message)
{
	std::cout << TAB_SPACE << "WHEN :    " << message << std::endl;
}
void THEN(const std::string& message)
{
	std::cout << TAB_SPACE << "THEN :    " << message << std::endl;
}
void AND(const std::string& message)
{
	std::cout << TAB_SPACE << "AND :     " << message << std::endl;
}

void RESULT(const std::string& message)
{
	std::cout << "[  RESULT  ] : " << message << std::endl;
}

