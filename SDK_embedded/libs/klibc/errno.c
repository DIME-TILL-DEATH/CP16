int errno_val ;



int* __attribute__((used)) __errno(void)
{
  return (int*)&errno_val ;
}



