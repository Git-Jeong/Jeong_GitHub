/*
    �ý��� ���α׷��� ������
    20194380 ������


    ���� �̹� �б��� ���� ����Ⱓ�� ���������� �ʾҴ� ���� �ش� �ڵ忡�� �߿��� �κе��� ���� ���� �ص׽��ϴ�.


*/
#define _CRT_SECURE_NO_WARNINGS	
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "my_assembler_20194380.h"
#include "Operation_status.h"   
//���� ������ �۾��� �ߴ��� ��Ÿ���� ���Ϸ� ������� ���Ұ��� ����

int main(void) {
    init_my_assembler();
    //����� �۾��� ���� �غ� ����
    assem_pass1();
    //pass1 ����]

    //token table ��¹�
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