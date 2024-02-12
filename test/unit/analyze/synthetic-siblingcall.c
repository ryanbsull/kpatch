int bar(int a, int b);
int bar(int a, int b) {
	return a - b;
}

int foo(int a, int b);
int foo(int a, int b) {
	a =+ b;
	return bar(b, a);
}
