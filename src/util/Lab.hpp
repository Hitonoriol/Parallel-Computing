#ifndef UTIL_LAB_HPP_
#define UTIL_LAB_HPP_

template<typename T>
void run_tasks(int argc, char *argv[])
{
	T task;
	task.print_title();
	task.run(argc, argv);
}

template<typename T, typename ... Ts>
typename std::enable_if<sizeof...(Ts)>::type
run_tasks(int argc, char *argv[])
{
	run_tasks<T>(argc, argv);
	run_tasks<Ts...>(argc, argv);
}

#endif /* UTIL_LAB_HPP_ */
