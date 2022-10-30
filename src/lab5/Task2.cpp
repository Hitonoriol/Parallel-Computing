#include "Task2.hpp"

#include <iostream>

#include <util/commons.hpp>
#include <util/random_data.hpp>
#include "MatrixMultiplicator.hpp"

using namespace std;

namespace Lab5
{

Task2::Task2()
: Task("Task 2: multiply two matrices using specified number of threads (using OpenMP loops) \n"
		"  (Accepts an optional argument: [threads] - number of threads to perform matrix multiplication with)")
{
}

void benchmark()
{
	for (auto size : {100, 200, 300, 400, 500,
					  600, 700, 800, 900, 1000,
					  1200, 1400, 1600, 1800, 2000}) {
		Matrix<double> a, b;
		generate_square_matrix(a, size);
		generate_square_matrix(b, size);
		MatrixMultiplicator multiplicator(a, b);
		multiplicator.multiply(1);
	}
}

/*
 * ./parallel-computing-lab5 <num_threads> [</path/to/matrix_a> </path/to/matrix_b>]
 * 		or
 * ./parallel-computing-lab5 <num_threads> benchmark2
 */
void Task2::run(int argc, char **argv)
{
	size_t threads = argc >= 2 ? atol(argv[1]) : 1;
	if (argc == 3 && std::string(argv[2]) == "benchmark2") {
		benchmark();
		return;
	}

	string file_a("matrix_a.txt"), file_b("matrix_b.txt");
	if (argc == 4) {
		file_a = std::string(argv[2]);
		file_b = std::string(argv[3]);
	}

	MatrixMultiplicator multiplicator(file_a, file_b);
	auto &result = multiplicator.multiply(threads);
	cout << "Multiplication result:" << NL << result.to_string();
}

}
