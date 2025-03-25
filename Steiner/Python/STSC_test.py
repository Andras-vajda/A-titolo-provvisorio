#!/usr/bin/env python3
# -*- coding: utf-8 -*-
################################################################################
#
# STSC_test.py - Steiner Triple System Verification Utilities
#
# DESCRIPTION:
#   Provides utilities for verifying Steiner Triple Systems (STS) through
#   incidence and Cartesian coverage matrices. Can be used as a standalone
#   verification tool for CSV files or imported by other modules.
#
# METRICS:
#   - Lines of Code (LOC).................: 102
#   - Source Lines of Code (SLOC)..........: 76
#   - Comment Lines (CLOC).................: 18
#   - Comment to Code Ratio (CCR)..........: 0.24
#   - Cyclomatic Complexity (CC)..........: 10
#   - Function Count.....................: 4
#   - Average Function Length.............: 17 SLOC
#
# REFERENCES:
#   [1] Colbourn, C.J., & Rosa, A. (1999). "Triple Systems."
#       Oxford University Press. ISBN: 978-0198535768.
#   [2] Lindner, C.C., & Rodger, C.A. (2008). "Design Theory (2nd Edition)."
#       Chapman & Hall/CRC. ISBN: 978-1420082968.
#
# AUTHOR: M.A.W. 1968
#
# LICENSE: MIT License
# Copyright (C) 2000-2025 M.A.W. 1968
#
################################################################################

import sys
import csv
import numpy as np

def genera_e_verifica_IM(terne, v):
    """
    Generate and verify the incidence matrix (IM) of a Steiner Triple System.
    
    The incidence matrix IM has dimensions b×v where:
    - Each row corresponds to a triple (block)
    - Each column corresponds to an element (point)
    - IM[i,j] = 1 if element j appears in triple i, otherwise 0
    
    For a valid STS, each row must sum to 3 (k=3) and each column must sum to
    r = (v-1)/2 (each element appears in exactly r triples).
    
    Parameters:
        terne (list): The list of triples composing the STS
        v (int): The order of the STS
        
    Returns:
        tuple: (IM, valid) where IM is the incidence matrix and valid is a boolean
               indicating whether the matrix satisfies the STS properties
    """
    # Number of blocks b = |T|
    b = len(terne)
    r = (v - 1) // 2  # Each element must appear exactly r times
    
    # Boolean incidence matrix
    IM = np.zeros((b, v), dtype=int)
    
    # Fill the incidence matrix
    for i, triple in enumerate(terne):
        for val in triple:
            IM[i, val] = 1
    
    # Verify row and column sums
    righe_valide = np.all(np.sum(IM, axis=1) == 3)  # Each row sums to 3
    colonne_valide = np.all(np.sum(IM, axis=0) == r)  # Each column sums to r
    
    return IM, righe_valide and colonne_valide

def genera_e_verifica_CIM(terne, v):
    """
    Generate and verify the Cartesian coverage matrix (CIM) of a Steiner Triple System.
    
    The Cartesian coverage matrix is a v×v matrix where:
    - CIM[i,j] = 1 if elements i and j appear together in a triple, otherwise 0
    
    For a valid STS, each pair of distinct elements must appear together in exactly
    one triple (λ=1), meaning all entries above the main diagonal must be 1.
    
    Parameters:
        terne (list): The list of triples composing the STS
        v (int): The order of the STS
        
    Returns:
        tuple: (CIM, valid) where CIM is the Cartesian coverage matrix and valid is a boolean
               indicating whether all pairs are covered exactly once
    """
    CIM = np.zeros((v, v), dtype=int)
    coppie_coperte = set()

    # Fill the Cartesian matrix and track covered pairs
    for triple in terne:
        for i in range(3):
            for j in range(i + 1, 3):
                CIM[triple[i], triple[j]] = 1
                CIM[triple[j], triple[i]] = 1
                coppie_coperte.add(tuple(sorted([triple[i], triple[j]])))

    # All possible pairs for the complete set
    totale_coppie = {(i, j) for i in range(v) for j in range(i + 1, v)}
    coppie_non_coperte = totale_coppie - coppie_coperte

    if coppie_non_coperte:
        print("** Coppie non coperte:", coppie_non_coperte)

    return CIM, not coppie_non_coperte

def salva_terne_csv(v, terne):
    """
    Save a Steiner Triple System to a CSV file.
    
    The CSV format has a header line with columns a,b,c followed by
    the order v and the number of triples b.
    
    Parameters:
        v (int): The order of the STS
        terne (list): The list of triples composing the STS
        
    Returns:
        None
    """
    nome_file = f"STS_{v}.csv"
    with open(nome_file, mode='w', newline='') as file_csv:
        writer = csv.writer(file_csv, quoting=csv.QUOTE_MINIMAL)
        file_csv.write(f"a,b,c,{v},{len(terne)}\n")
        writer.writerows(terne)
    print(f"Le terne sono state salvate nel file................ \"{nome_file}\"")
    
def verifica_STS_da_csv(nome_file):
    """
    Verify a Steiner Triple System from a CSV file.
    
    The function reads the STS from the file, then verifies it by
    checking its incidence and Cartesian coverage matrices.
    
    Parameters:
        nome_file (str): Path to the CSV file
        
    Returns:
        bool: True if the file contains a valid STS, False otherwise
    """
    try:
        with open(nome_file, mode='r') as file_csv:
            reader = csv.reader(file_csv)
            header = next(reader)  # Read the header
            
            if len(header) < 5 or not header[3].isdigit() or not header[4].isdigit():
                print(f"## Errore: Intestazione del file {nome_file} non valida.")
                return False
            
            v = int(header[3])
            b = int(header[4])
            
            terne = [list(map(int, row)) for row in reader]
        
        if len(terne) != b:
            print(f"## Errore: Il file {nome_file} contiene {len(terne)} blocchi, attesi {b}.")
            return False
        
        IM, valida_IM = genera_e_verifica_IM(terne, v)
        CIM, valida_CIM = genera_e_verifica_CIM(terne, v)
        
        if valida_IM and valida_CIM:
            print(f"** Il file {nome_file} contiene un STS({v}) valido con {b} blocchi.")
        else:
            print(f"## Errore: il file {nome_file} non e' un STS({v}) valido.")
        
        return valida_IM and valida_CIM
    except Exception as e:
        print(f"## Errore durante la verifica del file: {str(e)}")
        return False

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Uso: python STSC_test.py <file_csv>")
        sys.exit(1)
    
    file_csv = sys.argv[1]
    result = verifica_STS_da_csv(file_csv)
    sys.exit(0 if result else 1)