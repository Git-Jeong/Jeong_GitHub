#include <stdio.h>
#include <string.h>

// 연락처 구조체 정의
struct Contact {
    char name[30];
    char phone[15];
    char email[30];
};

// 연락처 구조체를 Contact 타입으로 정의
typedef struct Contact Contact;

// 연락처 추가 함수
void addContact(Contact* contacts, int* contactCount) {
    // 연락처 배열이 가득 찼는지 확인
    if (*contactCount >= 10) {
        printf("연락처를 더 이상 추가할 수 없습니다.\n");
        return;
    }

    // 사용자로부터 정보 입력 받음
    printf("이름: ");
    scanf("%s", contacts[*contactCount].name);

    printf("전화번호: ");
    scanf("%s", contacts[*contactCount].phone);

    printf("이메일: ");
    scanf("%s", contacts[*contactCount].email);

    (*contactCount)++;  // 연락처 개수 증가

    printf("연락처가 추가되었습니다.\n");
}

// 연락처 목록 출력 함수
void displayContacts(Contact* contacts, int contactCount) {
    // 연락처가 비어 있는지 확인
    if (contactCount == 0) {
        printf("연락처가 비어 있습니다.\n");
        return;
    }

    // 헤더 출력
    printf("%-20s %-15s %-30s\n", "이름", "전화번호", "이메일");

    // 모든 연락처 정보 출력
    for (int i = 0; i < contactCount; i++) {
        printf("%-20s %-15s %-30s\n", contacts[i].name, contacts[i].phone, contacts[i].email);
    }
}

int main(void) {
    Contact contacts[10];  // 최대 10개의 연락처를 저장할 배열
    int contactCount = 0;   // 현재 연락처의 개수

    int choice;
    do {
        // 메뉴 출력
        printf("\n===== 연락처 관리 프로그램 =====\n");
        printf("1. 연락처 추가\n");
        printf("2. 연락처 목록 출력\n");
        printf("3. 종료\n");
        printf("메뉴 선택: ");
        scanf("%d", &choice);

        switch (choice) {
        case 1:
            addContact(contacts, &contactCount);
            break;
        case 2:
            displayContacts(contacts, contactCount);
            break;
        case 3:
            printf("프로그램을 종료합니다.\n");
            break;
        default:
            printf("올바른 메뉴를 선택하세요.\n");
        }

    } while (choice != 3);  // 3을 선택할 때까지 반복

    return 0;
}
