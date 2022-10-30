#include "MatrixMultiplicator.hpp"

#include <omp.h>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdexcept>

#include <util/time_utils.hpp>

using namespace std;

namespace Lab5
{

MatrixMultiplicator::MatrixMultiplicator(
		const std::string &path_a,
		const std::string &path_b
		) : path_a(path_a), path_b(path_b)
{
}

MatrixMultiplicator::MatrixMultiplicator(Matrix<double> &a, Matrix<double> &b)
	: a(a), b(b)
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
		cout << "Loaded matrix " << quoted(file_path);
		if (matrix.size() <= 10000) {
			cout << ":" << endl
							<< matrix;
		} else
			cout << " (too large to display)";
		cout << endl;
	}
}

void MatrixMultiplicator::multiply_matrices(size_t threads)
{
	auto start = chrono::high_resolution_clock::now();
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
			<< "] Starting " << threads << " threads"
			<< " (" << elems_per_thread << " col-row multiplications per thread)" << endl;

	/* Account for integer division */
	auto extra = total_elems - (elems_per_thread * threads);
	if (extra != 0) {
		cout << "  With last thread doing " << extra
				<< " extra multiplication(s)." << endl;
	}

	/* Calculate output matrix segments in parallel */
	#pragma omp parallel for num_threads(threads)
	for (size_t i = 0; i < threads; ++i) {
		size_t length = elems_per_thread + (i == threads - 1 ? extra : 0);
		size_t offset = i * elems_per_thread;
		size_t i_start = result.to_i(offset);
		size_t j_start = result.to_j(offset);
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
	cout << "All calculations finished (" << elapsed_time_str(start) << " elapsed)" << endl;
}

Matrix<double>& MatrixMultiplicator::multiply(size_t threads)
{
	/* Load matrices in parallel */
	if (!path_a.empty() && !path_b.empty()) {
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
	}

	cout << "Starting multiplication using " << threads << " threads..." << endl;
	multiply_matrices(threads);
	cout << "Multiplication finished." << endl;
	return result;
}

}
