#include "Task1.hpp"
#include "Task2.hpp"

#include <util/Lab.hpp>

using namespace std;

namespace Lab1
{

int run(int argc, char *argv[])
{
	run_tasks<Task1, Task2>(argc, argv);
	cout << "Done." << endl;
	pthread_exit(NULL);
	return 0;
}

}
