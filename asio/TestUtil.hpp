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


