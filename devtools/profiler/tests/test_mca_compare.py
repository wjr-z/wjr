"""Real model/CLI integration; changed metadata cases test eligibility, not performance."""

import copy
import hashlib
import json
from pathlib import Path
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))

from wjr_profiler.__main__ import main
from wjr_profiler.adapters.llvm_mca import import_artifact
from wjr_profiler.compare import compare

FIXTURE = ROOT / 'tests/fixtures/llvm_mca'


def bundle():
    raw = json.loads((FIXTURE / 'raw.json').read_bytes())
    manifest = json.loads((FIXTURE / 'baseline.manifest.json').read_text())
    observations, diagnostics = import_artifact(raw, manifest)
    return dict(manifest=manifest, normalized=dict(observations=observations, diagnostics=diagnostics)), raw


class ModelCompareTests(unittest.TestCase):
    def test_same_model_is_not_five_runtime_repetitions(self):
        baseline, _ = bundle()
        candidate = copy.deepcopy(baseline)
        candidate['manifest']['host'] = dict(cpu='different analysis host', os='other OS')
        candidate['manifest']['build'] = dict(compiler='not a compiler run')
        report = compare(baseline, candidate)
        self.assertTrue(all(row['classification'] == 'comparable' for row in report['comparisons']))
        self.assertTrue(all(row['baseline']['statistics']['n'] == 1 for row in report['comparisons']))
        self.assertTrue(all(row['baseline']['statistics']['variance'] is None for row in report['comparisons']))
        self.assertTrue(all(row['absolute_change'] == 0 for row in report['comparisons']))

    def test_model_options_and_region_contract_cannot_be_bypassed(self):
        for change in ('analyzer_version', 'cpu_model', 'target_triple', 'simulation_parameters',
                       'analyzer_options', 'iterations', 'resource_names', 'exit_status'):
            with self.subTest(change=change):
                baseline, _ = bundle()
                candidate = copy.deepcopy(baseline)
                for observation in candidate['normalized']['observations']:
                    observation['metadata'][change] = 'different'
                rows = compare(baseline, candidate, exploratory=True)['comparisons']
                self.assertTrue(all(row['classification'] == 'inconclusive' for row in rows))
                self.assertTrue(all(row['absolute_change'] is None for row in rows))
        baseline, _ = bundle()
        candidate = copy.deepcopy(baseline)
        candidate['manifest']['semantic_contracts']['dependent-identity']['semantics'] = 'different'
        rows = compare(baseline, candidate, exploratory=True)['comparisons']
        self.assertTrue(all(row['absolute_change'] is None for row in rows if row['region_id'] == 'dependent'))

    def test_unknown_options_model_status_and_calls_stay_exploratory(self):
        for change in ('options', 'model', 'status', 'call', 'ignored'):
            with self.subTest(change=change):
                baseline, raw = bundle()
                manifest = copy.deepcopy(baseline['manifest'])
                if change == 'options':
                    del manifest['producer']['options']
                elif change == 'model':
                    raw['TargetInfo']['CPUName'] = 'unknown'
                elif change == 'status':
                    manifest['producer']['exit_status'] = 1
                elif change == 'ignored':
                    manifest['producer']['options'].append('-skip-unsupported-instructions')
                else:
                    raw['CodeRegions'][0]['Instructions'][0] = 'callq callee'
                observations, diagnostics = import_artifact(raw, manifest)
                self.assertTrue(diagnostics)
                candidate = dict(manifest=manifest, normalized=dict(observations=observations, diagnostics=diagnostics))
                rows = compare(baseline, candidate, exploratory=True)['comparisons']
                selected = [row for row in rows if row['region_id'] == 'dependent']
                self.assertTrue(all(row['absolute_change'] is None for row in selected))

    def test_throughput_total_cycles_and_average_are_different(self):
        evidence, _ = bundle()
        rows = {row['metric']: row for row in evidence['normalized']['observations'] if row['region_id'] == 'dependent'}
        self.assertEqual(rows['instructions']['samples'], [200])
        self.assertEqual(rows['block_rthroughput']['metadata']['static_instruction_count'], 2)
        self.assertEqual(rows['block_rthroughput']['samples'], [0.5])
        self.assertEqual(rows['cycles_per_iteration']['samples'], [1.04])
        self.assertEqual(rows['cycles_per_iteration']['metadata']['inherits_kind'], 'estimate')

    def test_real_cli_roundtrip_and_raw_bytes(self):
        digest = hashlib.sha256((FIXTURE / 'raw.json').read_bytes()).hexdigest()
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            for name in ('baseline', 'candidate'):
                self.assertEqual(main(['import', '--provider', 'llvm-mca', '--raw', str(FIXTURE / 'raw.json'),
                                       '--manifest', str(FIXTURE / (name + '.manifest.json')), '--out', str(root / name)]), 0)
                self.assertEqual(hashlib.sha256((root / name / 'raw/input.json').read_bytes()).hexdigest(), digest)
            self.assertEqual(main(['compare', str(root / 'baseline'), str(root / 'candidate'), '--out', str(root / 'report')]), 0)
            report = json.loads((root / 'report/report.json').read_text())
            self.assertTrue(all(row['classification'] == 'comparable' for row in report['comparisons']))
            text = (root / 'report/report.md').read_text()
            self.assertIn('resource_pressure:SKLPort0', text)
            self.assertIn('model', text)


if __name__ == '__main__':
    unittest.main()
