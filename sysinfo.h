#include <ncurses.h>

#define MAXBUFLEN 256

#define CPU_IDLE 4
#define MEM_TOTAL 0
#define MEM_AVAIL 2
#define MEM_LOC 1
#define ONE_MIN 0
#define FIVE_MIN 1
#define FIFTEEN_MIN 2
#define UPTIME 0

struct
cpu_usage {
  unsigned long long idle_time; // 10ms
  unsigned long long total_time; // 10ms
};

struct
cpu_info {
  float diff_total;
  float diff_idle;
  int temp; // 1/1000 * degrees C
};

struct
mem_usage {
  float total; // kB
  float avail; // kB
};

struct
load_avg {
  float one_min;
  float five_min;
  float fifteen_min;
};

struct
uptime {
  int seconds;
};

struct
sysinfo {
  int settings;
  struct cpu_info cpu_info;
  struct mem_usage mem_usage;
  struct load_avg load_avg;
  struct uptime uptime;
};

struct cpu_usage get_cpu_usage();
float  get_cpu_temp();
struct mem_usage get_mem_usage();
struct load_avg  get_la();
struct uptime    get_uptime();
void ncurses_cpu_usage(WINDOW* screen, int y, int x, float diff_total, float diff_idle);
void ncurses_cpu_temp(WINDOW* screen, int y, int x, int temp);
void ncurses_mem(WINDOW* screen, int y, int x, struct mem_usage mem); 
void ncurses_la(WINDOW* screen, int y, int x, struct load_avg load); 
void ncurses_uptime(WINDOW* screen, int y, int x, struct uptime up); 
void ncurses_selected(WINDOW* screen, int y, int x, struct sysinfo sys);


