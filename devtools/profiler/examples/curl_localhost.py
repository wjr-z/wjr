"""Produce curl fixtures only against this script's owned localhost HTTP/1.1 server."""

import argparse
import hashlib
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
import json
import os
from pathlib import Path
import platform
import subprocess
import threading
import time


class Handler(BaseHTTPRequestHandler):
    protocol_version = "HTTP/1.1"

    def log_message(self, *args):
        pass

    def do_GET(self):
        time.sleep(0.015)
        self.send_response(302 if self.path == "/redirect" else 200)
        self.send_header("Content-Length", "128")
        self.send_header("Connection", "close")
        if self.path == "/redirect":
            self.send_header("Location", "/normal")
        self.end_headers()
        self.wfile.write(b"A" * 64)
        self.wfile.flush()
        if self.path != "/partial":
            time.sleep(0.025)
            self.wfile.write(b"B" * 64)
        self.close_connection = True


def capture(output):
    output.mkdir(parents=True, exist_ok=False)
    version = subprocess.check_output(["C:/Windows/System32/curl.exe", "--version"], timeout=10).decode().strip()
    server = ThreadingHTTPServer(("127.0.0.1", 0), Handler)
    thread = threading.Thread(target=server.serve_forever, daemon=True)
    thread.start()
    provenance = dict(real=True, curl_binary="C:/Windows/System32/curl.exe", curl_version=version,
                      platform=platform.platform(), python=platform.python_version(),
                      scope="owned localhost HTTP/1.1 server; one independent curl process per record",
                      header_delay_seconds=0.015, body_chunk_delay_seconds=0.025,
                      expected_body_bytes=128, captures=[])
    try:
        for name, count in (("normal", 6), ("partial", 1), ("redirect", 1)):
            raw, errors, processes = bytearray(), bytearray(), []
            for line in range(1, count + 1):
                url = f"http://127.0.0.1:{server.server_port}/{name}"
                result = subprocess.run(["C:/Windows/System32/curl.exe", "--disable", "--http1.1", "--silent", "--show-error",
                        "--noproxy", "*", "--proxy", "", "--no-location", "--retry", "0",
                        "--max-time", "5", "--header", "Connection: close", "--output", os.devnull,
                        "--write-out", "%{json}\\n", url], capture_output=True, timeout=10)
                json.loads(result.stdout)
                raw.extend(result.stdout)
                errors.extend(result.stderr)
                processes.append(dict(line=line, argv=result.args, exit_status=result.returncode))
            (output / (name + ".jsonl")).write_bytes(raw)
            (output / (name + ".stderr.txt")).write_bytes(errors)
            provenance["captures"].append(dict(name=name, processes=processes,
                raw_sha256=hashlib.sha256(raw).hexdigest(), stderr_sha256=hashlib.sha256(errors).hexdigest()))
    finally:
        server.shutdown()
        server.server_close()
        thread.join()
    (output / "provenance.json").write_text(json.dumps(provenance, indent=2) + "\n", encoding="utf-8", newline="\n")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--producer", choices=("windows-system-curl",), required=True,
                        help="explicit fixed producer profile; arbitrary executable paths are not accepted")
    parser.add_argument("--out", required=True, help="new output directory")
    args = parser.parse_args()
    capture(Path(args.out))
