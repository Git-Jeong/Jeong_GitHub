/*
배열 number가 오름차순으로 정렬되어 출력 
*/
int main(void){
#define num 20
int i, j;
int number[num] = {11,234,23,4,1,5,6,2,65,764,825,46,72,47,26,69,793,25,498,245};
int temp;
for(i = 0 ; i < num ; i ++) {
     for(j = 0 ; j < num -i -1 ; j ++) {
                if(number[j]>number[j+1]) {
                    temp = number[j];
                    number[j] = number[j+1];
                    number[j+1] = temp;
         }
    }
}
         
for(i = 0 ; i < 20 ; i ++) {
     printf("%d, ", number[i]);
}

}
