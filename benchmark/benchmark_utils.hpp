#pragma once

#include <chrono>

template<class TimeUnit = std::chrono::milliseconds, class Closure>
static TimeUnit measure(Closure&& closure){
    using namespace std::chrono;
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
        closure();
    high_resolution_clock::time_point t2 = high_resolution_clock::now();

    return duration_cast<TimeUnit>(t2-t1)/*.count()*/;
}

template<class TimeUnit = std::chrono::milliseconds, class Closure>
static TimeUnit benchmark(unsigned int times, Closure&& closure){
	return measure<TimeUnit>([&]() {
	   for (unsigned int i = 0; i < times; ++i) closure();
	});
}