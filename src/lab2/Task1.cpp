#include "Task1.hpp"

#include <thread>
#include <mutex>
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

using namespace std;

namespace Lab2
{

mutex mtx;

Task1::Task1()
: Task("Task 1: analyze two files in two different C++11 threads")
{
}

void Task1::process_chars(const std::string &file_path)
{
	auto chars = load_data<char>(file_path);
	size_t char_count = str_length(chars);
	size_t words = 0, lines = 0;
	size_t letters = 0, digits = 0, punctuators = 0;

	for (auto chr : chars) {
		if (chr == ' ')
			++words;
		if (chr == '\n')
			++lines;
		if (ispunct(chr))
			++punctuators;
		if (isdigit(chr))
			++digits;
	}

	/* Estimate how many of the characters are letters */
	letters = char_count - words - digits - punctuators;

	{
		lock_guard<mutex> lock(mtx);
		cout << "Text file analysis:" << NL
				<< "  Total characters: " << char_count << NL
				<< "  Punctuation characters: " << punctuators << NL
				<< "  Letters: " << letters << NL
				<< "  Lines: " << lines << NL
				<< "  Words: " << words << NL
				<< "  Digits: " << digits << NL;
		cout << NL << endl;
	}
}

void Task1::process_nums(const std::string &file_path)
{
	auto nums = load_data<double>(file_path);
	auto min = *min_element(nums.begin(), nums.end());
	auto max = *max_element(nums.begin(), nums.end());
	auto average = reduce(nums.begin(), nums.end()) / static_cast<double>(nums.size());
	auto num_positive = count_if(nums.begin(), nums.end(), [](auto n) {return n > 0;});
	auto num_negative = count_if(nums.begin(), nums.end(), [](auto n) {return n < 0;});
	auto num_zeroes = nums.size() - (num_positive + num_negative);

	{
		lock_guard<mutex> lock(mtx);
		cout << "Number file analysis:" << NL
				<< "  Min: " << min << NL
				<< "  Max: " << max << NL
				<< "  Average: " << average << NL
				<< "  Total number: " << nums.size() << NL
				<< "  Number of positive elements: " << num_positive << NL
				<< "  Number of negative elements: " << num_negative << NL
				<< "  Number of zeroes: " << num_zeroes;
		cout << NL << endl;
	}
}

void Task1::run(int argc, char **argv)
{
	thread num_thread(&Task1::process_nums, "numbers.txt");
	thread char_thread(&Task1::process_chars, "chars.txt");

	cout << "Waiting for threads..." << endl;
	num_thread.join();
	char_thread.join();

	cout << "All threads finished executing." << endl;
}

}
