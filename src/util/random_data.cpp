#include "random_data.hpp"

#include <random>
#include <fstream>
#include <iostream>
#include <iomanip>

char characters[] = {
		' ', ' ', ' ', ' ', ' ', // For spaces to occur more frequently in the generated text
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
		'!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
		'-', '_', '+', '=', '`', '~', ';', ':', '\'', '"', ',',
		'.', '<', '>', '/', '?', '\\', '|',
		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i',
		'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
		's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I',
		'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
		'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
};

std::mt19937 rng(std::random_device { }());

std::uniform_int_distribution<> idx_gen(0, std::size(characters) - 1);
std::uniform_real_distribution<double> double_gen(-100.0, 100.0);

char next_char()
{
	return characters[idx_gen(rng)];
}

void generate_chars(const std::string &file, size_t quantity)
{
	std::ofstream out(file);
	for (size_t i = 0; i < quantity; ++i)
		out << next_char();
	out.close();
	std::cout << "Wrote " << quantity << " chars to " << std::quoted(file) << std::endl;
}

void generate_doubles(const std::string &file, size_t quantity)
{
	std::ofstream out(file);
	for (size_t i = 0; i < quantity; ++i)
		out << double_gen(rng) << ' ';
	out.close();
	std::cout << "Wrote " << quantity << " doubles to " << std::quoted(file) << std::endl;
}

std::uniform_real_distribution<double> matrix_gen(-1, 1);
void generate_square_matrix(Matrix<double> &matrix, size_t size)
{
	matrix.set_size(size, size);
	for (size_t i = 0; i < size; ++i)
		for (size_t j = 0; j < size; ++j)
			matrix[i][j] = matrix_gen(rng);

	std::cout << "Generated a " << size << "x" << size << " matrix." << std::endl;
}
