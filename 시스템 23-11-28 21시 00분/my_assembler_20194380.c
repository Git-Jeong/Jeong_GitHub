/*
    시스템 프로그래밍 과제물
    20194380 문정진


    아직 이번 학기의 과제 제출기간이 마감되지도 않았다 보니 해당 코드에서 중요한 부분들을 전부 삭제 해뒀습니다.


*/
#define _CRT_SECURE_NO_WARNINGS	
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "my_assembler_20194380.h"
#include "Operation_status.h"   
//내가 어디까지 작업을 했는지 나타내는 파일로 어셈블러의 역할과는 무관

int main(void) {
    init_my_assembler();
    //어셈블러 작업을 위한 준비 시작
    assem_pass1();
    //pass1 시작]

    //token table 출력문
    /*
    for (int i = 0; i < token_line; i++) {
        printf("[LINE : %2d]\t", i+1);
        if (token_table[i]->label != NULL) {
            printf("%s\t", token_table[i]->label);
            }
        else {
            printf("\t");
        }   
        printf("%s\t", token_table[i]->operator);
        for (int j = 0; j < MAX_OPERAND; j++) { 
            uchar buf_operand[20]; 
            size_t length_to_copy = sizeof(buf_operand) - 1; 
            strncpy(buf_operand, token_table[i]->operand[j], length_to_copy);
            if (j == 0) {
                if ((buf_operand[0] != '!') && (buf_operand[1] != '!')) {
                    printf("%s\t", buf_operand);
                    printf("%s\n", token_table[i]->comment);
                }
                else {
                    printf("\n");
                }
            }
            else if((j != 0) && (buf_operand[0] != '!')){
                if ((buf_operand[0] != '!') && (buf_operand[1] != '!')) {
                    printf("\t\t\t\t");
                    printf("%s\n", buf_operand);
                }
            }
        }
    }
    */
}