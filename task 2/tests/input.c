//=============================================================================
// FILE:
//      input.c
//
// DESCRIPTION:
//      Sample input file
//
// License: MIT
//=============================================================================
int
foo(int a) {
  return a * 2;
}

int
bar(int a, int b) {
  return (a + foo(b) * 2);
}

int
fez(int a, int b, int c) {
  return (a + bar(a, b) * 2 + c * 3);
}

int
main(int argc, char *argv[]) {
  int a = 123;
  int ret = 0;

  ret += foo(a);
  ret += bar(a, ret);
  ret += fez(a, ret, 123);

  return ret;
}

int
req1(int a) {
prev:
  a+=1;
  if (a > 0)
    goto next;
  else
    goto next_next;

next:
  a += 2;
  goto prev;

next_next:
  a -= 5;
  goto nextnextnext;

nextnextnext:
  a *= 7;
  if (a > 0)
    goto next_next;
  else
    goto prev;

  return a;
}