#ifndef UTIL_RANDOM_DATA_HPP_
#define UTIL_RANDOM_DATA_HPP_

#include <util/matrix.hpp>
#include <string>

void generate_chars(const std::string&, size_t);
void generate_doubles(const std::string&, size_t);
void generate_square_matrix(Matrix<double>&, size_t);

#endif /* UTIL_RANDOM_DATA_HPP_ */
