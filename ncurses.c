/*
   09/03/2016

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
   MA 02110-1301, USA.

  Compile with:
   gcc -std=c99 -D_POSIX_C_SOURCE=200112L -Wall -Wextra -O2 ncurses.c -o pinky_curses -lncurses
  BSD users should type -D_DEFAULT_SOURCE instead.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>

#include <locale.h>
#include <unistd.h>
#include <signal.h>

#include <ncurses.h>

#define VLA 1000
#define EIGHT 8
#define WIN95 stdscr

#define RESTORE_CURSOR() \
  echo(); \
  nl(); \
  curs_set(TRUE); \
  endwin();

#define SET_CRL(x) A_BOLD | COLOR_PAIR(x * EIGHT)

void unuglify(char *);
void sighandler(int num);
void init_da_handler(void);

volatile sig_atomic_t call_it_quits = 0;

int main(void) {
  init_da_handler();
  if (NULL == (setlocale(LC_ALL, ""))) {
    return EXIT_FAILURE;
  }

  int16_t color_pair = 1, fg = 1, bg = 1, x = 0, z = 0;
  int32_t old_x = 0, old_y = 0, new_y = 0, new_x = 0;
  char buf[VLA];

  initscr();
  noecho();
  nocbreak();
  nodelay(WIN95, TRUE);
  nonl();
  intrflush(WIN95, FALSE);
  curs_set(FALSE);

  if (FALSE == (has_colors())) {
    RESTORE_CURSOR();
    return EXIT_FAILURE;
  }
  start_color();

  for (x = 0; x < EIGHT; x++) {
    for (z = 0; z < EIGHT; z++) {
      init_pair((int16_t)((x * EIGHT) + z), x, z);
    }
  }

  pair_content(color_pair, &fg, &bg);
  getmaxyx(WIN95, old_y, old_x);

  while (1) {
    getmaxyx(WIN95, new_y, new_x);
    if (old_y != new_y || old_x != new_x) {
      old_y = new_y;
      old_x = new_x;
      werase(WIN95);
      wrefresh(WIN95);
    }
    if (NULL != (fgets(buf, VLA, stdin))) {
      unuglify(buf);
    }
    if (1 == call_it_quits) {
      break;
    }
  }

  RESTORE_CURSOR();
  fprintf(stderr, "\n%s\n", "See you later");

  return EXIT_SUCCESS;
}

/* 
 * The signal handler is based on
 * https://www.gnu.org/software/libc/manual/html_node/Advanced-Signal-Handling.html#Advanced-Signal-Handling
*/
void init_da_handler(void) {
  struct sigaction setup_action;
  memset(&setup_action, 0, sizeof(struct sigaction));

  setup_action.sa_handler = &sighandler;

  if (-1 == (sigaction(SIGINT, &setup_action, NULL))) {
    fprintf(stderr, "%s\n", "sigaction() failed");
    exit(EXIT_FAILURE);
  }
}

/* !!! WARNING !!! */
void sighandler(int num) {
/* ASYNC CODE ONLY */
  (void)num;
  call_it_quits = 1;
}

void unuglify(char *str1) {
  char *ptr = str1;
  unsigned char sup = '\0';
  static const char discard[] = "\"*;`{}~|";
  static uint32_t req_clr = 1;
  static uint32_t clr_arr[] = {
    SET_CRL(COLOR_BLUE),
    SET_CRL(COLOR_MAGENTA),
    SET_CRL(COLOR_YELLOW),
    SET_CRL(COLOR_RED),
    SET_CRL(COLOR_CYAN),
    SET_CRL(COLOR_WHITE),
    SET_CRL(COLOR_GREEN)
  };

  wmove(WIN95, 0, 0);
  wclrtoeol(WIN95);

  for (; *ptr; ptr++) {
    sup = (unsigned char) *ptr;
    if ((0 != (iscntrl(sup))) || (0 == (isprint(sup)))) {
      continue;
    }
    if ('^' == *ptr)  {
      ++ptr;
      switch((toupper((unsigned char) *ptr))) {
        case 'B':
          req_clr = 0;
          break;
        case 'M':
          req_clr = 1;
          break;
        case 'Y':
          req_clr = 2;
          break;
        case 'R':
          req_clr = 3;
          break;
        case 'C':
          req_clr = 4;
          break;
        case 'W':
          req_clr = 5;
          break;
        case 'G':
          req_clr = 6;
          break;
        default:
          /* Can be cntrl since we advanced with 1 char */
          break;
      }
    } else {
      if (NULL == (strchr(discard, *ptr))) {
        waddch(WIN95, (chtype)*ptr | clr_arr[req_clr]);
      }
    }
  }
  wrefresh(WIN95);
}
