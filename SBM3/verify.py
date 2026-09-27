#!/usr/bin/env python3
"""Oracoli indipendenti; nessuna dipendenza esterna. Python >= 3.9.

Uso: python verify.py [./sbm3 | sbm3.exe] [--slow]
--slow aggiunge la scansione completa n=8 con il kernel ottimizzato.
"""
from collections import defaultdict
from fractions import Fraction
from itertools import combinations, product
from math import comb, factorial
from pathlib import Path
import subprocess
import sys


def stanley(n):
    total = Fraction(0)
    for a in range(n + 1):
        for b in range(n - a + 1):
            c = n - a - b
            total += Fraction(
                (-1)**b * factorial(b + 3*c) * 2**a * 3**b,
                factorial(a) * factorial(b) * factorial(c)**2 * 6**c)
    value = Fraction(factorial(n)**2, 6**n) * total
    if value.denominator != 1:
        raise ArithmeticError("Il risultato di Stanley non e' intero")
    return value.numerator


def ordered_degree_dp(n):
    """DP su istogrammi dei gradi delle colonne; righe ordinate.

    Nessuna formula di Stanley, nessun quoziente per righe, nessun odometro.
    Si scelgono tre colonne distinte fra le classi di grado 0, 1, 2.
    """
    states = {(n, 0, 0, 0): 1}
    choices = [(a, b, 3-a-b) for a in range(4) for b in range(4-a)]
    for _ in range(n):
        nxt = defaultdict(int)
        for hist, count in states.items():
            for chosen in choices:
                if any(chosen[d] > hist[d] for d in range(3)):
                    continue
                h = list(hist)
                ways = 1
                for d, number in enumerate(chosen):
                    h[d] -= number
                    h[d + 1] += number
                    ways *= comb(hist[d], number)
                nxt[tuple(h)] += count * ways
        states = nxt
    return states.get((0, 0, 0, n), 0)


def check(condition, message):
    if not condition:
        raise AssertionError(message)


def fields(text):
    return dict(line.split("=", 1) for line in text.splitlines() if "=" in line)


def main():
    args = [a for a in sys.argv[1:] if a != "--slow"]
    binary = str(Path(args[0] if args else "./sbm3").resolve())
    slow = "--slow" in sys.argv[1:]

    def run(*arguments):
        return subprocess.run([binary, *map(str, arguments)], text=True,
                              capture_output=True, check=True)

    for n in range(13):
        check(stanley(n) == ordered_degree_dp(n), f"oracoli diversi a n={n}")
    check(stanley(11) <= 2**64-1 < stanley(12), "soglia uint64_t errata")
    print("Stanley esatto = DP indipendente per n=0..12; soglia 11/12 confermata.")

    for n in range(3, 8 + int(slow)):
        engines = [()] if n == 8 else [(), ("--reference",)]
        for options in engines:
            data = fields(run(n, *options).stdout)
            check(data["status"] == "complete", "risultato parziale inatteso")
            check(int(data["visited"]) == comb(comb(n, 3)+n-1, n), "candidati")
            check(int(data["matrices_counted"]) == stanley(n), "conteggio C")
            check(int(data["simple_matrices_counted"]) ==
                  factorial(n)*int(data["simple_bases_counted"]), "peso semplice")
        print(f"C n={n}: f(n)={data['matrices_counted']}, basi={data['bases_counted']}, "
              f"basi semplici={data['simple_bases_counted']}")

    # Matrici ordinate per prodotto cartesiano: n=5 ha 10^5 candidati.
    for n in range(3, 6):
        catalog = [row for row in product((0, 1), repeat=n) if sum(row) == 3]
        expected = {tuple("".join(map(str, r)) for r in rows)
                    for rows in product(catalog, repeat=n)
                    if all(sum(r[j] for r in rows) == 3 for j in range(n))}
        lines = run(n, "--emit-matrices").stdout.splitlines()
        check(all(line.startswith("M ") for line in lines), "formato emissione")
        actual = [tuple(line.split()[1:]) for line in lines]
        check(len(actual) == len(set(actual)), "matrici duplicate")
        check(set(actual) == expected, "generazione esplicita incompleta")
        base_lines = run(n, "--emit-bases").stdout.splitlines()
        weights = [int(line.split()[1]) for line in base_lines]
        check(sum(weights) == len(expected), "somma dei pesi delle basi")
    print("Emissione di tutte le matrici: identita' con il prodotto cartesiano per n=3..5.")

    # Design semplici: combinazioni ordinarie di blocchi, senza pesi.
    for n in range(3, 7):
        catalog = list(combinations(range(n), 3))
        expected = {frozenset(blocks) for blocks in combinations(catalog, n)
                    if all(sum(j in block for block in blocks) == 3 for j in range(n))}
        actual = set()
        for line in run(n, "--emit-bases", "--simple").stdout.splitlines():
            parts = line.split()
            check(int(parts[1]) == factorial(n), "denominatore non unitario")
            blocks = [tuple(j for j, value in enumerate(row) if value == "1")
                      for row in parts[2:]]
            check(len(set(blocks)) == n, "blocco ripetuto")
            actual.add(frozenset(blocks))
        check(actual == expected, "design semplici")
    print("Design semplici: identita' con combinazioni di blocchi per n=3..6.")

    bad_arguments = [["2"], ["12"], ["-1"], ["3x"], [""], ["+3"],
                     ["18446744073709551616"], ["--limit"],
                     ["--limit", "-1"], ["--limit", "18446744073709551616"]]
    for arguments in bad_arguments:
        result = subprocess.run([binary, *arguments], capture_output=True)
        check(result.returncode != 0, f"input erroneo accettato: {arguments}")
    for n in (3, 4, 5, 11):
        data = fields(run(n, "--limit", 1).stdout)
        check(int(data["visited"]) == 1, "budget di un candidato")
        check(data["status"] == ("complete" if n == 3 else "PARTIAL"), "fase")
    print("Driver: parsing, limiti, caso terminale n=3 e prefisso n=11 verificati.")


if __name__ == "__main__":
    main()
