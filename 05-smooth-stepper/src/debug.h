
#ifdef DEBUG_STEPPER
#define stepper_debug(text) print(text)
#else
#define stepper_debug(text) void()
#endif
