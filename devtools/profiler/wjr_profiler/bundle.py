"""Immutable raw-byte bundles with explicit manifests and rebuildable normalized JSON."""

import hashlib
import json
from pathlib import Path

from . import __version__
from .model import require, validate_manifest, validate_observations


def read_json(path):
    return json.loads(Path(path).read_text(encoding="utf-8-sig"))


def write_json(path, document):
    Path(path).write_text(json.dumps(document, indent=2, sort_keys=True, allow_nan=False) + "\n",
                          encoding="utf-8", newline="\n")


def manifest_digest(manifest):
    content = {key: value for key, value in manifest.items() if key != "normalized_sha256"}
    return hashlib.sha256(json.dumps(content, sort_keys=True, allow_nan=False).encode()).hexdigest()


def import_bundle(provider, adapter, raw_path, manifest_path, destination):
    raw_bytes = Path(raw_path).read_bytes()
    manifest = validate_manifest(read_json(manifest_path))
    require(manifest["producer"]["provider"] == provider, "provider does not match manifest.producer")
    require(hashlib.sha256(raw_bytes).hexdigest() == manifest["raw_source"]["sha256"].lower(),
            "raw SHA-256 does not match manifest")
    raw = json.loads(raw_bytes.decode("utf-8-sig"))
    observations, diagnostics = adapter(raw, manifest)
    validate_observations(observations, manifest)
    manifest["raw_source"] = dict(manifest["raw_source"], path="raw/input.json")
    manifest["adapter_version"] = __version__
    normalized = dict(schema_version=1, provider=provider, adapter_version=__version__,
                      manifest_sha256=manifest_digest(manifest), observations=observations, diagnostics=diagnostics)
    # Validate serialization before creating the exclusive output directory.
    json.dumps(normalized, allow_nan=False)
    json.dumps(manifest, allow_nan=False)
    output = Path(destination)
    output.mkdir(parents=True, exist_ok=False)
    (output / "raw").mkdir()
    (output / "raw/input.json").write_bytes(raw_bytes)
    write_json(output / "normalized.json", normalized)
    manifest["normalized_sha256"] = hashlib.sha256((output / "normalized.json").read_bytes()).hexdigest()
    write_json(output / "manifest.json", manifest)
    return normalized


def load_bundle(path):
    root = Path(path).resolve()
    manifest = validate_manifest(read_json(root / "manifest.json"))
    relative = manifest["raw_source"].get("path", "raw/input.json")
    require(isinstance(relative, str), "raw_source.path must be a string")
    raw_path = (root / relative).resolve()
    require(raw_path.is_relative_to(root), "raw_source.path escapes bundle directory")
    require(hashlib.sha256(raw_path.read_bytes()).hexdigest() == manifest["raw_source"]["sha256"].lower(),
            "bundle raw SHA-256 mismatch")
    normalized_bytes = (root / "normalized.json").read_bytes()
    require(hashlib.sha256(normalized_bytes).hexdigest() == manifest.get("normalized_sha256"),
            "normalized SHA-256 mismatch; import raw into a new directory to rebuild")
    normalized = json.loads(normalized_bytes)
    require(isinstance(normalized, dict), "normalized document must be an object")
    require(type(normalized.get("schema_version")) is int and normalized["schema_version"] == 1,
            "unsupported normalized schema_version")
    require(normalized.get("provider") == manifest["producer"]["provider"], "normalized provider mismatch")
    require(normalized.get("manifest_sha256") == manifest_digest(manifest),
            "manifest changed after normalization; import raw into a new directory to rebuild")
    validate_observations(normalized.get("observations"), manifest)
    require(isinstance(normalized.get("diagnostics"), list), "normalized diagnostics must be a list")
    return dict(manifest=manifest, normalized=normalized)
