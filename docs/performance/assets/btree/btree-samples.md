| Profile | Case | Operation | WJR median [min–max] ns/op | Abseil median [min–max] ns/op | WJR/Abseil |
| --- | --- | --- | ---: | ---: | ---: |
| Linux GCC | uint64 set | Build + destroy | 30.34 [29.53–30.63] | 83.44 [83.31–83.86] | 0.364 |
| Linux GCC | uint64 set | Lower bound (50% keys present) | 18.08 [18.00–18.18] | 21.61 [21.59–21.62] | 0.837 |
| Linux GCC | uint64 set | Mixed 90/5/5 | 44.86 [43.05–46.70] | 79.56 [79.13–79.66] | 0.564 |
| Linux GCC | uint64 set | Key scan | 1.41 [1.41–1.45] | 1.41 [1.41–1.41] | 1.000 |
| Linux GCC | uint64 map | Build + destroy | 53.28 [52.83–54.97] | 71.97 [71.67–72.32] | 0.740 |
| Linux GCC | uint64 map | Lower bound (50% keys present) | 22.32 [22.17–22.77] | 35.39 [35.37–35.60] | 0.631 |
| Linux GCC | uint64 map | Mixed 90/5/5 | 60.64 [60.40–61.91] | 46.95 [46.81–47.10] | 1.292 |
| Linux GCC | uint64 map | Key scan | 1.41 [1.41–1.41] | 1.43 [1.43–1.44] | 0.984 |
| Linux GCC | 64-byte mapped map | Build + destroy | 58.51 [56.96–59.20] | 140.69 [139.53–143.21] | 0.416 |
| Linux GCC | 64-byte mapped map | Lower bound (50% keys present) | 24.09 [24.03–24.81] | 26.46 [26.33–27.15] | 0.910 |
| Linux GCC | 64-byte mapped map | Mixed 90/5/5 | 67.48 [66.83–67.61] | 110.99 [110.89–111.46] | 0.608 |
| Linux GCC | 64-byte mapped map | Key scan | 1.44 [1.44–1.45] | 3.23 [3.20–3.26] | 0.447 |
| Linux CLANG | uint64 set | Build + destroy | 26.12 [25.66–26.66] | 56.09 [53.71–59.78] | 0.466 |
| Linux CLANG | uint64 set | Lower bound (50% keys present) | 14.29 [14.25–14.35] | 16.55 [16.52–16.63] | 0.863 |
| Linux CLANG | uint64 set | Mixed 90/5/5 | 48.12 [47.60–48.54] | 80.45 [80.22–80.67] | 0.598 |
| Linux CLANG | uint64 set | Key scan | 1.41 [1.41–1.41] | 1.41 [1.41–1.41] | 0.998 |
| Linux CLANG | uint64 map | Build + destroy | 43.75 [43.05–44.13] | 65.06 [60.05–71.00] | 0.673 |
| Linux CLANG | uint64 map | Lower bound (50% keys present) | 17.12 [17.07–17.20] | 18.73 [18.68–18.76] | 0.914 |
| Linux CLANG | uint64 map | Mixed 90/5/5 | 61.32 [60.63–61.43] | 85.76 [85.59–86.18] | 0.715 |
| Linux CLANG | uint64 map | Key scan | 1.41 [1.41–1.41] | 1.43 [1.42–1.43] | 0.989 |
| Linux CLANG | 64-byte mapped map | Build + destroy | 47.38 [46.94–49.33] | 138.73 [137.28–141.27] | 0.342 |
| Linux CLANG | 64-byte mapped map | Lower bound (50% keys present) | 18.50 [18.38–18.60] | 29.13 [28.87–29.18] | 0.635 |
| Linux CLANG | 64-byte mapped map | Mixed 90/5/5 | 64.24 [63.73–67.27] | 112.68 [112.16–112.71] | 0.570 |
| Linux CLANG | 64-byte mapped map | Key scan | 1.42 [1.42–1.42] | 3.42 [3.40–3.42] | 0.415 |
| Windows Clang 18 | uint64 set | Build + destroy | 50.54 [49.93–52.08] | 76.93 [76.10–78.53] | 0.657 |
| Windows Clang 18 | uint64 set | Lower bound (50% keys present) | 17.64 [17.07–18.55] | 45.22 [44.42–46.39] | 0.390 |
| Windows Clang 18 | uint64 set | Mixed 90/5/5 | 38.34 [38.06–40.71] | 59.55 [58.16–60.93] | 0.644 |
| Windows Clang 18 | uint64 set | Key scan | 1.15 [1.14–1.17] | 1.17 [1.16–1.24] | 0.989 |
| Windows Clang 18 | uint64 map | Build + destroy | 100.49 [99.31–102.39] | 80.24 [78.92–85.27] | 1.252 |
| Windows Clang 18 | uint64 map | Lower bound (50% keys present) | 21.46 [21.21–22.65] | 53.46 [52.14–55.14] | 0.401 |
| Windows Clang 18 | uint64 map | Mixed 90/5/5 | 48.33 [47.69–49.06] | 63.55 [62.71–64.16] | 0.761 |
| Windows Clang 18 | uint64 map | Key scan | 1.15 [1.14–1.18] | 1.15 [1.14–1.16] | 1.003 |
| Windows Clang 18 | 64-byte mapped map | Build + destroy | 94.74 [92.25–98.18] | 112.16 [111.69–116.46] | 0.845 |
| Windows Clang 18 | 64-byte mapped map | Lower bound (50% keys present) | 22.95 [21.61–23.68] | 67.11 [65.20–68.72] | 0.342 |
| Windows Clang 18 | 64-byte mapped map | Mixed 90/5/5 | 50.06 [49.71–50.60] | 80.17 [78.81–81.53] | 0.624 |
| Windows Clang 18 | 64-byte mapped map | Key scan | 1.15 [1.13–1.17] | 2.22 [2.12–2.25] | 0.519 |

