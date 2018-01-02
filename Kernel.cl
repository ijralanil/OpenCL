//#define VALUE 1
__kernel void abc(__global int input[],__global int res[])
{
		int i=0;
		for(i=0;i<9;i++)
		{
			res[i] = 2*input[i];
			//printf("%d %d\n",input[i],res[i]);
		}
		res[i]=VALUE;
}
