/*
	2019년을 기준으로 한국식 나이를 구하는 코드 
*/
#include <stdio.h>
int main(void)
{
	int year;
	printf("Your Birth year : ");
	scanf("%d", &year);
	printf("you are %d\n", 2019 - year + 1);
	return 0;
}
