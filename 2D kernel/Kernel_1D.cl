//1-D kernel

// ON HOST, set : 
//                  size_t global[1] = {100};

__kernel void abc(__global int input[],__global int res[])
{
		int i=get_global_id(0);				//10 work-items are launched at a time.
		res[i] = 2*input[i];
   
}
