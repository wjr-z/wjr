| Profile | Case | Operation | WJR median [min–max] ns/op | Abseil median [min–max] ns/op | WJR/Abseil |
| --- | --- | --- | ---: | ---: | ---: |
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
| Windows Clang 18 | uint64 set | wjr16 | 68064 | 68064 | 399 |
| Windows Clang 18 | uint64 set | absl | 44488 | 44488 | 177 |
| Windows Clang 18 | uint64 map | wjr16 | 133600 | 133600 | 4751 |
| Windows Clang 18 | uint64 map | absl | 87424 | 87936 | 338 |
| Windows Clang 18 | 64-byte mapped map | wjr16 | 362976 | 362976 | 4751 |
| Windows Clang 18 | 64-byte mapped map | absl | 408328 | 409240 | 1326 |
