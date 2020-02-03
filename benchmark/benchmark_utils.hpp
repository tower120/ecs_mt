#pragma once

#include <chrono>

template<class TimeUnit = std::chrono::milliseconds, class Closure>
TimeUnit measure(Closure&& closure){
    using namespace std::chrono;
    const high_resolution_clock::time_point t1 = high_resolution_clock::now();
        closure();
    const high_resolution_clock::time_point t2 = high_resolution_clock::now();

    return duration_cast<TimeUnit>(t2-t1);
}


template<class TimeUnit = std::chrono::milliseconds, class Closure>
TimeUnit benchmark(unsigned int times, Closure&& closure){
	return measure<TimeUnit>([&]() {
	   for (unsigned int i = 0; i < times; ++i) closure();
	});
}


///
/// Play nicely with MSVC.
/// Usage:
/// std::chrono::milliseconds t;
/// {
///    scoped_profiler profiler(t);
///    ...
/// }
///
template<class TimeUnit>
class scoped_profiler{
public:
    explicit scoped_profiler(TimeUnit& t) noexcept
        : t(t)
        , t_begin(std::chrono::high_resolution_clock::now())
    {}
    scoped_profiler(const scoped_profiler&) = delete;
    scoped_profiler(scoped_profiler&&)      = delete;

    ~scoped_profiler() noexcept {
        using namespace std::chrono;
        const high_resolution_clock::time_point t_end = high_resolution_clock::now();
        t = duration_cast<TimeUnit>(t_end - t_begin);
    };
private:
    TimeUnit& t;
    const std::chrono::high_resolution_clock::time_point t_begin;
};