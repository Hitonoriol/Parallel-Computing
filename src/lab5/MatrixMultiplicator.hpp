#ifndef MATRIXMULTIPLICATOR_HPP_
#define MATRIXMULTIPLICATOR_HPP_

#include <util/matrix.hpp>
#include <string>


namespace Lab5
{

class MatrixMultiplicator
{
	private:
		std::string path_a, path_b;
		Matrix<double> a, b, result;

		/* Load a matrix from file (using Matrix<T>'s `>>` operator) */
		void load_matrix(Matrix<double>&, const std::string&);

		/* Multiply matrices `a` and `b` in parallel,
		 *  writing calculation results to the `result` matrix
		 *  using the specified number of threads. */
		void multiply_matrices(size_t threads);

	public:
		MatrixMultiplicator(const std::string&, const std::string&);
		MatrixMultiplicator(Matrix<double> &a, Matrix<double> &b);
		~MatrixMultiplicator() = default;

		/* Loads matrices asynchronously & multiplies them using the specified number of threads */
		Matrix<double>& multiply(size_t threads = 1);
};

}
#endif /* MATRIXMULTIPLICATOR_HPP_ */
