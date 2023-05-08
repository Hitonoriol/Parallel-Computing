#include "Lab7.hpp"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>

using namespace std;

Lab7::Lab7(const std::string &path_a, const std::string &path_b)
{
	ifstream file_a(path_a), file_b(path_b);
	if (!file_a.is_open() || !file_b.is_open()) {
		cerr << "Failed to open matrix files.\n";
		exit(1);
	}

	file_a >> a;
	file_b >> b;
	result.set_size(b.get_width(), a.get_height());

	cout << "Loaded matrices:\n";
	cout << " A (" << quoted(path_a) << "):\n" << a;
	cout<<" B (" << quoted(path_b) << "):\n" << b;
	cout << "Allocated multiplication result matrix A * B = C ["
			<< result.get_height() << "x" << result.get_width() << "]\n\n";
}

cl::Context Lab7::init_cl_context()
{
	cout << "Initializing OpenCL context...\n";
	vector<cl::Platform> all_platforms;
	cl::Platform::get(&all_platforms);
	if (all_platforms.empty()) {
		cerr << "No platforms found. Check OpenCL installation!\n";
		exit(1);
	}

	auto &default_platform = all_platforms.front();
	cout << "Using platform: " << default_platform.getInfo<CL_PLATFORM_NAME>() << "\n";

	default_platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
	if (devices.empty()) {
		cerr << "No devices found. Check OpenCL installation!\n";
		exit(1);
	}

	if (devices.size() > 1)
		devices.erase(next(devices.begin()));

	auto &default_device = devices.front();
	cout << "Using device: " << default_device.getInfo<CL_DEVICE_NAME>() << "\n\n";

	return cl::Context({ default_device });
}

cl::Kernel Lab7::load_kernel(const cl::Context &context, const std::string &name, const std::string &path)
{
	std::fstream src_file(path, std::ios_base::in);
	cl::Program::Sources sources { string(istreambuf_iterator<char> { src_file }, { }) };
	cl::Program program(context, sources);
	if (program.build(devices) != CL_SUCCESS) {
		cerr << "Error building: "
				<< program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices.front()) << "\n";
		exit(1);
	}

	return cl::Kernel(program, name.c_str());
}

void Lab7::run()
{
	auto context = init_cl_context();
	cl::CommandQueue queue(context, devices.front());

	auto size_a = a.size() * sizeof(double);
	auto size_b = b.size() * sizeof(double);
	auto result_size = result.size() * sizeof(double);
	cl_int result_width = result.get_width();
	cl_int width_a = a.get_width();
	cl_int width_b = b.get_width(), height_b = b.get_height();

	cl::Buffer buffer_a(context, CL_MEM_READ_WRITE, size_a);
	cl::Buffer buffer_b(context, CL_MEM_READ_WRITE, size_b);
	cl::Buffer result_buffer(context, CL_MEM_READ_WRITE, result_size);

	queue.enqueueWriteBuffer(buffer_a, CL_TRUE, 0, size_a, a.data());
	queue.enqueueWriteBuffer(buffer_b, CL_TRUE, 0, size_b, b.data());

	cout << "Starting matrix multiplication kernel...\n";
	auto multiplicator = load_kernel(context, "multiply_matrices", "multiply_matrices.cl");
	size_t argn = 0;
	multiplicator.setArg(argn++, buffer_a);
	multiplicator.setArg(argn++, buffer_b);
	multiplicator.setArg(argn++, result_buffer);
	multiplicator.setArg(argn++, result_width);
	multiplicator.setArg(argn++, height_b);
	multiplicator.setArg(argn++, width_a);
	multiplicator.setArg(argn++, width_b);
	queue.enqueueNDRangeKernel(multiplicator, cl::NullRange, cl::NDRange(result_width, result.get_height()));
	queue.finish();
	cout << "Kernel finished executing!\n";

	queue.enqueueReadBuffer(result_buffer, CL_TRUE, 0, result_size, result.data());
	cout << "Multiplication result: \n" << result;
}

int Lab7::run(int argc, char *argv[])
{
	std::string path_a("matrix_a.txt"), path_b("matrix_b.txt");
	if (argc >= 3) {
		path_a = std::string(argv[1]);
		path_b = std::string(argv[2]);
	}

	Lab7(path_a, path_b).run();
	return 0;
}
