ncurses based status line/bar program.

![](6.png)

# Installation

```bash
make
sudo make install
```

# Usage

```bash
# ^B - Blue , ^M - Magenta , ^Y - Yellow
# ^R - Red , ^C - Cyan, ^W - White, ^G - Green
while true; do echo "^BOh ^Mhello ^Ydear, ^Rsome ^Crandom ^Wtext ^G123";sleep 1;done | ./pinky_curses

# CTRL + C to stop it
```

# Requirements

* gcc/clang
* ncurses
* glibc/libc
