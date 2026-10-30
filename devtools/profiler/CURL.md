<!-- markdownlint-disable MD043 -->

# curl request stages (P05)

This offline provider reads **curl 8.11.1** native `%{json}` JSONL: one HTTP/1.1
GET request per independently started curl process. Python 3.10+ and the standard
library suffice for import and reports. curl and a network connection are needed
only on the producer. No artifact command or URL is executed by the importer.
The older design reference to curl 8.10.1 does not establish support for that version.

From `devtools/profiler`:

```sh
python -m wjr_profiler import --provider curl-json   --raw tests/fixtures/curl/normal.jsonl   --manifest tests/fixtures/curl/normal.manifest.json --out build/curl-bundle
python -m wjr_profiler curl-summary build/curl-bundle --out build/curl-report
```

`report.md` shows each request's status, downloaded bytes and phases in ms.
`report.json` includes ns/request statistics, source line locators and diagnostics.
Each raw request is one sample: the six independent fixture processes give n=6;
a single request has n=1 and variance=null. Bytes and cumulative timestamps never
become extra repetitions. Shared localhost runs demonstrate the format and stage
interpretation, not production network performance or statistical significance.

## Verified semantics

The verified subset is one successful 2xx HTTP/1.1 GET, no upload, TLS, redirects,
retries, proxy or connection reuse. The manifest must explicitly declare the
`producer.curl` contract shown in the fixture and record every selected process's
exit status. Each region selects explicit **physical JSONL line numbers** through
`request_lines`. Different regions cannot reuse the same request. The workload
scope is `single_http_request`, with `independent_process_run` samples.

Native JSONL is preserved byte-for-byte at `raw/input.json` in a bundle. It remains
JSONL despite the common bundle filename. Source locators `/lines/N` identify
physical line N; fields within that line are the native curl JSON object. Blank
lines do not renumber requests. JSON arrays are not a supported producer format.
Manifest, normalized data and raw bytes use the existing bundle SHA-256 checks.

curl reports cumulative seconds from request start. Nonnegative, finite, monotonic
`time_namelookup <= time_connect <= time_pretransfer <= time_starttransfer <= time_total`
are required before these disjoint phases can be formed:

| Phase | Seconds before conversion to ns/request | Meaning |
| --- | --- | --- |
| Name lookup | `time_namelookup` | curl name lookup bookkeeping; the IP-literal fixture does not benchmark a DNS server |
| TCP connect | `time_connect - time_namelookup` | connection establishment after lookup |
| Protocol setup | `time_pretransfer - time_connect` | remaining preparation before transfer |
| First-byte wait | `time_starttransfer - time_pretransfer` | request transmission, network and server waiting; **not pure server CPU** |
| Body transfer | `time_total - time_starttransfer` | transfer after the first byte, including waits for later chunks |
| Total | `time_total` | cumulative request duration, shown separately and never added to phases |
| TLS | not applicable for HTTP | no subtraction from HTTP's zero `time_appconnect` |

The implementation never clamps a negative difference to zero. Missing or
nonfinite timestamps, failed curl/process exit status, partial downloads,
non-2xx responses, redirects, reuse, retry, proxy, HTTPS and HTTP/2 prevent exact
phase interpretation. Raw evidence, response status, received bytes and reasons
remain available. A group containing invalid requests has no successful-only
samples; failures cannot disappear through sample selection. Unsupported versions
are exploratory rather than silently verified. Generic comparison remains
conservative for derived phases, and differing curl backend/build strings block
even exploratory deltas for measured totals. Use the request summary for stage
diagnosis; there is no server CPU attribution, request concurrency model, workload
inference, cross-provider score or regression gate.

## Real fixture and reproduction

`tests/fixtures/curl/provenance.json` records the explicit producer binary,
full version/build, platform, commands, per-process exit status and raw/stderr
SHA-256. The verified binary was **Windows system curl 8.11.1**, distinct from
the Git installation's curl 8.2.1. No public endpoint was contacted.

The standard-library producer starts its own IPv4 loopback server with HTTP/1.1
and `Connection: close`, waits 15 ms before headers, then sends two 64-byte chunks
with a 25 ms gap. Six normal requests are separate curl processes. Additional
real fixtures preserve a partial 64-byte response (curl exit 18) and an unfollowed
302 response. Request/response bodies and header contents are not retained;
JSONL includes native write-out URL, address, byte-count and timestamp metadata.

Reproduce on Windows into a new directory using the explicit fixed producer
profile (the system binary is `C:/Windows/System32/curl.exe`):

```sh
python examples/curl_localhost.py --producer windows-system-curl \
  --out build/curl-capture
```

The helper does not accept arbitrary executable paths. Offline import works on
Linux and Windows; this reproducer uses the verified Windows system binary.
It only accesses its own localhost server; it is not a general URL
collector. It disables curl configuration files, retries, redirect following and
proxy use, requests HTTP/1.1, discards the body and captures native JSON plus stderr.
No collection is run by the offline test suite. To import new evidence, prepare an
explicit manifest based on the fixture, update raw SHA-256, version, processes,
region selections, workload and actual environment. Unknown properties stay
`unknown`; never copy the fixture host identity to a different producer.

```sh
python -m unittest discover -s devtools/profiler/tests -v
```

The fixture raw bytes use local `.gitattributes` rules. Synthetic error tests are
mutations of those records, not additional measured performance evidence. Native
field meanings are pinned to the [curl 8.11.1 write-out specification](https://github.com/curl/curl/blob/curl-8_11_1/docs/cmdline-opts/write-out.md)
and [that release's producer implementation](https://github.com/curl/curl/blob/curl-8_11_1/src/tool_writeout.c).
