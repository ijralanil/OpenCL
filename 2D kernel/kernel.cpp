//Simple kernel
__kernel void abc(__global int input[],__global int res[])
{
  int i;
  for(i=0;i<100;i++)
    res[i] = 2 * input[i];

}
