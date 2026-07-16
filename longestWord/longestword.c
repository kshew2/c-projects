// write a program that reads a string and returns
//the longest word and the length.

/*
take in a string
split it into words 
compare words to find the longest 
return longest word and its length 
*/
#include <stdio.h>
#include <string.h>

char *arrlong(char s[]){
  char * buf = strtok(s, " ");
  char *longest = buf;
  while(buf != NULL){
    
    if(strlen(buf) > strlen(longest)){
      longest = buf; // need the full string not just first character
      int buflength = strlen(longest);
    }
    buf = strtok(NULL, " ");// this has to be after the if statement it was cause segmentation fault
    //because strlen(buf) when buf == NULL
  }
  return longest;
  }

char *arrlongCMD(char *words[], int count){ //=to **words which is an array of string pointers.
  char *longest = words[0];
  for (int i = 1; i < count; i++){
    if(strlen(words[i]) > strlen(longest)){
      longest = words[i];
    }
  }
  return longest;
}


int main(int argc, char *argv[]){
  char s[] = "the cat Jumped over the MOOOOOn";
  printf("largest word is: %s\n", arrlong(s));
  printf("largest word is: %s\n", arrlongCMD(argv +1, argc -1));//arg[0] is function name so we skip and need 1 less count to process.
  return 0;
}
