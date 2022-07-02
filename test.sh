#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./bin/main "$input" > tmp.s
  cc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 27 a=20+5-4\;b=2*3\;a+b\;
assert 5 2+3\;

echo OK
