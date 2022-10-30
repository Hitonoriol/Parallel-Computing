#ifndef LAB5_TASK2_HPP_
#define LAB5_TASK2_HPP_

#include <util/Task.hpp>

namespace Lab5
{

class Task2: public Task
{
	public:
		Task2();
		void run(int argc, char **argv) override;
};

}
#endif /* LAB5_TASK2_HPP_ */
