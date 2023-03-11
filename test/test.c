int minitest(int b){
  b=b+1;
  return b;
}


int test(int a, int b)
{
  int c = a + b;
  int d = b + c;
  int e = a + b;
  int f = b + e;
  
  if(a>0){
  minitest(a);
  } 
  
  a=1;
  return f;
  
}
