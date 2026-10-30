// create a lookup for the binary tree that generates a dot file
// with a red colored path to the node-
// representing the binary tree

// take in the file
// read each number
// create the tree nodes
// first number will be the root
// then create the rest by
// determining whether each value is smaller or bigger
// than the previous node.
// then output the tree to the dot format

#include <stdio.h>
#include <stdlib.h>

typedef struct node {
  int value;
  struct node *left;
  struct node *right;
} node;

node *lookup(node *root, int value, FILE *ptr) {
  if (root == NULL) {
    printf("%d is not a file value\n", value);
    return NULL;
  }
  if (root->value == value) {

    printf("success value %d found\n", root->value);
    // fprintf(ptr, "%d", root->value); unnecassary write. Handle only writing
    // edges.
    return root;
  } else if (root->value < value) {
    if (root->right == NULL) {
      printf("%d is not a file value\n", value);
      return NULL;
    }
    node *result = lookup(root->right, value, ptr);
    if (result != NULL) {
      printf("%d -> %d [color=red]\n", root->value, root->right->value);
      fprintf(ptr, "%d -> %d [color=red]\n", root->value, root->right->value);
    }
    return result;
  } else if (root->value > value) {
    if (root->left == NULL) {
      printf("%d is not a file value\n", value);
      return NULL;
    }
    node *result = lookup(root->left, value, ptr);

    if (result != NULL) {
      printf("%d -> %d [color=red]\n", root->value, root->left->value);
      fprintf(ptr, "%d -> %d [color=red]\n", root->value, root->left->value);
    }
    return result;
  }
  return NULL;
}

node *insert(node *root, int value) {
  // if node is NULLLL ceate node return node
  if (root == NULL) {
    node *currNode = malloc(sizeof(node));
    currNode->value = value;
    currNode->left = NULL;
    currNode->right = NULL;
    return currNode;
  }
  // if value < node->value (go left)
  else if (value < root->value) {
    root->left = insert(root->left, value);
    // root->left->value = value;
  }
  // else if value > node->value (go right)
  else if (value > root->value) {
    root->right = insert(root->right, value);
    // root->right->value = value;
  }
  return root;
  // return node
}

void dotFomat(node *currNode, FILE *fptr) {

  if (currNode == NULL) {
    return;
  }
  if (currNode->left != NULL) {
    // printf("%d -> %d\n", currNode->value, currNode->left->value);
    fprintf(fptr, "%d -> %d;\n", currNode->value, currNode->left->value);
    dotFomat(currNode->left, fptr);
  }
  if (currNode->right != NULL) {
    // printf("%d -> %d\n", currNode->value, currNode->right->value);
    fprintf(fptr, "%d -> %d;\n", currNode->value, currNode->right->value);
    dotFomat(currNode->right, fptr);
  }
  // fclose(fptr);
  // dotFomat(currNode);
}

void printTree(node *currNode) {
  if (currNode == NULL) {
    return;
  } else {
    printf("%d\n", currNode->value);
    printTree(currNode->left);
    printTree(currNode->right);
  }
}

void destroy(node *currNode) {
  if (currNode == NULL) {
    return;
  }

  destroy(currNode->left);
  destroy(currNode->right);
  free(currNode);
}

void tree(char *filename, int lookupValue) {
  // open file
  FILE *fptr = fopen(filename, "r");
  if (fptr == NULL) {
    printf("Could not open file\n");
    return;
  }

  int value;
  node *root = NULL;
  // parses through formatted data
  while (fscanf(fptr, "%d", &value) == 1) {
    // creates the tree through recursion
    root = insert(root, value);
  }
  fclose(fptr);
  // prints the order
  // printTree(root);
  // create .dot file
  FILE *dotptr;
  dotptr = fopen("file.dot", "w");
  // opening line
  fprintf(dotptr, "digraph BST {\n");
  // write to .dot
  dotFomat(root, dotptr);

  lookup(root, lookupValue, dotptr);
  // close bracket
  fprintf(dotptr, "}\n");

  fclose(dotptr);

  // frees the memory
  destroy(root);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Usage: %s <file> <lookup value>\n", argv[0]);
  }
  tree(argv[1], atoi(argv[2]));

  return 0;
}
