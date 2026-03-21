#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "systats.h" // includes <ncurses.h>

//TODO: change uptime from a struct to just an int
int
get_colour(float percent) {
  if (percent < 50) return 1;
  if (percent < 80) return 2;
  return 3;
}

void
history_push(struct history *h, float val) {
  h->data[h->head] = val;
  h->head = (h->head + 1) % HISTORY_LEN;
  if (h->count < HISTORY_LEN) h->count++;
}

char*
get_bar(float percent) {
  char *bars[] = {"▁","▂","▃","▄","▅","▆","▇","█"};

  int i = (int)(percent/100.0 * 7);
  if (i > 7) i = 7;
  if (i < 0) i = 0;
  return bars[i];
}

/*
 * @brief returns a struct containing the current CPU idle time and total time in units of 10ms
 */
struct cpu_usage
get_cpu_usage() {
  struct cpu_usage cpu = {0};

  char line_buffer[MAXBUFLEN];  

  FILE *fp = fopen("/proc/stat", "r");
  if (fp == NULL) return cpu;
  
  int i = 0;
    if (fgets(line_buffer, sizeof(line_buffer), fp)) {
      line_buffer[MAXBUFLEN-1] = '\0';
      char *token = strtok(line_buffer, " ");
      while (token != NULL) {
         if (i == CPU_IDLE) { // note that "cpu" is read but it returns 0 so idgaf
           cpu.idle_time = strtoll(token, NULL, 10);
         }
         cpu.total_time += strtoll(token, NULL, 10);
        i++;
        token = strtok(NULL, " ");
      }
    }

  fclose(fp);
  return cpu;
}

/*
 * @brief returns a float representing the cpu temp
 */
float
get_cpu_temp() { 
  int temp = -1; 
  char path[MAXBUFLEN];
  char name[MAXBUFLEN];
  char line_buffer[MAXBUFLEN];  

  for (int i = 0; i <10; i++) { // 10 is arbitrary
    snprintf(path, sizeof(path), "/sys/class/hwmon/hwmon%d/name", i);
    FILE *fp = fopen(path, "r");
    if (fp == NULL) continue;

    if (fgets(name, sizeof(line_buffer), fp)) {
      name[strcspn(name, "\n")] = '\0'; //strip newline  
    }
    fclose(fp);

    if (strncmp(name, "coretemp", MAXBUFLEN) != 0 && //intel
        strncmp(name, "k10temp", MAXBUFLEN) != 0)    //amd
        continue;

    snprintf(path, sizeof(path), "/sys/class/hwmon/hwmon%d/temp1_input", i);
    fp = fopen(path, "r");
    if (fp == NULL) continue;

    if (fgets(line_buffer, sizeof(line_buffer), fp)) {
      line_buffer[MAXBUFLEN-1] = '\0';
      char *token = strtok(line_buffer, " ");
      temp = (int)strtoll(token, NULL, 10);
    }

    fclose(fp);
    break;

  }

  return temp;
}

/*
 * @brief returns a struct containing the current memory usage (total and available)
 */
struct mem_usage 
get_mem_usage() {
  struct mem_usage mem = {0};

  char line_buffer[MAXBUFLEN];  

  FILE *fp = fopen("/proc/meminfo", "r");
  if (fp == NULL) return mem;
  
  int i = 0;

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

  fclose(fp);
  return mem;
}

/*
 * @brief returns a struct containing the load average
 */
struct load_avg
get_la() {
  struct load_avg load = {0};

  char line_buffer[MAXBUFLEN];  

  FILE *fp = fopen("/proc/loadavg", "r");
  if (fp == NULL) return load;
  
  int i = 0;

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


  fclose(fp);
  return load;
}

/*
 * @brief returns a struct containing the uptime
 */
struct uptime
get_uptime() {
  struct uptime uptime = {0};

  char line_buffer[MAXBUFLEN];  

  FILE *fp = fopen("/proc/uptime", "r");
  if (fp == NULL) return uptime;

  if (fgets(line_buffer, sizeof(line_buffer), fp)) {
    line_buffer[MAXBUFLEN-1] = '\0';
    char *token = strtok(line_buffer, " ");
    uptime.seconds = strtol(token, NULL, 10);
  }

  fclose(fp);
  return uptime;
}

