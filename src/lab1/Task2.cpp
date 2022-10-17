#include "Task2.hpp"

#include <iostream>

#include <util/commons.hpp>
#include "MatrixMultiplicator.hpp"

using namespace std;

Task2::Task2()
: Task("Task 2: multiply two matrices using specified number of threads \n"
		"  (Accepts an optional argument: [threads] - number of threads to perform matrix multiplication with)")
{
}

void Task2::run(int argc, char **argv)
{
	size_t threads = argc >= 2 ? atol(argv[1]) : 1;
	MatrixMultiplicator multiplicator("matrix_big_a.txt", "matrix_big_b.txt");
	auto &result = multiplicator.multiply(threads);
	cout << "Multiplication result:" << NL << result.to_string();
}
