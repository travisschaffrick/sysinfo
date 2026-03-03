#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sysinfo.h" // includes <ncurses.h>

//TODO: add cpu temp from /sys/class/hwmon/hwmon2/temp1_input 
/*
@brief returns a struct containing the current CPU idle time and total time in units of 10ms
*/
struct cpu_usage
get_cpu_usage() {
  struct cpu_usage cpu = {0};

  char line_buffer[MAXBUFLEN];  

  FILE *fp = fopen("/proc/stat", "r");
  
  int i = 0;
  if (fp != NULL) {
    if (fgets(line_buffer, sizeof(line_buffer), fp)) {
      line_buffer[MAXBUFLEN-1] = '\0';
      char *token = strtok(line_buffer, " ");
      while (token != NULL) {
         if (i == CPU_IDLE) {
           cpu.idle_time = strtoll(token, NULL, 10);
         }
         cpu.total_time += strtoll(token, NULL, 10);
        i++;
        if (i > 0)
          token = strtok(NULL, " ");
      }
    }
  }

  fclose(fp);
  return cpu;
}

float
get_cpu_temp() { 
  int temp; 
  FILE *fp = fopen("/sys/class/hwmon/hwmon2/temp1_input", "r");

  char line_buffer[MAXBUFLEN];  

  if (fp != NULL) {
    
    char *token = strtok(line_buffer, " ");
    if (fgets(line_buffer, sizeof(line_buffer), fp)) {
      line_buffer[MAXBUFLEN-1] = '\0';
      temp = (int)strtoll(token, NULL, 10);
    }
  }
  fclose(fp);

  return temp;
}

/*
@brief returns a struct containing the current memory usage (total and available)
*/
struct mem_usage 
get_mem_usage() {
  struct mem_usage mem = {0};

  char line_buffer[MAXBUFLEN];  

  FILE *fp = fopen("/proc/meminfo", "r");
  
  int i = 0;
  if (fp != NULL) {
    while (fgets(line_buffer, sizeof(line_buffer), fp)) {
      line_buffer[MAXBUFLEN-1] = '\0';
      char *token = strtok(line_buffer, " ");
      int j = 0;
      while (token != NULL) {
        if (j == MEM_LOC) {
          if (i == MEM_TOTAL) { 
            mem.total = strtof(token, NULL);
          }
          if (i == MEM_AVAIL)   
            mem.avail = strtof(token, NULL);
        }
        token = strtok(NULL, " ");
        j++;
      }
  
      i++;
    }
  }

  fclose(fp);
  return mem;
}

struct load_avg
get_la() {
  struct load_avg load = {0};

  char line_buffer[MAXBUFLEN];  

  FILE *fp = fopen("/proc/loadavg", "r");
  
  int i = 0;
  if (fp != NULL) {
    if (fgets(line_buffer, sizeof(line_buffer), fp)) {
      line_buffer[MAXBUFLEN-1] = '\0';
      char *token = strtok(line_buffer, " ");
      while (token != NULL) {
         if (i == ONE_MIN) {
            load.one_min = strtof(token, NULL);
         }
         if (i == FIVE_MIN) {
            load.five_min = strtof(token, NULL);
         }
         if (i == FIFTEEN_MIN) {
            load.fifteen_min = strtof(token, NULL);
         }
        i++;
        token = strtok(NULL, " ");
      }
    }
  }

  fclose(fp);
  return load;
}

struct uptime
get_uptime() {
  struct uptime uptime = {0};

  char line_buffer[MAXBUFLEN];  

  FILE *fp = fopen("/proc/uptime", "r");
  
  if (fp != NULL) {
    if (fgets(line_buffer, sizeof(line_buffer), fp)) {
      line_buffer[MAXBUFLEN-1] = '\0';
      char *token = strtok(line_buffer, " ");
      uptime.seconds = strtol(token, NULL, 10);
    }
  }

  fclose(fp);
  return uptime;
}

void 
ncurses_cpu_usage(WINDOW* screen, int y, int x, float diff_total, float diff_idle) {
  mvwprintw(screen, y, x, "CPU Usage: %.2f%%\n", (diff_total - diff_idle ) / diff_total * 100);
}

void 
ncurses_cpu_temp(WINDOW* screen, int y, int x, int temp) {
  mvwprintw(screen, y, x, "CPU Temp: %.2f\u00B0C", temp / 1000.00);
}

void
ncurses_mem(WINDOW* screen, int y, int x, struct mem_usage mem) {
  mvwprintw(screen, y, x, "Memory Usage: %.2fGiB/%.2fGiB (%.2f%%)",
      (mem.total - mem.avail) / (1024 * 1024),
      mem.total / (1024 * 1024),    
      (mem.total - mem.avail) / mem.total * 100);
}

void
ncurses_la(WINDOW* screen, int y, int x, struct load_avg load) {
  mvwprintw(screen, y, x, "Load Avg: %.2f %.2f %.2f", load.one_min, load.five_min, load.fifteen_min);
}

void
ncurses_uptime(WINDOW* screen, int y, int x, struct uptime up) {
  mvwprintw(screen, y, x, "Uptime: %d hours, %d minutes, %d seconds\n", up.seconds/60/60, up.seconds/60 % 60, up.seconds%60);
}

void
ncurses_selected(WINDOW* screen, int y, int x, struct sysinfo sys) {
    initscr();
    noecho();
   
    mvwprintw(screen, y, x, "'q' to modify settings.");
    int i = 2; // offset the status messages from the above message
    if (sys.settings & (1 << 0)) {
      if (sys.cpu_info.diff_total != 0) {
        ncurses_cpu_usage(screen, y+i, x, sys.cpu_info.diff_total, sys.cpu_info.diff_idle);
        i++;
      }
    }

    if (sys.settings & (1 << 1)) {
      ncurses_cpu_temp(screen, y+i, x, sys.cpu_info.temp);
      i++;
    }

    if (sys.settings & (1 << 2)) {
      ncurses_mem(screen, y+i, x, sys.mem_usage);
      i++;
    }

    if (sys.settings & (1 << 3)) {
      ncurses_la(screen, y+i, x, sys.load_avg);
      i++;
    }

    if (sys.settings & (1 << 4)) {
      ncurses_uptime(screen, y+i, x, sys.uptime);
      i++;
    }

  refresh();
  clear();
  }

