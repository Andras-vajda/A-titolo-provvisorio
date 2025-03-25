#!/usr/bin/env python3
# -*- coding: utf-8 -*-
################################################################################
#
# STSC_LUT.py - Cyclic Steiner Triple System Generator (LUT-based)
#
# DESCRIPTION:
#   Generator of cyclic Steiner Triple Systems (STS) using the Heffter-Peltesohn
#   method with precomputed difference triples stored in lookup tables.
#   Supports orders v where v ≡ 1 or 3 (mod 6), v ≥ 7, excluding v = 9.
#
# METRICS:
#   - Lines of Code (LOC).................: 122
#   - Source Lines of Code (SLOC)..........: 85
#   - Comment Lines (CLOC).................: 25
#   - Comment to Code Ratio (CCR)..........: 0.29
#   - Cyclomatic Complexity (CC)..........: 11
#   - Function Count.....................: 4
#   - Average Function Length.............: 19 SLOC
#
# REFERENCES:
#   [1] Peltesohn, R. (1939). "Eine Loesung der beiden Heffterschen Differenzenprobleme."
#       Compositio Mathematica, 6, 251-257.
#       URL: http://www.numdam.org/item/CM_1939__6__251_0/
#   [2] Lindner, C.C., & Rodger, C.A. (2008). "Design Theory (2nd Edition)."
#       Chapman & Hall/CRC. ISBN: 978-1420082968.
#   [3] Colbourn, C.J., & Dinitz, J.H. (2006). "Handbook of Combinatorial Designs (2nd Edition)."
#       Chapman & Hall/CRC. ISBN: 978-1584885061.
#
# AUTHOR: M.A.W. 1968
#
# LICENSE: MIT License
# Copyright (C) 2000-2025 M.A.W. 1968
#
################################################################################

# Import the precomputed difference triples
from Peltesohn import TERNE_PELTESOHN

# Import verification and utility functions
from STSC_test import genera_e_verifica_IM, genera_e_verifica_CIM, salva_terne_csv

def genera_STS_ciclico(v, stampa=True):
    """
    Generate a cyclic Steiner Triple System of order v.
    
    This implementation uses the Heffter-Peltesohn method, which constructs
    an STS by generating all possible cyclic shifts of a set of carefully chosen
    "base triples" derived from precomputed difference triples.
    
    Parameters:
        v (int): The order of the STS. Must be v ≡ 1 or 3 (mod 6), v ≥ 7, v ≠ 9.
        stampa (bool): Whether to print the generation process. Default is True.
    
    Returns:
        list: List of triples that form the STS(v).
    
    Note:
        For v ≡ 3 (mod 6), additional "short orbit" triples are added to complete the system.
    """
    STS_triples = []
    counter = 1
    
    # Phase 1: Generate triples from base triples derived from difference triples
    for base_index, triple in enumerate(TERNE_PELTESOHN[v], start=1):
        x, y, z = triple
        base = [0, x, (x + y) % v]
        if stampa:
            print(f"{counter:2d}. Differenza: {triple} -> Base {base_index}: {base} ")
            
        # Generate all cyclic shifts of the base triple
        for i in range(v):
            derived = [(val + i) % v for val in base]
            STS_triples.append(derived)
            if stampa:
                print(f"  {counter:2d}.{i + 1:03d}: {derived}")
        
        counter += 1
            
    # Phase 2: Add "short orbit" triples for v ≡ 3 (mod 6)
    if v % 6 == 3:
        n = (v - 3) // 6
        base_short = [0, 2 * n + 1, 4 * n + 2]
        if stampa:
            print(f"{counter:2d}. Base short orbit: {base_short}")
        
        # Generate 2n+1 cyclic shifts of the short orbit base triple
        for i in range(2 * n + 1):
            derived = [(val + i) % v for val in base_short]
            STS_triples.append(derived)
            if stampa:
                print(f"  {counter:2d}.{i + 1:03d}: {derived} (SO)")
        
        counter += 1

    return STS_triples

def calcola_numero_terne(v):
    """
    Calculate the expected number of triples in an STS of order v.
    
    The formula is b = v(v-1)/6, derived from the combinatorial properties
    of Steiner Triple Systems.
    
    Parameters:
        v (int): The order of the STS.
    
    Returns:
        int: The number of triples.
    """
    return v * (v - 1) // 6

def main():
    """
    Interactive function to generate and verify Steiner Triple Systems.
    
    Allows the user to:
    - Enter a value of v
    - Choose whether to display the triples
    - Save the system to a CSV file
    - Verify the generated system
    
    The function continues in a loop until the user enters 0 to exit.
    """
    print("Cyclic Steiner Triple System Generator (Heffter-Peltesohn Method)")
    print("================================================================")
    
    while True:
        try:
            v = int(input("\nInserisci la dimensione v per STS (0 per terminare): "))
            if v == 0:
                print("\nTerminazione del programma. Arrivederci!")
                break

            if v % 6 not in {1, 3} or v == 9 or v not in TERNE_PELTESOHN:
                print(f"Dimensione non valida. v deve essere ≡ 1 o 3 (mod 6), v ≥ 7, v ≠ 9, e v ≤ 99.")
                continue

            print(f"\nGenerazione STS({v}).................................")

            num_terne = calcola_numero_terne(v)
            print(f"Numero atteso di terne.............................. {num_terne}")
                    
            stampa_a_video = input("Vuoi stampare le terne a video (s/n)?............... ").strip().lower() == 's'
            STS_triples = genera_STS_ciclico(v, stampa_a_video)

            salva = input("Vuoi salvare le terne in un file CSV (s/n)?......... ").strip().lower() == 's'
            if salva:
                salva_terne_csv(v, STS_triples)

            matrice_IM, valida_IM = genera_e_verifica_IM(STS_triples, v)
            if valida_IM:
                print("## Matrice di incidenza (IM) verificata.............. STS valido.")
            else:
                print("** Errore: matrice di incidenza non valida.")

            matrice_CIM, valida_CIM = genera_e_verifica_CIM(STS_triples, v)
            if valida_CIM:
                print("## Matrice di copertura cartesiana (CIM) verificata.. Tutte le coppie coperte.")
            else:
                print("** Errore: matrice di copertura cartesiana non valida.")

            print(f"Totale terne generate............................... {len(STS_triples)}")
        except ValueError:
            print("Input non valido. Inserire un numero intero.")
        except Exception as e:
            print(f"Si è verificato un errore: {str(e)}")

if __name__ == "__main__":
    main()