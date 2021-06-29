#include "metrics.h"

// mx_observe records basic loop time metrics.
// ATTENTION: Make sure to not record when running slow `Serial` commands.
//            We are only intersted in the timing of regular control code.
//            When printing to the Serial, programs will always be slow.
void LoopMetrics::observe(unsigned long loop_time_ms) {
    if (loop_time_ms > max_loop_time) max_loop_time = loop_time_ms;
    sum_loop_time += loop_time_ms;
    num_loops++;
}

void LoopMetrics::reset() {
    max_loop_time = 0;
    sum_loop_time = 0;
    num_loops = 0;
}
