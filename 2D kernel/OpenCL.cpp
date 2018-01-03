#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <fstream>
#include<CL/cl.h>
#define KERNEL_FILE "Kernel.cl"
#define SETUP_INFO 0
#define BUF_SIZE 100
struct OpenCL
{
	cl_platform_id platform[2];
	cl_device_id device;
	cl_context context;
	cl_command_queue queue;
	cl_program program;
	cl_kernel kernel;

};	

struct Buffers
{
		cl_mem input_buffer,Output_buffer;

};


int arr2[BUF_SIZE];
void find_error(int err);
void file_read(char **str,size_t &size);
void Setup_buffers(struct OpenCL *CL,struct Buffers *BUF);
void Kernel_setup(struct OpenCL *CL,char* kernel_name,const char * Kbuild_options);
void Platform_setup(struct OpenCL *CL);


#if SETUP_INFO
void Print_Platform_info(cl_platform_id platform[], cl_uint num_plat);
void Print_Device_info(cl_device_id *device);
void Print_context_info(cl_context *context);
void Print_kernel_info(cl_kernel *kernel);
#endif

void Print_build_log(cl_program *program,cl_device_id *device);

using namespace std;

int main()
{

	struct OpenCL CL;
	struct Buffers BUF;
	cl_int err;
	int arr1[BUF_SIZE]={0};														//Output array
	int arr[BUF_SIZE]={1,2,3,4,5,6,7,8,9,10,
						11,12,13,14,15,16,17,18,19,20,
						21,22,23,24,25,26,27,28,29,30,
						31,32,33,34,35,36,37,38,39,40,
						41,42,43,44,45,46,47,48,49,50,
						51,52,53,54,55,56,57,58,59,60,
						61,62,63,64,65,66,67,68,69,70,
						71,72,73,74,75,76,77,78,79,80,
						81,82,83,84,85,86,87,88,89,90,
						91,92,93,94,95,96,97,98,99,100};


	//Setting up the device to start launching kernel

	const char options[] = "-D VALUE=100";														//Defining VALUE AS a MACRO for kernel
	Platform_setup(&CL);														//Setup platform, select device and create context
	Kernel_setup(&CL,"abc",options);						//setup program,kernel and command queue



	//Read only Buffer, Input to Kernel
	// No need to write data to kernel buffer coz - "CL_MEM_USE_HOST_PTR"

	BUF.input_buffer = clCreateBuffer(CL.context,CL_MEM_READ_ONLY|CL_MEM_USE_HOST_PTR,sizeof(int)*BUF_SIZE,arr,&err);
	if(err!=CL_SUCCESS)
	{
		cout<<"Could not create input Buffer : ";find_error(err);Sleep(5000);exit(1);
	}

	//Write only Buffer, Output from Kernel

	BUF.Output_buffer = clCreateBuffer(CL.context,CL_MEM_READ_WRITE,sizeof(int)*BUF_SIZE,NULL,&err);
	if(err!=CL_SUCCESS)
	{
		cout<<"Could not create output Buffer : ";find_error(err);Sleep(5000);exit(1);
	}

	
	err = clSetKernelArg(CL.kernel, 0 ,sizeof(cl_mem),&(BUF.input_buffer));
	if(err!=CL_SUCCESS)
	{
		cout<<"Could not set argument 0 : ";find_error(err);Sleep(5000);exit(1);
	}

	err = clSetKernelArg(CL.kernel, 1 ,sizeof(cl_mem),&(BUF.Output_buffer));
	if(err!=CL_SUCCESS)
	{
		cout<<"Could not set argument 0 : ";find_error(err);Sleep(5000);exit(1);
	}


	size_t global[2]= {100};					//1D work dimension,
	//size_t local[2]= {2,2};						//total 				

	err = clEnqueueNDRangeKernel(CL.queue,CL.kernel,2,NULL,global,local,NULL,NULL,NULL);
	if(err!=CL_SUCCESS)
	{
		cout<<"could not set kernel argument : ";find_error(err);Sleep(5000);return 1;
	}


	void *ptr;
	ptr = clEnqueueMapBuffer(CL.queue,BUF.Output_buffer,TRUE,CL_MAP_READ,0,sizeof(int)*BUF_SIZE,0,NULL,NULL,&err);
	if(err!=CL_SUCCESS)
	{
		cout<<"Could not read the output buffer : ";find_error(err);Sleep(5000);
	}
	
	memcpy(arr1,ptr,sizeof(int)*BUF_SIZE);
	Sleep(1000);
	for(int i=0;i<BUF_SIZE;i++)
		cout<<arr1[i]<<" ";
	cout<<endl;

	getchar();
	clReleaseMemObject(BUF.input_buffer);
	clReleaseMemObject(BUF.Output_buffer);
	clReleaseCommandQueue(CL.queue);
	clReleaseKernel(CL.kernel);
	clReleaseProgram(CL.program);
	clReleaseContext(CL.context);
	clReleaseDevice(CL.device);
	return 0;
}


