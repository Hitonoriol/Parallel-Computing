#ifndef UTIL_TASK_HPP_
#define UTIL_TASK_HPP_

#include <string>

class Task
{
	private:
		std::string title;

	public:
		Task(const std::string &title);
		virtual ~Task() = default;

		void print_title();

		virtual void run(int argc, char *argv[]) = 0;
};

#endif /* UTIL_TASK_HPP_ */
