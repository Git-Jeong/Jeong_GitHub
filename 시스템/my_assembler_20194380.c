
/*
    시스템 프로그래밍 과제물
    20194380 문정진
*/
#define _CRT_SECURE_NO_WARNINGS	
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "my_assembler_20194380.h"
//#include "Operation_status.h"   
//내가 어디까지 작업을 했는지 나타내는 파일로 어셈블러의 역할과는 무관

int main(void) {
    init_my_assembler();
    assem_pass1();
    assem_pass2();
    token_test();      //token_parsing, locctr, object_code를 전체적으로 테스트 돌리는 함수.
    //literal_test();      //리터럴 테이블 테스트 함수.
}

int init_my_assembler(void) {
    init_inst_file("inst.data");
    //inst.data파일을 inst_table구조체에 저장하기 위한 함수
    init_input_file("input.txt");
    //input.txt파일을 토큰화를 위해 라인별로 불러오기 위한 함수
    return 0;
}


/* -----------------------------------------------------------------------------------
* 설명 : optable 구현을 위해 inst_file을 불러오는 함수
* 매계 : 실행 파일(inst_file = inst.data)
* 반환 : 성공 = 0, 실패 = -1
* 주의 : token을 따로 돌리는게 아니라 fscanf를 통해 하다보니 정형화된 파일을 넣어야만 가능함.
* -----------------------------------------------------------------------------------
*/
int init_inst_file(uchar* inst_file) {
    FILE* inst_data_file = fopen(inst_file, "r");
    //inst_file로 전달받은 inst.data파일을 읽기 모드로 불러온 후 포인터형 inst_data_file변수에 대입 
    if (inst_data_file == NULL) {
        //inst.data파일 불러오기 오류`  
        printf("파일을 열 수 없습니다.\n");
        return -1;
    }
    inst_index = 0;
    for (inst_index = 0; inst_index < MAX_INST; inst_index++) {
        inst_table[inst_index] = (inst*)malloc(sizeof(inst));
    }
    //불러오는 inst_file이 끝날때까지 inst_table의 구조체를 동적할당
    inst_index = 0;
    uchar temp_storage[3];
    //읽어올 글자와, \0의 저장 공간을 고려하여 [3]으로 해둠.
    while (inst_index < MAX_INST && fscanf(inst_data_file, "%s %s %d %02X",
        inst_table[inst_index]->str,
        &temp_storage[0],
        &inst_table[inst_index]->format,
        &inst_table[inst_index]->op) != EOF) {
        // 읽은 데이터를 구조체에 저장하는 작업 수행.
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
    //for문으로 반복되는 작업을 통해 inst파일에서 1줄씩 계속 불러와서 구조체에 저장함.
    fclose(inst_data_file);
    // 파일 닫기
    return 0;
}


/* -----------------------------------------------------------------------------------
* 설명 : token_table 구현을 위해 input_file을 불러오는 함수
* 매계 : 실행 파일(input_file = input.txt)
* 반환 : 성공 = 0, 실패 = -1
* 주의 : 
* -----------------------------------------------------------------------------------
*/
int init_input_file(uchar* input_file) {
    int i = 0;
    uchar temp_char[MAX_LINES];
    FILE* input_txt_file = fopen(input_file, "r");
    //input_file로 전달받은 input.txt파일을 읽기 모드로 불러온 후 포인터형 input_txt_file변수에 대입 
    if (input_txt_file == NULL) {
        printf("파일을 열 수 없습니다.\n");
        return -1;
    }
    for (i = 0; i < MAX_LINES; i++) {
        temp_char[i] = (uchar*)malloc(MAX_LINES);
    }
    i = 0;
    while (i < MAX_LINES && fgets(temp_char, MAX_LINES, input_txt_file) != NULL) {
        bool isTabBeforeString = false;
        char* ptr = temp_char;
        char newString[MAX_LINES + 1]; // +1은 '-'를 추가하기 위함
        while (*ptr != '\0' && (*ptr == ' ' || *ptr == '\t')) {
            isTabBeforeString = true;
            strcpy(newString, "-");
            strcat(newString, temp_char);
            ptr++;
        }
        if (isTabBeforeString == true) {
            //공백이 먼저 나온 경우 실행
            token_parsing(newString);
        }
        else {
            //문자가 먼저 나온 경우 실행
            token_parsing(temp_char);
        }
        i++;
    }
    fclose(input_txt_file);
    return 0;
}


/* -----------------------------------------------------------------------------------
* 설명 : input_file을 토큰화하고 저장하는 함수.
* 매계 : 전달받은 문자열(uchar* before_str)
* 반환 : 성공 = 0
* 주의 : 공란 관리를 space로 하는게 아니라 !!랑 -의 기호를 통해 하다보니
            코드를 짜면서도 내가 무엇을 공란 기호로 했는지 확인이 필요
            및 실제 sic/xe에서 사용이 되는 기호일 경우 다른 기호로 변경이 필요함.
* -----------------------------------------------------------------------------------
*/
int token_parsing(uchar* before_str) {
    token_table[token_line] = (token*)malloc(sizeof(token));
    token_table[token_line]->label = NULL;
    token_table[token_line]->operator = NULL;
    for (int j = 0; j < MAX_OPERAND; ++j) {
        token_table[token_line]->operand[j][0] = '!!';  // 추후 공란을 확인하기 위해 !!로 초기화 - 실제 사용되지 않을 기호로 정한 것
    }
    token_table[token_line]->comment[0] = '\0';  // 빈 문자열로 초기화
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

    //밑은 ,이 있는지 확인하여 operand에 값을 넣는 코드
    int comma_contain_test;
    int space_test = 0;
    token_str = strtok(NULL, "\t\n");
    char** token_buf = (char**)malloc(sizeof(char*) * sizeof(token_str));
    if (token_buf == NULL) {
        printf(stderr, "메모리 할당 오류\n");
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
        //만약 문자열에 ,이 있다면 
        // strtok을 이용하여 ,로 토큰화
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
                token_table[token_line]->operand[i][0] = '!!';  // 빈 문자열로 
                next_operator_index_test++;
            }
        }
    }
    else if (comma_contain_test == 0) {
        //,이 없다면 operand는 하나이므로 [0]에 전부 넣고, 나머지 operand는 공란.
        if (token_str != NULL) {
            strcpy(token_table[token_line]->operand[0], token_str);
            next_operator_index_test = +MAX_OPERAND;
        }
    }

    token_str = strtok(NULL, "\n");
    //주석의 토큰화
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
* 설명 : op table을 구현하고, txt로 출력하는 함수.
* 매계 : 전달받은 파일의 이름 및 구조체(optable_txt, token_table)
* 반환 : 성공 = 0, 실패 -1
* 주의 : token_table에서의 포인터 접근에러 현상으로 인해 버퍼를 만들어서 여기에 값을 넣고, 이걸 검사시킴
------------------------------------------------------------------------------------*/
void make_opcode_output(uchar* optable_txt) {
    FILE* optab_file = fopen("optab.txt", "w");
    if (optab_file == NULL) {
        printf("optab.txt 파일을 만들 수 없습니다.\n");
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
                    memmove(plus_position, plus_position + 1, strlen(plus_position));  // +를 제거
                }
                if ((strcmp(temp_operator, inst_table[i]->str) == 0) && (inst_table[i]->format != 0)) {
                    // token_table[j]->operator와 inst_table[i]->str이 동일하면서, format != 0
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
        //이제 사용하지 않을 예정이라 inst_table은 날려버림
    }
    fclose(optab_file);
    return 0;
}



