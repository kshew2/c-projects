//return sums of evens and sums of odds indexes in an integers arr
//even and odd sums
//when even
//for i in arr 
//where indexes are even add to sum else skip 
//where odd 
//for i in arr 
//where indexes odd add to sum else skip 
//return sum 
  
#include <stdio.h>
#include <stdbool.h>

int evenOddSums(int nums[], int length, bool evens){
  int sum = 0;
  //turns out switch isent even necassary. Could use if else 
  //But conditional isnt required since the logic can be 
  //completed actually in the for loop
  switch(evens){
    case 1:
    for(int i = 0; i < length; i++){
        if(i % 2 == 0){
          sum += nums[i];
        }
      }
    break;
    case 0:
    for(int i = 0; i < length; i++){
        if(i % 2 != 0){
          sum += nums[i];
        }
      }
    break;
  }
  return sum;
}

int main(){
  int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9}; //even: 20, odd: 25
  int length = sizeof(arr) / sizeof(arr[0]);

  printf("even sums are: %d, odd sums are: %d\n", evenOddSums(arr, length, true), evenOddSums(arr, length, false));
}
