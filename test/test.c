int minitest(int a){
  a=a+1;
  return a;
}


int test(int a, int b)
{
  int c = a + b;
  int d = b + c;
  int e = a + b;
  int f = b + e;
  if(1){
  minitest(a);
  } 
  c=a;
  return f;
  
}
