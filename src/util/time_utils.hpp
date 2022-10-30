#ifndef UTIL_TIME_UTILS_HPP_
#define UTIL_TIME_UTILS_HPP_

#include <chrono>
#include <sstream>

inline double seconds_since(std::chrono::high_resolution_clock::time_point time_point)
{
	return std::chrono::duration_cast<std::chrono::microseconds>
			(std::chrono::high_resolution_clock::now() - time_point).count() / 1000000.0;
}

inline std::string elapsed_time_str(std::chrono::high_resolution_clock::time_point time_point)
{
	auto seconds = seconds_since(time_point);
	std::stringstream ss;
	ss << std::fixed << std::setprecision(9) << seconds << " seconds";
	return ss.str();
}

#endif /* UTIL_TIME_UTILS_HPP_ */
