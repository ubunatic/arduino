
class LoopMetrics {
private:
    unsigned long max_loop_time = 0;
    unsigned long sum_loop_time = 0;
    unsigned long num_loops = 0;
public:
    inline LoopMetrics() {};
    ~LoopMetrics() {};
    void reset();
    void observe(unsigned long loop_time_ms);
    unsigned long maxLoopTime() { return max_loop_time; };
    unsigned long avgLoopTime() { return num_loops != 0? sum_loop_time/num_loops : max_loop_time; };
};
