
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
//#include "Operation_status.h"   
//���� ������ �۾��� �ߴ��� ��Ÿ���� ���Ϸ� ������� ���Ұ��� ����

int main(void) {
    init_my_assembler();
    assem_pass1();
    assem_pass2();
    token_test();      //token_parsing, locctr, object_code�� ��ü������ �׽�Ʈ ������ �Լ�.
    //literal_test();      //���ͷ� ���̺� �׽�Ʈ �Լ�.
}

int init_my_assembler(void) {
    init_inst_file("inst.data");
    //inst.data������ inst_table����ü�� �����ϱ� ���� �Լ�
    init_input_file("input.txt");
    //input.txt������ ��ūȭ�� ���� ���κ��� �ҷ����� ���� �Լ�
    return 0;
}


/* -----------------------------------------------------------------------------------
* ���� : optable ������ ���� inst_file�� �ҷ����� �Լ�
* �Ű� : ���� ����(inst_file = inst.data)
* ��ȯ : ���� = 0, ���� = -1
* ���� : token�� ���� �����°� �ƴ϶� fscanf�� ���� �ϴٺ��� ����ȭ�� ������ �־�߸� ������.
* -----------------------------------------------------------------------------------
*/
int init_inst_file(uchar* inst_file) {
    FILE* inst_data_file = fopen(inst_file, "r");
    //inst_file�� ���޹��� inst.data������ �б� ���� �ҷ��� �� �������� inst_data_file������ ���� 
    if (inst_data_file == NULL) {
        //inst.data���� �ҷ����� ����`  
        printf("������ �� �� �����ϴ�.\n");
        return -1;
    }
    inst_index = 0;
    for (inst_index = 0; inst_index < MAX_INST; inst_index++) {
        inst_table[inst_index] = (inst*)malloc(sizeof(inst));
    }
    //�ҷ����� inst_file�� ���������� inst_table�� ����ü�� �����Ҵ�
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
    //for������ �ݺ��Ǵ� �۾��� ���� inst���Ͽ��� 1�پ� ��� �ҷ��ͼ� ����ü�� ������.
    fclose(inst_data_file);
    // ���� �ݱ�
    return 0;
}


