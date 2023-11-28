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
    //�о�� ���ڿ�, \0�� ���� ������ ����Ͽ� [3]���� �ص�.
    while (inst_index < MAX_INST && fscanf(inst_data_file, "%s %s %d %02X",
        inst_table[inst_index]->str,
        &temp_storage[0],
        &inst_table[inst_index]->format,
        &inst_table[inst_index]->op) != EOF) {
        // ���� �����͸� ����ü�� �����ϴ� �۾� ����.
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
    return 0;
}

int token_parsing(uchar* before_str) {
    token_table[token_line] = (token*)malloc(sizeof(token));

    token_table[token_line]->label = NULL;
    token_table[token_line]->operator = NULL;
    for (int j = 0; j < MAX_OPERAND; ++j) {
        token_table[token_line]->operand[j][0] = '!!';  // ���� ������ Ȯ���ϱ� ���� !!�� �ʱ�ȭ
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
        if ((strcmp(token_table[token_line]->operator, "START") == 0) && (operator_START_test == 0)) {
            operator_START_test = 1;
        }
        else if ((strcmp(token_table[token_line]->operator, "START") != 0) && (operator_START_test == 1)) {
            operator_START_test = 2;
            next_operator_index = next_operator_index_test;
        }
    }
    else{
        strcpy(token_table[token_line]->operator, " ");
    }
    
    //���� ,�� �ִ��� Ȯ���Ͽ� operand�� ���� �ִ� �ڵ�
    int comma_contain_test;
    int space_test = 0;
    token_str = strtok(NULL, "\t\n");
    char** token_buf = (char**)malloc(sizeof(char*) * sizeof(token_str));
    if (token_buf == NULL) {
        printf(stderr, "�޸� �Ҵ� ����\n");
            return 1;
    }
    token_buf = _strdup(token_str);
    if (token_buf != NULL && strchr(token_buf, ',') != NULL) {
        comma_contain_test = 1;
    }
    else if (token_buf != NULL && strchr(token_buf, ' ') != NULL) {
        comma_contain_test = 0;
        space_test = 1;
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
            next_operator_index_test++;
        }
        if (i < MAX_OPERAND) {
            for (i; i < MAX_OPERAND; i++) {
                token_table[token_line]->operand[i][0] = '!!';  // �� ���ڿ��� 
                next_operator_index_test++;
            }
        }
    }
    else if (comma_contain_test == 0) {
        if (token_str != NULL) {
            strcpy(token_table[token_line]->operand[0], token_str);
            next_operator_index_test = +MAX_OPERAND;
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
    fprintf(optab_file, "String\t\tFormat\tOP\n\n");
    for (int i = 0; i < inst_index; i++) {
        for (int j = 0; j <= token_line; j++) {
            if (token_table[j] != NULL && token_table[j]->operator != NULL) {
                if (strcmp(token_table[j]->operator, inst_table[i]->str) == 0 && inst_table[i]->format != 0) {
                    // token_table[j]->operator�� inst_table[i]->str�� �����ϸ鼭, format != 0
                    if (optab_file != NULL) {
                        fprintf(optab_file, "%-10s\t%-2d\t%02X\n", inst_table[i]->str, inst_table[i]->format, inst_table[i]->op);
                        break;
                    }
                }
            }
        }
    }
    fclose(optab_file);
    return 0;
}

void make_symtab_output(uchar* symboltable_txt) {
    FILE* symtab_file = fopen("symtab.txt", "w");
    if (symtab_file == NULL) {
        printf("symtab.txt ������ ���� �� �����ϴ�.\n");
        return;
    }
    int index = token_line;
    uchar buf_symbol_operator[8];
    uchar buf_symbol_label[8];
    uchar buf_symbol_operand[MAX_OPERAND][20];
    int symbol_index = 0;
    int buffer_locctr;
    int bufend_loctr;
    for (int i = 0; i < index; i++) {
        if (token_table[i]->label != NULL) {
            size_t length_to_copy = sizeof(buf_symbol_label) - 1;
            strncpy(buf_symbol_label, token_table[i]->label, length_to_copy);
        }
        else {
            buf_symbol_label[0] = '\0';
        }

        if (token_table[i]->operator != NULL) {
            size_t length_to_copy = sizeof(buf_symbol_operator) - 1;
            strncpy(buf_symbol_operator, token_table[i]->operator, length_to_copy);
            buf_symbol_operator[length_to_copy] = '\0';
        }
        else {
            buf_symbol_operator[0] = '\0';
        }
        int j;
        for (j = 0; j < MAX_OPERAND; j++) {
            if (token_table[i]->operand[j][0] != '!') {
                size_t length_to_copy = sizeof(buf_symbol_operand[j]) - 1;
                strncpy(buf_symbol_operand[j], token_table[i]->operand[j], length_to_copy);
                buf_symbol_operand[j][length_to_copy] = '\0';  // Null ���� ���� �߰�
            }
            else {
                buf_symbol_operand[j][0] = '\0';  // '\0' �߰�
            }
        }

        if (strcmp(buf_symbol_operator, "START") == 0) {
            //START�� ������
            locctr = strtol(token_table[operator_START]->operand[0], NULL, 16);
            //���ڿ� ���·� operand���� ��� ������, 0�� �ش��ϴ� ���� ���� ��. 
            i = next_operator_index - 1;
        }

        else if (strcmp(buf_symbol_operator, "RESB") == 0){
            //RESB�� ������
            strncpy(sym_table[symbol_line].symbol, buf_symbol_label, sizeof(sym_table[i].symbol) - 1);
            sym_table[symbol_line].addr = locctr;
            long int hexValue = strtol(buf_symbol_operand[0], NULL, 10);
            locctr = locctr + hexValue;
            symbol_line++;
        }

        else if (buf_symbol_operator[0] != '\0') {
            if (strcmp(buf_symbol_label, "FIRST") == 0) {
                //Fisrt�� ������
                strncpy(sym_table[symbol_line].symbol, buf_symbol_label, sizeof(sym_table[i].symbol) - 1);
                sym_table[symbol_line].addr = locctr;
                symbol_line++;
            }
            else if ((buf_symbol_label[0] != '\0') && (strcmp(buf_symbol_label, "FIRST") != 0)) {
                //START, FIRST, RESB�� ���� �ƴ϶��
                strncpy(sym_table[symbol_line].symbol, buf_symbol_label, sizeof(sym_table[i].symbol) - 1);
                sym_table[symbol_line].addr = locctr;
                symbol_line++;
            }

            if (buf_symbol_operator[0] == '+') {
                //4������ ���
                locctr = locctr + 4;
            }
            else if (buf_symbol_operator[0] != '+') {
                //4������ �ƴ� ���.
                locctr = locctr + 3;
            }
            //printf("string = %s, loctr = %d\n", buf_symbol_operator, locctr);
        }

    }
    fprintf(symtab_file, "Line\t\tSymbol\t\tlocctr\n");
    fprintf(symtab_file, "--------------------------------------\n\n");
    for (int i = 0; i < symbol_line; i++) {
        fprintf(symtab_file, "[%02d]\t\t", i + 1);
        fprintf(symtab_file, "%s\t\t", sym_table[i].symbol);
        fprintf(symtab_file, "%04X\n", sym_table[i].addr);
    }
    fclose(symtab_file);
    return;
}

int search_opcode(uchar* str);
static int assem_pass2(void);
void make_objectcode_output(uchar* file_name);