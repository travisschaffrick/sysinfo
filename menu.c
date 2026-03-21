#include <locale.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "systats.h" // includes <ncurses.h>

int 
main() {
  /*
   *   MENU INIT
   */
  setlocale(LC_ALL, "");
  initscr();
  start_color();
  use_default_colors();
  keypad(stdscr, TRUE);  
  cbreak();
  noecho();

  curs_set(0);
  init_pair(1, COLOR_GREEN, -1);   // low
  init_pair(2, COLOR_YELLOW, -1);  // medium
  init_pair(3, COLOR_RED, -1);     // high

  int term_y = getmaxy(stdscr);
  int term_x = getmaxx(stdscr);

  int height = 15;
  int width = 60;
  int start_y = (term_y - height) / 2;
  int start_x = (term_x - width) / 2;
  int center_y = height / 2;
  int center_x = width / 2;

  /*
   *   MENU
   */
  char *options[] = {"CPU Info", "CPU Temp", "Memory Info", "Load Avg. Info", "Uptime"};
  int settings = 0;

  WINDOW* menu = newwin(height, width, start_y, start_x);

  size_t count = sizeof(options)/sizeof(options[0]);

  struct systats sys = {0};
  // user input
  int input = 0;
  int selected = 0;

  int running = 1;
  while (running) {

    while(input != 's') {
      refresh();
      wclear(menu);
      box(menu, 0, 0);
      mvwprintw(menu, 1, center_x - strlen("Select Variables to Monitor")/2, "Select Variables to Monitor");
      mvwprintw(menu, 2, center_x - strlen("Press 's' to start!")/2, "Press 's' to start!");

      int option_x = center_x - (strlen(options[0]) + 4) / 2; // +4 for "[x] ";

      for (size_t i = OFFSET; i < count + OFFSET; i++) {
        // print selected status
        char box[4];
        if (settings & 1 << (i - OFFSET)) {
          strcpy(box, "[x]");
        }
        else {
          strcpy(box, "[ ]");
        }

        mvwprintw(menu, i, option_x, "%s %s", box, options[i-OFFSET]);
      }

      if (selected >= 0 && selected < (int)count) {
        mvwchgat(menu, selected+OFFSET, option_x, strnlen(options[selected], MAX) + 4, A_REVERSE, 0, NULL); 
      }

      // Print frame
      wrefresh(menu);

      //Get input
      input = getch();
      if (input == ERR) input = 0;

      if (input == KEY_UP) {
        if (selected == 0) {
          selected = (int)count - 1;
        }
        else {
          selected--;
        }
      }
      if (input == KEY_DOWN) {
        if (selected == (int)count - 1) {
          selected = 0;
        }
        else {
          selected++;
        } 
      }

      if (input == ENTER) {
        settings ^= (1 << selected);
      }

    }

    wclear(menu);

    WINDOW* loading = newwin(height, width, start_y, start_x);
    box(loading, 0, 0);

    mvwprintw(loading, center_y, center_x - strlen("Loading...")/2, "Loading...");


    wrefresh(loading);

    delwin(loading);

    /*
     *   PROGRAM LOOP
     */
    WINDOW* display = newwin(height, width, start_y, start_x);
    nodelay(stdscr, TRUE);

    // init cpu
    struct cpu_usage prev = get_cpu_usage();

    while (1) {
      int quit = 0;

      for (int i = 0; i < 4; i++) {
        usleep(250000);
        int ch = getch();

        if (ch == 'q') {
          nodelay(stdscr, FALSE);
          input = 0;
          running = 0;
          quit = 1;
          break;
        }

        if (ch == 's') {
          nodelay(stdscr, FALSE);
          input = 0;
          quit = 1;
          break;
        }
      }

      if (quit) break;

      struct cpu_usage curr = get_cpu_usage();
      struct cpu_info cpu_info;

      float diff_total = curr.total_time - prev.total_time; 
      float diff_idle = curr.idle_time - prev.idle_time;

      cpu_info.usage = (diff_total - diff_idle ) / diff_total * 100;
      cpu_info.temp = get_cpu_temp();

      sys.cpu_info = cpu_info;

      sys.mem_usage = get_mem_usage();
      sys.load_avg = get_la();
      sys.uptime = get_uptime();

      sys.settings = settings;

      float mem_usage = (sys.mem_usage.total - sys.mem_usage.avail) / sys.mem_usage.total * 100;

      history_push(&sys.histories.cpu_usage, cpu_info.usage);
      history_push(&sys.histories.cpu_temp, cpu_info.temp / 1000.0 / 95.0 * 100.0); // assume 95 is max
                                                                                   // converting mC to C, scaling, -> %
      history_push(&sys.histories.mem, mem_usage);

      wclear(display);                       // clear
      ncurses_selected(display, 1, 2, sys); // draw new frame 
      wrefresh(display);                     // show it

      // update cpu
      prev = curr;
    }
    delwin(display);
  }
  endwin();
  return 0;
}


