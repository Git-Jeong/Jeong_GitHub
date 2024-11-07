#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_SAWON_NUM 5
#define MAX_YEARS 10
//���� ������� �⵵���� ���� �����ϱ� ���Ͽ� define����� Ȱ����

struct wage {
	int salary;	//����, ������ 1õ��
	int rate;	//�ӱ� �λ��. ������ %
	int raised;	//���� �λ��. ������ õ��
	int monthly;//���� �ӱݾ�. ������ õ��
};
struct sawon {
	char name[100];			//�ִ� 100���� ���� �̸�
	struct wage income[MAX_YEARS];	//10�Ⱓ�� �ӱ� ���� ����
};


void rate_function(struct sawon* sawon_list) {
	for (int i = 0; i < MAX_YEARS; i++) {
		int year_rate = (rand() % 5 + 1);
		//1~5������ ������ ���� +1�� �� ���� �����λ���� ����
		for (int j = 0; j < MAX_SAWON_NUM; j++) {
			sawon_list[j].income[i].rate = year_rate;
			//��� ����ڿ��� �ų� ���� �λ���� ����
		}
	}
}


int count_digits(int num) {
	//��¹����� %d�� �ڸ��� ������ ���� ���
	int count = 1;
	//�ڸ��� ī���͸� ���� ������
	while (num > 0) {
		//�������� ����  0q���� ũ�� ����.
		num /= 10; // ���� 10���� ������ �ڸ����� �ϳ��� ����
		count++;
	}
	return count;
}

void sawon_income_year_monthly(struct sawon* sawon_list) {
	//10�Ⱓ�� ����, �����λ��, ���� �Ի� �Լ�
	int next_year_raised;
	//���⿡ ����� �����λ��
	for (int j = 0; j < MAX_SAWON_NUM; j++) {
		for (int i = 0; i < MAX_YEARS; i++) {
			if (i == 0) {
				//���� �Ի����� ù �ض��
				next_year_raised = (sawon_list[j].income[i].salary * (sawon_list[j].income[i].rate) / 100);
				//���⿡ ����� �����λ�� = ���� ���� * (�����λ� �ۼ�Ʈ /100)
				sawon_list[j].income[i].raised = next_year_raised;
				//�� ����� �����λ�� ĭ�� ��갪 ����
			}
			else if (i != 0) {
				//ù�ذ� �ƴ϶��
				sawon_list[j].income[i].salary = (sawon_list[j].income[i - 1].salary + sawon_list[j].income[i - 1].raised);
				//�۳⿡ ������ �λ���� �۳⿬���� ���ؼ� ���� ������ ����.
				next_year_raised = (sawon_list[j].income[i].salary * (sawon_list[j].income[i].rate) / 100);
				//���⿡ ����� �����λ�� = ���� ���� * (�����λ� �ۼ�Ʈ /100)
				sawon_list[j].income[i].raised = next_year_raised;
				//�� ����� �����λ�� ĭ�� ��갪 ����
			}
		}
	}

	for (int i = 0; i < MAX_YEARS; i++) {
		for (int j = 0; j < MAX_SAWON_NUM; j++) {
			sawon_list[j].income[i].monthly = sawon_list[j].income[i].salary / 12;
			//������ 12�� ������ �Ŵ� ���� ������ ����Ͽ� ����
		}
	}
}



int main(void) {
	struct sawon sawon_list[MAX_SAWON_NUM];
	rate_function(sawon_list);
	//10�Ⱓ�� ��������� �����ϴ� �Լ�.
	int countDigits = count_digits(MAX_YEARS);
	//������ �ڸ��� ���
	int countDigits_sawon = count_digits(MAX_SAWON_NUM);
	//������� �ڸ��� ���
	char buf_char[100];
	//�ӽ÷� ���ڿ��� ������ ����
	int starting_salary;
	//�ʺ��� ������ ����
	for (int i = 0; i < MAX_SAWON_NUM; i++) {
		printf("%*d��° ����� �̸��� �ۼ����ּ���. : ", countDigits_sawon, i + 1);
		//��¹��� Ȱ���� ����ڿ��� ����� �̸��� �ۼ��ϰ� �ȳ�
		scanf(" %99[^\n]", buf_char);
		//���⸦ �����Ͽ� ���ڿ� �Է¹���
		strcpy(sawon_list[i].name, buf_char);
		//�ӽ� ������ ����� �̸��� ����ü�� �ִ� ����̸��� ������
		printf("%s ����� �ʺ��� ���ΰ���?(���⼭ ������ 1õ������ ���ڸ� �Է����ּ���.) : ", sawon_list[i].name);
		//����ڰ� �Է��� ����� �ʺ��� ����޶�� �ȳ�
		scanf("%d", &starting_salary);
		//����ڰ� ���� ���ڸ� ������ ����
		sawon_list[i].income[0].salary = starting_salary;
		//����ڰ� ���� ���� ����ü���� ����
	}
	sawon_income_year_monthly(sawon_list);
	//10�Ⱓ�� ����, �����λ��, ���� �Ի� �Լ�
	for (int j = 0; j < MAX_SAWON_NUM; j++) {
		printf("�̸� : %s\n", sawon_list[j].name);
		for (int i = 0; i < MAX_YEARS; i++) {
			if (i != (MAX_YEARS - 1)) {
				printf("%*d���� ���� : %9dõ��,\t", countDigits, i + 1, sawon_list[j].income[i].salary);
				printf("�ſ� �޴� �ӱ� : %8dõ��\t", sawon_list[j].income[i].monthly);
				printf("�����λ�� : %1d%%\t", sawon_list[j].income[i].rate);
				printf("�λ�� : %7dõ��\n", sawon_list[j].income[i].raised);
			}
			else {
				printf("%*d���� ���� : %9dõ��,\t", countDigits, i + 1, sawon_list[j].income[i].salary);
				printf("�ſ� �޴� �ӱ� : %8dõ��\n", sawon_list[j].income[i].monthly);
			}
		}
		printf("\n");
	}
	return 0;
}