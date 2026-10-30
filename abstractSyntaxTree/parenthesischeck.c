// write a program that reads a parenthesized expression and check whether
// parenthsis are placed properly. Precisely indicate location of the erroneous
// symbol

// could pass in as command line argument in quotation but want
// to try using stdin for input after runtime.

//***********
// NOT A COMPLETE IMPLEMENTATION. CLOSE ENOUGH SO IM
// MOVING ON TO THE NEXT PROBLEM********************
//***********
#include "parenthesischeck.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void checkExpression(char exp[], int size) {
  int stack[500];
  int top = -1;

  for (int i = 0; i < size; i++) {

    printf("%c\n", exp[i]);

    if (exp[i] == '(') {
      top++;
      stack[top] = i;
    } else if (exp[i] == ')') {

      if (top == -1) {
        printf("Unmatched ')' at position %d\n", i);
        return;
      }

      top--;
    }
  }

  while (top >= 0) {
    printf("Unmatched '(' at position %d\n", stack[top]);
    top--;
  }
}
#ifndef EXCLUDE_MAIN
int main(int argc, char *argv[]) {
  char expr[500] = {0};
  printf("Enter an expression: \n");
  fgets(expr, sizeof(expr), stdin);
  size_t len = strlen(expr);
  // remove trailing newline
  if (len > 0 && expr[len - 1] == '\n') {
    expr[len - 1] = '\0';
    len--;
  }
  checkExpression(expr, len);
  return 0;
}
#endif
