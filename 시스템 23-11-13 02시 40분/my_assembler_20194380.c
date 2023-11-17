/*
    �ý��� ���α׷��� ������
    20194380 ������
*/
#define _CRT_SECURE_NO_WARNINGS	
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "my_assembler_20194380.h"
#include "Operation_status.h"   //���� ������ �۾��� �ߴ��� ��Ÿ���� ���Ϸ� ������� ���Ұ��� ����
int main(void) {
    init_my_assembler();
    //����� �۾��� ���� �غ� ����

    //��ūȭ �۾� �׽�Ʈ �ڵ�
    for (int arry_i = 0; arry_i < token_line; arry_i++) {
        printf("[LINE : %2d]\n", arry_i);
        if (token_table[arry_i]->label != NULL) {
            printf("label : %s\n", token_table[arry_i]->label);
        }
        else {
            printf("\n");
        }
        printf("operator : %s\n", token_table[arry_i]->operator);
        for (int print_i = 0; print_i < MAX_OPERAND; print_i++) {
            if (token_table[arry_i]->operand[print_i][0] != '\0') {
                printf("Operand %d: %s\n", print_i + 1, token_table[arry_i]->operand[print_i]);
            }
        }
        printf("Comment: %s\n", token_table[arry_i]->comment);
        printf("\n\n");
    }

    assem_pass1();
    //pass1 ����
}

int init_my_assembler(void) {
    init_inst_file("inst.data");
    //inst.data������ inst_table����ü�� �����ϱ� ���� �Լ�
    init_input_file("input.txt");
    //input.txt������ input_data�迭�� �����ϱ� ���� �Լ�
    return 0;
}