void Platform_setup(struct OpenCL *CL)
{
	cl_int err;
	cl_uint num_plat,num_device;

	err = clGetPlatformIDs(NULL,NULL,&num_plat);
	if(err!=CL_SUCCESS)
	{
		printf("Can't get Platform id : ");find_error(err);Sleep(5000);exit(1);
	}

	cout<<"No.of Plaform : "<<num_plat<<endl<<endl;

	err = clGetPlatformIDs(num_plat,CL->platform,NULL); 
	if(err!=CL_SUCCESS)
	{
		printf("Can't get Platform ids : ");find_error(err);Sleep(5000);exit(1);
	}

#if SETUP_INFO
	Print_Platform_info(CL->platform,num_plat);	//Print Platform information
#endif

	//Get a GPU device
	err = clGetDeviceIDs(CL->platform[1],CL_DEVICE_TYPE_GPU,1,&CL->device,&num_device);
	if(err!=CL_SUCCESS)
	{
		cout<<"Can't get Device id for GPU devices : ";find_error(err);Sleep(5000);exit(1);
	}

	cout<<"No. of GPU device :"<<num_device<<endl;

#if SETUP_INFO
	Print_Device_info(&CL->device);
#endif

	CL->context = clCreateContext(NULL,1,&CL->device,NULL,NULL,&err);
	if(err!=CL_SUCCESS)
	{
		cout<<"Could not create context : ";find_error(err);Sleep(5000);exit(1);
	}

#if SETUP_INFO
	Print_context_info(&CL->context);
#endif

}

void Kernel_setup(struct OpenCL *CL,char* kernel_name,const char * Kbuild_options)
{
	cl_int err;
	char *src_str;
	size_t size;
	
	file_read(&src_str,size);
	CL->program = clCreateProgramWithSource(CL->context,1,(const char**)&src_str,&size,&err);
	if(err!=CL_SUCCESS)
	{
			cout<<"Could not create program : ";find_error(err);Sleep(5000);exit(1);
	}


	err = clBuildProgram(CL->program,1,&CL->device,Kbuild_options,NULL,NULL);
	if(err!=CL_SUCCESS)
	{
		Print_build_log(&CL->program,&CL->device);Sleep(8000);Sleep(5000);exit(1);
	}

	cout<<"build Successful"<<endl;
	
	CL->kernel = clCreateKernel(CL->program,kernel_name,&err);
	if(err!=CL_SUCCESS)
	{
		cout<<"Could not create kernel : ";find_error(err);Sleep(5000);exit(1);
	}

#if SETUP_INFO
	Print_kernel_info(&CL->kernel);
#endif

	char* queue_properties ="CL_QUEUE_PROFILING_ENABLE";
	CL->queue = clCreateCommandQueue(CL->context,CL->device,NULL,&err);
	if(err!=CL_SUCCESS)
	{
		cout<<"Could not create Command Queue";find_error(err);Sleep(5000);exit(1);
	}



}

#if SETUP_INFO
	//Get Platform info//
	// CL_PLATFORM_NAME , CL_PLATFORM_VENDOR, CL_PLATFORM_VERSION, CL_PLATFORM_PROFILE
