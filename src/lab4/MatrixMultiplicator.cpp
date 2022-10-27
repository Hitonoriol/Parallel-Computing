#include "MatrixMultiplicator.hpp"

#include <omp.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdexcept>

using namespace std;

namespace Lab4
{

MatrixMultiplicator::MatrixMultiplicator(
		const std::string &path_a,
		const std::string &path_b
		) : path_a(path_a), path_b(path_b)
{
}

void MatrixMultiplicator::load_matrix(Matrix<double> &matrix, const std::string &file_path)
{
	fstream file(file_path);
	if (!file.is_open()) {
		cerr << "Failed to open matrix file: " << file_path << endl;
		exit(-1);
	}
	file >> matrix;
	#pragma omp critical(out)
	{
		cout << "Loaded matrix " << quoted(file_path) << ":" << endl
				<< matrix << endl;
	}
}

void MatrixMultiplicator::calculate_segment(size_t i_start, size_t j_start, size_t length)
{
	auto height_b = b.get_height();
	auto idx = result.to_idx(i_start, j_start);
	auto end_idx = idx + length - 1;
	auto height = result.get_height(), width = result.get_width();

	double tmp;
	size_t j = j_start;
	for (size_t i = i_start; i < height; ++i) {
		for (; j < width; ++j) {
			tmp = 0;
			for (size_t k = 0; k < height_b; k++)
				tmp += a[i][k] * b[k][j];
			#pragma omp critical(out_matrix_write)
			{
				result[i][j] = tmp;
				//cout << "Set " << i << ", " << j << " to: " << descriptor.output[i][j] << endl;
			}
			if (idx++ >= end_idx)
				break;
		}
		j = 0;
	}

	#pragma omp critical(out)
	{
		auto i_end = result.to_i(end_idx), j_end = result.to_j(end_idx);
		cout << "[Thread #" << omp_get_thread_num() << " | " <<
				"(" << i_start << ", " << j_start << ") - "
				"(" << i_end << ", " << j_end << "), "
				<< length << " elements] Done calculating." << endl;
	}
}

void MatrixMultiplicator::multiply_matrices(size_t threads)
{
	if (a.get_width() != b.get_height()) {
		cerr << "Number of columns of the LHS matrix != number of rows of the RHS" << endl;
		pthread_exit(NULL);
	}

	size_t height_a = a.get_height(), width_b = b.get_width();
	size_t total_elems = width_b * height_a;
	size_t elems_per_thread = total_elems / threads;

	#pragma omp critical(out_matrix_write)
	{
		result.set_size(width_b, height_a);
	}

	cout << "[Thread #" << omp_get_thread_num()
			<< "] Scheduling " << threads << " tasks"
			<< " (" << elems_per_thread << " col-row multiplications per task)" << endl;

	/* Account for integer division */
	auto extra = total_elems - (elems_per_thread * threads);
	if (extra != 0) {
		cout << "  With last thread doing " << extra
				<< " extra multiplication(s)." << endl;
	}

	/* Schedule segment calculation subtasks for the current thread team */
	for (size_t i = 0, offset, length; i < threads; ++i)
		#pragma omp task
		{
			length = elems_per_thread + (i == threads - 1 ? extra : 0);
			offset = i * elems_per_thread;
			calculate_segment(result.to_i(offset), result.to_j(offset), length);
		}
}

Matrix<double>& MatrixMultiplicator::multiply(size_t threads)
{
	/* Load matrices in parallel */
	cout << "Loading matrices..." << endl;
	#pragma omp parallel sections
	{
		#pragma omp section
		{
			load_matrix(ref(a), path_a);
		}

		#pragma omp section
		{
			load_matrix(ref(b), path_b);
		}
	}
	cout << "Finished loading." << '\n' << endl;

	cout << "Starting multiplication using " << threads << " threads..." << endl;
	/* Create a team of set number of processes */
	#pragma omp parallel num_threads(threads)
	{
		/* Make one of the threads schedule N matrix segment calculation tasks */
		#pragma omp single nowait
		{
			multiply_matrices(threads);
		}
		/* Implicit barrier here will block main thread
		 *  until all tasks are completed */
	}
	cout << "* All subtasks finished calculating" << endl;
	cout << "Multiplication finished." << endl;
	return result;
}

}
