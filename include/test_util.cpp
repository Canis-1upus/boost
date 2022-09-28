#include <iostream>

#include "test_util.hpp"

void FEATURE(const std::string& message) { MACRO_TEST_OUTPUT(message); }
void SCENARIO(const std::string& message) { MACRO_TEST_OUTPUT(message); }
void GIVEN(const std::string& message) { MACRO_TEST_OUTPUT(message); }
void WHEN(const std::string& message) { MACRO_TEST_OUTPUT(message); }
void THEN(const std::string& message) { MACRO_TEST_OUTPUT(message); }
void AND(const std::string& message) { MACRO_TEST_OUTPUT(message); }

void RESULT(const std::string& message) { MACRO_TEST_OUTPUT(message); }