| Profile | Case | Backend | Live requested bytes after mixed | Peak requested bytes | Allocations (build + mixed) |
| --- | --- | --- | ---: | ---: | ---: |
| Linux GCC | uint64 set | wjr16 | 67232 | 67232 | 393 |
| Linux GCC | uint64 set | absl | 43976 | 43976 | 170 |
| Linux GCC | uint64 map | wjr16 | 132768 | 132768 | 4745 |
| Linux GCC | uint64 map | absl | 88192 | 88448 | 338 |
| Linux GCC | 64-byte mapped map | wjr16 | 362144 | 362144 | 4745 |
| Linux GCC | 64-byte mapped map | absl | 410048 | 410048 | 1326 |
| Linux CLANG | uint64 set | wjr16 | 67232 | 67232 | 393 |
| Linux CLANG | uint64 set | absl | 43976 | 43976 | 170 |
| Linux CLANG | uint64 map | wjr16 | 132768 | 132768 | 4745 |
| Linux CLANG | uint64 map | absl | 88192 | 88448 | 338 |
| Linux CLANG | 64-byte mapped map | wjr16 | 362144 | 362144 | 4745 |
| Linux CLANG | 64-byte mapped map | absl | 410048 | 410048 | 1326 |
| Windows Clang 18 | uint64 set | wjr16 | 68064 | 68064 | 399 |
| Windows Clang 18 | uint64 set | absl | 44488 | 44488 | 177 |
| Windows Clang 18 | uint64 map | wjr16 | 133600 | 133600 | 4751 |
| Windows Clang 18 | uint64 map | absl | 87424 | 87936 | 338 |
| Windows Clang 18 | 64-byte mapped map | wjr16 | 362976 | 362976 | 4751 |
| Windows Clang 18 | 64-byte mapped map | absl | 408328 | 409240 | 1326 |
