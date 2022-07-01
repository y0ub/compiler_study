#include "compiler.h"

// indicate now token
Token *token;

// input program
char *user_input;

/*
 * make node of operation
 */
Node *new_node_operation(NodeKind kind, Node *lhs, Node *rhs)
{
  Node *node = calloc(1, sizeof(Node));
  node -> kind = kind;
  node -> lhs  = lhs;
  node -> rhs  = rhs;
  return node;
}

/*
 * make node of number
 */
Node *new_node_number(int val)
{
  Node *node = calloc(1, sizeof(Node));
  node -> kind = ND_NUM;
  node -> val  = val;
  return node;
}

/*
 * what: determin if it is a reserved token
 */
bool consume(char *op)
{
  if (token -> kind != TK_RESERVED
      || strlen(op) != token -> len
      || memcmp(token -> str, op, token -> len))
    return false;
  token = token -> next;
  return true;
}

void expect(char *op)
{
  if (token -> kind != TK_RESERVED
      || strlen(op) != token -> len
      || memcmp(token -> str, op, token -> len))
    error_at(token -> str, "not '%s'", op);
  token = token -> next;
}

int expect_number()
{
  if (token -> kind != TK_NUM)
    error_at(token -> str, "not a number");
  int val = token -> val;
  token = token -> next;
  return val;
}

bool at_eof()
{
  return token -> kind == TK_EOF;
}

Token *new_token(TokenKind kind, Token *cur, char *str, int len)
{
  Token *tok  = calloc(1, sizeof(Token));
  tok -> kind = kind;
  tok -> str  = str;
  tok -> len  = len;
  cur -> next = tok;
  return tok;
}

bool start_swith(char *p, char *q)
{
  return memcmp(p, q, strlen(q)) == 0;
}

bool is_tk_reserved(char *p)
{
  return *p == '+' || *p == '-'
    || *p == '*' || *p == '/'
    || *p == '(' || *p == ')'
    || *p == '<' || *p == '>';
}

bool is_two_char_operation(char *p)
{
  return start_swith(p, "==")
    || start_swith(p, "!=")
    || start_swith(p, "<=")
    || start_swith(p, ">=");
}

Token *tokenize()
{
  char *p = user_input;
  Token head;
  head.next  = NULL;
  Token *cur = &head;

  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (is_two_char_operation(p)) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }

    if (is_tk_reserved(p)) {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    if (isdigit(*p)) {
      cur        = new_token(TK_NUM, cur, p, 0);
      char *q = p;
      cur -> val = strtol(p, &p, 10);
      cur -> len = p - q;
      continue;
    }

    error_at(token -> str, "cannot tokenize");
  }

  new_token(TK_EOF, cur, p, 0);
  return head.next;
}

// the head of the definition of the EBNF

/*
 * expr = equality
 */
Node *expr()
{
  return equality();
}

/*
 * equality = relational ("==" | "!=" relational)*
 */
Node *equality()
{
  Node *node = relational();

  for (;;) {
    if (consume("=="))
      node = new_node_operation(ND_EQ, node, relational());
    else if (consume("!="))
      node = new_node_operation(ND_NE, node, relational());
    else
      return node;
  }
}

/*
 * relational = add ("<" add | "<=" add | ">" add | ">=" add)*
 */
Node *relational()
{
  Node *node = add();

  for (;;) {
    if (consume("<"))
      node = new_node_operation(ND_LT, node, add());
    else if (consume("<="))
      node = new_node_operation(ND_LE, node, add());
    else if (consume(">"))
      node = new_node_operation(ND_LT, add(), node);
    else if (consume(">="))
      node = new_node_operation(ND_LE, add(), node);
    else
      return node;
  }
}
/*
 * add = mul ("+" mul | "-" mul)*
 */
Node *add()
{
  Node *node = mul();

  for (;;) {
    if (consume("+"))
      node = new_node_operation(ND_ADD, node, mul());
    else if (consume("-"))
      node = new_node_operation(ND_SUB, node, mul());
    else
      return node;
  }
}

/*
 * mul     = unary ("*" unary | "/" unary)*
 */
Node *mul()
{
  Node *node = unary();

  for (;;) {
    if (consume("*"))
      node = new_node_operation(ND_MUL, node, unary());
    else if (consume("/"))
      node = new_node_operation(ND_DIV, node, unary());
    else
      return node;
  }
}

/*
 * unary   = ("+" | "-")? primary
 */
Node *unary()
{
  if (consume("+"))
    return primary();
  if (consume("-"))
    // replace "-x" to "0-x"
    return new_node_operation(ND_SUB, new_node_number(0), primary());
  return primary();
}

/*
 * primary = num | "(" expr ")"
 */ 
Node *primary()
{
  // expect ()
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  // expect number
  return new_node_number(expect_number());
}

// the end of the definition of the EBNF