void Print_Platform_info(cl_platform_id platform[], cl_uint num_plat)
{

	cl_uint i;
	cl_int err;
	size_t size;
	char *P_Name, *P_vendor, *P_version;

	for(i = 0;i<num_plat;i++)
	{

		err = clGetPlatformInfo(platform[i],CL_PLATFORM_NAME,NULL,NULL,&size);	//get the size of resulting string
		if(err)
		{
			cout<<"Failed to get CL_PLATFORM_NAME : ";find_error(err); return;
		}

		P_Name = (char*) malloc(size);
		err = clGetPlatformInfo(platform[i],CL_PLATFORM_NAME,size,P_Name,NULL);
		if(err)
		{
			cout<<"Failed to get CL_PLATFORM_NAME : ";find_error(err); return;
		}


		err = clGetPlatformInfo(platform[i],CL_PLATFORM_VENDOR,NULL,NULL,&size);
		if(err)
		{
			cout<<"Failed to get CL_PLATFORM_NAME : ";find_error(err); return;
		}

		P_vendor = (char*) malloc(size);
		err = clGetPlatformInfo(platform[i],CL_PLATFORM_VENDOR,size,P_vendor,NULL);
		if(err)
		{
			cout<<"Failed to get CL_PLATFORM_NAME : ";find_error(err); return;
		}


		err = clGetPlatformInfo(platform[i],CL_PLATFORM_VERSION,NULL,NULL,&size);
		if(err)
		{
			cout<<"Failed to get CL_PLATFORM_NAME : ";find_error(err); return;
		}

		P_version = (char*) malloc(size);
		err = clGetPlatformInfo(platform[i],CL_PLATFORM_VERSION,size,P_version,NULL);
		if(err)
		{
			cout<<"Failed to get CL_PLATFORM_NAME : ";find_error(err); return;
		}

		cout<<"Platform Name "<< i+1 <<" : "<<P_Name<<endl;
		cout<<"Vendor Name "<< i+1 <<" : "<<P_vendor<<endl;
		cout<<"OpenCL Version  "<< i+1 <<" : "<<P_version<<endl<<endl;


	}

}

void Print_Device_info(cl_device_id *device)
{

	cl_int err;
	size_t size;
	char *ptr;

	err = clGetDeviceInfo(device[0],CL_DEVICE_NAME,NULL,NULL,&size);
	if(err<0){
		cout<<"Could not get CL_DEVICE_NAME : ";find_error(err); return;
	}

	ptr = (char*)malloc(size);
	err = clGetDeviceInfo(device[0],CL_DEVICE_NAME,size,ptr,NULL);
	if(err<0){
		cout<<"Could not get CL_DEVICE_NAME : ";find_error(err); return;
	}

	cout<<"Device Name: "<<ptr<<endl;
	free(ptr);

	err = clGetDeviceInfo(device[0],CL_DEVICE_VENDOR,NULL,NULL,&size);
	if(err<0){
		cout<<"Could not get CL_DEVICE_NAME : ";find_error(err); return;
	}

	ptr = (char*)malloc(size);
	err = clGetDeviceInfo(device[0],CL_DEVICE_VENDOR,size,ptr,NULL);
	if(err<0){
		cout<<"Could not get CL_DEVICE_NAME : ";find_error(err); return;
	}

	cout<<"Device Vendor: "<<ptr<<endl<<endl;
	free(ptr);





}

void Print_context_info(cl_context *context)
{
	cl_int err;
	cl_uint dev;
	err = clGetContextInfo(context[0], CL_CONTEXT_NUM_DEVICES,sizeof(cl_uint),&dev,NULL);
	if(err<0)
	{
			cout<<"could not get num_device info from context : ";find_error(err); return;
	}

	cout<<"No. of device in context : "<<dev<<endl;

	err=clGetContextInfo(context[0],CL_CONTEXT_REFERENCE_COUNT,sizeof(cl_int),&dev,NULL);
	if(err<0)
	{
			cout<<"could not get refernce count info from context : ";find_error(err); return;
	}

		cout<<"No. of reference count : "<<dev<<endl;



}

void Print_kernel_info(cl_kernel *kernel)
{
	cl_int err;
	size_t size;
	cl_uint num_arg_kernel;
	char * name;
	err = clGetKernelInfo(kernel[0],CL_KERNEL_FUNCTION_NAME,NULL,NULL,&size);
	if(err!=CL_SUCCESS)
	{
		cout<<"could not get function name info : ";find_error(err);Sleep(2000); return;
	}
	
	name = new char[size+1];
	err = clGetKernelInfo(kernel[0],CL_KERNEL_FUNCTION_NAME,size+1,name,NULL);
	if(err!=CL_SUCCESS) 
	{
		cout<<"could not get function name info : ";find_error(err);Sleep(2000); return;
	}

	cout<<"kernel name "<<name<<endl;

	err = clGetKernelInfo(kernel[0],CL_KERNEL_NUM_ARGS,sizeof(cl_uint),&num_arg_kernel,NULL);
	if(err!=CL_SUCCESS) 
	{
		cout<<"could not get kernel arg info : ";find_error(err);Sleep(2000); return;
	}
	cout<<"Total kernel arg : "<<num_arg_kernel<<endl;


}


