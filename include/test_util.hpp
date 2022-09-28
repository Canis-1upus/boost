#ifndef ___TEST_UTIL_HPP___
#define ___TEST_UTIL_HPP___

#include <string>

// �׽�Ʈ ����� ���/å���� ���
void FEATURE(const std::string& message);

// �׽�Ʈ ������ ���� ��Ȳ�� ����
void SCENARIO(const std::string& message);

// �ó����� ���࿡ �ʿ��� ���� ����
void GIVEN(const std::string& message);

// �ó������� �����ϴµ� �ʿ��� ������ ���
void WHEN(const std::string& message);

// �ó������� �Ϸ����� �� �����ؾ� �ϴ� ����� ���
void THEN(const std::string& message);
void AND(const std::string& message);

//
void RESULT(const std::string& message);

#define TEXT_UTIL_PREFIX	"[==========]"

/*
		foreground background
black        30(90)     40
red          31         41
green        32         42
yellow       33         43
blue         34         44
magenta      35         45
cyan         36         46
white        37         47

reset				0  (everything back to normal)
bold / bright       1  (often a brighter shade of the same colour)
underline			4
inverse				7  (swap foreground and background colours)
bold / bright off	21
underline off		24
inverse off			27

*/

#define TEXT_UTIL_SET_CLR	"\033[1;90m"
#define TEXT_UTIL_RESET		"\033[0m"

#define MACRO_TEST_OUTPUT(message)						\
	std::cout << TEXT_UTIL_SET_CLR << TEXT_UTIL_PREFIX	\
		<< " " << __func__ << ": " << message			\
		<< TEXT_UTIL_RESET << std::endl

#endif	// ___TEST_UTIL_HPP___