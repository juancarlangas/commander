#include "timer.hpp"

Juanca::Timer::Timer() {
		time.tv_sec = 0;
		time.tv_nsec = 0;
}

auto Juanca::Timer::sleep(__time_t _NanoSeconds) -> void {
	time.tv_nsec = _NanoSeconds;

	nanosleep(&time, NULL);
}

