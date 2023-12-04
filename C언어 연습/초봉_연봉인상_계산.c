#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_SAWON_NUM 5
#define MAX_YEARS 10
//추후 사원수와 년도수를 쉽게 수정하기 위하여 define기능을 활용함

struct wage {
	int salary;	//연봉, 단위는 1천원
	int rate;	//임금 인상률. 단위는 %
	int raised;	//연간 인상액. 단위는 천원
	int monthly;//월간 임금액. 단위는 천원
};
struct sawon {
	char name[100];			//최대 100자의 영문 이름
	struct wage income[MAX_YEARS];	//10년간의 임금 정보 내포
};


void rate_function(struct sawon* sawon_list) {
	for (int i = 0; i < MAX_YEARS; i++) {
		int year_rate = (rand() % 5 + 1);
		//1~5까지의 임의의 수에 +1을 한 값을 연봉인상률로 결정
		for (int j = 0; j < MAX_SAWON_NUM; j++) {
			sawon_list[j].income[i].rate = year_rate;
			//모든 사용자에게 매년 같은 인상률을 대입
		}
	}
}


int count_digits(int num) {
	//출력문에서 %d의 자릿수 맞춤을 위한 기능
	int count = 1;
	//자릿수 카운터를 우한 변수들
	while (num > 0) {
		//제공받은 수가  0q보다 크면 실행.
		num /= 10; // 수를 10으로 나누어 자릿수를 하나씩 줄임
		count++;
	}
	return count;
}

void sawon_income_year_monthly(struct sawon* sawon_list) {
	//10년간의 연봉, 연봉인상액, 월급 게산 함수
	int next_year_raised;
	//내년에 적용될 연봉인상액
	for (int j = 0; j < MAX_SAWON_NUM; j++) {
		for (int i = 0; i < MAX_YEARS; i++) {
			if (i == 0) {
				//만약 입사한지 첫 해라면
				next_year_raised = (sawon_list[j].income[i].salary * (sawon_list[j].income[i].rate) / 100);
				//내년에 적용될 연봉인상액 = 현재 연봉 * (연봉인상 퍼센트 /100)
				sawon_list[j].income[i].raised = next_year_raised;
				//각 사원별 연봉인상액 칸에 계산값 대입
			}
			else if (i != 0) {
				//첫해가 아니라면
				sawon_list[j].income[i].salary = (sawon_list[j].income[i - 1].salary + sawon_list[j].income[i - 1].raised);
				//작년에 구해진 인상액을 작년연봉에 더해서 현재 연봉울 구함.
				next_year_raised = (sawon_list[j].income[i].salary * (sawon_list[j].income[i].rate) / 100);
				//내년에 적용될 연봉인상액 = 현재 연봉 * (연봉인상 퍼센트 /100)
				sawon_list[j].income[i].raised = next_year_raised;
				//각 사원별 연봉인상액 칸에 계산값 대입
			}
		}
	}

	for (int i = 0; i < MAX_YEARS; i++) {
		for (int j = 0; j < MAX_SAWON_NUM; j++) {
			sawon_list[j].income[i].monthly = sawon_list[j].income[i].salary / 12;
			//연봉을 12로 나누어 매달 받을 월급을 계산하여 대입
		}
	}
}



int main(void) {
	struct sawon sawon_list[MAX_SAWON_NUM];
	rate_function(sawon_list);
	//10년간의 물가상승을 저장하는 함수.
	int countDigits = count_digits(MAX_YEARS);
	//연차의 자릿수 계산
	int countDigits_sawon = count_digits(MAX_SAWON_NUM);
	//사원수의 자릿수 계산
	char buf_char[100];
	//임시로 문자열을 저장할 버퍼
	int starting_salary;
	//초봉을 보관할 변수
	for (int i = 0; i < MAX_SAWON_NUM; i++) {
		printf("%*d번째 사원의 이름을 작성해주세요. : ", countDigits_sawon, i + 1);
		//출력문을 활용해 사용자에게 사원의 이름을 작성하게 안내
		scanf(" %99[^\n]", buf_char);
		//띄어쓰기를 포함하여 문자열 입력받음
		strcpy(sawon_list[i].name, buf_char);
		//임시 저장한 사용자 이름을 구조체에 있는 사원이름에 복사함
		printf("%s 사원의 초봉은 얼마인가요?(여기서 단위는 1천원으로 숫자만 입력해주세요.) : ", sawon_list[i].name);
		//사용자가 입력한 사원의 초봉을 적어달라고 안내
		scanf("%d", &starting_salary);
		//사용자가 적은 숫자를 변수에 보관
		sawon_list[i].income[0].salary = starting_salary;
		//사용자가 적은 수를 구조체에도 저장
	}
	sawon_income_year_monthly(sawon_list);
	//10년간의 연봉, 연봉인상액, 월급 게산 함수
	for (int j = 0; j < MAX_SAWON_NUM; j++) {
		printf("이름 : %s\n", sawon_list[j].name);
		for (int i = 0; i < MAX_YEARS; i++) {
			if (i != (MAX_YEARS - 1)) {
				printf("%*d년차 연봉 : %9d천원,\t", countDigits, i + 1, sawon_list[j].income[i].salary);
				printf("매월 받는 임금 : %8d천원\t", sawon_list[j].income[i].monthly);
				printf("연봉인상률 : %1d%%\t", sawon_list[j].income[i].rate);
				printf("인상액 : %7d천원\n", sawon_list[j].income[i].raised);
			}
			else {
				printf("%*d년차 연봉 : %9d천원,\t", countDigits, i + 1, sawon_list[j].income[i].salary);
				printf("매월 받는 임금 : %8d천원\n", sawon_list[j].income[i].monthly);
			}
		}
		printf("\n");
	}
	return 0;
}