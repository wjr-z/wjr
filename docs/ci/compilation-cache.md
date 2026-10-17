# Linux compilation cache

Linux CI uses the maintained
[ccache action](https://github.com/hendrikmuhs/ccache-action/tree/f09c25b45002a07be2955cbe52e8cee55643f89d)
at v1.2.24. It owns installation, restore/save, the 500 MB working-cache limit,
and cache statistics. The existing FetchContent cache remains responsible for
dependency sources. Windows CI and local CMake defaults are unchanged.

In [Linux run 39030693225](https://github.com/wjr-z/wjr/actions/runs/39030693225),
native jobs spent 89–137 seconds building, 3–11 seconds configuring, and
17–28 seconds running tests. The Clang 20 native job already restored about
58 MB of FetchContent data in one second, yet its build took 104 seconds.
These are observations from that run, before enabling object caching. There
is no measured Linux speedup yet; cold builds also pay cache and launcher costs.

## Shared objects and native objects

The action's cache pool is partitioned by actual OS/image identity, compiler
identity, language standard, and the selected WJR CPU profile.
**The pool is shared across runner CPUs.**
Ordinary dependencies and baseline WJR modules can reuse objects across machines
when their actual compiler inputs match.

The CI-only C/C++ launcher examines the actual compiler arguments, including
response files. Only invocations containing `-march=native`, `-mtune=native`, or
`-mcpu=native` add the native-target fingerprint to `CCACHE_EXTRAFILES`.
This includes tests, benchmarks, or dependencies that request native themselves;
it does not guess from a target name. Other invocations keep ordinary ccache
behavior and any independently supplied extra hash files.

`-march=native` has the same text on different hosts but can select different
instructions and tuning. Before restoring objects, the fingerprint script asks
both selected compilers to expand their native frontend command with `-###`.
It records the complete arguments, including Clang target features and GCC's
cache-tuning `--param` values. An explicit `-mtune=native` probe also records the
tuning CPU when the normal command leaves it implicit. Syntax-only probes avoid
temporary output filenames; they do not change the real build flags.

Missing or ambiguous frontend commands, unresolved native CPUs, missing feature
expansion, or missing native tuning fail the setup step. A native invocation
also fails if its fingerprint file is missing. There is no generic `native`
fallback identity. Current probes target the Linux x64 GCC/Clang CI toolchains;
additional architectures need a reviewed probe before enabling this cache there.

The file lives outside the restored cache directory. Its content enters each
native object's ccache hash, so native objects for several CPUs safely coexist
in the shared pool. Baseline objects do not hash this file. Changing CPUs can
therefore invalidate native results while preserving matching baseline results.
See the official [extra-files and input-hashing documentation](https://ccache.dev/manual/4.11.3.html#_configuration_options).

## Cache lifecycle and correctness

Linux, coverage, pre-commit, and benchmark jobs use `ubuntu-24.04`. GitHub has
[announced](https://github.com/actions/runner-images/issues/14748) a gradual
`ubuntu-latest` migration to Ubuntu 26.04 from October 19 through November 19,
2026, which may change toolchains and system packages. The explicit label
keeps that migration separate from library changes; it is not evidence of a
current failure. Validate Ubuntu 26.04 deliberately before switching. The cache
pool key includes `ImageOS` plus `/etc/os-release` ID and VERSION_ID so a future
OS upgrade cannot silently keep the same pool identity. The label does not
freeze daily image/package updates; ccache still validates actual build inputs.

The action receives no broader `restore-keys`. Its reviewed
[restore implementation](https://github.com/hendrikmuhs/ccache-action/blob/f09c25b45002a07be2955cbe52e8cee55643f89d/src/restore.ts)
uses the complete toolchain/standard/profile key as its prefix; the default
timestamp suffix lets later runs save updated objects. A matching pool is only
transport: ccache still checks individual source, header, compiler-option, and
extra-file inputs. `CCACHE_COMPILERCHECK=content` checks compiler binary content.
No sloppiness options or whole build directories are cached.

Each pool's working set is limited to 500 MB. GitHub can retain multiple saved
versions under its repository cache quota; this limit is not a repository-wide
storage cap. Restore/save overhead, compressed size, and actual hit rates should
guide later adjustments. GitHub's
[cache access rules](https://docs.github.com/en/actions/reference/workflows-and-actions/dependency-caching)
also mean an initial PR may be cold until a reusable default-branch cache exists.

The launchers are set only by the Linux workflow through
[`CMAKE_<LANG>_COMPILER_LAUNCHER`](https://cmake.org/cmake/help/latest/variable/CMAKE_LANG_COMPILER_LAUNCHER.html).
They do not change flags, exported library requirements, ISA selection, or the
default developer build. NASM compilation and linking continue normally.

## Validation

The fingerprint and launcher guard tests run in the existing Clang 20/C++20 job:

```sh
python3 .github/scripts/test_compiler_fingerprint.py
```

Local checks used the real Clang 18 and GCC 12 native expansions and official
ccache 4.13.6 on Windows; full Linux workflow execution remains a CI check.
An isolated real-ccache fixture observed:

| Change | Baseline | Native |
| --- | --- | --- |
| Initial compile | Miss | Miss |
| Identical compile | Hit | Hit |
| Simulated CPU fingerprint change | Hit | Miss |
| Used header change | Miss | Miss |
| Optimization flag change | Miss | Miss |

A separate CMake/Ninja fixture used both C and C++ launchers, a WJR public
allocator header, native flags, and ASan. Its cold build had two misses, a clean
rebuild had two direct hits, and its runtime CTest passed. These checks establish
cache selection and invalidation behavior, not a cross-machine speedup estimate.
