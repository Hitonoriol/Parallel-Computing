#include "Task1.hpp"

#include <omp.h>
#include <chrono>
#include <cctype>
#include <numeric>
#include <locale>
#include <algorithm>
#include <unistd.h>
#include <random>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <cstring>
#include <cerrno>
#include <type_traits>

#include <util/time_utils.hpp>
#include <util/random_data.hpp>

using namespace std;

namespace Lab5
{

Task1::Task1()
: Task("Task 1: analyze two files using OpenMP parallel loops")
{
}

void Task1::process_chars(const std::string &file_path)
{
	auto chars = load_data<char>(file_path);
	auto start = chrono::high_resolution_clock::now();
	size_t char_count = str_length(chars);
	size_t words = 0, lines = 0;
	size_t letters = 0, digits = 0, punctuators = 0;

	auto end = chars.end();
	#pragma omp parallel for num_threads(1)
	for (auto it = chars.begin(); it != end; ++it) {
		char chr;
		#pragma omp atomic read
		chr = *it;

		if (chr == ' ')
			#pragma omp atomic
			++words;
		if (chr == '\n')
			#pragma omp atomic
			++lines;
		if (ispunct(chr))
			#pragma omp atomic
			++punctuators;
		if (isdigit(chr))
			#pragma omp atomic
			++digits;
	}

	/* Estimate how many of the characters are letters */
	letters = char_count - words - digits - punctuators;

	#pragma omp critical(out)
	{
		cout << "OMP Thread #" << omp_get_thread_num()
				<< " finished processing " << quoted(file_path) << endl;
		cout << "Text file analysis:" << NL
				<< "  Total characters: " << char_count << NL
				<< "  Punctuation characters: " << punctuators << NL
				<< "  Letters: " << letters << NL
				<< "  Lines: " << lines << NL
				<< "  Words: " << words << NL
				<< "  Digits: " << digits << NL;
		cout << "[" << chars.size() << " elements processed in " << elapsed_time_str(start) << "]" << NL;
		cout << NL << endl;
	}
}

void Task1::process_nums(const std::string &file_path)
{
	auto nums = load_data<double>(file_path);
	auto start = chrono::high_resolution_clock::now();
	double min = nums[0], max = nums[0];
	double average = 0.0;
	size_t num_positive = 0, num_negative = 0, num_zeroes = 0;
	auto end = nums.end();
	#pragma omp parallel for num_threads(1)
	for (auto it = nums.begin(); it != end; ++it) {
		double n;
		#pragma omp atomic read
		n = *it;

		if (n < min)
			#pragma omp atomic write
			min = n;
		if (n > max)
			#pragma omp atomic write
			max = n;

		if (n > 0)
			#pragma omp atomic
			++num_positive;
		else if (n < 0)
			#pragma omp atomic
			++num_negative;
		else
			#pragma omp atomic
			++num_zeroes;

		#pragma omp atomic
		average += n;
	}
	average /= nums.size();

	#pragma omp critical(out)
	{
		cout << "OMP Thread #" << omp_get_thread_num()
				<< " finished processing " << quoted(file_path) << endl;
		cout << "Number file analysis:" << NL
				<< "  Min: " << min << NL
				<< "  Max: " << max << NL
				<< "  Average: " << average << NL
				<< "  Total number: " << nums.size() << NL
				<< "  Number of positive elements: " << num_positive << NL
				<< "  Number of negative elements: " << num_negative << NL
				<< "  Number of zeroes: " << num_zeroes << NL;
		cout << "[" << nums.size() << " elements processed in " << elapsed_time_str(start) << "]" << NL;
		cout << NL << endl;
	}
}

void Task1::benchmark()
{
	for (auto quantity : { 64000, 128000, 256000, 512000, 1024000, 2048000, 4096000, 8192000, 16384000, 32768000 }) {
		auto charfile = "chars-" + std::to_string(quantity / 1000) + "k.txt";
		auto numfile = "nums-" + std::to_string(quantity / 1000) + "k.txt";
		generate_chars(charfile, quantity);
		generate_doubles(numfile, quantity);
		cout << "Launching analysis tasks as parallel sections..." << endl;
		#pragma omp parallel sections
		{
			#pragma omp section
			{
				process_nums(numfile);
			}

			#pragma omp section
			{
				process_chars(charfile);
			}
		}
		cout << "All threads finished executing." << endl;
	}
}

void Task1::run(int argc, char **argv)
{
	omp_set_nested(true);
	if (argc > 2 && std::string(argv[2]) == "benchmark1") {
		benchmark();
		return;
	}
	cout << "Launching analysis tasks as parallel sections..." << endl;
	#pragma omp parallel sections
	{
		#pragma omp section
		{
			process_nums("numbers.txt");
		}

		#pragma omp section
		{
			process_chars("chars.txt");
		}
	}
	cout << "All threads finished executing." << endl;
}

}
