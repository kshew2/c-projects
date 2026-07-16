//read a text file and return the frequncy of each 
//ASCII character 
/*
LOGIC: open file 
read and parse the file, write each character to an array 
increcment each character by the frequency.
return the all characters whose frequency is > 0
*/
#include <stdio.h>


void frequencyOfChar(char *filename, int counts[128]){
  //opens the file in read implemntation "r"
  FILE *file = fopen(filename, "r");
  if(file == NULL){
    printf("Could not open file\n");
    return;
  }
  //use fgets to read contents which uses the storage array, 
  //size of data, 
  //and the pointer to the file
  //read the contents and store it inside buf
  int c;
  while((c = fgetc(file)) != EOF){
    counts[c]++;
  //printf("%d: %i,\n", c, counts[c]);
  }
  fclose(file);
}

int main(int argc, char *argv[]){
  if(argc < 2){
    printf("Usage: %s <filename>\n", argv[0]);
    return 1;
  }

  int counts[128] = {0};
  frequencyOfChar(argv[1], counts);

  for(int i = 0; i<128; i++){
    if(counts[i] > 0){
      printf("%c: %d\n", i, counts[i]);
    }
  }
  return 0;
}
//make second implemntation that takes command line arguments
/*int main(){
  int counts[128]={0};
  frequencyOfChar(counts);
  for(int i = 0; i < 128; i++){
    printf("%i, %i,\n", i, counts[i]);
  }
  return 0;
}*/ 
