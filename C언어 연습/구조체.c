#include <stdio.h>

// person 구조체 정의
struct person {
    char name[20];
    char tel[20];
    char address[20];
};

// person 구조체를 person이라는 새로운 타입으로 정의
typedef struct person person;

int main(void) {
    int i;

    // person 구조체 배열 초기화
    person p[2] = { {"Hong gil dong", "010-1111-1111", "Jeonnam Sunchon"},
                   {"Lee sun sin", "010-2222-2222", "Jeonnam Sunchon"} };

    // 헤더 출력
    printf("%16s, %20s, %30s,\n\n", "name", "telephone number", "address");

    // 배열의 각 요소를 출력
    for (i = 0; i <= 1; i++) {
        printf("%16s, %20s, %30s\n", p[i].name, p[i].tel, p[i].address);
    }

    return 0;
}