/* -----------------------------------------------------------------------------------
* ���� : token_table ������ ���� input_file�� �ҷ����� �Լ�
* �Ű� : ���� ����(input_file = input.txt)
* ��ȯ : ���� = 0, ���� = -1
* ���� : 
* -----------------------------------------------------------------------------------
*/
int init_input_file(uchar* input_file) {
    int i = 0;
    uchar temp_char[MAX_LINES];
    FILE* input_txt_file = fopen(input_file, "r");
    //input_file�� ���޹��� input.txt������ �б� ���� �ҷ��� �� �������� input_txt_file������ ���� 
    if (input_txt_file == NULL) {
        printf("������ �� �� �����ϴ�.\n");
        return -1;
    }
    for (i = 0; i < MAX_LINES; i++) {
        temp_char[i] = (uchar*)malloc(MAX_LINES);
    }
    i = 0;
    while (i < MAX_LINES && fgets(temp_char, MAX_LINES, input_txt_file) != NULL) {
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


/* -----------------------------------------------------------------------------------
* ���� : input_file�� ��ūȭ�ϰ� �����ϴ� �Լ�.
* �Ű� : ���޹��� ���ڿ�(uchar* before_str)
* ��ȯ : ���� = 0
* ���� : ���� ������ space�� �ϴ°� �ƴ϶� !!�� -�� ��ȣ�� ���� �ϴٺ���
            �ڵ带 ¥�鼭�� ���� ������ ���� ��ȣ�� �ߴ��� Ȯ���� �ʿ�
            �� ���� sic/xe���� ����� �Ǵ� ��ȣ�� ��� �ٸ� ��ȣ�� ������ �ʿ���.
* -----------------------------------------------------------------------------------
*/
int token_parsing(uchar* before_str) {
    token_table[token_line] = (token*)malloc(sizeof(token));
    token_table[token_line]->label = NULL;
    token_table[token_line]->operator = NULL;
    for (int j = 0; j < MAX_OPERAND; ++j) {
        token_table[token_line]->operand[j][0] = '!!';  // ���� ������ Ȯ���ϱ� ���� !!�� �ʱ�ȭ - ���� ������ ���� ��ȣ�� ���� ��
    }
    token_table[token_line]->comment[0] = '\0';  // �� ���ڿ��� �ʱ�ȭ
    token_table[token_line]->format = 0;
    token_table[token_line]->opcode = '\0';

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
    else {
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
        //,�� ���ٸ� operand�� �ϳ��̹Ƿ� [0]�� ���� �ְ�, ������ operand�� ����.
        if (token_str != NULL) {
            strcpy(token_table[token_line]->operand[0], token_str);
            next_operator_index_test = +MAX_OPERAND;
        }
    }

    token_str = strtok(NULL, "\n");
    //�ּ��� ��ūȭ
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
    return NULL;
}



/* -----------------------------------------------------------------------------------
* ���� : op table�� �����ϰ�, txt�� ����ϴ� �Լ�.
* �Ű� : ���޹��� ������ �̸� �� ����ü(optable_txt, token_table)
* ��ȯ : ���� = 0, ���� -1
* ���� : token_table������ ������ ���ٿ��� �������� ���� ���۸� ���� ���⿡ ���� �ְ�, �̰� �˻��Ŵ
------------------------------------------------------------------------------------*/
void make_opcode_output(uchar* optable_txt) {
    FILE* optab_file = fopen("optab.txt", "w");
    if (optab_file == NULL) {
        printf("optab.txt ������ ���� �� �����ϴ�.\n");
        return -1;
    }
    fprintf(optab_file, "String\t\tFormat\tOP\n\n");
    for (int i = 0; i < inst_index; i++) {
        for (int j = 0; j <= token_line; j++) { 
            if (token_table[j] != NULL && token_table[j]->operator != NULL) {
                char temp_operator[10];
                strcpy(temp_operator, token_table[j]->operator); 
                char* plus_position = strchr(temp_operator, '+');
                if (plus_position != NULL) {
                    memmove(plus_position, plus_position + 1, strlen(plus_position));  // +�� ����
                }
                if ((strcmp(temp_operator, inst_table[i]->str) == 0) && (inst_table[i]->format != 0)) {
                    // token_table[j]->operator�� inst_table[i]->str�� �����ϸ鼭, format != 0
                    if (optab_file != NULL){
                        op_table = (opt*)realloc(op_table, (opt_index + 1) * sizeof(opt));
                        strcpy(op_table[opt_index].str, inst_table[i]->str);
                        op_table[opt_index].op = inst_table[i]->op;
                        op_table[opt_index].format = inst_table[i]->format;
                        opt_index++;
                        fprintf(optab_file, "%-10s\t%-2d\t%02X\n", inst_table[i]->str, inst_table[i]->format, inst_table[i]->op);
                        break;
                    }
                }
            }
        }
    }
    for (int i = 0; i < inst_index; ++i) {
        free(inst_table[i]);
        //���� ������� ���� �����̶� inst_table�� ��������
    }
    fclose(optab_file);
    return 0;
}



/* -----------------------------------------------------------------------------------
* ���� : symbol table�� ����ϰ�, txt�� ����ϴ� �Լ�.
* �Է� : ����ü(token_table)
* ��� : symboltable_txt(symtab.txt)
* ��ȯ : ���� = 0, ���� -1
* ���� : token_table������ ������ ���ٿ��� �������� ���� ���۸� ���� ���⿡ ���� �ְ�, �̰� �˻��Ŵ
------------------------------------------------------------------------------------*/
void make_symtab_output(uchar* symboltable_txt) {
    FILE* symtab_file = fopen("symtab.txt", "w");
    if (symtab_file == NULL) {
        printf("symtab.txt ������ ���� �� �����ϴ�.\n");
        return -1;
    }
    uchar buf_symbol_operator[8];
    uchar buf_symbol_label[8];
    uchar buf_symbol_operand[MAX_OPERAND][20];
    int symbol_index = 0;
    int buffer_locctr;
    int bufend_loctr;
    int buf_maxlen_len = 0;
    for (int i = 0; i < token_line; i++) {
        int locctr_count_true = 0;
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
                //operand�� null�� �ƴ϶��
                size_t length_to_copy = sizeof(buf_symbol_operand[j]) - 1;
                strncpy(buf_symbol_operand[j], token_table[i]->operand[j], length_to_copy);
                if (buf_symbol_operand[j][0] == '=') {
                    uchar copied_array[20];
                    strcpy(copied_array, &buf_symbol_operand[j]);
                    literal_table_make(copied_array, i);
                }
                buf_symbol_operand[j][length_to_copy] = '\0';  // Null ���� ���� �߰�
            }
            else {
                buf_symbol_operand[j][0] = '\0';  // '\0' �߰�
            }
        }

        if (strcmp(buf_symbol_operator, "START") == 0) {
            //START�� ������
            locctr = start_locctr = strtol(buf_symbol_operand[0], NULL, 16);
            token_table[i]->locctr = locctr;    
            //���ڿ� ���·� operand���� ��� ������, 0�� �ش��ϴ� ���� ���� ��. 
        }

        if (strcmp(buf_symbol_label, "MAXLEN") == 0) {
            //label�� maxlen
            int temp = find_format(buf_symbol_operator);
            if (temp == -1) {
                temp = 0;
            }
        }
        if (intermediate_first_loc == 1) {
            //ó�� 1ȸ�� �̽��� �Ǵ� �ڵ���. �̴� start�� �����߸� �۵��� ������.
            if (strcmp(buf_symbol_operator, "EXTDEF") == 0) {
                //��.. �̰� ������ ���� �ؾ� �ɱ�??
                locctr_count_true = 1;
            }

            else if (strcmp(buf_symbol_operator, "EXTREF") == 0) {
                locctr_count_true = 1;
            }


            if (strcmp(buf_symbol_operator, "EQU") == 0) {
                uchar taget_token_char;
                for (int n = 0; n < MAX_OPERAND; n++) {
                    if (strstr(buf_symbol_operand[n], "-") != NULL) {
                        taget_token_char = '-';
                        equ_control_return_value = equ_token_control(buf_symbol_operand[n], taget_token_char);
                        locctr = equ_control_return_value;
                    }
                    else if (strstr(buf_symbol_operand[n], "+") != NULL) {
                        taget_token_char = '+';
                        equ_control_return_value = equ_token_control(buf_symbol_operand[n], taget_token_char);
                        locctr = equ_control_return_value;
                    }
                }
            }
            if (strcmp(buf_symbol_operator, "CSECT") == 0) {
                //section�� �ʱ�ȭ�ϴ� ��.
                locctr_system_history(locctr, 2, buf_maxlen_len);
                locctr = strtol(token_table[i]->operand[0], NULL, 16);
                start_locctr = locctr;
                locctr_count_true = 1;
                section++;
            }
            make_intermediate(i, locctr);
            if (strcmp(buf_symbol_operator, "RESB") == 0) {
                //RESB�� ������
                strncpy(sym_table[symbol_line].symbol, buf_symbol_label, sizeof(sym_table[i].symbol) - 1);
                sym_table[symbol_line].addr = locctr;
                long int hexValue = strtol(buf_symbol_operand[0], NULL, 10);
                locctr = locctr + hexValue;
                symbol_line++;
                locctr_count_true = 1;
            }

            else if (buf_symbol_operator[0] != '\0') {
                int if_for_control = 1;
                if (strcmp(buf_symbol_label, "FIRST") == 0) {
                    //Fisrt�� ������
                    strncpy(sym_table[symbol_line].symbol, buf_symbol_operator, sizeof(sym_table[i].symbol) - 1);
                }
            }

            else if (buf_symbol_operator[0] == '\0') {
                //���� �̹� operator�� NULL�̶�� 
                locctr_count_true = 1;
            }

            if (locctr_count_true == 0) {
                locctr_system_history(locctr, 0, 0);
                int isDuplicate = 0;
                if (buf_symbol_label[0] != '\0') {
                    for (int i = 0; i < symbol_line; i++) {
                        if (strcmp(sym_table[i].symbol, buf_symbol_label) == 0) {
                            if (sym_table[i].section == section) {
                                // �ߺ� ã��
                                isDuplicate = 1;
                                break;
                            }
                        }
                    }
                    if (!isDuplicate) {
                        // �ߺ��� �ƴ� ���
                        strncpy(sym_table[symbol_line].symbol, buf_symbol_label, sizeof(sym_table[symbol_line].symbol) - 1);
                        sym_table[symbol_line].addr = locctr;
                        sym_table[symbol_line].section = section;
                        symbol_line++;
                    }
                }
                //LOCCTR���� ���ϴ� �Լ�.
                locctr_plus(buf_symbol_operator, i);
                //locctr���� ���ϱ� ���� �Լ� ȣ��
            }
            if (strcmp(buf_symbol_operator, "END") == 0) {
                locctr_system_history(locctr, 1, 1);
                break;
            }
        }
        else {
            intermediate_first_loc = 1;
        }
    }
    //�ؿ��� symboll table�� ������ �Ϸ������. �̰� ����ϴ� ��
    fprintf(symtab_file, "Line\t\tSymbol\t\tsection\tlocctr\n");
    fprintf(symtab_file, "---------------------------------------------\n\n");
    for (int i = 0; i < symbol_line; i++) {
        fprintf(symtab_file, "[%02d]\t\t", i + 1);
        fprintf(symtab_file, "%s\t\t", sym_table[i].symbol);
        fprintf(symtab_file, "%d\t", sym_table[i].section);
        fprintf(symtab_file, "%04X\n", sym_table[i].addr);
    }
    fclose(symtab_file);
    return 0;
}


/* -----------------------------------------------------------------------------------
* ���� : �ý����� ���̸� �����ϴµ� ������ �ִ� �Լ�
* �Է� : ������ locctr��, ����� �����ϱ� ���� ���� i, ������ size
* ��ȯ : 
* ���� : CSECT���� i���� 1�̰�, END�� �����ٸ� i�� ���� 2�� �Է¹��� ����.
------------------------------------------------------------------------------------*/
void locctr_system_history(int now_locctr, int i, int size) {
    if (locctr_history[1] != 0) {
        locctr_history[2] = locctr_history[1];
    }
    if (locctr_history[0] != 0) {
        locctr_history[1] = locctr_history[0];
    }
    locctr_history[0] = now_locctr;
    //�ֱ� 3ȸ�� locctr���� �����ϴ� �뵵
    if ((i == 1) || (i == 2)) {
        int temp = locctr_history[0];
        for (int i = 1; i < 3; ++i) {
            if (locctr_history[i] > temp) {
                temp = locctr_history[i];
            }
        }
        //�ֱ� 3ȸ�� locctr��, ���� ū ���� temp�� ����.
        int temp2 = temp + size;
        //���� ��ɾ��� ũ�⸦ temp�� ���Ͽ� ���������� temp2�� ����.
        sections[section_line].sys_len = temp2;
        //���Ǻ��� ũ�⸦ ����ϴ� ����ü�� temp2�� ����.
        section_line++;
        program_length(temp);
        //������ ���α׷� ���̴� temp�� ����.
    }

    else if (i == 2) {
        for (int j = 0; j < 3; j++) {
            locctr_history[i] = NULL;
        }
    }
}


