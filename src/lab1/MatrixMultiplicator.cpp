#include "MatrixMultiplicator.hpp"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdexcept>

#include <util/pthread_wrapper.hpp>

using namespace std;

MatrixMultiplicator::MatrixMultiplicator(
		const std::string &path_a,
		const std::string &path_b
		) : path_a(path_a), path_b(path_b)
{
	if (pthread_mutex_init(&lock, NULL)) {
		throw runtime_error("Failed to initialize mutex for matrix multiplicator");
	}
}

MatrixMultiplicator::~MatrixMultiplicator()
{
	pthread_mutex_destroy(&lock);
}

void* MatrixMultiplicator::load_matrix(void *descriptor_ptr)
{
	auto &descriptor = *reinterpret_cast<MatrixDescriptor*>(descriptor_ptr);
	fstream file(descriptor.filename);
	if (!file.is_open()) {
		cerr << "Failed to open matrix file: " << descriptor.filename << endl;
		pthread_exit(NULL);
	}
	file >> descriptor.matrix;
	pthread_mutex_lock(&descriptor.lock);
	cout << "Loaded matrix " << quoted(descriptor.filename) << ":" << endl
			<< descriptor.matrix << endl;
	pthread_mutex_unlock(&descriptor.lock);
	pthread_exit(NULL);
	return NULL;
}

void* MatrixMultiplicator::calculate_segment(void *descriptor_ptr)
{
	auto &descriptor = *reinterpret_cast<MultiplicationSegmentDescriptor*>(descriptor_ptr);
	auto height_b = descriptor.b.get_height();
	auto idx = descriptor.output.to_idx(descriptor.i_start, descriptor.j_start);
	auto end_idx = idx + descriptor.length - 1;
	auto height = descriptor.output.get_height(), width = descriptor.output.get_width();

	double tmp;
	size_t j = descriptor.j_start;
	for (size_t i = descriptor.i_start; i < height; ++i) {
		for (; j < width; ++j) {
			tmp = 0;
			for (size_t k = 0; k < height_b; k++)
				tmp += descriptor.a[i][k] * descriptor.b[k][j];
			pthread_mutex_lock(&descriptor.lock);
			descriptor.output[i][j] = tmp;
			//cout << "Set " << i << ", " << j << " to: " << descriptor.output[i][j] << endl;
			pthread_mutex_unlock(&descriptor.lock);
			if (idx++ >= end_idx)
				break;
		}
		j = 0;
	}

	auto i_end = descriptor.output.to_i(end_idx), j_end = descriptor.output.to_j(end_idx);
	pthread_mutex_lock(&descriptor.lock);
	cout << "[(" << descriptor.i_start << ", " << descriptor.j_start << ") - "
			"(" << i_end << ", " << j_end << ")] Done calculating." << endl;
	pthread_mutex_unlock(&descriptor.lock);
	pthread_exit(NULL);
	return NULL;
}

void* MatrixMultiplicator::multiply_matrices(void *descriptor_ptr)
{
	auto &descriptor = *reinterpret_cast<MultiplicationDescriptor*>(descriptor_ptr);

	if (descriptor.a.get_width() != descriptor.b.get_height()) {
		cerr << "Number of columns of the LHS matrix != number of rows of the RHS" << endl;
		pthread_exit(NULL);
	}

	size_t height_a = descriptor.a.get_height(), width_b = descriptor.b.get_width();

	size_t total_elems = width_b * height_a;
	size_t elems_per_thread = total_elems / descriptor.threads;

	pthread_mutex_lock(&descriptor.lock);
	descriptor.output.set_size(width_b, height_a);
	pthread_mutex_unlock(&descriptor.lock);

	/* Prepare matrix segment descriptors for each thread */
	vector<MultiplicationSegmentDescriptor> segments;
	for (size_t i = 0, offset; i < descriptor.threads; ++i) {
		MultiplicationSegmentDescriptor segment(descriptor);
		offset = i * elems_per_thread;
		segment.set_position(descriptor.output.to_i(offset), descriptor.output.to_j(offset), elems_per_thread);
		segments.push_back(segment);
	}

	cout << "* Starting " << descriptor.threads << " threads"
			<< " (" << elems_per_thread << " col-row multiplications per thread)" << endl;

	/* Account for integer division */
	auto scheduled = elems_per_thread * descriptor.threads;
	if (scheduled < total_elems) {
		segments.back().length += total_elems - scheduled;
		cout << "  With last thread doing " << (total_elems - scheduled) << " extra multiplication(s)." << endl;
	}

	/* Start segment calculation subtasks */
	vector<shared_ptr<pthread_t>> threads;
	threads.reserve(descriptor.threads);
	for (auto &segment : segments)
		threads.push_back(run_pthread(&MatrixMultiplicator::calculate_segment, &segment));

	/* Wait for segment calculation subtasks to finish */
	for (auto &thread : threads)
		pthread_join(*thread, NULL);

	cout << "* All subtasks finished calculating" << endl;
	pthread_exit(NULL);
	return NULL;
}

Matrix<double>& MatrixMultiplicator::multiply(size_t threads)
{
	/* Load matrices asynchronously */
	MatrixDescriptor load_desc_a(path_a, a, lock), load_desc_b(path_b, b, lock);
	auto load_thread_a = run_pthread(&MatrixMultiplicator::load_matrix, &load_desc_a);
	auto load_thread_b = run_pthread(&MatrixMultiplicator::load_matrix, &load_desc_b);
	cout << "Loading matrices..." << endl;
	pthread_join(*load_thread_a, NULL);
	pthread_join(*load_thread_b, NULL);
	cout << "Finished loading." << '\n' << endl;

	/* Start main multiplication thread */
	cout << "Starting multiplication using " << threads << " threads..." << endl;
	MultiplicationDescriptor mul_desc(a, b, result, lock, threads);
	auto mul_thread = run_pthread(&MatrixMultiplicator::multiply_matrices, &mul_desc);
	pthread_join(*mul_thread, NULL);
	cout << "Multiplication finished." << endl;
	return result;
}
