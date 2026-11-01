# Reusing fmt

WJR first reuses an existing `fmt::fmt` target supplied by its parent project.
Otherwise it searches for an installed fmt **11.2.1** config package, matching
the reviewed bundled version, before building `third-party/fmt`. An incompatible
or absent package uses the bundled fallback. This avoids recompiling an available
dependency without silently adopting a different fmt version.

```sh
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/fmt/prefix
```

`fmt_DIR` may instead name the directory containing `fmt-config.cmake`. To force
the bundled version in a standalone build:

```sh
cmake -S . -B build -DCMAKE_DISABLE_FIND_PACKAGE_fmt=ON
```

An existing parent target is always preserved, including when discovery is
disabled. The parent owns its compatibility and build configuration. A parent
requiring an external package can call `find_package(fmt 11.2.1 EXACT CONFIG
REQUIRED)` before adding WJR.

Installed WJR consumers must still be able to find fmt; externally supplied fmt
is not copied into WJR's install prefix. Keep the external dependency's compiler,
CRT, linkage and sanitizer requirements compatible with the consuming build.
This change concerns fmt only; it does not enable additional dependency downloads
or alter the other provider policies.
