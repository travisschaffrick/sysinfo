# sysinfo
An ncurses-based system monitor for Linux.

## Features
Monitor
- CPU Usage
- CPU Temp
- Memory Usage
- Load Average
- Uptime
- History graphs for CPU, temp, and memory

## Dependencies:
- ncurses

## Usage:
1. Be on linux
2. `make`
3. `./sysinfo`

## Taster:
```
Menu:
┌──────────────────────────────────────────────────────────┐
│                Select Variables to Monitor               │
│                    Press 's' to start!                   │
│                                                          │
│                       [ ] CPU Info                       │
│                       [ ] CPU Temp                       │
│                       [ ] Memory Info                    │
│                       [ ] Load Avg. Info                 │
│                       [ ] Uptime                         │
│                                                          │
│                                                          │
│                                                          │
│                                                          │
│                                                          │
└──────────────────────────────────────────────────────────┘
```
```
Monitoring:
┌──────────────────────────────────────────────────────────┐
│ 'q' to quit | 's' to change settings                     │
│                                                          │
│                                                          │
│ CPU Usage: 4.17%                                         │
│ ▁▃▂▅▃▁▂▄▁▂▃▁▄▂▁▃▂▄▁▂                                │
│ CPU Temp: 42.75°C                                        │
│ ▃▄▃▄▅▃▄▃▅▄▃▄▃▄▄▅▃▄▅▄                                  │
│ Memory Usage: 6.93GiB/30.46GiB (22.74%)                  │
│ ▂▂▃▂▂▂▃▂▂▃▂▂▂▃▂▂▃▂▂▂                               │
│ Load Avg: 0.26 0.31 0.40                                 │
│ Uptime: 13 hours, 42 minutes, 24 seconds                 │
│                                                          │
│                                                          │
└──────────────────────────────────────────────────────────┘
```

## Known Limitations
- CPU temp scanning supports Intel (coretemp) and AMD (k10temp)
- Terminal resize not supported
