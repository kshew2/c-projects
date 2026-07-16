//Write a C program that returns an approximation of Phi using the Fibonacci series. PHI -> ϕ≈1.618033988...
//take in a number of iterations to do then show the approcimation 
//take in num 
//calulate the fib numbers into an array 
//show the approxmation ratio of fn+1/fn  
//print the results
#include <stdio.h>
#include <stdlib.h>

void fibs(int fibArr[], int iterations){
  fibArr[0] = 0;
  fibArr[1]= 1;
  for(int i = 2; i < iterations; i++){
    fibArr[i] = (fibArr[i-1] + fibArr[i-2]);
    //printf("%d",fibArr[i]);
  }
}

int main(int argc, char *argv[]){
  if(argc > 1){
    int iterations = atoi(argv[1]);
    int *arr = malloc(sizeof(int) * iterations);
    fibs(arr, iterations);
    for(int i =0; i < iterations; i++){
      if(i < iterations -1){
        if(i > 1){
      printf("%d - Phi approximation: %.10f, \n", arr[i], (double)arr[i]/(double)arr[i-1]);
        } else {
      printf("%d, \n", arr[i]);

        }
      } else{
      printf("%d - Phi approximation: %.10f \n", arr[i], (double)arr[i]/(double)arr[i-1]);
      }
    }
  free(arr);
  } else {
    printf("%s <iterations>\n", argv[0]);
  }
  return 0;
}
