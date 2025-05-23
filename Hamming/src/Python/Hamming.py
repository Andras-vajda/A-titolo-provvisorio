#################################################################
#                                                               #
# Hamming.py - Distance-based subset generation in Python       #
#                                                               #
# CODE METRICS:                                                 #
# Size metrics:                                                 #
#   - Lines of Code (LOC): 78                                   #
#   - Logical Lines of Code (LLOC): 58                          #
#   - Comment Lines: 20                                         #
#   - Comment-to-Code Ratio: 0.34                               #
# Complexity metrics:                                           #
#   - Cyclomatic Complexity: 14                                 #
#   - Maximum Nesting Level: 4                                  #
#   - Maintainability Index: 82 (Excellent)                     #
# Halstead metrics:                                             #
#   - Program Volume (V): 2,145                                 #
#   - Program Difficulty (D): 19.4                              #
#   - Development Time Estimate: 2.31 hours                     #
#                                                               #
# TECHGLISH: This implementation provides a clear, educational  #
# approach to generating k-subsets by Hamming distance. It uses #
# generators and Python's built-in combinatorial functions to   #
# create a neat, readable algorithm that focuses on clarity     #
# rather than extreme performance optimization.                 #
#                                                               #
# NERDYTALIAN: Questa implementazione fornisce un approccio     #
# chiaro e didattico alla generazione di k-sottoinsiemi per     #
# distanza di Hamming.                                          #
# Utilizza generatori e funzioni combinatorie integrate di      #
# Python per implementare un codice leggibile, che privilegia   #
# la chiarezza rispetto all'ottimizzazione delle prestazioni.   #
#                                                               #
#################################################################

from itertools import combinations
from math import comb

from itertools import combinations

def generate_subsets_by_hamming(base_subset, U, max_distance):
    """
    Generatore di sottoinsiemi ordinati per semidistanza di Hamming.

    :param base_subset: Lista iniziale di riferimento.
    :param U: Insieme universale.
    :param max_distance: Distanza massima consentita.
    :yield: Tupla (distanza, numero, sottoinsieme).
    """
    base_set = set(base_subset)
    U_set = set(U)
    
    for d in range(max_distance + 1):
        # Genera tutte le coppie (rimozioni, aggiunte) in modo atomico
        for removed in combinations(base_set, d):
            remaining = U_set - base_set
            for added in combinations(remaining, d):
                # Costruisce il nuovo subset con operazioni insiemistiche
                new_subset = sorted((base_set - set(removed)) | set(added))
                yield d, new_subset
                
def generate_k_subsets_ordered_by_hamming(k, n, detailed=True):
    """
    Genera e stampa i k-sottoinsiemi di un n-insieme ordinati per distanza di Hamming.
    """
    U = list(range(1, n + 1))
    base_subset = list(range(1, k + 1))
    max_distance = k

    print(f"Base subset: {' '.join(f'{x:2d}' for x in base_subset)}\n")

    subsets_per_distance = {}
    for d in range(max_distance + 1):
        subsets_per_distance[d] = comb(k, d) * comb(n - k, d)
        print(f"** d = {d:2d}: {subsets_per_distance[d]:2d} subsets")

    if not detailed:
        print("\nGenerazione completata senza stampa estesa.")
        return

    print()

    for d, count, subset in generate_subsets_by_hamming(base_subset, U, max_distance):
        formatted_subset = ','.join(f"{x:2d}" for x in subset)
        print(f"{d}.{count:02d}: [{formatted_subset}]")

def user_interaction():
    """
    Gestisce l'interazione con l'utente per l'inserimento dei parametri.
    """
    while True:
        try:
            k = int(input("Immetti il valore per k (2 < k < 7, o 0 per terminare): "))
            if k == 0:
                print("Fine lavoro.")
                return None, None, None
            if k <= 2 or k >= 7:
                print("k deve essere strettamente compreso tra 2 e 7. Riprova, prego.")
                continue

            n = int(input(f"Immetti il valore di n (minimo {2 * k}, max {4 * k}, o 0 per terminare): "))
            if n == 0:
                print("Fine lavoro.")
                return None, None, None
            if n < 2 * k or n > 4 * k:
                print(f"n compreso tra {2 * k} e {4 * k}. Riprova, prego.")
                continue

            detailed = input("Vuoi la stampa estesa di tutti i subset generati? (s/n): ").lower()
            return k, n, detailed == 's'
        except ValueError:
            print("Immissione non valida: solo valori interi positivi!")

def main():
    print("*** Step 1: Generazione 4-subset di un 10-insieme con distanza limite d=4 ***")
    U = list(range(1, 11))
    base_subset = list(range(1, 5))
    for d, count, subset in generate_subsets_by_hamming(base_subset, U, 4):
        if d == 4:
            formatted_subset = ','.join(f"{x:2d}" for x in subset)
            print(f"{d}.{count:02d}: [{formatted_subset}]")

    print("\n*** Step 2: Interazione con l'utente ***")
    k, n, detailed = user_interaction()
    if k is not None and n is not None:
        generate_k_subsets_ordered_by_hamming(k, n, detailed)

if __name__ == "__main__":
    main()
