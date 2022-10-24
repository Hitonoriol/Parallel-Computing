#ifndef LAB2_TASK2_HPP_
#define LAB2_TASK2_HPP_

#include <util/Task.hpp>

namespace Lab2
{

class Task2: public Task
{
	public:
		Task2();
		void run(int argc, char **argv) override;
};

}
#endif /* LAB2_TASK2_HPP_ */
