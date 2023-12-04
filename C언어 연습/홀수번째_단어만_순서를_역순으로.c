1. #define _CRT_SECURE_NO_WARNINGS
//vs에서 빌드를 위한 줄
#include <stdio.h>
#include <string.h>
#define Minimum_word 4
//최소 단어 수를 정의

void reverseString(char* str, int start, int end) {
    //main함수로 부터 역전할 단어의 시작, 끝 배열번호를 받음
    while (start < end) {
        //단어가 끝날때까지 반복
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
        //srart는 앞에서 부터 시작하고, end는 뒤에서 부터 시작하여 서로 값을 바꿔 가다가 서로 만나는 시점까지 시행
    }
}

int main() {
    char input[1000];
    //사용자가 적을 문자를 보관할 배열
    printf("적어도 %d개의 단어를 포함하는 문자열을 입력하세요: ", Minimum_word);
    //사용자에게 문자열을 적도록 안내
    do {
        fgets(input, sizeof(input), stdin);

        input[strcspn(input, "\n")] = '\0';
        //사용자가 엔터를 눌러 생겼을 \n을 제거함

        int wordCount = 0;
        //입력한 단어의 수 계산
        int length = strlen(input);
        //lengh변수는 사용자가 입력한 문자열의 크기만큼 할당함
        int wordStart = 0;
        int i;
        for (i = 0; i <= length; ++i) {
            if (input[i] == ' ' || input[i] == '\t' || input[i] == '\0') {
                //사용자가 띄어쓰기나, tab을 통해 단ㅇ어를 구분하였다면 그걸 확인하여 wordcount 변수에 값을 더함.
                wordCount++;
                if (wordCount % 2 == 1) {
                    //만약 홀수라면
                    int end = i - 1;
                    //띄어쓰기 바로 앞에 있는 배열칸을 end변수에 저장
                    reverseString(input, wordStart, end);
                    //배열 순서를 뒤집는 함수에 문자열, 단어의 시작번호, 끝번호를 전달함
                }
                wordStart = i + 1;
                //띄어쓰기를 만난 직후의 배열 번호를 wordstart에 저장
            }
        }

        if (wordCount < Minimum_word) {
            //입력한 문자열의 단어 수를 확인하여 반려함
            printf("적어도 %d개의 단어를 포함하는 문자열을 다시 입력하세요: ", Minimum_word);
        }
        else {
            break;
            //위의 if문에 따라 조건이 맞으면 do문을 탈출함.
        }
    } while (1);


    printf("철자 순서가 뒤집힌 결과: %s\n", input);

    return 0;
}