int init_inst_file(uchar* inst_file) {
    FILE* inst_data_file = fopen(inst_file, "r");
    //inst_file�� ���޹��� inst.data������ �б� ���� �ҷ��� �� �������� inst_data_file������ ���� 
    if (inst_data_file == NULL) {
        //inst.data���� �ҷ����� ����
        printf("������ �� �� �����ϴ�.\n");
        return 1;
    }
    inst_index = 0;
    for (inst_index = 0; inst_index < MAX_INST; inst_index++) {
        inst_table[inst_index] = (inst*)malloc(sizeof(inst));
    }
    inst_index = 0;
    uchar temp_storage[3];
    while (inst_index < MAX_INST && fscanf(inst_data_file, "%s %s %d %02x",
        inst_table[inst_index]->str,
        &temp_storage[0],
        &inst_table[inst_index]->format,
        &inst_table[inst_index]->op) != EOF) {
        // ���� �����͸� ����ü�� �����ϴ� �۾� ����.
        // ���� ����ü �ε����� �̵�
        //	0:-		1:M		2:R		3:N		4:RR	5:RN
        if (strcmp(temp_storage, "-") == 0) {
            inst_table[inst_index]->ops = 0;
        }
        else if (strcmp(temp_storage, "M") == 0) {
            inst_table[inst_index]->ops = 1;
        }
        else if (strcmp(temp_storage, "R") == 0) {
            inst_table[inst_index]->ops = 2;
        }
        else if (strcmp(temp_storage, "N") == 0) {
            inst_table[inst_index]->ops = 3;
        }
        else if (strcmp(temp_storage, "RR") == 0) {
            inst_table[inst_index]->ops = 4;
        }
        else if (strcmp(temp_storage, "RN") == 0) {
            inst_table[inst_index]->ops = 5;
        }
        inst_index++;
    }

    //�ؿ� �ڵ�� inst.data������ �� �����ߴ��� Ȯ���� �ϴ� �ڵ���.
    /*
    int i;
    for (i = 0; i < inst_index; i++) {
        printf("[Line : %2d] : str: %9s,\t ops : %d,\t format : %d,\t op : %02x\n",
            i + 1,
            inst_table[i]->str,
            inst_table[i]->ops,
            inst_table[i]->format,
            inst_table[i]->op);
    }
    */
    fclose(inst_data_file);
    // ���� �ݱ�
    return 0;
}
int init_input_file(uchar* input_file) {
    int i = 0;
    uchar temp_char[MAX_LINES];
    FILE* input_txt_file = fopen(input_file, "r");
    //input_file�� ���޹��� input.txt������ �б� ���� �ҷ��� �� �������� input_txt_file������ ���� 
    if (input_txt_file == NULL) {
        printf("������ �� �� �����ϴ�.\n");
        return 1;
    }
    for (i = 0; i < MAX_LINES; i++) {
        input_data[i] = (uchar*)malloc(MAX_LINES);
        temp_char[i] = (uchar*)malloc(MAX_LINES);
    }
    i = 0;
    while (i < MAX_LINES && fgets(temp_char, MAX_LINES, input_txt_file) != NULL) {
        temp_char[strcspn(temp_char, "\n")] = '\0';
        strcpy(input_data[i], temp_char);
        bool isTabBeforeString = false;
        char* ptr = temp_char;
        char newString[MAX_LINES + 1]; // +1�� '-'�� �߰��ϱ� ����
        while (*ptr != '\0' && (*ptr == ' ' || *ptr == '\t')) {
            isTabBeforeString = true;
            strcpy(newString, "-");
            strcat(newString, temp_char);
            ptr++;
        }
        if (isTabBeforeString == true) {
            //������ ���� ���� ��� ����
            token_parsing(newString);
        }
        else {
            //���ڰ� ���� ���� ��� ����
            token_parsing(temp_char);
        }
        i++;
    }
    fclose(input_txt_file);
    //�ؿ� �ִ� �ּ� ó���� �ڵ�� input.txt������ �� ������ �ߴ��� Ȯ���� �ϱ� ���� �ۼ��� �ڵ���.
    /*
    int test_index;
    printf("���Ͽ��� ���� ������:\n");
    for (test_index = 0; test_index < i; test_index++) {
        printf("Line %2d: %s\n", test_index + 1, input_data[test_index]);
    }
    */
    return 0;
}
int token_parsing(uchar* before_str) {
    token_table[token_line] = (token*)malloc(sizeof(token));

    token_table[token_line]->label = NULL;
    token_table[token_line]->operator = NULL;
    for (int j = 0; j < MAX_OPERAND; ++j) {
        token_table[token_line]->operand[j][0] = '\0';  // �� ���ڿ��� �ʱ�ȭ
    }
    token_table[token_line]->comment[0] = '\0';  // �� ���ڿ��� �ʱ�ȭ


    char* token_str = strtok(before_str, " \t\n");

    if (token_str != NULL && token_str[0] != '.' && token_str[0] != '-') {
        token_table[token_line]->label = _strdup(token_str);
    }
    else if (token_str == NULL && token_str[0] != '.') {
        token_table[token_line]->label = "\0";
    }
    else if (token_str[0] == '.') {
        return;
    }

    token_str = strtok(NULL, " \t\n");
    if (token_str != NULL) {    
        token_table[token_line]->operator = _strdup(token_str);
    }
    else{
        strcpy(token_table[token_line]->operator, " ");
    }

    //���� ,�� �ִ��� Ȯ���Ͽ� operand�� ���� �ִ� �ڵ�
    int comma_contain_test;
    token_str = strtok(NULL, " \t\n");
    char** token_buf = (char**)malloc(sizeof(char*) * sizeof(token_str));
    if (token_buf == NULL) {
        printf(stderr, "�޸� �Ҵ� ����\n");
            return 1;
    }
    token_buf = _strdup(token_str);
    if (token_buf != NULL && strchr(token_buf, ',') != NULL) {
        comma_contain_test = 1;
    }
    else {
        comma_contain_test = 0;
    }
        
    if (comma_contain_test == 1) {
        //���� ���ڿ��� ,�� �ִٸ� 
        // strtok�� �̿��Ͽ� ,�� ��ūȭ
        char* comma_token = strtok(token_buf, ",");
        int i = 0;
        while (comma_token != NULL) {
            strcpy(token_table[token_line]->operand[i], comma_token);
            comma_token = strtok(NULL, ",");
            i++;
        }
        if (i < MAX_OPERAND) {
            for (i; i < MAX_OPERAND; i++) {
                token_table[token_line]->operand[i][0] = '\0';  // �� ���ڿ��� �ʱ�ȭ
            }
        }
    }
    else if (comma_contain_test == 0) {
        if (token_str != NULL) {
            strcpy(token_table[token_line]->operand[0], token_str);
            token_table[token_line]->operand[1][0] = '\0';
            token_table[token_line]->operand[2][0] = '\0';
        }
    }

    token_str = strtok(NULL, "\n");
    if (token_str != NULL) {
        strcpy(token_table[token_line]->comment, token_str);
    }
    else {
        strcpy(token_table[token_line]->comment, "");
    }
    token_line++;
    return 0;
}

static int assem_pass1(void) {
    make_opcode_output("optab.txt");
    make_symtab_output("symtab.txt");
}

void make_opcode_output(uchar* optable_txt) {
    FILE* optab_file = fopen("optab.txt", "w");
    if (optab_file == NULL) {
        printf("optab.txt ������ ���� �� �����ϴ�.\n");
        return 1;
    }
    return 0;
}


void make_symtab_output(uchar* symboltable_txt) {
    FILE* symtab_file = fopen("symtab.txt", "w");
    if (symtab_file == NULL) {
        printf("symtab.txt������ ���� �� �����ϴ�.\n");
        return 1;
    }

    return 0;
}

int search_opcode(uchar* str);

static int assem_pass2(void);
void make_objectcode_output(uchar* file_name);

