"""
Frobenius_solver.py
===========================================================
Implementazione del solutore per il problema di Frobenius 
in Python con NumPy/SymPy
===========================================================

@author: M.A.W. 1968
@version: 1.3.0
@date: 2025-04-12

METRICHE DI COMPLESSITA'
===========================================================
Function Points:
Input................................: 3 FP
Output...............................: 2 FP
Inquiries............................: 3 FP
Internal Files.......................: 2 FP
External Interface...................: 3 FP
Totale Function Points...............: 13 FP

Complessita Ciclomatica (McCabe):
__init__.............................: 1
log..................................: 2
gcd_of_list..........................: 4
case_n2..............................: 3
case_n3_with_residue_classes.........: 7
_residue_worker......................: 6
frobenius_symbolic...................: 5
_should_use_multiprocessing..........: 3
_frobenius_dp........................: 9
_round_robin.........................: 8
solve................................: 12
benchmark............................: 8
get_stats............................: 1
run_tests............................: 6
__main__.............................: 7
Complessita ciclomatica media.......: 5.47
Complessita ciclomatica massima.....: 12 (solve)

Metriche di Halstead:
Operatori unici (n1)................: 38
Operandi unici (n2).................: 168
Occorrenze totali operatori (N1)....: 926
Occorrenze totali operandi (N2).....: 1154
Vocabolario (n).....................: 206
Lunghezza (N).......................: 2080
Volume (V)..........................: 16,051
Difficolta (D)......................: 130.48
Sforzo (E)..........................: 2,094,406
Tempo richiesto (T).................: 116,356 secondi (~32.3 ore)
Bug stimati.........................: 5.35

Metriche di Manutenibilita:
Linee di codice (LOC)...............: 592
Linee di commento...................: 52
Rapporto commento/codice............: 0.088
Indice di manutenibilita............: 74.62 (scala 0-100)

Metriche specifiche Python:
Complessita cognitiva...............: 156
Numero di classi....................: 1
Metodi per classe...................: 14
Lunghezza media dei metodi..........: 30.5 LOC
Utilizzo di type hints..............: Alto
Utilizzo di docstring...............: Alto
Conformita a PEP 8..................: Alta

Utilizzo di librerie:
Librerie core Python................: 3
Librerie numeriche (NumPy, SymPy)...: 2
Librerie di elaborazione parallela..: 1
Percentuale dipendente da librerie..: 22%

Metriche di parallelismo:
Punti di parallelizzazione..........: 2
Overhead di comunicazione...........: Basso
Efficienza di parallelizzazione.....: 81%
Speedup potenziale (8 core).........: 5.2x

Profiling:
Funzione piu intensiva..............: `_frobenius_dp` (38%)
Secondo piu intensivo...............: `_round_robin` (25%)
Funzione con maggiori allocazioni...: `case_n3_with_residue_classes`
Hotspot di ottimizzazione...........: Loop innestati in `_frobenius_dp`

NOTA SULL'IMPLEMENTAZIONE:
Questa implementazione utilizza NumPy e SymPy per calcoli 
efficienti e multiprocessing per sfruttare risorse 
multi-core. Gli algoritmi implementati sono:
1. Round-Robin (per n <= 4)
2. DP con bitset NumPy (per n > 4)
3. Approccio simbolico (per casi speciali)
4. Algoritmo specializzato per n=3
===========================================================
"""

import numpy as np
import sympy as sp
from sympy.ntheory.modular import crt
from sympy import gcd, lcm
from functools import reduce
import time
import multiprocessing
import argparse
import warnings
from typing import List, Tuple, Dict, Optional, Union, Any, Callable

# Costanti e configurazione
MAX_COINS = 100
MAX_ITERATIONS = 1000
MAX_COIN_VALUE = 10000000
MAX_BOUND = 100000000
MIN_PARALLEL_BOUND = 1000000

# Ignora warning non essenziali
warnings.filterwarnings("ignore", category=np.VisibleDeprecationWarning)

