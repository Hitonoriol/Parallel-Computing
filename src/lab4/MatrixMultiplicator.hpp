#ifndef MATRIXMULTIPLICATOR_HPP_
#define MATRIXMULTIPLICATOR_HPP_

#include <string>

#include <util/matrix.hpp>

namespace Lab4
{

class MatrixMultiplicator
{
	private:
		std::string path_a, path_b;
		Matrix<double> a, b, result;

		/* Load a matrix from file (using Matrix<T>'s `>>` operator) */
		void load_matrix(Matrix<double>&, const std::string&);

		/* Executed by the multiplication thread */
		void multiply_matrices(size_t threads);

		/* Executed by a multiplication subtask thread.
		 * Calculates an output matrix's segment */
		void calculate_segment(size_t i_start, size_t j_start, size_t length);

	public:
		MatrixMultiplicator(const std::string&, const std::string&);
		~MatrixMultiplicator() = default;

		/* Loads matrices asynchronously & multiplies them using the specified number of threads */
		Matrix<double>& multiply(size_t threads = 1);
};

}
#endif /* MATRIXMULTIPLICATOR_HPP_ */
