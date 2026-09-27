#!/usr/bin/env python3
"""Confronta i validatori sullo stesso odometro: Python >= 3.9.

Uso: python benchmark.py [./sbm3 | sbm3.exe] [--n 7] [--runs 5]
Il JSON su stdout puo' essere salvato con > benchmark-locale.json.
"""
import argparse
import json
from pathlib import Path
from statistics import median
import subprocess


def fields(text):
    return dict(line.split("=", 1) for line in text.splitlines() if "=" in line)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("binary", nargs="?", default="./sbm3")
    parser.add_argument("--n", type=int, choices=range(3, 9), default=7)
    parser.add_argument("--runs", type=int, default=5)
    args = parser.parse_args()
    if args.runs < 1:
        parser.error("--runs deve essere positivo")
    binary = str(Path(args.binary).resolve())
    result = {}
    signature = None
    keys = ("visited", "bases_counted", "matrices_counted",
            "simple_bases_counted", "simple_matrices_counted")
    for engine, options in (("cached", []), ("reference", ["--reference"])):
        times = []
        for _ in range(args.runs):
            process = subprocess.run([binary, str(args.n), *options],
                                     capture_output=True, text=True, check=True)
            data = fields(process.stdout)
            if data.get("status") != "complete":
                raise RuntimeError("Il benchmark richiede una scansione completa")
            current = tuple(int(data[key]) for key in keys)
            if signature is None:
                signature = current
            if current != signature:
                raise RuntimeError("I validatori non producono gli stessi contatori")
            if "cpu_seconds" not in data:
                raise RuntimeError("Il driver non dispone di una misura CPU valida")
            times.append(float(data["cpu_seconds"]))
        result[engine] = {"n": args.n, "runs_cpu_seconds": times,
                          "median_cpu_seconds": median(times),
                          "candidates": current[0]}
    cached = result["cached"]["median_cpu_seconds"]
    result["speedup"] = result["reference"]["median_cpu_seconds"] / cached if cached else None
    print(json.dumps(result, indent=2))


if __name__ == "__main__":
    main()