#endif

void file_read(char **str,size_t &size)
{
	int offset;
	char ch;
	ifstream file;
	file.open(KERNEL_FILE,ios::in);
	if(!file)
	{
		cout<<"could not open file for reading"<<endl; return;
	}

	file.seekg(0,ios::end);
	size  = (size_t)file.tellg();
	//cout<<size<<endl;
	file.seekg(0,ios::beg);
	*str = new char[size+1];
	file.read(*str,size);
	(*str)[size+1]=0;

	offset = size;
	do{
		ch = (*str)[--offset];
		//cout<<ch<<endl;
	//	getchar();
	}
	while(ch!= '}');
	//cout<<size-offset<<endl;
	(*str)[size - (size-offset)+1]=0;
	file.close();
	}

void Print_build_log(cl_program *program,cl_device_id *device)
{
	size_t size;
	cl_int err;
	err = clGetProgramBuildInfo(program[0],device[0],CL_PROGRAM_BUILD_LOG,NULL,NULL,&size);
	if(err!=CL_SUCCESS)
	{
		cout<<"could not get build info"<<endl;//exit(1);
	}
	cout<<size<<endl;
	char *LOG = new char[size+1];
	err = clGetProgramBuildInfo(program[0],device[0],CL_PROGRAM_BUILD_LOG,size+1,LOG,NULL);
	if(err!=CL_SUCCESS)
	{
		cout<<"could not get build info"<<endl;//exit(1);
	}

	cout<<LOG<<endl;
}

