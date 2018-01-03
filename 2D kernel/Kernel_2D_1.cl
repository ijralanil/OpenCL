//2D kernel with Global id Access method

/*
  ON HOST, set the folowwing with
size_t global[2] = {10,10};								// Dim(0) -> 10/1 = 10  ,  Dim(1) -> 10/1 = 10 ,      10*10 = 100 work groups
size_t local[2]= {1,1};

err = clEnqueueNDRangeKernel(CL.queue,CL.kernel,2,NULL,global,local,NULL,NULL,NULL);
*/


__kernel void abc(__global int input[],__global int res[])
{
		int i=get_global_id(0);				// Id in 0th dimension
		int j=get_global_id(1);				// Id in 1st dimension
		int size = get_global_size(0);		// Size in 0th dimension

		int index = j*size+i;				//Index calculation based on every ID
		res[index] = 2*input[index];
}
