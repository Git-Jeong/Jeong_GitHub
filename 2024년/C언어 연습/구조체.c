#include <stdio.h>

// person ����ü ����
struct person {
    char name[20];
    char tel[20];
    char address[20];
};

// person ����ü�� person�̶�� ���ο� Ÿ������ ����
typedef struct person person;

int main(void) {
    int i;

    // person ����ü �迭 �ʱ�ȭ
    person p[2] = { {"Hong gil dong", "010-1111-1111", "Jeonnam Sunchon"},
                   {"Lee sun sin", "010-2222-2222", "Jeonnam Sunchon"} };

    // ��� ���
    printf("%16s, %20s, %30s,\n\n", "name", "telephone number", "address");

    // �迭�� �� ��Ҹ� ���
    for (i = 0; i <= 1; i++) {
        printf("%16s, %20s, %30s\n", p[i].name, p[i].tel, p[i].address);
    }

    return 0;
}
