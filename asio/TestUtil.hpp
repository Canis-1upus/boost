#include <string>

// 테스트 대상의 기능/책임을 명시
void FEATURE(const std::string& message);

// 테스트 목적에 대한 상황을 설명
void SCENARIO(const std::string& message);

// 시나리오 진행에 필요한 값을 설정
void GIVEN(const std::string& message);

// 시나리오를 진행하는데 필요한 조건을 명시
void WHEN(const std::string& message);

// 시나리오를 완료했을 때 보장해야 하는 결과를 명시
void THEN(const std::string& message);
void AND(const std::string& message);

//
void RESULT(const std::string& message);


