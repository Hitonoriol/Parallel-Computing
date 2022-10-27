#ifndef LAB4_TASK1_HPP_
#define LAB4_TASK1_HPP_

#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <cstring>
#include <cerrno>
#include <type_traits>
#include <algorithm>

#include <util/commons.hpp>
#include <util/meta.hpp>
#include <util/Task.hpp>

namespace Lab4
{

class Task1: public Task
{
	private:
		/* Load space-separated elements into a std::vector<T> from file `path`.
		 * When T is a char-like type, all characters are read into the vector
		 *   (including whitespaces & newlines). */
		template<typename T>
		static std::vector<T> load_data(const std::string &path)
		{
			std::fstream file(path, std::ios_base::in);
			if (!file.is_open()) {
				std::cout << "Error opening file " << std::quoted(path)
						<< ", error: " << std::strerror(errno);
				pthread_exit(NULL);
			}

			T elem;
			std::vector<T> data;

			/* Do not ignore whitespace / newline characters when loading as a vector of chars */
			if constexpr (is_char_type<T>) {
				while (file.get(elem))
					data.push_back(elem);
			}
			else {
				while (file >> elem)
					data.push_back(elem);
			}
			std::cout << "* Loaded " << data.size() << " " << type_name<T>()
					<< " elements"
					<< '\n';
			file.close();
			return data;
		}

		/* Takes multi-byte characters into account */
		static inline size_t str_length(std::vector<char> &str)
		{
			return str.size() - std::count_if(str.begin(), str.end(),
					[](char c) -> bool {return (c & 0xC0) == 0x80;});
		}

		/* Load and analyze file from `file_path` as a text file. */
		static void process_chars(const std::string &file_path);

		/* Load and analyze file from `file_path` as a numeric data file. */
		static void process_nums(const std::string &file_path);

	public:
		Task1();
		void run(int argc, char **argv) override;
};

}
#endif /* LAB4_TASK1_HPP_ */