/* -----------------------------------------------------------------------------------
* ���� : equ�� �ش�Ǵ� ���ڿ��� �޾Ƽ� equ ���̺��� �����ϴ� �Լ�
* �Է� : equ�� �ش��ϴ� str, equ�� �ִ� ������ operator�� equ_token_operator
* ��ȯ : +, -�� ����� ��ģ �����
* ���� : 
------------------------------------------------------------------------------------*/
int equ_token_control(uchar str[20], uchar equ_token_operator) {
    int return_value = 0;
    char temp_str[20];
    char first_token[10];
    char next_token[10];

    strcpy(temp_str, str);
    // ��ūȭ
    char* token_start = strchr(str, equ_token_operator);

    // ��ū�� �����ϴ� ���
    if (token_start != NULL) {
        // ��ȣ�� �������� �� �κ��� ��ū���� ���
        int token_length = token_start - str;
        strncpy(first_token, str, token_length);
        first_token[token_length] = '\0';  // ���ڿ� ���� NULL �߰�

        // ã�� ��ġ���� ���ڿ��� ������
        // ��ū ������ ���ڿ��� ����Ϸ��� token + 1�� ����� �� ����
        strcpy(next_token, token_start + 1);
    }
    else {
        // ��ȣ�� ���� ��� ��ü ���ڿ��� ��ū���� ���
        printf("Token err: %s\n", str);
    }

    // equ a-b Ȥ�� a+b���� ��Ȳ�� ó���ϱ� ����.
    int first_addr = find_symbol_addr(first_token, section);
    int next_addr = find_symbol_addr(next_token, section);
    switch (equ_token_operator) {
    case '+':
        return_value = first_addr + next_addr;
        break;
    case '-':
        return_value = first_addr - next_addr;
        break;
    default:
        printf("Error: Unknown operator\n");
        return -1;
    }
    return return_value;
}



/* -----------------------------------------------------------------------------------
* ���� : ���޹��� token�� symtab���� �˻��Ͽ� addr�� ��ȯ�ϴ� �Լ�
* �Է� : ���޹��� ����, ������ section��
* ��ȯ : ã�� : symtab�� locctr��,  ��ã�� : -1
* ���� : ���� ���� ã�� section�� ��ġ�� ����� ������ �ڵ�����
------------------------------------------------------------------------------------*/
int find_symbol_addr(const char* token, int sec) {
    // sym_table���� token�� ã�� �ش��ϴ� symbol ����ü ��ȯ
    int check = 0;
    for (int i = 0; i < MAX_LINES; ++i) {
        if (strcmp(token, sym_table[i].symbol) == 0) {
            if (sym_table[i].section == sec) {
                check = 1;
                return sym_table[i].addr; //  �ش��ϴ� addr ��ȯ
            }
        }
    }
    if (check == 0) {
        //���� ã�� ���Ѵٸ� �ܺ������� ��.
        for (int i = 0; i < MAX_LINES; ++i) {
            if (strcmp(token, sym_table[i].symbol) == 0) {
                 check = 1;
                 return sym_table[i].addr; //  �ش��ϴ� addr ��ȯ
            }
        }
    }
    return -1; // ã�� ���� ��� -1 ��ȯ
}

void locctr_plus(uchar loc_str[20], int loc_token_line) {

    if (loc_str[0] == '+') {
        //4������ ���
        token_table[loc_token_line]->format = 4;
        locctr += 4;
    }
    else if (strcmp(loc_str, "RESW") == 0) {
        int resw_loc;
        int n = token_table[loc_token_line]->operand[0][0] - 48;
        resw_loc = 3 * n;   
        locctr += resw_loc;
    }
    else if (strcmp(loc_str, "LTORG") == 0) {
        int size_LTORG = global_liter_table[literal_table_line - 1].size;
        locctr += size_LTORG;
    }
    else if (strcmp(loc_str, "BYTE") == 0) {
        uchar str_BYTE[20];
        strcpy(str_BYTE, token_table[loc_token_line]->operand[0]);
        int size_BYTE = byte_size_check(str_BYTE, 0);
        locctr += size_BYTE;
    }
    else {
        int format = find_format(loc_str);
        token_table[loc_token_line]->format = format;
        locctr += format;
    }
}

int byte_size_check(uchar input_str[20], int i) {
    // ��������ǥ�� ã�� ���� ������
    uchar* first_quote = strchr(input_str, '\'');
    uchar* second_quote = NULL;
    uchar result1[2];
    uchar result2[3]; 
    // ù ��° ����ǥ ������ ���ڿ����� �� ��° ����ǥ ã��
    if (first_quote != NULL) {
        second_quote = strchr(first_quote + 1, '\'');
    }
    if (first_quote != NULL) {
        size_t length = first_quote - input_str;
        strncpy(result1, input_str, length);
        result1[length] = '\0';
    }
    if (first_quote != NULL && second_quote != NULL) {
        size_t length = second_quote - (first_quote + 1);
        strncpy(result2, first_quote + 1, length);
        result2[length] = '\0';
    }
    if (result1[0] == 'X'){
        int temp;
        sscanf(result2, "%X", &temp);
        if (i == 1) {
            return temp;
        }
        size_t byteCount = 0;
        while (temp != 0) {
            temp >>= 8;
            byteCount++;
        }
        return byteCount;
    }
}

int find_format(const uchar loc_str[20]) {
    for (int i = 0; i < opt_index; ++i) {
        if (strcmp((char*)loc_str, (char*)op_table[i].str) == 0) {
            return op_table[i].format;
        }
    }
    // ã�� ���� ���, �⺻������ 0�� ��ȯ�ϰų� ���� ó���� ������ �� �ֽ��ϴ�.
    return -1;
}