/*
 * @brief print cpu usage with ncurses
 */
void 
ncurses_cpu_usage(WINDOW* screen, int y, int x, struct cpu_info c) {
  mvwprintw(screen, y, x, "CPU Usage: ");
  wattron(screen, A_BOLD | COLOR_PAIR(get_colour(c.usage)));
  wprintw(screen, "%.2f%%", c.usage);
  wattroff(screen, A_BOLD | COLOR_PAIR(get_colour(c.usage)));
}

/*
 * @brief print cpu temp with ncurses
 */
void 
ncurses_cpu_temp(WINDOW* screen, int y, int x, int temp) {
  if (temp == -1) {
    mvwprintw(screen, y, x, "CPU Temp: N/A");
    return;
  }

  mvwprintw(screen, y, x, "CPU Temp: ");
  wattron(screen, A_BOLD | COLOR_PAIR(get_colour(temp / 1000.0 / (float)MAX_CPU_TEMP * 100)));
  wprintw(screen, "%.2f\u00B0", temp / 1000.00);
  wattroff(screen, A_BOLD | COLOR_PAIR(get_colour(temp / 1000.0 / (float)MAX_CPU_TEMP * 100)));
  wprintw(screen, "C");
}

/*
 * @brief print memory info with ncurses
 */
void
ncurses_mem(WINDOW* screen, int y, int x, struct mem_usage mem) {
  float percent = (mem.total - mem.avail) / mem.total * 100;
  mvwprintw(screen, y, x, "Memory Usage: %.2fGiB/%.2fGiB (",
      (mem.total - mem.avail) / (1024 * 1024),
      mem.total / (1024 * 1024));
  wattron(screen, A_BOLD | COLOR_PAIR(get_colour(percent)));
  wprintw(screen, "%.2f%%", percent);
  wattroff(screen, A_BOLD | COLOR_PAIR(get_colour(percent)));
  wprintw(screen, ")");
}

/*
 * @brief print load average with ncurses
 */
void
ncurses_la(WINDOW* screen, int y, int x, struct load_avg load) {
  mvwprintw(screen, y, x, "Load Avg: %.2f %.2f %.2f", load.one_min, load.five_min, load.fifteen_min);
}

/*
 * @brief print uptime with ncurses
 */
void
ncurses_uptime(WINDOW* screen, int y, int x, struct uptime up) {
  mvwprintw(screen, y, x, "Uptime: %d hours, %d minutes, %d seconds\n", up.seconds/60/60, up.seconds/60 % 60, up.seconds%60);
}

/*
 * @brief print history bars
 */
void
ncurses_history(WINDOW* screen, int y, int x, struct history h) {
  char buf[HISTORY_LEN * 4 + 1];
  char *ptr = buf;
  
  for (int i = 0; i < h.count; i++) {
    int idx = (h.head - h.count + i + HISTORY_LEN) % HISTORY_LEN;
    const char *bar = get_bar(h.data[idx]);
    ptr += snprintf(ptr, 4, "%s", bar);
  }
  *ptr = '\0';
  
  mvwprintw(screen, y, x, "%s", buf);
}

/*
 * @brief print selected options 
 */
void
ncurses_selected(WINDOW* screen, int y, int x, struct systats sys) {
    mvwprintw(screen, y, x, "'q' to quit | 's' to change settings");
    int i = 3; // offset the status messages from the above message
    if (sys.settings & (1 << 0)) {
      if (sys.cpu_info.usage != 0) {
        ncurses_cpu_usage(screen, y+i, x, sys.cpu_info);
        i++;
        ncurses_history(screen, y+i, x, sys.histories.cpu_usage);
        i++;
      }
    }

    if (sys.settings & (1 << 1)) {
      ncurses_cpu_temp(screen, y+i, x, sys.cpu_info.temp);
      i++;
      ncurses_history(screen, y+i, x, sys.histories.cpu_temp);
      i++;
    }

    if (sys.settings & (1 << 2)) {
      ncurses_mem(screen, y+i, x, sys.mem_usage);
      i++;
      ncurses_history(screen, y+i, x, sys.histories.mem);
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
    box(screen, 0, 0);
  }

