# Codacy checks and coverage

Codacy provides additional diagnostics. A finding must be checked against the
actual C++ contract, platform guards and lifetime rules before changing code.
The repository's clang-format 18.1.8 configuration remains authoritative.

## Repository settings

The following settings were applied through the repository-scoped Codacy API
on 2026-11-01. They do not change the organization coding standard or quality gates.

| Tool | Setting | Result |
| --- | --- | --- |
| Cppcheck | `algorithmOutOfBounds` | Enabled |
| Cppcheck | `containerOutOfBoundsIndexExpression` | Enabled |
| Cppcheck | `invalidContainerReference` | Enabled |
| Cppcheck | `uninitvar` | Enabled |
| markdownlint | MD013 prose, headings and code blocks | 100 columns |
| markdownlint | MD013 tables | Exempt |
| markdownlint | MD043 required headings | Disabled; documents have different structures |
| Lizard | Function cyclomatic complexity | Threshold 20, previously 8 |
| Lizard | Function non-comment lines | Threshold 100, previously 50 |

Lizard's inherited 500-line file and eight-parameter thresholds remain active.
The function thresholds keep representative test matrices together and still
flag larger functions for review. They do not establish source API requirements.
Cppcheck's additional findings require the same manual review as existing rules;
unused union storage and deliberately uninitialized intrusive metadata are not
automatically bugs.

Existing Flawfinder, Opengrep and Trivy security checks remain enabled, including
Trivy secrets, malicious packages and dependency vulnerability detection.
Clang-Tidy is a client-side tool in Codacy: its enabled setting alone does not
mean Codacy has run it. This workflow does not add a Clang-Tidy scan.

## Coverage uploads

The existing Clang `coverage` target now writes `build/coverage/lcov.info` as
well as the HTML dashboard. It uses the same profile and filename exclusions as
the dashboard, converts source paths to repository-relative forward-slash paths,
and omits files outside the source checkout. No raw profile needs to be uploaded.

HTML, summary and LCOV reports select the repository's `include/` and `src/`
directories explicitly. Legacy SIMD exclusions are scoped below the source root,
so names such as `benchmark` or `build` in the checkout path do not hide sources.

The Coverage workflow continues to run every Monday at 04:43 UTC and on manual
dispatch. Only runs on `main` upload coverage to Codacy and publish Pages.
Other branches produce report artifacts for verification. Pushes and pull requests
do not trigger coverage builds.

Store a Codacy **repository** API token as the GitHub repository Actions secret
`CODACY_PROJECT_TOKEN`. The workflow passes it through the environment to the
official Codacy Coverage Reporter 14.1.3, without putting it in command arguments.
The pinned official downloader verifies the reporter binary's SHA-512 checksum.
If the secret is absent, the workflow emits a notice and retains the HTML and
LCOV artifacts. When configured, an upload failure fails the coverage job.

Coverage in Codacy describes the last uploaded `main` commit and can predate
HEAD. Since PR coverage is not uploaded, this setup does not establish a per-PR
coverage quality gate or change existing quality gate settings.

References: [Codacy API](https://docs.codacy.com/codacy-api/using-the-codacy-api/),
[Coverage Reporter](https://github.com/codacy/codacy-coverage-reporter), and
[llvm-cov export](https://llvm.org/docs/CommandGuide/llvm-cov.html#llvm-cov-export).
