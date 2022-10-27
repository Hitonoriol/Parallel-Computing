#ifndef LAB4_TASK2_HPP_
#define LAB4_TASK2_HPP_

#include <util/Task.hpp>

namespace Lab4
{

class Task2: public Task
{
	public:
		Task2();
		void run(int argc, char **argv) override;
};

}
#endif /* LAB4_TASK2_HPP_ */
