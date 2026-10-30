"""Guard against publishing incomplete or incomparable benchmark charts."""

import csv
import json
import tempfile
import unittest
from pathlib import Path

import btree_showcase as showcase


class EvidenceValidation(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporary.cleanup)
        self.root = Path(self.temporary.name)
        self.source = json.loads(
            (
                Path(__file__).parents[2]
                / "docs/performance/data/btree-windows-clang18.json"
            ).read_text()
        )
        self.context = self.source["provenance"]
        self.rows = []
        for name, values in self.source["ns_per_operation_samples"].items():
            case, operation, backend = name.split("/")
            count = (
                2048
                if operation == "lower_bound_50"
                else 5120 if operation == "mixed_90_5_5" else showcase.N
            )
            self.rows.extend(
                dict(
                    name=f"{case}/{backend}/{operation}/4096/repeats:5/real_time",
                    run_type="iteration",
                    real_time=value * count,
                    time_unit="ns",
                )
                for value in values
            )

    def run_collection(self, context=None):
        timing = self.root / "timing.json"
        timing.write_text(
            json.dumps(dict(context=context or self.context, benchmarks=self.rows))
        )
        memory = self.root / "memory.csv"
        with memory.open("w", newline="") as stream:
            stream.write(
                f"# source={self.context['wjr_source']} compiler={self.context['wjr_compiler']} "
                f"stdlib={self.context['standard_library']} flags={self.context['wjr_flags']} string_sizeof=32\n"
            )
            fields = ["case", "n", *next(iter(self.source["memory"].values()))]
            writer = csv.DictWriter(stream, fieldnames=fields)
            writer.writeheader()
            for name, values in self.source["memory"].items():
                writer.writerow(dict(case=name, n=4096, **values))
        metadata = self.root / "metadata.json"
        metadata.write_text(json.dumps(self.context))
        output = self.root / "compact.json"
        showcase.collect(timing, memory, metadata, output)
        return json.loads(output.read_text())

    def test_preserves_every_selected_raw_repetition(self):
        result = self.run_collection()
        for name, values in self.source["ns_per_operation_samples"].items():
            self.assertEqual(len(result["ns_per_operation_samples"][name]), 5)
            for actual, expected in zip(
                result["ns_per_operation_samples"][name], values
            ):
                self.assertAlmostEqual(actual, expected)

    def test_requires_strong_comparison_backend(self):
        self.rows = [row for row in self.rows if "/absl/" not in row["name"]]
        with self.assertRaisesRegex(ValueError, "every selected case/backend"):
            self.run_collection()

    def test_requires_five_repetitions(self):
        self.rows.pop()
        with self.assertRaisesRegex(ValueError, "five raw repetitions"):
            self.run_collection()

    def test_rejects_mixed_compilers(self):
        with self.assertRaisesRegex(ValueError, "source/compiler differ"):
            self.run_collection(dict(self.context, wjr_compiler="different"))

    def test_rejects_mixed_standard_libraries(self):
        with self.assertRaisesRegex(ValueError, "standard libraries differ"):
            self.run_collection(dict(self.context, standard_library="different"))

    def test_rejects_nonpositive_time(self):
        self.rows[0]["real_time"] = 0
        with self.assertRaisesRegex(ValueError, "positive"):
            self.run_collection()

    def test_rejects_different_compiler_flags(self):
        with self.assertRaisesRegex(ValueError, "compiler flags differ"):
            self.run_collection(dict(self.context, wjr_flags="different"))


if __name__ == "__main__":
    unittest.main()
