//2D kernel, Acessing values with Local id and group size.
/*
	size_t global[2]= {10,10};					//2D work dimension,
	size_t local[2]= {2,2};						//total 				

	err = clEnqueueNDRangeKernel(CL.queue,CL.kernel,2,NULL,global,local,NULL,NULL,NULL);

*/

__kernel void abc(__global int input[],__global int res[])
{
		int Gr_id_0 = get_group_id(0);				//10 work-items are launched at a time.
		int Gr_id_1 = get_group_id(1);
		int Gr_size_0 = get_num_groups(0);
		int Gr_size_1 = get_num_groups(1);
    
		int Lid_0 = get_local_id(0);
		int Lid_1 = get_local_id(1);
		int Lsize_0 = get_local_size(0);
		int Lsize_1 = get_local_size(1);

		int group_size = Lsize_0*Lsize_1;
		int local_index = Lid_1 * Lsize_0 + Lid_0;						//Index within a group to access elements
		int dim0_index = Gr_id_0 * group_size;							//Index with in dimension 0 to access respective group
		int dim1_index = Gr_id_1 *Gr_size_0 * group_size;				//Index with in a dimension 1 to access respective new dimesion 0

		int index = dim1_index + dim0_index +local_index;
		res[index] = 2*input[index];
}