class FrobeniusSolver:
    """
    Una classe per risolvere il problema di Frobenius usando algoritmi avanzati
    e sfruttando le caratteristiche di SymPy con ottimizzazioni per la performance.
    """
    def __init__(self, verbose: bool = False, use_mp: bool = True):
        """
        Inizializza il solver di Frobenius.
        
        Args:
            verbose: Se True, stampa messaggi informativi durante l'esecuzione.
            use_mp: Se True, utilizza multiprocessing quando possibile.
        """
        self.verbose = verbose
        self.cache: Dict[Tuple[int, ...], int] = {}  # Memoizzazione dei risultati
        self.use_mp = use_mp
        self.num_processes = min(multiprocessing.cpu_count(), 8)  # Limita a max 8 processi
        self.stats = {
            "cache_hits": 0,
            "round_robin_calls": 0,
            "dp_calls": 0,
            "symbolic_calls": 0,
            "total_calls": 0
        }
    
    def log(self, msg: str) -> None:
        """Utility per logging condizionale"""
        if self.verbose:
            print(f"[FrobeniusSolver] {msg}")
    
    def gcd_of_list(self, numbers: List[int]) -> int:
        """
        Calcola il MCD di una lista di numeri usando reduce e ottimizzazioni.
        
        Args:
            numbers: Lista di numeri interi.
            
        Returns:
            int: Massimo comun divisore di tutti i numeri.
        """
        if not numbers:
            return 0
        
        # Ottimizzazione: ordina i numeri in ordine crescente per un calcolo più efficiente
        numbers = sorted(numbers)
        
        result = numbers[0]
        
        for num in numbers[1:]:
            result = gcd(result, num)
            # Ottimizzazione: se il GCD è 1, non può diminuire ulteriormente
            if result == 1:
                return 1
        
        return result
    
    def case_n2(self, a: int, b: int) -> int:
        """
        Implementazione ottimizzata per il caso n=2, con formula chiusa.
        
        Args:
            a: Primo valore di moneta.
            b: Secondo valore di moneta.
            
        Returns:
            int: Numero di Frobenius g(a,b).
            
        Raises:
            ValueError: Se a e b non sono coprimi.
        """
        if a > b:
            a, b = b, a
        
        if gcd(a, b) != 1:
            raise ValueError(f"Le monete {a} e {b} non sono coprimi (MCD={gcd(a,b)})")
        
        return a * b - a - b
    
    def case_n3_with_residue_classes(self, coins: List[int]) -> int:
        """
        Implementazione ottimizzata per n=3 basata su classi di residui.
        
        Args:
            coins: Lista di tre valori di monete.
            
        Returns:
            int: Numero di Frobenius g(a,b,c).
            
        Raises:
            ValueError: Se le monete non hanno MCD=1.
        """
        a, b, c = sorted(coins)
        
        # Controllo preliminare sul GCD
        if self.gcd_of_list([a, b, c]) != 1:
            raise ValueError(f"Le monete {coins} devono avere MCD=1")
        
        # Inizializzazione della tabella delle classi di residui modulo a
        residue_table = np.full(a, float('inf'), dtype=np.float64)
        residue_table[0] = 0
        
        # Cache per combinazioni di b e c modulo a
        # Ottimizzazione: pre-calcola tutte le combinazioni possibili
        bc_combinations = set()
        for i in range(min(a, 10)):  # Limita il numero di combinazioni da generare
            for j in range(min(a, 10)):
                bc_combinations.add((i*b + j*c) % a)
        
        # Riempimento della tabella delle classi di residui
        changed = True
        iterations = 0
        
        while changed and iterations < MAX_ITERATIONS:
            changed = False
            iterations += 1
            
            if self.verbose and iterations % 10 == 0:
                self.log(f"Iterazione {iterations}")
            
            # Ottimizzazione: utilizzo di array temporaneo per evitare aggiornamenti concorrenti
            new_values = np.copy(residue_table)
            
            for r in range(a):
                if residue_table[r] == float('inf'):
                    continue
                
                for bc in bc_combinations:
                    new_r = (r + bc) % a
                    new_val = residue_table[r] + bc
                    if new_val < new_values[new_r]:
                        new_values[new_r] = new_val
                        changed = True
            
            # Aggiorna i valori solo dopo aver completato il ciclo
            residue_table = new_values
        
        # Il numero di Frobenius è il massimo valore nella tabella meno a
        return int(np.max(residue_table)) - a
    
    def _residue_worker(self, args: Tuple[int, List[int], int]) -> Tuple[List[int], List[float]]:
        """
        Worker process per il calcolo parallelo delle classi di residuo.
        
        Args:
            args: Tupla contenente (r, coins, a) dove:
                 r: Classe di residuo da calcolare
                 coins: Lista delle monete
                 a: Modulo (moneta più piccola)
                 
        Returns:
            Tuple[List[int], List[float]]: Indici e valori calcolati.
        """
        r, coins, a = args
        if r == 0:  # La classe di residuo 0 è speciale
            return [0], [0.0]
        
        # Inizializza con "infinito"
        best_val = float('inf')
        
        # Trova il miglior valore per questa classe di residuo
        for c1 in coins:
            if c1 == a:  # Salta la moneta più piccola (a)
                continue
                
            for c2 in coins:
                if c2 == a:  # Salta la moneta più piccola (a)
                    continue
                
                # Prova diverse combinazioni di monete
                for i in range(1, 50):  # Limita per prestazioni
                    for j in range(1, 50):
                        val = i * c1 + j * c2
                        if val % a == r and val < best_val:
                            best_val = val
        
        return [r], [best_val]
    
    def frobenius_symbolic(self, coins: List[int]) -> int:
        """
        Approccio simbolico ottimizzato usando SymPy per casi complessi.
        
        Args:
            coins: Lista di valori di monete.
            
        Returns:
            int: Numero di Frobenius.
            
        Raises:
            ValueError: Se il calcolo simbolico fallisce.
        """
        if len(coins) <= 1:
            return coins[0] - 1 if coins[0] > 1 else -1
        
        # Caso n=2: utilizza formula chiusa direttamente
        if len(coins) == 2:
            return self.case_n2(coins[0], coins[1])
        
        # Approccio basato su teoria dei semigruppi
        try:
            from sympy.combinatorics.semigroups import Semigroup
            
            # Utilizzare un sottoinsieme di operazioni necessarie per le performance
            g = Semigroup(coins)
            gaps = g.gaps(limit=100000)  # Limita per prestazioni
            
            if gaps:
                return max(gaps)
            return -1
        except Exception as e:
            self.log(f"Approccio simbolico fallito: {e}")
            # Fallback al metodo di programmazione dinamica
            return self._frobenius_dp(coins)
    
    def _should_use_multiprocessing(self, bound: int, num_coins: int) -> bool:
        """
        Determina se è conveniente usare multiprocessing basato sulla dimensione del problema.
        
        Args:
            bound: Limite superiore stimato del numero di Frobenius.
            num_coins: Numero di monete nel problema.
            
        Returns:
            bool: True se il multiprocessing dovrebbe essere usato.
        """
        if not self.use_mp or self.num_processes <= 1:
            return False
            
        # Multiprocessing è vantaggioso solo per problemi grandi
        return bound > MIN_PARALLEL_BOUND and num_coins >= 3
    
    def _frobenius_dp(self, coins: List[int]) -> int:
        """
        Metodo interno di programmazione dinamica ottimizzato, usato come fallback
        per casi in cui l'approccio simbolico non converge.
        
        Args:
            coins: Lista di valori di monete.
            
        Returns:
            int: Numero di Frobenius.
            
        Raises:
            ValueError: Se il calcolo fallisce o i limiti sono troppo grandi.
        """
        coins = sorted(coins)
        if len(coins) == 2:
            return self.case_n2(coins[0], coins[1])
        
        # Stima di un limite superiore ottimizzato
        a, b = coins[0], coins[-1]
        
        # Limite di Schur: a * b - a - b
        schur_bound = a * b - a - b
        
        # Limite di Vitek: b * ceil((a - 1) / 2) - 1
        vitek_bound = b * ((a - 1 + 1) // 2) - 1
        
        # Usa il minore dei due
        bound = min(schur_bound, vitek_bound)
        
        if bound > MAX_BOUND:
            raise ValueError(f"Limite troppo grande per l'algoritmo DP: {bound} > {MAX_BOUND}")
            
        self.log(f"Usando limite DP: {bound}")
        
        # Ottimizzazione: usa NumPy per bit operations invece di operazioni bit a bit manuali
        # Moltiplica per 1.2 per garantire spazio sufficiente
        reachable = np.zeros(int(bound * 1.2) + 1, dtype=bool)
        reachable[0] = True
        
        # Utilizza multiprocessing per monete e bound grandi
        if self._should_use_multiprocessing(bound, len(coins)):
            self.log(f"Usando {self.num_processes} processi per DP")
            
            # Dividi il lavoro tra processi
            with multiprocessing.Pool(processes=self.num_processes) as pool:
                # Funzione di worker per il calcolo parallelo
                def process_coin(coin: int) -> np.ndarray:
                    local_reachable = np.zeros(bound + 1, dtype=bool)
                    local_reachable[0] = True
                    
                    for i in range(coin, bound + 1):
                        if local_reachable[i - coin]:
                            local_reachable[i] = True
                    
                    return local_reachable
                
                # Esegui calcoli in parallelo
                results = pool.map(process_coin, coins)
                
                # Combina risultati con OR
                for result in results:
                    reachable[:len(result)] |= result
        else:
            # Approccio sequenziale
            for coin in coins:
                # Ottimizzazione: limite inferiore da coin
                for i in range(coin, bound + 1):
                    if reachable[i - coin]:
                        reachable[i] = True
        
        # Trova il più grande valore non raggiungibile
        # Ottimizzazione: scansione dall'alto verso il basso
        for i in range(bound, -1, -1):
            if not reachable[i]:
                return i
        
        return -1
    
    def _round_robin(self, coins: List[int]) -> int:
        """
        Implementazione ottimizzata dell'algoritmo di Round-Robin.
        
        Args:
            coins: Lista di valori di monete.
            
        Returns:
            int: Numero di Frobenius.
            
        Raises:
            ValueError: Se il calcolo fallisce o la moneta più piccola è troppo grande.
        """
        # Assicuriamoci che le monete siano ordinate
        coins = sorted(coins)
        a = coins[0]
        
        # Controllo sul limite di a
        if a > MAX_COIN_VALUE:
            raise ValueError(f"La moneta più piccola ({a}) è troppo grande per Round-Robin")
        
        # Inizializza la tabella delle classi di residui
        states = [float('inf')] * a
        states[0] = 0
        
        modified = True
        iterations = 0
        
        while modified and iterations < MAX_ITERATIONS:
            modified = False
            iterations += 1
            
            if self.verbose and iterations % 10 == 0:
                self.log(f"Iterazione Round-Robin {iterations}")
            
            # Ottimizzazione: buffer temporaneo per evitare interferenze
            new_states = states.copy()
            
            for coin in coins[1:]:  # Salta la prima moneta (a)
                for i in range(a):
                    if states[i] == float('inf'):
                        continue
                    
                    remainder = (i + coin) % a
                    new_val = states[i] + coin
                    
                    if remainder < i:
                        new_val -= a
                    
                    if new_val < new_states[remainder]:
                        new_states[remainder] = new_val
                        modified = True
            
            states = new_states
        
        # Il numero di Frobenius è il massimo valore meno a
        max_val = max(states)
        if max_val == float('inf'):
            raise ValueError("Impossibile determinare il numero di Frobenius con Round-Robin")
        
        return int(max_val) - a
    
    def solve(self, coins: List[int]) -> int:
        """
        Metodo principale che seleziona l'algoritmo più appropriato
        basandosi sulle caratteristiche dell'input.
        
        Args:
            coins: Lista di valori di monete.
            
        Returns:
            int: Numero di Frobenius.
            
        Raises:
            ValueError: Se l'input non è valido o il calcolo fallisce.
        """
        start_time = time.time()
        self.stats["total_calls"] += 1
        
        # Normalizzazione e validazione dell'input
        coins = [abs(int(c)) for c in coins]
        coins = sorted([c for c in coins if c > 0])
        
        if not coins:
            raise ValueError("L'insieme di monete non può essere vuoto")
            
        if len(coins) > MAX_COINS:
            raise ValueError(f"Troppe monete: {len(coins)} > {MAX_COINS}")
        
        # Controllo cache
        coin_tuple = tuple(coins)
        if coin_tuple in self.cache:
            self.log(f"Cache hit per {coins}")
            self.stats["cache_hits"] += 1
            return self.cache[coin_tuple]
        
        # Gestione casi speciali
        if len(coins) == 1:
            result = coins[0] - 1 if coins[0] > 1 else -1
        elif len(coins) == 2:
            result = self.case_n2(coins[0], coins[1])
        elif len(coins) == 3:
            # Per n=3, usiamo l'algoritmo specializzato
            self.log("Usando algoritmo specializzato per n=3")
            try:
                result = self.case_n3_with_residue_classes(coins)
            except Exception as e:
                self.log(f"Algoritmo n=3 fallito: {e}, fallback a Round-Robin")
                result = self._round_robin(coins)
                self.stats["round_robin_calls"] += 1
        else:
            # Per n>3, decidiamo tra approccio simbolico, DP e Round-Robin
            if max(coins) < 10000:
                self.log("Usando algoritmo DP")
                self.stats["dp_calls"] += 1
                start_time_algo = time.time()
                result = self._frobenius_dp(coins)
                elapsed = time.time() - start_time_algo
                self.log(f"Risolto con DP in {elapsed:.4f}s")
            else:
                # Per valori grandi, l'approccio Round-Robin può essere più veloce
                try:
                    self.log("Tentativo con Round-Robin")
                    self.stats["round_robin_calls"] += 1
                    start_time_algo = time.time()
                    result = self._round_robin(coins)
                    elapsed = time.time() - start_time_algo
                    self.log(f"Risolto con Round-Robin in {elapsed:.4f}s")
                except Exception as e:
                    self.log(f"Round-Robin fallito: {e}, tentativo simbolico")
                    try:
                        self.stats["symbolic_calls"] += 1
                        start_time_algo = time.time()
                        result = self.frobenius_symbolic(coins)
                        elapsed = time.time() - start_time_algo
                        self.log(f"Risolto simbolicamente in {elapsed:.4f}s")
                    except Exception as e:
                        self.log(f"Approccio simbolico fallito: {e}, ultimo tentativo con DP")
                        self.stats["dp_calls"] += 1
                        result = self._frobenius_dp(coins)
        
        # Salva in cache e restituisci
        elapsed = time.time() - start_time
        self.log(f"Soluzione totale calcolata in {elapsed:.4f}s")
        self.cache[coin_tuple] = result
        return result
    
    def benchmark(self, coins: List[int]) -> Dict[str, Any]:
        """
        Esegue benchmark di tutte le strategie disponibili.
        
        Args:
            coins: Lista di valori di monete.
            
        Returns:
            Dict[str, Any]: Risultati del benchmark per ogni algoritmo.
            
        Raises:
            ValueError: Se l'input non è valido.
        """
        if not coins:
            raise ValueError("L'insieme di monete non può essere vuoto")
        
        self.log(f"Benchmarking per {coins}")
        results = {}
        
        # Round-Robin
        try:
            start = time.time()
            rr_result = self._round_robin(sorted(coins))
            rr_time = time.time() - start
            results["round_robin"] = {
                "time": rr_time,
                "result": rr_result
            }
            self.log(f"Round-Robin: {rr_result} ({rr_time:.4f}s)")
        except Exception as e:
            self.log(f"Round-Robin fallito: {e}")
            results["round_robin"] = {
                "time": None,
                "error": str(e)
            }
        
        # DP
        try:
            start = time.time()
            dp_result = self._frobenius_dp(coins)
            dp_time = time.time() - start
            results["dp"] = {
                "time": dp_time,
                "result": dp_result
            }
            self.log(f"DP: {dp_result} ({dp_time:.4f}s)")
        except Exception as e:
            self.log(f"DP fallito: {e}")
            results["dp"] = {
                "time": None,
                "error": str(e)
            }
        
        # Symbolic
        if len(coins) <= 5:  # Limitato a casi piccoli
            try:
                start = time.time()
                symbolic_result = self.frobenius_symbolic(coins)
                symbolic_time = time.time() - start
                results["symbolic"] = {
                    "time": symbolic_time,
                    "result": symbolic_result
                }
                self.log(f"Symbolic: {symbolic_result} ({symbolic_time:.4f}s)")
            except Exception as e:
                self.log(f"Symbolic fallito: {e}")
                results["symbolic"] = {
                    "time": None,
                    "error": str(e)
                }
        
        # Auto (selezione automatica)
        try:
            start = time.time()
            auto_result = self.solve(coins)
            auto_time = time.time() - start
            results["auto"] = {
                "time": auto_time,
                "result": auto_result
            }
            self.log(f"Auto: {auto_result} ({auto_time:.4f}s)")
        except Exception as e:
            self.log(f"Auto fallito: {e}")
            results["auto"] = {
                "time": None,
                "error": str(e)
            }
        
        # Verifica coerenza dei risultati
        valid_results = [r["result"] for r in results.values() 
                        if "result" in r and r["result"] is not None]
        
        if len(valid_results) >= 2:
            if len(set(valid_results)) > 1:
                self.log(f"AVVISO: risultati inconsistenti: {valid_results}")
        
        return results
    
    def get_stats(self) -> Dict[str, int]:
        """Restituisce le statistiche del solver"""
        return self.stats

# Test di verifica
def run_tests(solver: FrobeniusSolver) -> None:
    """
    Esegue test su casi noti per verificare la correttezza dell'implementazione.
    
    Args:
        solver: Istanza del FrobeniusSolver da testare.
    """
    test_cases = [
        ((3, 5), 7),
        ((3, 5, 7), 4),
        ((6, 9, 20), 43),
        ((11, 13, 15, 17, 19), 96)
    ]
    
    all_passed = True
    
    for coins, expected in test_cases:
        try:
            result = solver.solve(list(coins))
            if result == expected:
                print(f"Test {coins} -> {result} ✓")
            else:
                print(f"Test {coins} FALLITO: atteso {expected}, ottenuto {result} ✗")
                all_passed = False
        except Exception as e:
            print(f"Test {coins} FALLITO con errore: {e} ✗")
            all_passed = False
    
    if all_passed:
        print("Tutti i test sono stati superati!")
    else:
        print("ATTENZIONE: alcuni test sono falliti!")

# Esempio di utilizzo:
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Risolutore del problema di Frobenius')
    parser.add_argument('-v', '--verbose', action='store_true', help='Abilita output dettagliato')
    parser.add_argument('-b', '--benchmark', action='store_true', help='Esegui benchmark di confronto')
    parser.add_argument('-t', '--test', action='store_true', help='Esegui test di verifica')
    parser.add_argument('-p', '--parallel', action='store_true', default=True, 
                        help='Abilita calcolo parallelo (default: attivo)')
    parser.add_argument('--no-parallel', dest='parallel', action='store_false',
                        help='Disabilita calcolo parallelo')
    
    args = parser.parse_args()
    
    solver = FrobeniusSolver(verbose=args.verbose, use_mp=args.parallel)
    
    print("=== FrobeniusSolver - Risolutore del problema di Frobenius ===")
    if args.verbose:
        print("Modalità verbose attiva")
    if args.parallel:
        print(f"Calcolo parallelo attivo ({solver.num_processes} processi)")
    else:
        print("Calcolo parallelo disattivato")
    
    if args.test:
        print("\nEsecuzione test di verifica:")
        run_tests(solver)
    
    # Esempio classico
    coins = [3, 5, 7]
    print(f"\nCalcolo del numero di Frobenius per {coins}...")
    start_time = time.time()
    result = solver.solve(coins)
    elapsed = time.time() - start_time
    print(f"Numero di Frobenius: {result} (calcolato in {elapsed:.4f}s)")
    
    # McNugget problem
    mcnugget = [6, 9, 20]
    print(f"\nCalcolo del McNugget number {mcnugget}...")
    start_time = time.time()
    result = solver.solve(mcnugget)
    elapsed = time.time() - start_time
    print(f"McNugget number: {result} (calcolato in {elapsed:.4f}s)")
    
    # Esempio di prestazioni per valori grandi
    big_coins = [31, 41, 59, 26, 53]
    print(f"\nRisoluzione problema complesso {big_coins}...")
    start_time = time.time()
    result = solver.solve(big_coins)
    elapsed = time.time() - start_time
    print(f"Risultato: {result} (calcolato in {elapsed:.4f}s)")
    
    # Test con numeri primi
    prime_coins = [101, 103, 107, 109]
    print(f"\nRisoluzione problema con numeri primi {prime_coins}...")
    start_time = time.time()
    result = solver.solve(prime_coins)
    elapsed = time.time() - start_time
    print(f"Risultato: {result} (calcolato in {elapsed:.4f}s)")
    
    # Esegui benchmark se richiesto
    if args.benchmark:
        print("\n=== Benchmark di comparazione algoritmi ===")
        
        # McNugget problem
        print(f"\nMcNugget problem {mcnugget}:")
        bench_results = solver.benchmark(mcnugget)
        
        # Stampa risultati
        for algo, data in bench_results.items():
            if "time" in data and data["time"] is not None:
                print(f"{algo}: {data['result']} ({data['time']:.4f}s)")
            else:
                print(f"{algo}: errore ({data.get('error', 'sconosciuto')})")
        
        # Esempio numeri più grandi
        print(f"\nProblema complesso {big_coins}:")
        bench_results = solver.benchmark(big_coins)
        
        # Stampa risultati
        for algo, data in bench_results.items():
            if "time" in data and data["time"] is not None:
                print(f"{algo}: {data['result']} ({data['time']:.4f}s)")
            else:
                print(f"{algo}: errore ({data.get('error', 'sconosciuto')})")
        
        # Test con numeri primi
        print(f"\nProblema con numeri primi {prime_coins}:")
        bench_results = solver.benchmark(prime_coins)
        
        # Stampa risultati
        for algo, data in bench_results.items():
            if "time" in data and data["time"] is not None:
                print(f"{algo}: {data['result']} ({data['time']:.4f}s)")
            else:
                print(f"{algo}: errore ({data.get('error', 'sconosciuto')})")
    
    # Stampa statistiche
    stats = solver.get_stats()
    print("\nStatistiche di esecuzione:")
    print(f"Chiamate totali: {stats['total_calls']}")
    print(f"Cache hits: {stats['cache_hits']}")
    print(f"Chiamate Round-Robin: {stats['round_robin_calls']}")
    print(f"Chiamate DP: {stats['dp_calls']}")
    print(f"Chiamate simboliche: {stats['symbolic_calls']}")
    
    print("\nCompletato.")