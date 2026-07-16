//simply return the sum of integers in an array

//sum = 0
//for each num in array
//    add num to sum
//return sum 
#include <stdio.h>

int sumArray(int nums[], int length){
int sum = 0;
//int length = sizeof(nums)/sizeof(nums[0]);
for(int i = 0; i < length; i++){
    sum += nums[i];
  }
  return sum;
}

int main(){
int arr1[] = {1, 3, 5, 7}; // 16
int arr2[] = {2, 4, 6, 8, 10}; // 30
int length = sizeof(arr1) / sizeof(arr1[0]);
printf("Array 1 sum is %d\n", sumArray(arr1, length));
  length = sizeof(arr2) / sizeof(arr2[0]);
printf("Array 2 sum is %d\n", sumArray(arr2, length));

}
