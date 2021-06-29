#define FDIR_UNSPECIFIED 0
#define FDIR_A 69
#define FDIR_B 71
#define FDIR_C 9
#define FDIR_X 64
#define FDIR_LEFT 68
#define FDIR_RIGHT 67
#define FDIR_UP 70
#define FDIR_DOWN 21
#define FDIR_0 7
#define FDIR_1 22
#define FDIR_2 25
#define FDIR_3 13
#define FDIR_4 12
#define FDIR_5 24
#define FDIR_6 94
#define FDIR_7 8
#define FDIR_8 28
#define FDIR_9 90

const char* funduino_command(int command_code) {
    switch (command_code) {
    case FDIR_UNSPECIFIED: return "UNSPECIFIED";
    case FDIR_A:     return "A";
    case FDIR_B:     return "B";
    case FDIR_C:     return "C";
    case FDIR_X:     return "X";
    case FDIR_LEFT:  return "LEFT";
    case FDIR_RIGHT: return "RIGHT";
    case FDIR_UP:    return "UP";
    case FDIR_DOWN:  return "DOWN";
    case FDIR_0:     return "0";
    case FDIR_1:     return "1";
    case FDIR_2:     return "2";
    case FDIR_3:     return "3";
    case FDIR_4:     return "4";
    case FDIR_5:     return "5";
    case FDIR_6:     return "6";
    case FDIR_7:     return "7";
    case FDIR_8:     return "8";
    case FDIR_9:     return "9";
    default:         return "UNKNOWN";
    }
}
