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

// простая проверка свертки, должно свернуться в 30
int constant_fold_basic() {
  int x = 10;
  int y = 20;
  int z = x + y;
  return z;
}

// тут цепочка вычислений, все должно уйти в одну константу
int constant_fold_chain() {
  int a = 5;
  int b = a * 3;
  int c = b + 7;
  int d = c - 2;
  return d;
}

// else ветка мертвая, проверяю что пасс ее уберет
int dead_branch_true() {
  int x = 1;
  if (x) {
    return 42;
  } else {
    return 99;
  }
}

int dead_branch_false() {
  int x = 0;
  if (x) {
    return 42;
  } else {
    return 99;
  }
}

// 10 > 20 всегда false, значит true недостижима
int dead_branch_icmp() {
  int a = 10;
  int b = 20;
  if (a > b) {
    return 1;
  } else {
    return 0;
  }
}

// phi от двух одинаковых констант должно свернуться
int phi_both_const_same(int cond) {
  int x;
  if (cond) {
    x = 42;
  } else {
    x = 42;
  }
  return x;
}

// phi от разных констант, значит overdef, сворачивать нельзя
int phi_both_const_diff(int cond) {
  int x;
  if (cond) {
    x = 10;
  } else {
    x = 20;
  }
  return x;
}

// cond=1, else ветка мертвая, phi должна взять 77
int phi_one_dead_branch() {
  int x;
  int cond = 1;
  if (cond) {
    x = 77;
  } else {
    x = 99;
  }
  return x;
}

int nested_branches() {
  int a = 5;
  int b = 10;
  int result;
  if (a < b) {
    if (a + b > 12) {
      result = 1;
    } else {
      result = 2;
    }
  } else {
    result = 3;
  }
  return result;
}

// n - аргумент, поэтому y не свернется (overdef от аргумента)
int overdef_from_arg(int n) {
  int x = 10;
  int y = x + n;
  return y;
}

int mul_by_zero() {
  int a = 999;
  int b = 0;
  return a * b;
}

int complex_chain() {
  int a = 2;
  int b = 3;
  int c = a + b;
  int d = c * 2;
  int e = d - 1;
  int f = e + a;
  return f;
}

// в цикле phi для sum и i, они не должны сворачиваться
int loop_with_const_exit(int n) {
  int sum = 0;
  for (int i = 0; i < n; i++) {
    sum += i;
  }
  return sum;
}

int multi_phi_cascade(int cond1, int cond2) {
  int a;
  if (cond1) {
    a = 10;
  } else {
    a = 20;
  }
  int b;
  if (cond2) {
    b = a + 1;
  } else {
    b = a + 2;
  }
  return b;
}