/* -----------------------------------------------------------------------------------
* 설명 : symbol table을 계산하고, txt로 출력하는 함수.
* 입력 : 구조체(token_table)
* 출력 : symboltable_txt(symtab.txt)
* 반환 : 성공 = 0, 실패 -1
* 주의 : token_table에서의 포인터 접근에러 현상으로 인해 버퍼를 만들어서 여기에 값을 넣고, 이걸 검사시킴
------------------------------------------------------------------------------------*/
void make_symtab_output(uchar* symboltable_txt) {
    FILE* symtab_file = fopen("symtab.txt", "w");
    if (symtab_file == NULL) {
        printf("symtab.txt 파일을 만들 수 없습니다.\n");
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
                //operand가 null이 아니라면
                size_t length_to_copy = sizeof(buf_symbol_operand[j]) - 1;
                strncpy(buf_symbol_operand[j], token_table[i]->operand[j], length_to_copy);
                if (buf_symbol_operand[j][0] == '=') {
                    uchar copied_array[20];
                    strcpy(copied_array, &buf_symbol_operand[j]);
                    literal_table_make(copied_array, i);
                }
                buf_symbol_operand[j][length_to_copy] = '\0';  // Null 종료 문자 추가
            }
            else {
                buf_symbol_operand[j][0] = '\0';  // '\0' 추가
            }
        }

        if (strcmp(buf_symbol_operator, "START") == 0) {
            //START를 만나면
            locctr = start_locctr = strtol(buf_symbol_operand[0], NULL, 16);
            token_table[i]->locctr = locctr;    
            //문자열 형태로 operand값을 얻어 왔으니, 0에 해당하는 값을 빼면 됨. 
        }

        if (strcmp(buf_symbol_label, "MAXLEN") == 0) {
            //label이 maxlen
            int temp = find_format(buf_symbol_operator);
            if (temp == -1) {
                temp = 0;
            }
        }
        if (intermediate_first_loc == 1) {
            //처음 1회는 미실행 되는 코드임. 이는 start를 만나야만 작동이 시작함.
            if (strcmp(buf_symbol_operator, "EXTDEF") == 0) {
                //어.. 이건 구현을 어찌 해야 될까??
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
                //section을 초기화하는 것.
                locctr_system_history(locctr, 2, buf_maxlen_len);
                locctr = strtol(token_table[i]->operand[0], NULL, 16);
                start_locctr = locctr;
                locctr_count_true = 1;
                section++;
            }
            make_intermediate(i, locctr);
            if (strcmp(buf_symbol_operator, "RESB") == 0) {
                //RESB를 만나면
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
                    //Fisrt를 만나면
                    strncpy(sym_table[symbol_line].symbol, buf_symbol_operator, sizeof(sym_table[i].symbol) - 1);
                }
            }

            else if (buf_symbol_operator[0] == '\0') {
                //만약 이번 operator이 NULL이라면 
                locctr_count_true = 1;
            }

            if (locctr_count_true == 0) {
                locctr_system_history(locctr, 0, 0);
                int isDuplicate = 0;
                if (buf_symbol_label[0] != '\0') {
                    for (int i = 0; i < symbol_line; i++) {
                        if (strcmp(sym_table[i].symbol, buf_symbol_label) == 0) {
                            if (sym_table[i].section == section) {
                                // 중복 찾음
                                isDuplicate = 1;
                                break;
                            }
                        }
                    }
                    if (!isDuplicate) {
                        // 중복이 아닌 경우
                        strncpy(sym_table[symbol_line].symbol, buf_symbol_label, sizeof(sym_table[symbol_line].symbol) - 1);
                        sym_table[symbol_line].addr = locctr;
                        sym_table[symbol_line].section = section;
                        symbol_line++;
                    }
                }
                //LOCCTR값을 더하는 함수.
                locctr_plus(buf_symbol_operator, i);
                //locctr값을 더하기 위한 함수 호출
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
    //밑에는 symboll table의 구현이 완료됐으니. 이걸 출력하는 것
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
* 설명 : 시스템의 길이를 측정하는데 도움을 주는 함수
* 입력 : 현재의 locctr값, 명령을 제어하기 위한 변수 i, 현재의 size
* 반환 : 
* 주의 : CSECT에선 i값이 1이고, END를 만났다면 i의 값인 2를 입력받을 것임.
------------------------------------------------------------------------------------*/
void locctr_system_history(int now_locctr, int i, int size) {
    if (locctr_history[1] != 0) {
        locctr_history[2] = locctr_history[1];
    }
    if (locctr_history[0] != 0) {
        locctr_history[1] = locctr_history[0];
    }
    locctr_history[0] = now_locctr;
    //최근 3회의 locctr값을 보관하는 용도
    if ((i == 1) || (i == 2)) {
        int temp = locctr_history[0];
        for (int i = 1; i < 3; ++i) {
            if (locctr_history[i] > temp) {
                temp = locctr_history[i];
            }
        }
        //최근 3회의 locctr중, 가중 큰 값을 temp에 저장.
        int temp2 = temp + size;
        //현재 명령어의 크기를 temp에 더하여 최종적으로 temp2에 저장.
        sections[section_line].sys_len = temp2;
        //섹션별로 크기를 담당하는 구조체에 temp2를 저장.
        section_line++;
        program_length(temp);
        //현재의 프로그램 길이는 temp를 저장.
    }

    else if (i == 2) {
        for (int j = 0; j < 3; j++) {
            locctr_history[i] = NULL;
        }
    }
}


/* -----------------------------------------------------------------------------------
* 설명 : equ에 해당되는 문자열을 받아서 equ 테이블을 구성하는 함수
* 입력 : equ에 해당하는 str, equ가 있는 라인의 operator인 equ_token_operator
* 반환 : +, -의 연사늘 마친 결과값
* 주의 : 
------------------------------------------------------------------------------------*/
int equ_token_control(uchar str[20], uchar equ_token_operator) {
    int return_value = 0;
    char temp_str[20];
    char first_token[10];
    char next_token[10];

    strcpy(temp_str, str);
    // 토큰화
    char* token_start = strchr(str, equ_token_operator);

    // 토큰이 존재하는 경우
    if (token_start != NULL) {
        // 기호를 기준으로 앞 부분을 토큰으로 출력
        int token_length = token_start - str;
        strncpy(first_token, str, token_length);
        first_token[token_length] = '\0';  // 문자열 끝에 NULL 추가

        // 찾은 위치에서 문자열을 끊어줌
        // 토큰 다음의 문자열을 출력하려면 token + 1을 사용할 수 있음
        strcpy(next_token, token_start + 1);
    }
    else {
        // 기호가 없는 경우 전체 문자열을 토큰으로 출력
        printf("Token err: %s\n", str);
    }

    // equ a-b 혹은 a+b같은 상황을 처리하기 위함.
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
* 설명 : 전달받은 token을 symtab에서 검사하여 addr을 반환하는 함수
* 입력 : 전달받은 문자, 문자의 section값
* 반환 : 찾음 : symtab의 locctr값,  못찾음 : -1
* 주의 : 만약 내가 찾는 section의 위치에 결과가 없으면 자동으ㅗ
------------------------------------------------------------------------------------*/
int find_symbol_addr(const char* token, int sec) {
    // sym_table에서 token을 찾아 해당하는 symbol 구조체 반환
    int check = 0;
    for (int i = 0; i < MAX_LINES; ++i) {
        if (strcmp(token, sym_table[i].symbol) == 0) {
            if (sym_table[i].section == sec) {
                check = 1;
                return sym_table[i].addr; //  해당하는 addr 반환
            }
        }
    }
    if (check == 0) {
        //만약 찾지 못한다면 외부참조인 것.
        for (int i = 0; i < MAX_LINES; ++i) {
            if (strcmp(token, sym_table[i].symbol) == 0) {
                 check = 1;
                 return sym_table[i].addr; //  해당하는 addr 반환
            }
        }
    }
    return -1; // 찾지 못한 경우 -1 반환
}

void locctr_plus(uchar loc_str[20], int loc_token_line) {

    if (loc_str[0] == '+') {
        //4형식인 경우
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
    // 작은따옴표를 찾기 위한 포인터
    uchar* first_quote = strchr(input_str, '\'');
    uchar* second_quote = NULL;
    uchar result1[2];
    uchar result2[3]; 
    // 첫 번째 따옴표 이후의 문자열에서 두 번째 따옴표 찾기
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
    // 찾지 못한 경우, 기본값으로 0을 반환하거나 에러 처리를 수행할 수 있습니다.
    return -1;
}



void literal_table_make(uchar str[20], int line) {
    //printf("전달받은 문자열 확인: %s\n", str);
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
        printf("symtab.txt 파일을 만들 수 없습니다.\n");
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
                //첫 번쨰 작은따음표의 위치를 기록
                quote_count++;
            }
            else if (quote_count == 1) {
                finish_index = i;
                //2번 째 작은 따음표의 위치를 기록
                break;
            }
        }
    }
    if (global_liter_table == NULL)  
    {
        global_liter_table = (liter_table*)malloc(MAX_LINES * sizeof(liter_table));
        if (global_liter_table == NULL)
        {
            printf("메모리 동적할당 실패\n");
            return;
        }
        // 초기화
        for (int i = 0; i < MAX_LINES; ++i)
        {
            global_liter_table[i].size = 0;
        }
    }
    int isDuplicate = 0;
    for (int i = 0; i < literal_table_line; i++) {
        if (strcmp(global_liter_table[i].literal, str) == 0){
            // 문자열의 중복을 발견했다면
            if (global_liter_table[i].section == section) {
                isDuplicate = 1;
                break;
            }
        }
    }
    if (!isDuplicate) {
        // 중복이 아닌 경우
        int temp;
        size_t str_len = 0;
        while (str[str_len] != '\0') {
            ++str_len;
        }
        int check = 0;
        for (int i = line; i < token_line; ++i) {
            if (strstr(token_table[i]->operator, str) != NULL || strstr(token_table[i]->operator, "LTORG") != NULL) {
                global_liter_table[literal_table_line].LTORG_or_STR_line = i;
                //printf("리터럴 테이블 함수에서 찾은 것. %s, %d\n", token_table[i]->operator, i);
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
    //토큰화 작업을 확인하는 함수임
    int countDig = countDigits(token_line);
    printf("line\tobject\t\tformat\topcode\tlocctr\tlabel\toperator\toperand\t\t\tcomment\n");
    printf("--------------------------------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < token_line; i++) {
        //밑은 line 출력문
        printf("[%0*d]\t", countDig, i + 1);
        if(token_table[i]->object_code != NULL){
            printf("%-8X\t", token_table[i]->object_code);
        }
        else{
            printf("\t\t");
        }

        //밑은 format 출력문
        if (token_table[i]->format != -1) {
            printf("%01d\t", token_table[i]->format);
        }
        else {
            printf("\t");
        }

        //밑은 opcode 출력문
        if (token_table[i]->opcode != NULL) {
            printf("%02X\t", token_table[i]->opcode);
        }
        else {
            printf("\t");
        }

        //밑은 locctr 출력문
        printf("%04X\t", token_table[i]->locctr);

        //밑은 label 출력문
        if (token_table[i]->label != NULL) {
            printf("%s\t", token_table[i]->label);
        }
        else {
            printf("\t");
        }

        //밑은 operator 출력문
        printf("%s\t\t", token_table[i]->operator);

        //밑은 operand 출력문
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
    // 예외 처리: 음수를 양수로 변환하여 계산
    if (number < 0) {
        number = -number;
    }

    // 자릿수를 세는 변수 초기화
    int digitCount = 0;

    // 0인 경우 예외 처리
    if (number == 0) {
        return 1;
    }

    // 숫자의 각 자릿수를 세는 반복문
    while (number > 0) {
        number /= 10;  // 숫자를 10으로 나누어 다음 자릿수로 이동
        digitCount++;  // 자릿수 증가
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
        printf("symtab.txt 파일을 만들 수 없습니다.\n");
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
        printf("final.txt 파일을 만들 수 없습니다.\n");
        return;
    }
    int sec = 0;
    uchar buf_operator[8];
    //token_table에서 operator를 임시 저장할 배열
    uchar buf_label[8];
    //token_table에서 label를 임시 저장할 배열
    uchar buf_operand[MAX_OPERAND][20];
    //token_table에서 operand를 임시 저장할 배열
    int buf_locctr;
    //해당 라인의 locctr를 보관할 변수
    int buf_format;
    int first_line_check = 1;
    int check_def = 0;
    int check_ref = 0;
    uchar buf_extdef[MAX_OPERAND][20];
    uchar buf_extref[MAX_OPERAND][20];
    int def_ref_procedure = 0;
    //def와 ref중 무엇이 먼저 왔는지 확인하는 변수.
    for (int i = 0; i < token_line; i++) {
        //반복문을 token_line까지 반복
        buf_format = token_table[i]->format;
        buf_locctr = token_table[i]->locctr;
        if (token_table[i]->label != NULL) {
            //->label이 null이 아니라면 해당 내용을 복사
            size_t length_to_copy = sizeof(buf_label) - 1;
            strncpy(buf_label, token_table[i]->label, length_to_copy);
        }
        else {
            //null이면 \0를 넣음
            buf_label[0] = '\0';
        }
        if (token_table[i]->operator != NULL) {
            //->>operator가 null이 아니라면 해당 내용을 복사
            size_t length_to_copy = sizeof(buf_operator) - 1;
            strncpy(buf_operator, token_table[i]->operator, length_to_copy);
            buf_operator[length_to_copy] = '\0';
        }
        else {
            //null이면 \0를 넣음
            buf_operator[0] = '\0';
        }
        int j;
        //operand index용 변수
        //printf("%dline \t", i);
        for (j = 0; j < MAX_OPERAND; j++) {
            if (token_table[i]->operand[j][0] != '!') {
                size_t length_to_copy = sizeof(buf_operand[j]) - 1;
                strncpy(buf_operand[j], token_table[i]->operand[j], length_to_copy);
                buf_operand[j][length_to_copy] = '\0';  // Null 종료 문자 추가
                // printf("%d:%s\t", j, buf_objectcode_operand[j]);
            }
            else {
                buf_operand[j][0] = '!';  // '\0' 추가
            }
            if (j == 2) {
                // printf("\n\n");
            }
        }

        //---------------------------------불러오기는 종료----------------------------------------------

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
                //만약 def문장이 먼저 왔다면,.
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
                //만약 ref문장이 먼저 왔다면,.
                def_ref_procedure = 2;
            }
            check_ref = 1;
        }

        else{
            if (first_line_check == 0) {
                //파일의 시작이 아니라면
                if (def_ref_procedure > 0) {
                    if (def_ref_procedure == 1) {
                        //def가 먼저 나왔다면
                        if (check_def == 1) {
                            //EXTDEF가 있었다면
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
                            //EXTREF가 있었다면
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
                        //ref가 먼저 나왔다면
                        if (check_ref == 1) {
                            //EXTREF가 있었다면
                            fprintf(final_file, "R");
                            for (int j = 0; j < MAX_OPERAND; j++) {
                                if (buf_extref[j][0] != '!') {
                                    fprintf(final_file, "%s", buf_extref[j]);
                                }
                            }
                            fprintf(final_file, "\n");

                        }
                        if (check_def == 1) {
                            //EXTDEF가 있었다면
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
                    //다시 파일이 처음 시작이란 것을 안내해줌.
                    check_def = 0;
                    fprintf(final_file, "E");
                    if (sec==0){
                        fprintf(final_file, "%06X", start_locctr);
                    }
                    fprintf(final_file, "\n\n");
                    sec++;
                    //section을 하나 올림.
                    if (strcmp(buf_operator, "END") == 0) {
                        //END였다면 종료.
                        break;
                    }
                }
                else{

                }
            }
            else if (first_line_check == 1) {
                //해당 파일의 처음 라인이라면
                fprintf(final_file, "H%s\t%06X%06X\n", buf_label, start_locctr, sections[sec].sys_len);
                first_line_check = 0;
                //더 이상 첫줄이 아님을 안내함.
            }
        }



    }
    fclose(final_file);
}
void make_objectcode_output(uchar* object_txt){
    FILE* object_file = fopen(object_txt, "w");
    if (object_file == NULL) {
        printf("object.txt 파일을 만들 수 없습니다.\n");
        return;
    }

    int countDig = countDigits(token_line);
    int object_line = 0;
    int start_check = 0;
        //start를 만나면 값을 바꿔서 이후에 있는 라인들만 코드가 작동되도록 하는 것.
    int buf_format;
    int buf_opcode;
    uchar buf_objectcode_operator[8];
    //token_table에서 operator를 임시 저장할 배열
    uchar buf_objectcode_label[8];
    //token_table에서 label를 임시 저장할 배열
    uchar buf_objectcode_operand[MAX_OPERAND][20];
    //token_table에서 operand를 임시 저장할 배열
    int buffer_locctr;
    //해당 라인의 locctr를 보관할 변수
    char binary[9] = "";
    //16진수 2글자를 2진수로 변환하는 코드
    int sec = 0;
    for (int i = 0; i < token_line; i++) {
        //반복문을 token_line까지 반복
        int locctr_count_true = 0;
        buf_format = token_table[i]->format;
        if (token_table[i]->label != NULL) {
            //->label이 null이 아니라면 해당 내용을 복사
            size_t length_to_copy = sizeof(buf_objectcode_label) - 1;
            strncpy(buf_objectcode_label, token_table[i]->label, length_to_copy);
        }
        else {
            //null이면 \0를 넣음
            buf_objectcode_label[0] = '\0';
        }

        if (token_table[i]->operator != NULL) {
            //->>operator가 null이 아니라면 해당 내용을 복사
            size_t length_to_copy = sizeof(buf_objectcode_operator) - 1;
            strncpy(buf_objectcode_operator, token_table[i]->operator, length_to_copy);
            buf_objectcode_operator[length_to_copy] = '\0';
        }
        else {
            //null이면 \0를 넣음
            buf_objectcode_operator[0] = '\0';
        }
        int j;
        //operand index용 변수
        //printf("%dline \t", i);
        for (j = 0; j < MAX_OPERAND; j++) {
            if (token_table[i]->operand[j][0] != '!') {
                size_t length_to_copy = sizeof(buf_objectcode_operand[j]) - 1;
                strncpy(buf_objectcode_operand[j], token_table[i]->operand[j], length_to_copy);
                buf_objectcode_operand[j][length_to_copy] = '\0';  // Null 종료 문자 추가
               // printf("%d:%s\t", j, buf_objectcode_operand[j]);
            }
            else {
                buf_objectcode_operand[j][0] = '!';  // '\0' 추가
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
            fprintf(stderr, "메모리 할당 실패\n");
            exit(1); // 프로그램 종료 또는 적절한 오류 처리
        }
        int xbpe[4] = { 0 };

        if (start_check != 0) {

            if (strcmp(buf_objectcode_operator, "END") == 0) {
                break;
            }
            /*
                여기 밑에는 이제 START ~ END사이에 해당되는 라인들이 실행되는 것들.
            */
            if (token_table[i + 1]->locctr != NULL) {
                //pc를 저장함.
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
                    //format 1인 경우의 object_code는 opcode 1byte(=8bit) 그대로 씀.
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
                            //만약 null칸을 만난다면 -1값을 넣어버림. 이후에 -1은 null로 취급할 것.
                        }
                    }

                    if (found_operand_Addr[1] == -1) {
                        //operand가 하나만 있을 경우.
                        int binaryArray[4] = { 0 };
                        int locctr_overflowr_check;
                        if (buf_objectcode_operand[0][0] == '=') {
                            //리터럴 처리 object = op + 3;
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
                            //리터럴의 실제 위치를 반환
                            locctr_overflowr_check = litter_locctr - pc_count;
                            int temp;
                            if (abs(locctr_overflowr_check) >= 4096) {
                                //Overflowr검사 기능
                                xbpe[1] = 1;
                                temp = 4;
                                //4(10) = 0100(2)
                                xbpe_of_nixbpe(xbpe, i);
                                //b에 1을 대입.
                            }
                            else {
                                xbpe[2] = 1;
                                temp = 2;
                                //2(10) = 0010(2)
                                xbpe_of_nixbpe(xbpe, i);
                                //p에 1을 대입.
                            }
                            decimalToBinary(locctr_overflowr_check, i, 7);
                            //disp_1에 알맞은 값을 넣는 역할.
                            hex_binary_table[i].ob = ((buf_opcode + 3) << 16) + (temp << 12) + (locctr_overflowr_check);
                        }
                        else if (buf_objectcode_operand[0][0] == '@') {
                            //@가 간접주소였나? object = op + 2
                            decimalToBinary(buf_opcode + 2, i, 3);
                            locctr_overflowr_check = pc_count - found_operator_locctr;
                            int temp;
                            if ((abs(locctr_overflowr_check) >= 4096) && (locctr_overflowr_check >= 0)) {
                                //Overflowr발생 = base
                                xbpe[1] = 1;
                                temp = 4;
                                //4(10) = 0100(2)
                                xbpe_of_nixbpe(xbpe, i);
                                //b에 1을 대입.
                            }
                            else if ((locctr_overflowr_check < 4096) && (locctr_overflowr_check >= 0)){
                                //미발생한 양수
                                xbpe[2] = 1;
                                temp = 2;
                                //2(10) = 0010(2)
                                xbpe_of_nixbpe(xbpe, i);
                                //p에 1을 대입.
                            }
                            else {
                                //미발생한 음수
                            }
                            hex_binary_table[i].ob = ((buf_opcode + 2) << 16) + (temp << 12);
                        }
                        else if (buf_objectcode_operand[0][0] == '#') {
                            //#이 즉시주소로 #의 뒤에 있는 것을 사용함. object = op + 1
                            int temp = (int)buf_objectcode_operand[0][1]; 
                            //temp에 buf_objectcode_operand[0][1]의 아스키코드가 저장됨.
                            decimalToBinary(buf_opcode + 1, i, 3);
                            xbpe_of_nixbpe(xbpe, i);
                            //x, b, p, e에 0을 대입.
                            hex_binary_table[i].ob = ((buf_opcode + 1) << 16);
                            if (temp < 65){
                                //temp에 숫자가 아스키코드로 들어 있음.
                                temp -= 48;
                                //아크키코드 값을 빼서 실제 1의자리 정수로 만들어줌.
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
                            //operand가 없는 operator 처리  object = op + 3
                            hex_binary_table[i].ob = ((buf_opcode + 3) << 16);
                            //hex_binary_table[i].ob = buf_opcode + 3;
                            //operand가 없으므로, 해당 operator의 op값에 해당 길이인 3을 더하는 것.
                            decimalToBinary(buf_opcode + 3, i, 3);
                            for (j = 0; j < 4; j++) {
                                xbpe[j] = 0;
                            }
                            xbpe_of_nixbpe(xbpe, i);
                            //x, b, p, e에 0을 대입.
                        }
                        else {
                            //나머지인 일반 format3의 처리.
                            decimalToBinary(buf_opcode + 3, i, 3);
                            locctr_overflowr_check = found_operand_Addr[0] - pc_count;
                            //opcode - pc
                            int temp;
                            if ((abs(locctr_overflowr_check) >= 4096) && (locctr_overflowr_check >= 0)) {
                                //overflowr 발생.
                                xbpe[1] = 1;
                                xbpe_of_nixbpe(xbpe, i);
                                //b에 1을 대입.
                                temp = 4;
                                //2(10) = 0010(2)
                            }
                            else if ((locctr_overflowr_check < 4096) && (locctr_overflowr_check >= 0)) {
                                //overflowr 미발생 양수
                                xbpe[2] = 1;
                                xbpe_of_nixbpe(xbpe, i);
                                //p에 1을 대입.
                                temp = 2;
                                //2(10) = 0010(2)
                            }
                            else{    
                                //결과가 음수..
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
                        //현재 과제물에는 이런 상황이 없어서 코드를 미입력 상태.
                        //operand가 2개 있을 경우
                    }
                    else if (found_operand_Addr[2] != -1) {
                        //현대 과제물에는 이런 상황이  없어서 코드를 미입력 상태.
                        //operand가 3개 있을 경우
                    }
                    break;
                    //format 3종료

                case(4):
                    decimalToBinary(buf_opcode + 3, i, 4);
                    hex_binary_table[i].format = 4;
                    if (buf_objectcode_operator[0] == '+') {
                        hex_binary_table[i].ob = (buf_opcode + 3 << 24);
                        int found_operand_locctr = find_symbol_addr(buf_objectcode_operand[0], sec);
                        if (buf_objectcode_operand[1][0] != '!') {
                            //더이상 오퍼랜드가 있음.
                            if (buf_objectcode_operand[1][0] == 'X') {
                                xbpe[0] = 1;
                                xbpe[3] = 1;
                                xbpe_of_nixbpe(xbpe, i);
                                hex_binary_table[i].ob += (9 << 20);
                            }
                        }
                        else {
                            //추가로 오퍼랜드가 없음. 
                            xbpe[3] = 1;
                            xbpe_of_nixbpe(xbpe, i);
                            hex_binary_table[i].ob += (1 << 20);
                        }
                    }
                    else {
                        //현재 과제에는 없어서 구현해두지 않은 상태임.
                    }
                    break;

                default:
                    hex_binary_table[i].format = -1;
                    break;
                }
                token_table[i]->object_code = hex_binary_table[i].ob;
            }

            /*
                밑에는 object_file 출력을 하는 코드들임.
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
                        //format 1은 여기서 출력 종료
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
                        //format 2 출력 종료
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
                        //format 3은 여기서 출력 종료
                    }
                    else if (hex_binary_table[i].format == 4) {
                        for (int j = 0; j < 8; j++) {
                            fprintf(object_file, "%d", hex_binary_table[i].disp_2[j]);
                            if ((j + 1) % 4 == 0) {
                                fprintf(object_file, " ");
                            }
                        }
                        fprintf(object_file, " \n");
                        //format 4 출력 종료
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
        //buf_opcode에 있는 16진수를 2진수로 변환하여 보관할 변수.
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
        //format2에서 레지스터에 값을 넣는 것.
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

    // '+' 기호가 있는지 확인
    char temp[10];  // 필요에 따라 크기 조절
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
    temp[j] = '\0';  // temp 문자열에 null 종료 문자 추가
    if (hasPlus == 0) {
        //+가 발견되지 않았다면.
        strcpy(temp, str1);
    }
    //printf("%s\n", temp);
    // op_table과 비교할 때 '+'가 있으면 수정된 문자열(temp)로 비교
    for (int i = 0; i < opt_index; ++i) {
        if (strcmp(op_table[i].str, temp) == 0) {
            //printf("%s = %02X\n", str1, op_table[i].op);
            return op_table[i].op;
        }
    }

    return err;
}