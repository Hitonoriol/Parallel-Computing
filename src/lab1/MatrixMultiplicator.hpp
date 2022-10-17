#ifndef MATRIXMULTIPLICATOR_HPP_
#define MATRIXMULTIPLICATOR_HPP_

#include <string>
#include <pthread.h>

#include <util/matrix.hpp>

class MatrixMultiplicator
{
	private:
		std::string path_a, path_b;
		Matrix<double> a, b, result;
		pthread_mutex_t lock;

		/* Descriptor for `load_matrix` pthread function */
		struct MatrixDescriptor
		{
				const std::string &filename;
				Matrix<double> &matrix;
				pthread_mutex_t &lock;

				MatrixDescriptor(const std::string &filename, Matrix<double> &matrix, pthread_mutex_t &lock)
					: filename(filename), matrix(matrix), lock(lock) {}
		};

		/* Descriptor for `multiply_matrices` pthread function */
		struct MultiplicationDescriptor
		{
				Matrix<double> &a, &b, &output;
				pthread_mutex_t &lock;
				size_t threads;

				MultiplicationDescriptor(
						Matrix<double> &a, Matrix<double> &b,
						Matrix<double> &output,
						pthread_mutex_t &lock,
						size_t threads)
					: a(a), b(b), output(output), lock(lock), threads(threads) {}

				MultiplicationDescriptor(const MultiplicationDescriptor &rhs)
					: a(rhs.a), b(rhs.b), output(rhs.output), lock(rhs.lock), threads(rhs.threads) {}
		};

		/* Descriptor for `calculate_segment` pthread function */
		struct MultiplicationSegmentDescriptor: public MultiplicationDescriptor
		{
				size_t i_start = 0, j_start = 0, length = 0;
				MultiplicationSegmentDescriptor(const MultiplicationSegmentDescriptor &rhs)
					: MultiplicationDescriptor(rhs), i_start(rhs.i_start), j_start(rhs.j_start), length(rhs.length)
				{}

				MultiplicationSegmentDescriptor(const MultiplicationDescriptor &rhs)
					: MultiplicationDescriptor(rhs) {}

				inline MultiplicationSegmentDescriptor& set_position(
						size_t i_start, size_t j_start, size_t length)
				{
					this->i_start = i_start;
					this->j_start = j_start;
					this->length = length;
					return *this;
				}
		};

		/* Executed by a loading pthread, accepts a ptr to a `MatrixDescriptor` */
		static void* load_matrix(void *descriptor_ptr);

		/* Executed by a multiplication pthread, accepts a ptr to a `MultiplicationDescriptor` */
		static void* multiply_matrices(void *descriptor_ptr);

		/* Executed by a multiplication subtask pthread.
		 * Calculates an output matrix's segment described by a `MultiplicationSegmentDescriptor` */
		static void* calculate_segment(void *descriptor_ptr);

	public:
		MatrixMultiplicator(const std::string&, const std::string&);
		~MatrixMultiplicator();

		/* Loads matrices asynchronously & multiplies them using the specified number of threads */
		Matrix<double>& multiply(size_t threads = 1);
};

#endif /* MATRIXMULTIPLICATOR_HPP_ */
