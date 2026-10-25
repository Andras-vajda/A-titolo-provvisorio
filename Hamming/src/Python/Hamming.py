#################################################################
#                                                               #
# Hamming.py - Generazione di sottoinsiemi per distanza Hamming #
#            (C) 1990-2026 M.A.W. 1968 - MIT License            #
#################################################################

from itertools import combinations
from math import comb
from typing import List, Generator, Tuple, Set

def generate_subsets_by_hamming(
    base_subset: List[int],
    U: List[int],
    max_distance: int
) -> Generator[Tuple[int, List[int]], None, None]:
    """
    Genera k-sottoinsiemi ordinati per semidistanza di Hamming dal
    sottoinsieme base.

    :param base_subset: Lista di interi distinti, tutti contenuti in U
    :param U: Lista di interi distinti che rappresenta l'universo
    :param max_distance: Massimo numero di sostituzioni consentite
    :yield: Tupla (distanza, sottoinsieme ordinato)
    :raises ValueError: Duplicati, riferimento o distanza non validi
    """
    # Conversione a set per operazioni efficienti
    base_set: Set[int] = set(base_subset)
    U_set: Set[int] = set(U)

    # Validazione input
    if len(base_set) != len(base_subset):
        raise ValueError("Il base_subset non deve contenere duplicati")
    if len(U_set) != len(U):
        raise ValueError("U non deve contenere duplicati")
    if not base_set.issubset(U_set):
        raise ValueError("Il base_subset deve essere sottoinsieme di U")

    # Complemento e sequenze ordinate calcolati una sola volta
    base_elements = sorted(base_set)
    outside_elements = sorted(U_set - base_set)

    max_possible_d = min(len(base_elements), len(outside_elements))
    if not 0 <= max_distance <= max_possible_d:
        raise ValueError(
            f"Distanza {max_distance} impossibile. "
            f"Massima consentita: {max_possible_d}"
        )

    # Generazione efficiente con operazioni insiemistiche
    for d in range(max_distance + 1):
        yield from (
            (d, sorted((base_set - set(removed)) | set(added)))
            for removed in combinations(base_elements, d)
            for added in combinations(outside_elements, d)
        )
def generate_k_subsets_ordered_by_hamming(
    k: int, n: int, detailed: bool = True
) -> None:
    """
    Genera e stampa i k-sottoinsiemi di un n-insieme ordinati per
    semidistanza di Hamming

    :param k: Cardinalità dei sottoinsiemi
    :param n: Dimensione insieme universo
    :param detailed: Abilita stampa dettagliata
    """
    if not 0 <= k <= n:
        raise ValueError("Occorre 0 <= k <= n")
    max_distance = min(k, n - k)
    U = list(range(1, n + 1))
    base_subset = list(range(1, k + 1))

    # Header informativo
    print(f"\n{' BASE SUBSET ':=^40}")
    print(f"Universo: {U}\nSottoinsieme base: {base_subset}")
    print(f"Numero teorico totale: {comb(n, k)}\n")

    # Calcolo e stampa distribuzione distanze
    distance_distribution = {
        d: comb(k, d) * comb(n - k, d)
        for d in range(max_distance + 1)
    }

    print("{:-^40}".format(" DISTRIBUZIONE DISTANZE "))
    for d, count in distance_distribution.items():
        print(f"d={d:2}: {count:5} sottoinsiemi")

    if not detailed:
        print("\nModalità compatta: sole cardinalità, "
              "enumerazione non eseguita")
        return

    # Generazione e stampa effettiva
    print("\n{:-^40}".format(" SOTTOINSIEMI GENERATI "))
    for d, subset in generate_subsets_by_hamming(
        base_subset, U, max_distance
    ):
        print(f"d={d}: {subset}")

def user_interaction() -> Tuple[int, int, bool]:
    """Gestione input utente con validazione"""

    print("\n{:=^40}".format(" IMPOSTAZIONI "))

    while True:
        try:
            k = int(input("Inserisci k (2 < k < 7, 0 per uscire): "))
            if k == 0:
                return (0, 0, False)
            if not 2 < k < 7:
                raise ValueError

            n = int(input(f"Inserisci n ({2*k} <= n <= {4*k}): "))
            if not 2*k <= n <= 4*k:
                raise ValueError

            detailed = input("Stampa dettagliata? [s/n]: ").lower() == 's'
            return (k, n, detailed)

        except ValueError:
            print("Input non valido. Riprovare.\n")

def main():
    # Esempio dimostrativo preconfigurato
    print("{:=^40}".format(" DEMO AUTOMATICA #1 "))
    generate_k_subsets_ordered_by_hamming(3, 6, detailed=True)

    print("{:=^40}".format(" DEMO AUTOMATICA #2 "))
    generate_k_subsets_ordered_by_hamming(4, 10, detailed=False)

    # Interazione utente
    k, n, detailed = user_interaction()
    if k > 0:
        generate_k_subsets_ordered_by_hamming(k, n, detailed)

    input("Premere un tasto per terminare")

if __name__ == "__main__":
    main()