void literal_table_make(uchar str[20], int line) {
    //printf("���޹��� ���ڿ� Ȯ��: %s\n", str);
    char* file_mode;
    if (literal_count == 0) {
        file_mode = "w";
        literal_count = 1;
    }   
    else{
        file_mode = "a";
    }
    FILE* literal_table_file = fopen("literal_table.txt", file_mode);
    if (literal_table_file == NULL) {
        printf("symtab.txt ������ ���� �� �����ϴ�.\n");
        return;
    }
    int first_index = 0;
    int finish_index = 0;
    int quote_count = 0;
    const char* target = "'";
    for (int i = 0; i < sizeof(str); i++) {
        if (strncmp(&str[i], target, 1) == 0) {
            if (quote_count == 0) {
                first_index = i;
                //ù ���� ��������ǥ�� ��ġ�� ���
                quote_count++;
            }
            else if (quote_count == 1) {
                finish_index = i;
                //2�� ° ���� ����ǥ�� ��ġ�� ���
                break;
            }
        }
    }
    if (global_liter_table == NULL)  
    {
        global_liter_table = (liter_table*)malloc(MAX_LINES * sizeof(liter_table));
        if (global_liter_table == NULL)
        {
            printf("�޸� �����Ҵ� ����\n");
            return;
        }
        // �ʱ�ȭ
        for (int i = 0; i < MAX_LINES; ++i)
        {
            global_liter_table[i].size = 0;
        }
    }
    int isDuplicate = 0;
    for (int i = 0; i < literal_table_line; i++) {
        if (strcmp(global_liter_table[i].literal, str) == 0){
            // ���ڿ��� �ߺ��� �߰��ߴٸ�
            if (global_liter_table[i].section == section) {
                isDuplicate = 1;
                break;
            }
        }
    }
    if (!isDuplicate) {
        // �ߺ��� �ƴ� ���
        int temp;
        size_t str_len = 0;
        while (str[str_len] != '\0') {
            ++str_len;
        }
        int check = 0;
        for (int i = line; i < token_line; ++i) {
            if (strstr(token_table[i]->operator, str) != NULL || strstr(token_table[i]->operator, "LTORG") != NULL) {
                global_liter_table[literal_table_line].LTORG_or_STR_line = i;
                //printf("���ͷ� ���̺� �Լ����� ã�� ��. %s, %d\n", token_table[i]->operator, i);
                check = 1;
            }
        }
        if (check != 1) {
            global_liter_table[literal_table_line].LTORG_or_STR_line = -section - 1;
        }
        strcpy(global_liter_table[literal_table_line].literal, str);
        global_liter_table[literal_table_line].size = finish_index - first_index - 1;;
        global_liter_table[literal_table_line].addr = locctr;
        global_liter_table[literal_table_line].section = section;

        fprintf(literal_table_file, "[%02d]\t\t", literal_table_line + 1);
        fprintf(literal_table_file, "%15s\t\t", global_liter_table[literal_table_line].literal);
        if (global_liter_table[literal_table_line].LTORG_or_STR_line > 0) {
            fprintf(literal_table_file, "%04X\t", global_liter_table[literal_table_line].LTORG_or_STR_line);
        }
        else {
            fprintf(literal_table_file, "\t");
        }
        fprintf(literal_table_file, "%d\n", global_liter_table[literal_table_line].section);
        literal_table_line++;
    }
    fclose(literal_table_file);
    return;
}
void literal_test(void) {
    int countDig = countDigits(literal_table_line);
    printf("line\t   literal\tsize\trealline\taddr\n");
    printf("------------------------------------\n");
    for (int i = 0; i < literal_table_line; i++) {
        printf("[%0*d]\t", countDig, i+1);
        printf("%10s,\t", global_liter_table[i].literal);
        printf("%2d,\t", global_liter_table[i].size);
        printf("%2d,\t", global_liter_table[i].LTORG_or_STR_line);
        printf("%04X\n", global_liter_table[i].addr);
    }
    return;
}
void token_test(void) {
    //��ūȭ �۾��� Ȯ���ϴ� �Լ���
    int countDig = countDigits(token_line);
    printf("line\tobject\t\tformat\topcode\tlocctr\tlabel\toperator\toperand\t\t\tcomment\n");
    printf("--------------------------------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < token_line; i++) {
        //���� line ��¹�
        printf("[%0*d]\t", countDig, i + 1);
        if(token_table[i]->object_code != NULL){
            printf("%-8X\t", token_table[i]->object_code);
        }
        else{
            printf("\t\t");
        }

        //���� format ��¹�
        if (token_table[i]->format != -1) {
            printf("%01d\t", token_table[i]->format);
        }
        else {
            printf("\t");
        }

        //���� opcode ��¹�
        if (token_table[i]->opcode != NULL) {
            printf("%02X\t", token_table[i]->opcode);
        }
        else {
            printf("\t");
        }

        //���� locctr ��¹�
        printf("%04X\t", token_table[i]->locctr);

        //���� label ��¹�
        if (token_table[i]->label != NULL) {
            printf("%s\t", token_table[i]->label);
        }
        else {
            printf("\t");
        }

        //���� operator ��¹�
        printf("%s\t\t", token_table[i]->operator);

        //���� operand ��¹�
        for (int j = 0; j < MAX_OPERAND; j++) {
            uchar buf_operand[20];
            size_t length_to_copy = sizeof(buf_operand) - 1;
            strncpy(buf_operand, token_table[i]->operand[j], length_to_copy);
            if (j == 0) {
                if ((buf_operand[0] != '!') && (buf_operand[1] != '!')) {
                    printf("%-13s\t", buf_operand);
                    printf("%s\n", token_table[i]->comment);
                }
                else {
                    printf("\n");
                }
            }
            else if ((j != 0) && (buf_operand[0] != '!')) {
                if ((buf_operand[0] != '!') && (buf_operand[1] != '!')) {
                    printf("\t\t\t\t\t\t\t\t\t");
                    printf("%s\n", buf_operand);
                }
            }
        }
    }
    return;
}

int countDigits(int number) {
    // ���� ó��: ������ ����� ��ȯ�Ͽ� ���
    if (number < 0) {
        number = -number;
    }

    // �ڸ����� ���� ���� �ʱ�ȭ
    int digitCount = 0;

    // 0�� ��� ���� ó��
    if (number == 0) {
        return 1;
    }

    // ������ �� �ڸ����� ���� �ݺ���
    while (number > 0) {
        number /= 10;  // ���ڸ� 10���� ������ ���� �ڸ����� �̵�
        digitCount++;  // �ڸ��� ����
    }

    return digitCount;  
}

void make_intermediate(int line, int loc) {
    char* file_mode;
    if (intermediate_count == 0) {
        file_mode = "w";
    }
    else {
        file_mode = "a";
    }
    FILE* im_file = fopen("intermediate.txt", file_mode);
    if (im_file == NULL) {
        printf("symtab.txt ������ ���� �� �����ϴ�.\n");
        return;
    }
    fprintf(im_file, "[%2d]\t", line);
    token_table[line]->locctr = loc;
    fprintf(im_file, "%04X\t", loc);
    if (token_table[line]->label != NULL) {
        fprintf(im_file, "%s\t", token_table[line]->label);
    }
    else {
        fprintf(im_file, "\t");
    }
    fprintf(im_file, "%s\t\t", token_table[line]->operator);
    for (int j = 0; j < MAX_OPERAND; j++) {
        uchar buf_operand[20];
        size_t length_to_copy = sizeof(buf_operand) - 1;
        strncpy(buf_operand, token_table[line]->operand[j], length_to_copy);
        if (strstr(buf_operand, "!") == NULL) {
            fprintf(im_file, "%s\t", buf_operand);
        }
        else {
            fprintf(im_file, "\t");
        }
    }
    fprintf(im_file, "%s\n", token_table[line]->comment);

    intermediate_count = 1;
    fclose(im_file);
}

void program_length(int end_locctr) {
    int temp = end_locctr - start_locctr;
    system_program_length += temp;
    //printf("system_program_length = %04X\n ", system_program_length);
}


static int assem_pass2(void) {
    make_objectcode_output("object_code.txt");
    make_final_output("output.txt");
    return;
}