void find_error(int err)
{
	switch(err)
	{
	case -1:
		cout<<"CL_DEVICE_NOT_FOUND"<<endl;		//-1
	break;
	case -2:
		cout<<"CL_DEVICE_NOT_AVAILABLE"<<endl;            // -2
	break;
	case -3:
		cout<<"CL_COMPILER_NOT_AVAILABLE"<<endl;         //     -3
	break;
	case -4:
		cout<<"CL_MEM_OBJECT_ALLOCATION_FAILURE"<<endl;     // -4
	break;
	case -5:
		cout<<"CL_OUT_OF_RESOURCES"<<endl;             //   -5	
	break;
	case -6:
		cout<<"CL_OUT_OF_HOST_MEMORY"<<endl;        //       -6	
	break;
	case -7:
		cout<<"CL_PROFILING_INFO_NOT_AVAILABLE"<<endl;  //          -7
	break;
	case -8:
		cout<<"CL_MEM_COPY_OVERLAP"<<endl;             // -8
	break;
	case -9:
		cout<<"CL_IMAGE_FORMAT_MISMATCH"<<endl;          //     -9
	break;
	case -10:
		cout<<"CL_IMAGE_FORMAT_NOT_SUPPORTED"<<endl;       //      -10
	break;
	case -11:
		cout<<"CL_BUILD_PROGRAM_FAILURE"<<endl;            //   -11
	break;
	case -12:
		cout<<"CL_MAP_FAILURE"<<endl;            //  -12
	break;
	case -13:
		cout<<"CL_MISALIGNED_SUB_BUFFER_OFFSET"<<endl;       //    -13
	break;
	case -14:
		cout<<"CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST"<<endl; // -14
	break;
	case -15:
		cout<<"CL_COMPILE_PROGRAM_FAILURE"<<endl;                //  -15
	break;
	case -16:
		cout<<"CL_LINKER_NOT_AVAILABLE"<<endl;                   //  -16
	break;
	case -17:
		cout<<"CL_LINK_PROGRAM_FAILURE"<<endl;                   //  -17
	break;
	case -18:
		cout<<"CL_DEVICE_PARTITION_FAILED"<<endl;                //  -18
	break;
	case -19:
		cout<<"CL_KERNEL_ARG_INFO_NOT_AVAILABLE"<<endl;          //  -19
	break;
	case -30:
		cout<<"CL_INVALID_VALUE"<<endl;                          //  -30
	break;
	case -31:
		cout<<"CL_INVALID_DEVICE_TYPE"<<endl;                    //  -31
	break;
	case -32:
		cout<<"CL_INVALID_PLATFORM"<<endl;                       //  -32
	break;
	case -33:
		cout<<"CL_INVALID_DEVICE"<<endl;                         //  -33
	break;
	case -34:
		cout<<"CL_INVALID_CONTEXT"<<endl;                        //  -34
	break;
	case -35:
		cout<<"CL_INVALID_QUEUE_PROPERTIES"<<endl;              //   -35
	break;
	case -36:
		cout<<"CL_INVALID_COMMAND_QUEUE"<<endl;                  //  -36
	break;
	case -37:
		cout<<"CL_INVALID_HOST_PTR"<<endl;                       //  -37
	break;
	case -38:
		cout<<"CL_INVALID_MEM_OBJECT"<<endl;                     //  -38
	break;
	case -39:
		cout<<"CL_INVALID_IMAGE_FORMAT_DESCRIPTOR"<<endl;        //  -39
	break;
	case -40:
		cout<<"CL_INVALID_IMAGE_SIZE"<<endl;                     //  -40
	break;
	case -41:
		cout<<"CL_INVALID_SAMPLER"<<endl;                        //  -41
	break;
	case -42:
		cout<<"CL_INVALID_BINARY"<<endl;                         //  -42
	break;
	case -43:
		cout<<"CL_INVALID_BUILD_OPTIONS"<<endl;                  //  -43
	break;
	case -44:
		cout<<"CL_INVALID_PROGRAM"<<endl;                        //  -44
	break;
	case -45:
		cout<<"CL_INVALID_PROGRAM_EXECUTABLE"<<endl;             //  -45
	break;
	case -46:
		cout<<"CL_INVALID_KERNEL_NAME"<<endl;                    //  -46
	break;
	case -47:
		cout<<"CL_INVALID_KERNEL_DEFINITION"<<endl;             //   -47
	break;
	case -48:
		cout<<"CL_INVALID_KERNEL"<<endl;                         //  -48
	break;
	case -49:
		cout<<"CL_INVALID_ARG_INDEX"<<endl;                      //  -49
	break;
	case -50:
		cout<<"CL_INVALID_ARG_VALUE"<<endl;                      //  -50
	break;
	case -51:
		cout<<"CL_INVALID_ARG_SIZE"<<endl;                       //  -51
	break;
	case -52:
		cout<<"CL_INVALID_KERNEL_ARGS"<<endl;                   //   -52
	break;
	case -53:
		cout<<"CL_INVALID_WORK_DIMENSION"<<endl;                //   -53
	break;
	case -54:
		cout<<"CL_INVALID_WORK_GROUP_SIZE"<<endl;               //   -54
	break;
	case -55:
		cout<<"CL_INVALID_WORK_ITEM_SIZE"<<endl;                //   -55
	break;
	case -56:
		cout<<"CL_INVALID_GLOBAL_OFFSET"<<endl;                  //  -56
	break;
	case -57:
		cout<<"CL_INVALID_EVENT_WAIT_LIST"<<endl;                //  -57
	break;
	case -58:
		cout<<"CL_INVALID_EVENT"<<endl;                          //  -58
	break;
	case -59:
		cout<<"CL_INVALID_OPERATION"<<endl;                      //  -59
	break;
	case -60:
		cout<<"CL_INVALID_GL_OBJECT"<<endl;                      //  -60
	break;
	case -61:
		cout<<"CL_INVALID_BUFFER_SIZE"<<endl;                    //  -61
	break;
	case -62:
		cout<<"CL_INVALID_MIP_LEVEL"<<endl;                      //  -62
	break;
	case -63:
		cout<<"CL_INVALID_GLOBAL_WORK_SIZE"<<endl;               //  -63
	break;
	case -64:
		cout<<"CL_INVALID_PROPERTY"<<endl;                       //  -64
	break;
	case -65:
		cout<<"CL_INVALID_IMAGE_DESCRIPTOR"<<endl;               //  -65
	break;
	case -66:
		cout<<"CL_INVALID_COMPILER_OPTIONS"<<endl;               //  -66
	break;
	case -67:
		cout<<"CL_INVALID_LINKER_OPTIONS"<<endl;                 //  -67
	break;
	case -68:
		cout<<"CL_INVALID_DEVICE_PARTITION_COUNT"<<endl;           //-68
	break;
default:
	cout<<"Unknown error"<<endl;
	break;
	}
}
