int minitest(int b){
  b=b+1;
  return b;
}

void test() {
	int a, b, c, e;
	e = b + c;
	if (e > 0) {
		minitest(e);
	} 
	a = e + c;
}
