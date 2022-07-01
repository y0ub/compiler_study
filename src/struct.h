#ifndef COMPILER_SRC_H
#define COMPILER_SRC_H

struct Token
{
  TokenKind kind;  // type of token
  Token     *next; // next token
  int       val;   // number of token when kind is TK_NUM
  char      *str;  // string of token
  int       len;   // length of token
};

struct Node
{
  NodeKind kind; // type of node
  Node *lhs;     // left-hand side
  Node *rhs;     // right-hand size
  int val;       // only kind type is ND_NUM
};

#endif
