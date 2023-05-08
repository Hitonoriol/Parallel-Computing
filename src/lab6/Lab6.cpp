#include "Lab6.hpp"

#include <mpi.h>
#include <iostream>
#include <sstream>

#include <lab1/Task1.hpp>
#include <lab1/Task2.hpp>

using namespace std;

namespace Lab6
{

int world_size;
int world_rank;
char processor_name[MPI_MAX_PROCESSOR_NAME];
string id;

streambuf *stdout; // Original std::cout buffer
stringstream out; // Temporary output string buffer

int run(int argc, char *argv[])
{
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	int name_len;
	MPI_Get_processor_name(processor_name, &name_len);
	id = '[' + string(processor_name)
			+ " #" + to_string(world_rank) + '/' + to_string(world_size) + ']';

	/* Start input buffering by redirecting std::cout to a local temporary std::stringstream,
	 * which will be printed all at once when current process finishes execution. */
	stdout = cout.rdbuf();
	cout.rdbuf(out.rdbuf());

	/* Run Task1 on main machine and Task2 on the VM in parallel.
	 * If more than 2 hosts are specified, the rest of them do nothing. */
	cout << id << " Starting execution..." << endl;
	run_task_by_process_id(argc, argv);
	cout << id << " Done executing!" << endl;

	/* Switch std::cout back to stdout and output buffered text to it. */
	cout.rdbuf(stdout);
	cout << out.str() << endl;
	MPI_Finalize();
	return 0;
}

/* Choose what task to run based on the current thread id (rank) */
void run_task_by_process_id(int argc, char *argv[])
{
	switch (world_rank) {
	case 0:
		Lab1::Task1().run(argc, argv);
		break;

	case 1: {
		Lab1::Task2().run(argc, argv);
		break;
	}

	default:
		cout << "Idling...";
	}
}

}
