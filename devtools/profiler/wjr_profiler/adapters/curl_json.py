"""Read curl 8.11.1 write-out JSONL without performing network operations."""

import json

from ..model import is_finite_number, is_integer, require

SUPPORTED_VERSION = "8.11.1"
TIMES = ("time_namelookup", "time_connect", "time_pretransfer", "time_starttransfer", "time_total")
PHASES = ("dns", "tcp_connect", "protocol_setup", "first_byte_wait", "body_transfer", "total", "tls")
LIMITS = ["One independent curl process per request; observations never infer repetitions from bytes.",
          "First-byte waiting includes request transmission, network and server waiting; it is not server CPU time.",
          "HTTP has no TLS phase (not applicable); HTTPS, redirects, reuse, retries, proxies and HTTP/2 are not verified.",
          "Body transfer includes waiting for later body bytes; overlapping cumulative curl timestamps are not summed."]


def decode_raw(content):
    records = {}
    for number, line in enumerate(content.decode("utf-8-sig").splitlines(), 1):
        if not line.strip():
            continue
        try:
            row = json.loads(line)
        except ValueError as error:
            raise ValueError(f"curl JSONL line {number}: {error}") from error
        require(isinstance(row, dict), f"curl JSONL line {number}: expected an object")
        records[number] = row
    require(bool(records), "curl JSONL contains no request records")
    return records


def _contract(manifest):
    contract = manifest["producer"].get("curl")
    require(isinstance(contract, dict), "producer.curl collection contract is required")
    expected = dict(process_per_request=True, method="GET", protocol="HTTP/1.1", redirects=False,
                    retries=0, proxy=False, reuse=False)
    require(contract == expected and all(type(contract.get(key)) is type(value) for key, value in expected.items()), "curl collection contract must explicitly declare independent simple HTTP/1.1 GET requests")
    processes = manifest["producer"].get("processes")
    require(isinstance(processes, list), "producer.processes must record line and process exit_status")
    statuses = {}
    for process in processes:
        require(isinstance(process, dict) and is_integer(process.get("line")) and process["line"] > 0
                and is_integer(process.get("exit_status")), "invalid curl process provenance")
        require(process["line"] not in statuses, "duplicate curl process line provenance")
        statuses[process["line"]] = process["exit_status"]
    return statuses


def _request(row, line, process_status):
    pointer = f"/lines/{line}"
    result = dict(line=line, pointer=pointer, exitcode=row.get("exitcode"),
                  process_exit_status=process_status, http_status=row.get("response_code"),
                  http_version=row.get("http_version"), downloaded_bytes=row.get("size_download"),
                  phases_ns={}, reasons=[], curl_version=row.get("curl_version") if isinstance(row.get("curl_version"), str) else "unknown")
    reasons = result["reasons"]
    failed = not is_integer(row.get("exitcode")) or row["exitcode"] != 0 or process_status != 0
    if failed:
        reasons.append("curl transfer/process exit failed or is unknown; partial timings are not successful phases")
    for field, expected in (("method", "GET"), ("scheme", "http"), ("http_version", "1.1"),
                            ("num_redirects", 0), ("num_retries", 0), ("proxy_used", 0),
                            ("num_connects", 1), ("size_upload", 0), ("time_redirect", 0),
                            ("time_appconnect", 0)):
        value = row.get(field)
        if isinstance(value, bool) or value != expected:
            reasons.append(f"unsupported or missing {field}; exact request phases unavailable")
    status = row.get("response_code")
    if not is_integer(status) or not 200 <= status < 300:
        reasons.append("HTTP response is not a successful 2xx response")
        failed = True
    byte_count = row.get("size_download")
    if not is_integer(byte_count) or not is_finite_number(byte_count) or byte_count < 0:
        reasons.append("missing/invalid downloaded byte count")
        result["downloaded_bytes"] = None
    times = [row.get(field) for field in TIMES]
    if not all(is_finite_number(value) and value >= 0 for value in times):
        reasons.append("missing, negative or nonfinite cumulative timestamp")
    elif times != sorted(times):
        reasons.append("nonmonotonic cumulative timestamps; negative phase subtraction refused")
    elif not reasons:
        dns, connected, ready, first, total = times
        result["phases_ns"] = dict(zip(PHASES[:-1],
            (value * 1e9 for value in (dns, connected - dns, ready - connected,
                                      first - ready, total - first, total))))
        if not all(is_finite_number(value) for value in result["phases_ns"].values()):
            result["phases_ns"] = {}
            reasons.append("phase conversion overflow")
    result["quality"] = "failed" if failed else "missing" if reasons else "ok"
    result["tls"] = "not_applicable" if row.get("scheme") == "http" else "unverified"
    return result


