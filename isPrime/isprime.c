//Determine whether a given number is prime

//take in a number and output whther it is a prime number or not 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
int main(int argc, char *argv[]){
  if(argc < 2){
    printf("usage: %s <number>\n", argv[0]);
    return 1;
  }
  int number = atoi(argv[1]);
  if(number <= 1){
    printf("%d is not a prime\n", number);
    return 0;
  } else if(number == 2){
    printf("2 is prime\n");
    return 0;
  }
  int *divisors = malloc(sizeof(int) * number);//used malloc becuase is VLA 
  //could set to something smaller and realloc later as well.
  int count = 0; 
  bool flag = true;
  int limit = sqrt(number);
  if(number > 2){
    for(int i = 2; i <= limit; i++){
      if(number % i > 0){
      
      } else {
        divisors[count]=i;
        count++;
        flag = false;
      }
      }
    if(flag == true){
      printf("%d is a prime number", number);
    } else {
      printf("%d is not a prime number, it is divisible by: ", number);
      for(int i =0; i < count; i++){
        if(i < count-1){
        printf("%d, ", divisors[i]);
        } else {
        printf("%d ", divisors[i]);

        }
        }
    }
  }
  free(divisors);
  return 0;
}
