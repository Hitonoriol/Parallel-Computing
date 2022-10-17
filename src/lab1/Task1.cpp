#include "Task1.hpp"

#include <pthread.h>
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

#include <util/pthread_wrapper.hpp>

using namespace std;

pthread_mutex_t lock;

Task1::Task1()
: Task("Task 1: analyze two files in two different pthreads")
{
}

void* Task1::process_chars(void*)
{
	auto chars = load_data<char>("chars.txt");
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

	pthread_mutex_lock(&lock);
	{
		cout << "Text file analysis:" << NL
				<< "  Total characters: " << char_count << NL
				<< "  Punctuation characters: " << punctuators << NL
				<< "  Letters: " << letters << NL
				<< "  Lines: " << lines << NL
				<< "  Words: " << words << NL
				<< "  Digits: " << digits << NL;
		cout << NL << endl;
	}
	pthread_mutex_unlock(&lock);
	pthread_exit(NULL);
	return nullptr;
}

void* Task1::process_nums(void*)
{
	auto nums = load_data<double>("numbers.txt");
	auto min = *min_element(nums.begin(), nums.end());
	auto max = *max_element(nums.begin(), nums.end());
	auto average = reduce(nums.begin(), nums.end()) / static_cast<double>(nums.size());
	auto num_positive = count_if(nums.begin(), nums.end(), [](auto n) {return n > 0;});
	auto num_negative = count_if(nums.begin(), nums.end(), [](auto n) {return n < 0;});
	auto num_zeroes = nums.size() - (num_positive + num_negative);
	pthread_mutex_lock(&lock);
	{
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
	pthread_mutex_unlock(&lock);
	pthread_exit(NULL);
	return nullptr;
}

void Task1::run(int argc, char **argv)
{
	if (pthread_mutex_init(&lock, NULL)) {
		cerr << "Failed to initialize mutex" << endl;
		exit(-1);
	}

	auto num_thread = run_pthread(&Task1::process_nums);
	auto char_thread = run_pthread(&Task1::process_chars);

	cout << "Waiting for threads..." << endl;
	pthread_join(*num_thread, NULL);
	pthread_join(*char_thread, NULL);

	cout << "All threads finished executing." << endl;
	pthread_mutex_destroy(&lock);
}