void make_final_output(uchar* final_txt) {
    FILE* final_file = fopen(final_txt, "w");
    if (final_file == NULL) {
        printf("final.txt ������ ���� �� �����ϴ�.\n");
        return;
    }
    int sec = 0;
    uchar buf_operator[8];
    //token_table���� operator�� �ӽ� ������ �迭
    uchar buf_label[8];
    //token_table���� label�� �ӽ� ������ �迭
    uchar buf_operand[MAX_OPERAND][20];
    //token_table���� operand�� �ӽ� ������ �迭
    int buf_locctr;
    //�ش� ������ locctr�� ������ ����
    int buf_format;
    int first_line_check = 1;
    int check_def = 0;
    int check_ref = 0;
    uchar buf_extdef[MAX_OPERAND][20];
    uchar buf_extref[MAX_OPERAND][20];
    int def_ref_procedure = 0;
    //def�� ref�� ������ ���� �Դ��� Ȯ���ϴ� ����.
    for (int i = 0; i < token_line; i++) {
        //�ݺ����� token_line���� �ݺ�
        buf_format = token_table[i]->format;
        buf_locctr = token_table[i]->locctr;
        if (token_table[i]->label != NULL) {
            //->label�� null�� �ƴ϶�� �ش� ������ ����
            size_t length_to_copy = sizeof(buf_label) - 1;
            strncpy(buf_label, token_table[i]->label, length_to_copy);
        }
        else {
            //null�̸� \0�� ����
            buf_label[0] = '\0';
        }
        if (token_table[i]->operator != NULL) {
            //->>operator�� null�� �ƴ϶�� �ش� ������ ����
            size_t length_to_copy = sizeof(buf_operator) - 1;
            strncpy(buf_operator, token_table[i]->operator, length_to_copy);
            buf_operator[length_to_copy] = '\0';
        }
        else {
            //null�̸� \0�� ����
            buf_operator[0] = '\0';
        }
        int j;
        //operand index�� ����
        //printf("%dline \t", i);
        for (j = 0; j < MAX_OPERAND; j++) {
            if (token_table[i]->operand[j][0] != '!') {
                size_t length_to_copy = sizeof(buf_operand[j]) - 1;
                strncpy(buf_operand[j], token_table[i]->operand[j], length_to_copy);
                buf_operand[j][length_to_copy] = '\0';  // Null ���� ���� �߰�
                // printf("%d:%s\t", j, buf_objectcode_operand[j]);
            }
            else {
                buf_operand[j][0] = '!';  // '\0' �߰�
            }
            if (j == 2) {
                // printf("\n\n");
            }
        }

        //---------------------------------�ҷ������ ����----------------------------------------------

        if (strcmp(buf_operator, "EXTDEF") == 0) {
            for (int j = 0; j < MAX_OPERAND; j++) {
                if (buf_operand[j][0] != '!'){
                    strcpy(buf_extdef[j], buf_operand[j]);
                }
                else {
                    buf_extdef[j][0] = '!';
                }
            }
            if (check_ref == 0) {
                //���� def������ ���� �Դٸ�,.
                def_ref_procedure = 1;
            }
            check_def = 1;
        }
        else if (strcmp(buf_operator, "EXTREF") == 0) {
            for (int j = 0; j < MAX_OPERAND; j++) {
                if (buf_operand[j][0] != '!') {
                    strcpy(buf_extref[j], buf_operand[j]);
                }
                else {
                    buf_extref[j][0] = '!';
                }
            }
            if (check_def == 0) {
                //���� ref������ ���� �Դٸ�,.
                def_ref_procedure = 2;
            }
            check_ref = 1;
        }

        else{
            if (first_line_check == 0) {
                //������ ������ �ƴ϶��
                if (def_ref_procedure > 0) {
                    if (def_ref_procedure == 1) {
                        //def�� ���� ���Դٸ�
                        if (check_def == 1) {
                            //EXTDEF�� �־��ٸ�
                            fprintf(final_file, "D");
                            for (int j = 0; j < MAX_OPERAND; j++) {
                                if (buf_extdef[j][0] != '!') {
                                    fprintf(final_file, "%s", buf_extdef[j]);
                                    int temp = find_symbol_addr(buf_extdef[j], sec);
                                    fprintf(final_file, "%06X", temp);
                                }
                            }
                            fprintf(final_file, "\n");
                        }
                        if (check_ref == 1) {
                            //EXTREF�� �־��ٸ�
                            fprintf(final_file, "R");
                            for (int j = 0; j < MAX_OPERAND; j++) {
                                if (buf_extref[j][0] != '!') { 
                                    fprintf(final_file, "%s ", buf_extref[j]); 
                                }
                            }
                            fprintf(final_file, "\n");
                        }
                    }
                    else if (def_ref_procedure == 2) {
                        //ref�� ���� ���Դٸ�
                        if (check_ref == 1) {
                            //EXTREF�� �־��ٸ�
                            fprintf(final_file, "R");
                            for (int j = 0; j < MAX_OPERAND; j++) {
                                if (buf_extref[j][0] != '!') {
                                    fprintf(final_file, "%s", buf_extref[j]);
                                }
                            }
                            fprintf(final_file, "\n");

                        }
                        if (check_def == 1) {
                            //EXTDEF�� �־��ٸ�
                            fprintf(final_file, "D");
                            for (int j = 0; j < MAX_OPERAND; j++) {
                                if (buf_extdef[j][0] != '!') {
                                    fprintf(final_file, "%s", buf_extdef[j]);
                                    int temp = find_symbol_addr(buf_extdef[j], sec);
                                    fprintf(final_file, "%06X", temp);
                                }
                            }
                            fprintf(final_file, "\n");
                        }
                    }
                    check_def = 0;
                    check_ref = 0;
                    def_ref_procedure = 0;
                }
                else if ((strcmp(buf_operator, "CSECT") == 0) || (strcmp(buf_operator, "END") == 0) ){
                    first_line_check = 1;   
                    //�ٽ� ������ ó�� �����̶� ���� �ȳ�����.
                    check_def = 0;
                    fprintf(final_file, "E");
                    if (sec==0){
                        fprintf(final_file, "%06X", start_locctr);
                    }
                    fprintf(final_file, "\n\n");
                    sec++;
                    //section�� �ϳ� �ø�.
                    if (strcmp(buf_operator, "END") == 0) {
                        //END���ٸ� ����.
                        break;
                    }
                }
                else{

                }
            }
            else if (first_line_check == 1) {
                //�ش� ������ ó�� �����̶��
                fprintf(final_file, "H%s\t%06X%06X\n", buf_label, start_locctr, sections[sec].sys_len);
                first_line_check = 0;
                //�� �̻� ù���� �ƴ��� �ȳ���.
            }
        }



    }
    fclose(final_file);
}
void make_objectcode_output(uchar* object_txt){
    FILE* object_file = fopen(object_txt, "w");
    if (object_file == NULL) {
        printf("object.txt ������ ���� �� �����ϴ�.\n");
        return;
    }

    int countDig = countDigits(token_line);
    int object_line = 0;
    int start_check = 0;
        //start�� ������ ���� �ٲ㼭 ���Ŀ� �ִ� ���ε鸸 �ڵ尡 �۵��ǵ��� �ϴ� ��.
    int buf_format;
    int buf_opcode;
    uchar buf_objectcode_operator[8];
    //token_table���� operator�� �ӽ� ������ �迭
    uchar buf_objectcode_label[8];
    //token_table���� label�� �ӽ� ������ �迭
    uchar buf_objectcode_operand[MAX_OPERAND][20];
    //token_table���� operand�� �ӽ� ������ �迭
    int buffer_locctr;
    //�ش� ������ locctr�� ������ ����
    char binary[9] = "";
    //16���� 2���ڸ� 2������ ��ȯ�ϴ� �ڵ�
    int sec = 0;
    for (int i = 0; i < token_line; i++) {
        //�ݺ����� token_line���� �ݺ�
        int locctr_count_true = 0;
        buf_format = token_table[i]->format;
        if (token_table[i]->label != NULL) {
            //->label�� null�� �ƴ϶�� �ش� ������ ����
            size_t length_to_copy = sizeof(buf_objectcode_label) - 1;
            strncpy(buf_objectcode_label, token_table[i]->label, length_to_copy);
        }
        else {
            //null�̸� \0�� ����
            buf_objectcode_label[0] = '\0';
        }

        if (token_table[i]->operator != NULL) {
            //->>operator�� null�� �ƴ϶�� �ش� ������ ����
            size_t length_to_copy = sizeof(buf_objectcode_operator) - 1;
            strncpy(buf_objectcode_operator, token_table[i]->operator, length_to_copy);
            buf_objectcode_operator[length_to_copy] = '\0';
        }
        else {
            //null�̸� \0�� ����
            buf_objectcode_operator[0] = '\0';
        }
        int j;
        //operand index�� ����
        //printf("%dline \t", i);
        for (j = 0; j < MAX_OPERAND; j++) {
            if (token_table[i]->operand[j][0] != '!') {
                size_t length_to_copy = sizeof(buf_objectcode_operand[j]) - 1;
                strncpy(buf_objectcode_operand[j], token_table[i]->operand[j], length_to_copy);
                buf_objectcode_operand[j][length_to_copy] = '\0';  // Null ���� ���� �߰�
               // printf("%d:%s\t", j, buf_objectcode_operand[j]);
            }
            else {
                buf_objectcode_operand[j][0] = '!';  // '\0' �߰�
            }
            if (j == 2) {
               // printf("\n\n");
            }
        }

        token_table[i]->opcode = find_opcode(buf_objectcode_operator);
        buf_opcode = token_table[i]->opcode;
        token_table[i]->object_code = NULL;
        h_b* newUnit = (h_b*)malloc(sizeof(h_b));
        if (newUnit == NULL) {
            fprintf(stderr, "�޸� �Ҵ� ����\n");
            exit(1); // ���α׷� ���� �Ǵ� ������ ���� ó��
        }
        int xbpe[4] = { 0 };

        if (start_check != 0) {

            if (strcmp(buf_objectcode_operator, "END") == 0) {
                break;
            }
            /*
                ���� �ؿ��� ���� START ~ END���̿� �ش�Ǵ� ���ε��� ����Ǵ� �͵�.
            */
            if (token_table[i + 1]->locctr != NULL) {
                //pc�� ������.
                pc_count = token_table[i + 1]->locctr;
            }
            if (strcmp(buf_objectcode_operator, "CSECT") == 0) {
                hex_binary_table[i].format = -1;
                sec++;
            }
            if (strcmp(buf_objectcode_operator, "EXTDEF") == 0) {
                hex_binary_table[i].format = -1;
            }

            else if (strcmp(buf_objectcode_operator, "EXTREF") == 0) {
                hex_binary_table[i].format = -1;
            }

            else {
                switch (buf_format) {
                case(0):
                    if (strcmp(buf_objectcode_operator, "BYTE") == 0) {
                        int size = byte_size_check(buf_objectcode_operand[0], 1);
                        hex_binary_table[i].format = 0;
                        decimalToBinary(size, i, 1);
                        hex_binary_table[i].ob = size;
                    }
                    else if (strcmp(buf_objectcode_operator, "WORD") == 0) {
                        hex_binary_table[i].format = 0;
                        decimalToBinary(0, i, 1);
                        hex_binary_table[i].ob = (0 << 8);
                    }
                    else {
                        hex_binary_table[i].format = -1;
                    }
                    break;
                case(1):
                    //format 1�� ����� object_code�� opcode 1byte(=8bit) �״�� ��.
                    hex_binary_table[i].format = 1;
                    decimalToBinary(buf_opcode, i, 1);
                    hex_binary_table[i].ob = buf_opcode;
                    break;

                case(2):
                    int register_1;
                    int register_2;
                    uchar temp_str_1[] = { buf_objectcode_operand[0][0] };
                    uchar temp_str_2[] = { buf_objectcode_operand[1][0] };

                    if (temp_str_1[0] == 'X') {
                        register_1 = 1;
                    }
                    else if (temp_str_1[0] == 'L') {
                        register_1 = 2;
                    }
                    else if (temp_str_1[0] == 'B') {
                        register_1 = 3;
                    }
                    else if (temp_str_1[0] == 'S') {
                        register_1 = 4;
                    }
                    else if (temp_str_1[0] == 'T') {
                        register_1 = 5;
                    }
                    else if (temp_str_1[0] == 'F') {
                        register_1 = 6;
                    }
                    else {
                        register_1 = 0;
                    }

                    switch (temp_str_2[0]) {
                    case 'X':
                        register_2 = 1;
                        break;
                    case 'L':
                        register_2 = 2;
                        break;
                    case 'B':
                        register_2 = 3;
                        break;
                    case 'S':
                        register_2 = 4;
                        break;
                    case 'T':
                        register_2 = 5;
                        break;
                    case 'F':
                        register_2 = 6;
                        break;
                    default:
                        register_2 = 0;
                        break;
                    }
                    decimalToBinary(buf_opcode, i, 2);
                    decimalToBinary(register_1, i, 8);
                    decimalToBinary(register_2, i, 9);
                    hex_binary_table[i].format = 2;
                    hex_binary_table[i].ob = (buf_opcode << 8) + (register_1 << 4) + register_2;
                    break;


                case(3):
                    hex_binary_table[i].format = 3;
                    int found_operator_locctr = token_table[i]->locctr;
                    int found_operand_Addr[MAX_OPERAND];
                    for (int j = 0; j < MAX_OPERAND; j++) {
                        if (find_symbol_addr(buf_objectcode_operand[j], sec) != '!') {
                            found_operand_Addr[j] = find_symbol_addr(buf_objectcode_operand[j], sec);
                        }
                        else {
                            found_operand_Addr[j] = -1;
                            //���� nullĭ�� �����ٸ� -1���� �־����. ���Ŀ� -1�� null�� ����� ��.
                        }
                    }

                    if (found_operand_Addr[1] == -1) {
                        //operand�� �ϳ��� ���� ���.
                        int binaryArray[4] = { 0 };
                        int locctr_overflowr_check;
                        if (buf_objectcode_operand[0][0] == '=') {
                            //���ͷ� ó�� object = op + 3;
                            decimalToBinary(buf_opcode + 3, i, 3);
                            int litter_locctr = 0;
                            int n = found_litter_index(buf_objectcode_operand[0]);
                            if (n > 0) {
                                litter_locctr = token_table[n]->locctr;
                            }
                            else if(n < 0) {
                                int temp = -n;
                                temp--;
                                n = temp;
                                litter_locctr = sections[n].sys_len;
                            }
                            //���ͷ��� ���� ��ġ�� ��ȯ
                            locctr_overflowr_check = litter_locctr - pc_count;
                            int temp;
                            if (abs(locctr_overflowr_check) >= 4096) {
                                //Overflowr�˻� ���
                                xbpe[1] = 1;
                                temp = 4;
                                //4(10) = 0100(2)
                                xbpe_of_nixbpe(xbpe, i);
                                //b�� 1�� ����.
                            }
                            else {
                                xbpe[2] = 1;
                                temp = 2;
                                //2(10) = 0010(2)
                                xbpe_of_nixbpe(xbpe, i);
                                //p�� 1�� ����.
                            }
                            decimalToBinary(locctr_overflowr_check, i, 7);
                            //disp_1�� �˸��� ���� �ִ� ����.
                            hex_binary_table[i].ob = ((buf_opcode + 3) << 16) + (temp << 12) + (locctr_overflowr_check);
                        }
                        else if (buf_objectcode_operand[0][0] == '@') {
                            //@�� �����ּҿ���? object = op + 2
                            decimalToBinary(buf_opcode + 2, i, 3);
                            locctr_overflowr_check = pc_count - found_operator_locctr;
                            int temp;
                            if ((abs(locctr_overflowr_check) >= 4096) && (locctr_overflowr_check >= 0)) {
                                //Overflowr�߻� = base
                                xbpe[1] = 1;
                                temp = 4;
                                //4(10) = 0100(2)
                                xbpe_of_nixbpe(xbpe, i);
                                //b�� 1�� ����.
                            }
                            else if ((locctr_overflowr_check < 4096) && (locctr_overflowr_check >= 0)){
                                //�̹߻��� ���
                                xbpe[2] = 1;
                                temp = 2;
                                //2(10) = 0010(2)
                                xbpe_of_nixbpe(xbpe, i);
                                //p�� 1�� ����.
                            }
                            else {
                                //�̹߻��� ����
                            }
                            hex_binary_table[i].ob = ((buf_opcode + 2) << 16) + (temp << 12);
                        }
                        else if (buf_objectcode_operand[0][0] == '#') {
                            //#�� ����ּҷ� #�� �ڿ� �ִ� ���� �����. object = op + 1
                            int temp = (int)buf_objectcode_operand[0][1]; 
                            //temp�� buf_objectcode_operand[0][1]�� �ƽ�Ű�ڵ尡 �����.
                            decimalToBinary(buf_opcode + 1, i, 3);
                            xbpe_of_nixbpe(xbpe, i);
                            //x, b, p, e�� 0�� ����.
                            hex_binary_table[i].ob = ((buf_opcode + 1) << 16);
                            if (temp < 65){
                                //temp�� ���ڰ� �ƽ�Ű�ڵ�� ��� ����.
                                temp -= 48;
                                //��ũŰ�ڵ� ���� ���� ���� 1���ڸ� ������ �������.
                                decimalToBinary(temp, i, 7);
                                hex_binary_table[i].ob += temp;
                            }
                            else if (temp == 65)  {
                                //temp = A = 10(10) = 1010
                                int A = 10;
                                decimalToBinary(A, i, 7);
                                hex_binary_table[i].ob += A;
                            }
                            else if (temp == 66) {
                                //temp = B = 11(10) = 1011
                                int B = 11;
                                decimalToBinary(B, i, 7);
                                hex_binary_table[i].ob += B;
                            }
                            else if (temp == 67) {
                                //temp = C = 12(10) = 1100
                                int C = 12;
                                decimalToBinary(C, i, 7);
                                hex_binary_table[i].ob += C;
                            }
                            else if (temp == 68) {
                                //temp = D = 13(10) = 1101
                                int D = 13;
                                decimalToBinary(D, i, 7);
                                hex_binary_table[i].ob += D;
                            }
                            else if (temp == 69) {
                                //temp = E = 14(10) = 1110
                                int E = 14;
                                decimalToBinary(E, i, 7);
                                hex_binary_table[i].ob += E;
                            }
                            else if (temp == 70) {
                                //temp = F = 15(10) = 1111
                                int F = 15;
                                decimalToBinary(F, i, 7);
                                hex_binary_table[i].ob += F;
                            }
                        }
                        else if (buf_objectcode_operand[0][0] == '!') {
                            //operand�� ���� operator ó��  object = op + 3
                            hex_binary_table[i].ob = ((buf_opcode + 3) << 16);
                            //hex_binary_table[i].ob = buf_opcode + 3;
                            //operand�� �����Ƿ�, �ش� operator�� op���� �ش� ������ 3�� ���ϴ� ��.
                            decimalToBinary(buf_opcode + 3, i, 3);
                            for (j = 0; j < 4; j++) {
                                xbpe[j] = 0;
                            }
                            xbpe_of_nixbpe(xbpe, i);
                            //x, b, p, e�� 0�� ����.
                        }
                        else {
                            //�������� �Ϲ� format3�� ó��.
                            decimalToBinary(buf_opcode + 3, i, 3);
                            locctr_overflowr_check = found_operand_Addr[0] - pc_count;
                            //opcode - pc
                            int temp;
                            if ((abs(locctr_overflowr_check) >= 4096) && (locctr_overflowr_check >= 0)) {
                                //overflowr �߻�.
                                xbpe[1] = 1;
                                xbpe_of_nixbpe(xbpe, i);
                                //b�� 1�� ����.
                                temp = 4;
                                //2(10) = 0010(2)
                            }
                            else if ((locctr_overflowr_check < 4096) && (locctr_overflowr_check >= 0)) {
                                //overflowr �̹߻� ���
                                xbpe[2] = 1;
                                xbpe_of_nixbpe(xbpe, i);
                                //p�� 1�� ����.
                                temp = 2;
                                //2(10) = 0010(2)
                            }
                            else{    
                                //����� ����..
                                int t = -locctr_overflowr_check;
                                xbpe[2] = 1;
                                xbpe_of_nixbpe(xbpe, i);
                                temp = 3;
                            }
                            decimalToBinary(locctr_overflowr_check, i, 7);
                            hex_binary_table[i].ob = ((buf_opcode + 3) << 16) + (temp << 12) + locctr_overflowr_check;
                        }
                    }
                    else if (found_operand_Addr[2] == -1) {
                        //���� ���������� �̷� ��Ȳ�� ��� �ڵ带 ���Է� ����.
                        //operand�� 2�� ���� ���
                    }
                    else if (found_operand_Addr[2] != -1) {
                        //���� ���������� �̷� ��Ȳ��  ��� �ڵ带 ���Է� ����.
                        //operand�� 3�� ���� ���
                    }
                    break;
                    //format 3����

                case(4):
                    decimalToBinary(buf_opcode + 3, i, 4);
                    hex_binary_table[i].format = 4;
                    if (buf_objectcode_operator[0] == '+') {
                        hex_binary_table[i].ob = (buf_opcode + 3 << 24);
                        int found_operand_locctr = find_symbol_addr(buf_objectcode_operand[0], sec);
                        if (buf_objectcode_operand[1][0] != '!') {
                            //���̻� ���۷��尡 ����.
                            if (buf_objectcode_operand[1][0] == 'X') {
                                xbpe[0] = 1;
                                xbpe[3] = 1;
                                xbpe_of_nixbpe(xbpe, i);
                                hex_binary_table[i].ob += (9 << 20);
                            }
                        }
                        else {
                            //�߰��� ���۷��尡 ����. 
                            xbpe[3] = 1;
                            xbpe_of_nixbpe(xbpe, i);
                            hex_binary_table[i].ob += (1 << 20);
                        }
                    }
                    else {
                        //���� �������� ��� �����ص��� ���� ������.
                    }
                    break;

                default:
                    hex_binary_table[i].format = -1;
                    break;
                }
                token_table[i]->object_code = hex_binary_table[i].ob;
            }

            /*
                �ؿ��� object_file ����� �ϴ� �ڵ����.
            */
            int temp = hex_binary_table[i].format;
            if (temp == -1) {
                temp = 0;
            }
            fprintf(object_file, "[%0*d : %d]  ", countDig, i, temp);
            if (hex_binary_table[i].format == -1) {
                fprintf(object_file, " \n");
            }
            else{
                if ((hex_binary_table[i].format == 0) || (hex_binary_table[i].format == 1) || (hex_binary_table[i].format == 2)) {
                    //format 0, 1, 2
                    if (hex_binary_table[i].format == 0){
                        fprintf(object_file, "%2X       : \t", hex_binary_table[i].ob);
                    }
                    else if (hex_binary_table[i].format == 1) {
                        fprintf(object_file, "%02X       : \t", hex_binary_table[i].ob);
                    }
                    else if (hex_binary_table[i].format == 2) {
                        fprintf(object_file, "%04X     : \t", hex_binary_table[i].ob);
                    }
                    for (int j = 0; j < 8; j++) {
                        fprintf(object_file, "%d", hex_binary_table[i].op_for_1_2[j]);
                    }
                    if ((hex_binary_table[i].format == 0) || (hex_binary_table[i].format == 1)) {
                        fprintf(object_file, "\n");
                        //format 1�� ���⼭ ��� ����
                    }
                    else if (hex_binary_table[i].format == 2) {
                        fprintf(object_file, " ");
                        for (int j = 0; j < 4; j++) {
                            fprintf(object_file, "%d", hex_binary_table[i].r1[j]);
                        }
                        fprintf(object_file, " ");
                        for (int j = 0; j < 4; j++) {
                            fprintf(object_file, "%d", hex_binary_table[i].r2[j]);
                        }
                        fprintf(object_file, "\n");
                        //format 2 ��� ����
                    }
                }
                else if ((hex_binary_table[i].format == 3) || (hex_binary_table[i].format == 4)) {
                    //format 3, 4
                    if (hex_binary_table[i].format == 3) {
                        fprintf(object_file, "%06X   : \t", hex_binary_table[i].ob);
                    }
                    else if (hex_binary_table[i].format == 4) {
                        fprintf(object_file, "%08X : \t", hex_binary_table[i].ob);
                    }
                    for (int j = 0; j < 6; j++) {
                        fprintf(object_file, "%d", hex_binary_table[i].op_for_3_4[j]);
                    }
                    fprintf(object_file, " ");
                    fprintf(object_file, " ");
                    for (int j = 0; j < 6; j++) {
                        fprintf(object_file, "%d", hex_binary_table[i].nixbpe[j]);
                    }
                    fprintf(object_file, " ");
                    for (int j = 0; j < 12; j++) {
                        fprintf(object_file, "%d", hex_binary_table[i].disp_1[j]);
                        if ((j + 1) % 4 == 0) {
                            fprintf(object_file, " ");
                        }
                    }
                    fprintf(object_file, " ");
                    if (hex_binary_table[i].format == 3) {
                        fprintf(object_file, " \n");
                        //format 3�� ���⼭ ��� ����
                    }
                    else if (hex_binary_table[i].format == 4) {
                        for (int j = 0; j < 8; j++) {
                            fprintf(object_file, "%d", hex_binary_table[i].disp_2[j]);
                            if ((j + 1) % 4 == 0) {
                                fprintf(object_file, " ");
                            }
                        }
                        fprintf(object_file, " \n");
                        //format 4 ��� ����
                    }
                }
            }
            
        }
        else {
            if (strcmp(buf_objectcode_operator, "START") == 0) {
                start_check = 1;
            }
        }
        hex_binary_line++;
    }
    fclose(object_file);
    return;
}

