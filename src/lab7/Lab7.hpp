#ifndef LAB7_LAB7_HPP_
#define LAB7_LAB7_HPP_

#include <vector>

#define CL_HPP_TARGET_OPENCL_VERSION 300
#include <CL/opencl.hpp>

#include <util/matrix.hpp>

class Lab7
{
	private:
		std::vector<cl::Device> devices;
		Matrix<double> a, b, result;

		cl::Context init_cl_context();
		cl::Kernel load_kernel(const cl::Context&, const std::string &name, const std::string &path);

	public:
		Lab7(const std::string& path_a, const std::string& path_b);
		void run();
		static int run(int argc, char *argv[]);
};

#endif /* LAB7_LAB7_HPP_ */
