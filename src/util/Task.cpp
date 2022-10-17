#include "Task.hpp"

#include <iostream>

Task::Task(const std::string &title)
: title(title)
{
}

void Task::print_title()
{
	std::cout << '\n' << title << '\n' << std::endl;
}
