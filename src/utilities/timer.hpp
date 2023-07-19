#ifndef TIMER_HPP
#define TIMER_HPP

#include <ctime>

namespace Juanca {

	class Timer {
		public:
			Timer();
			auto sleep(__time_t _NanoSeconds) -> void;
		private:
			struct timespec time;
	};
}

#endif