def import_artifact(raw, manifest):
    require(isinstance(raw, dict), "decoded curl JSONL records expected")
    statuses = _contract(manifest)
    observations, diagnostics, selected = [], [], set()
    for region in manifest["regions"]:
        workload = manifest["workloads"][region["workload_id"]]
        require(workload.get("scope") == "single_http_request" and
                workload.get("sample_level") == "independent_process_run",
                "curl workload must declare single_http_request and independent_process_run")
        lines = region.get("request_lines")
        require(isinstance(lines, list) and bool(lines) and all(is_integer(x) and x > 0 for x in lines),
                "region.request_lines must select positive JSONL physical line numbers")
        require(len(set(lines)) == len(lines) and not selected.intersection(lines), "duplicate curl request selection")
        selected.update(lines)
        records = []
        for line in lines:
            if line not in raw:
                record = dict(line=line, pointer=f"/lines/{line}", phases_ns={}, quality="missing",
                              reasons=["selected request line is absent"], downloaded_bytes=None, tls="unknown")
            else:
                record = _request(raw[line], line, statuses.get(line))
            records.append(record)
            diagnostics.extend(dict(code=record["quality"], region_id=region["id"], pointer=record["pointer"], message=reason)
                               for reason in record["reasons"])
        versions = sorted({record.get("curl_version") or "unknown" for record in records})
        verified = manifest["producer"]["version"] == SUPPORTED_VERSION and all(
            version.startswith("libcurl/" + SUPPORTED_VERSION + " ") for version in versions)
        problems = [record for record in records if record["quality"] != "ok"]
        quality = "failed" if any(r["quality"] == "failed" for r in records) else "missing" if problems else (
            "exploratory" if not verified else "insufficient_samples" if len(records) < 5 else "ok")
        for metric in (*PHASES, "downloaded_bytes"):
            samples = [] if problems or metric == "tls" else [
                record["downloaded_bytes"] if metric == "downloaded_bytes" else record["phases_ns"][metric]
                for record in records]
            reason = "not_applicable: HTTP has no TLS handshake" if metric == "tls" else (
                "one or more requests failed validation; no successful-only sample selection" if problems else
                "unverified curl version" if not verified else "fewer than five independent requests" if len(records) < 5 else None)
            observations.append(dict(region_id=region["id"], metric=metric,
                kind="measurement" if metric in {"total", "downloaded_bytes"} else "derived",
                unit="bytes/request" if metric == "downloaded_bytes" else "ns/request", scope="single_http_request",
                time_basis="wall", direction="none" if metric == "downloaded_bytes" else "lower",
                samples=samples, sample_level="independent_process_run",
                quality="missing" if metric == "tls" else quality, missing_reason=reason,
                source_locators=[record["pointer"] for record in records],
                metadata=dict(requests=records, curl_versions=versions, capture=manifest["producer"]["curl"],
                              not_applicable=metric == "tls")))
    unselected = set(raw) - selected
    if unselected:
        diagnostics.append(dict(code="unselected_requests", pointer="/", message=f"Unselected raw lines retained: {sorted(unselected)}"))
    return observations, diagnostics
