#include "MatrixMultiplicator.hpp"

#include <thread>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdexcept>

using namespace std;

namespace Lab2
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
	{
		lock_guard<mutex> lock(mtx);
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
			{
				lock_guard<mutex> lock(mtx);
				result[i][j] = tmp;
				//cout << "Set " << i << ", " << j << " to: " << descriptor.output[i][j] << endl;
			}
			if (idx++ >= end_idx)
				break;
		}
		j = 0;
	}

	{
		lock_guard<mutex> lock(mtx);
		auto i_end = result.to_i(end_idx), j_end = result.to_j(end_idx);
		cout << "[(" << i_start << ", " << j_start << ") - "
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

	{
		lock_guard<mutex> lock(mtx);
		result.set_size(width_b, height_a);
	}

	cout << "* Starting " << threads << " threads"
			<< " (" << elems_per_thread << " col-row multiplications per thread)" << endl;

	/* Account for integer division */
	auto extra = total_elems - (elems_per_thread * threads);
	if (extra != 0) {
		cout << "  With last thread doing " << extra
				<< " extra multiplication(s)." << endl;
	}

	vector<thread> subtasks;
	subtasks.reserve(threads);
	/* Start segment calculation subtasks */
	for (size_t i = 0, offset, length; i < threads; ++i) {
		length = elems_per_thread + (i == threads - 1 ? extra : 0);
		offset = i * elems_per_thread;
		subtasks.emplace_back(
				&MatrixMultiplicator::calculate_segment, this,
				result.to_i(offset), result.to_j(offset), length
		);
	}

	/* Wait for segment calculation subtasks to finish */
	for (auto &thread : subtasks)
		thread.join();

	cout << "* All subtasks finished calculating" << endl;
}

Matrix<double>& MatrixMultiplicator::multiply(size_t threads)
{
	/* Load matrices asynchronously */
	thread load_thread_a(&MatrixMultiplicator::load_matrix, this, ref(a), path_a);
	thread load_thread_b(&MatrixMultiplicator::load_matrix, this, ref(b), path_b);
	cout << "Loading matrices..." << endl;
	load_thread_a.join();
	load_thread_b.join();
	cout << "Finished loading." << '\n' << endl;

	/* Start main multiplication thread */
	cout << "Starting multiplication using " << threads << " threads..." << endl;
	thread mul_thread(&MatrixMultiplicator::multiply_matrices, this, threads);
	mul_thread.join();
	cout << "Multiplication finished." << endl;
	return result;
}

}
