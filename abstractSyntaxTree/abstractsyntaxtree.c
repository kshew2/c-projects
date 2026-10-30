// description: Write a C program that builds the Abstract Syntax Tree of a
// mathematical expression.

// step 1: Tokens - split the input into Tokens
// step 2: define how expressions are structured for example -
//(+,-) bind left associatively and (*,/) bind right associatively
// step 3: build the ast each node represents an operation or value
// parenthesis dictate precedence

#include "parenthesischeck.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
  char value;
  struct Node *left;
  struct Node *right;
} Node;

int findRootIndex(char *expression) {
  int size = strlen(expression);

  for (int i = size - 1; i >= 0; i--) {
    if (expression[i] == '-' || expression[i] == '+') {
      return i;
    }
  }
  for (int i = size - 1; i >= 0; i--) {
    if (expression[i] == '/' || expression[i] == '*') {
      return i;
    }
  }
  return -1;
}

Node *build(char *expression) {
  int size = strlen(expression);
  int index = findRootIndex(expression);
  Node *curr = malloc(sizeof(Node));
  if (index == -1) {
    // leaf node
    //  curr->value = expression[index]; index is -1 so set it to expression
    //  itself no operator was found. So this is a value
    curr->value = expression[0];
    curr->left = NULL;
    curr->right = NULL;

  } else {
    curr->value = expression[index];
    char *left = malloc(index + 1);
    for (int i = 0; i < index; i++) {
      left[i] = expression[i];
    }
    left[index] = '\0';
    char *right = malloc(size - index);
    int k = 0;
    for (int i = index + 1; i < size; i++) {
      right[k] = expression[i];
      k++;
    }
    right[k] = '\0';
    curr->left = build(left);
    curr->right = build(right);
  }
  return curr;
}

void dotFomat(Node *currNode, FILE *fptr) {

  if (currNode == NULL) {
    return;
  }
  if (currNode->left != NULL) {
    // printf("%d -> %d\n", currNode->value, currNode->left->value);
    fprintf(fptr, "\"%c\" -> \"%c\";\n", currNode->value,
            currNode->left->value);
    dotFomat(currNode->left, fptr);
  }
  if (currNode->right != NULL) {
    // printf("%d -> %d\n", currNode->value, currNode->right->value);
    fprintf(fptr, "\"%c\" -> \"%c\";\n", currNode->value,
            currNode->right->value);
    dotFomat(currNode->right, fptr);
  }
  // fclose(fptr);
  // dotFomat(currNode);
}

void destroy(Node *currNode) {
  if (currNode == NULL) {
    return;
  }

  destroy(currNode->left);
  destroy(currNode->right);
  free(currNode);
}

int main(int argc, char *argv[]) {
  if (argc >= 2) {
    checkExpression(argv[1], strlen(argv[1]));
    int length = strlen(argv[1]);
    char *ptr = argv[1];

    Node *root = build(ptr);
    // create .dot file
    FILE *dotptr;
    dotptr = fopen("file.dot", "w");
    // opening line
    fprintf(dotptr, "digraph BST {\n");
    // write to .dot
    dotFomat(root, dotptr);
    // close bracket
    fprintf(dotptr, "}\n");

    fclose(dotptr);

    // frees the memory
    destroy(root);

  } else {
    printf("Usage: %s <expression>\n", argv[0]);
  }
}