int found_litter_index(uchar str[10]) {
    for (int i = 0; i < literal_table_line; ++i) {
        if (strstr(global_liter_table[i].literal, str) != NULL) {
            return global_liter_table[i].LTORG_or_STR_line;
        }
    }
}

void xbpe_of_nixbpe(int xbpe[4], int line) {
    for (int j = 2; j < 6; j++) {
        hex_binary_table[line].nixbpe[j] = xbpe[j-2];
    }
}

void decimalToBinary(int decimal, int line, int format) {
    if(format < 5){
        int binaryArray[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
        for (int j = 7; j >= 0; j--) {
            binaryArray[j] = decimal % 2;
            decimal /= 2;
        }
        //buf_opcode�� �ִ� 16������ 2������ ��ȯ�Ͽ� ������ ����.
        if ((format == 1) || (format == 2)) {
            for (int j = 0; j < 8; j++) {
                hex_binary_table[line].op_for_1_2[j] = binaryArray[j];
            }
            return;
        }
        else if ((format == 3) || (format == 4)) {
            for (int j = 0; j < 8; j++) {
                if (j < 6) {
                    hex_binary_table[line].op_for_3_4[j] = binaryArray[j];
                    //printf("%d", hex_binary_table[line].op[j]);
                }
                else if (j == 6) {
                    //printf("\t");
                    hex_binary_table[line].nixbpe[0] = binaryArray[j];
                    //printf("%d", hex_binary_table[line].nixbpe[0]);
                    // i bit
                }
                else if (j == 7) {
                    hex_binary_table[line].nixbpe[1] = binaryArray[j];
                    // i bit
                    // printf("%d", hex_binary_table[line].nixbpe[1]);
                    // printf("\n");
                }
            }
        }
        return;
    }
    else if (format == 6) {
        for (int i = 0; i < 8; ++i) {
            hex_binary_table[line].disp_2[8 - i] = decimal % 2;
            decimal /= 2;
        }
    }
    else if (format == 7) {
        if (decimal >= 0) {
            for (int i = 0; i < 12; ++i) {
                hex_binary_table[line].disp_1[11 - i] = decimal % 2;
                decimal /= 2;
            }
        }
        else if (decimal < 0){
            unsigned int positiveValue = (unsigned int)(-decimal);
            unsigned int twoComplement = positiveValue ^ 0xFFFFFFFF;
            twoComplement += 1;
            for (int i = 11; i >= 0; i--) {
                int bit = (twoComplement >> i) & 1;
                hex_binary_table[line].disp_1[11 - i] = bit;    
            }
        }
    }
    else if ((format == 8) || (format == 9)){
        //format2���� �������Ϳ� ���� �ִ� ��.
        int binaryArray4[4] = { 0, 0, 0, 0 };
        for (int j = 3; j >= 0; j--) {
            binaryArray4[j] = decimal % 2;
            decimal /= 2;
        }
        for (int j = 0; j < 4; j++) {
            if (format == 8) {
                hex_binary_table[line].r1[j] = binaryArray4[j];
            }
            if (format == 9) {
                hex_binary_table[line].r2[j] = binaryArray4[j];
            }
        }
    }
}

int find_opcode(const char* str1) {
    int err = 0;

    // '+' ��ȣ�� �ִ��� Ȯ��
    char temp[10];  // �ʿ信 ���� ũ�� ����
    int hasPlus = 0;
    int j = 0;
    for (int i = 0; str1[i] != '\0'; ++i) {
        if (str1[i] == '+') {
            hasPlus = 1;
        }
        else {
            temp[j++] = str1[i];
        }
    }
    temp[j] = '\0';  // temp ���ڿ��� null ���� ���� �߰�
    if (hasPlus == 0) {
        //+�� �߰ߵ��� �ʾҴٸ�.
        strcpy(temp, str1);
    }
    //printf("%s\n", temp);
    // op_table�� ���� �� '+'�� ������ ������ ���ڿ�(temp)�� ��
    for (int i = 0; i < opt_index; ++i) {
        if (strcmp(op_table[i].str, temp) == 0) {
            //printf("%s = %02X\n", str1, op_table[i].op);
            return op_table[i].op;
        }
    }

    return err;
}