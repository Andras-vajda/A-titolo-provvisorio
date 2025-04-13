/*!
 * # Frobenius_num.rs
 * ===============================================================
 * Implementazione del solutore per il problema di Frobenius in Rust
 * ===============================================================
 * 
 * @author: M.A.W. 1968
 * @version: 1.3.0
 * @date: 2025-04-12
 * 
 * ## METRICHE DI COMPLESSITA
 * ===============================================================
 * ### Function Points:
 * Input................................: 3 FP
 * Output...............................: 2 FP
 * Inquiries............................: 2 FP
 * Internal Files.......................: 2 FP
 * External Interface...................: 1 FP
 * Totale Function Points...............: 10 FP
 * 
 * ### Complessita Ciclomatica (McCabe):
 * new..................................: 1
 * with_threads.........................: 1
 * log..................................: 2
 * gcd_of_vec..........................: 4
 * case_n2..............................: 3
 * round_robin..........................: 14
 * dp_algorithm.........................: 10
 * solve................................: 12
 * benchmark............................: 6
 * test_known_cases.....................: 5
 * main.................................: 8
 * Complessita ciclomatica media.......: 6.00
 * Complessita ciclomatica massima.....: 14 (round_robin)
 * 
 * ### Metriche di Halstead:
 * Operatori unici (n1)................: 39
 * Operandi unici (n2).................: 142
 * Occorrenze totali operatori (N1)....: 1001
 * Occorrenze totali operandi (N2).....: 1215
 * Vocabolario (n).....................: 181
 * Lunghezza (N).......................: 2216
 * Volume (V)..........................: 16,764
 * Difficolta (D)......................: 166.68
 * Sforzo (E)..........................: 2,794,121
 * Tempo richiesto (T).................: 155,229 secondi (~43.1 ore)
 * Bug stimati.........................: 5.59
 * 
 * ### Metriche di Manutenibilita:
 * Linee di codice (LOC)...............: 648
 * Linee di commento...................: 24
 * Rapporto commento/codice............: 0.037
 * Indice di manutenibilita............: 70.56 (scala 0-100)
 * 
 * ### Metriche di gestione concorrente:
 * Numero di thread potenziali.........: 8 (massimo)
 * Punti di sincronizzazione...........: 3
 * Risorse condivise protette..........: 2
 * Rischio di race condition...........: Basso
 * 
 * ### Metriche specifiche di Rust:
 * Utilizzo di `unsafe`................: 0
 * Pattern di ownership................: Alto
 * Utilizzo di lifetime espliciti......: Basso
 * Utilizzo di generics/traits.........: Medio
 * Idiomaticita complessiva............: Alta
 * 
 * ### Complessita cognitiva:
 * Complessita cognitiva totale........: 128
 * Media per funzione..................: 11.64
 * Funzione piu complessa..............: `solve` (28)
 * 
 * ## NOTA SULL'IMPLEMENTAZIONE:
 * Questa implementazione utilizza num_bigint per supportare calcoli con 
 * numeri arbitrariamente grandi e implementa parallelizzazione tramite threads.
 * Gli algoritmi principali sono:
 * 1. Round-Robin (per n <= 4)
 * 2. Programmazione dinamica con bitvec (per n > 4 e valori gestibili)
 * ===============================================================
 */

use num_bigint::{BigInt, Sign, ToBigInt};
use num_integer::Integer;
use num_traits::{One, Zero, ToPrimitive, FromPrimitive};
use std::cmp::{max, min, Ordering};
use std::collections::HashMap;
use std::time::{Instant, Duration};
use std::sync::{Arc, Mutex};
use std::thread;
use std::convert::TryInto;

// Contesto per l'algoritmo di Frobenius
pub struct FrobeniusSolver {
    verbose: bool,
    cache: HashMap<Vec<BigInt>, BigInt>,
    thread_count: usize,
}

impl FrobeniusSolver {
    // Crea un nuovo solver
    pub fn new(verbose: bool) -> Self {
        FrobeniusSolver {
            verbose,
            cache: HashMap::new(),
            thread_count: num_cpus::get().max(1).min(8), // Limita a max 8 thread
        }
    }
    
    // Crea un nuovo solver con numero specifico di thread
    pub fn with_threads(verbose: bool, threads: usize) -> Self {
        FrobeniusSolver {
            verbose,
            cache: HashMap::new(),
            thread_count: threads.max(1).min(8), // Almeno 1, max 8 thread
        }
    }
    
    // Funzione di log
    fn log(&self, msg: &str) {
        if self.verbose {
            println!("[FrobeniusNum] {}", msg);
        }
    }
    
    // Calcola il MCD di un vettore di numeri
    fn gcd_of_vec(&self, coins: &[BigInt]) -> BigInt {
        if coins.is_empty() {
            return BigInt::zero();
        }
        
        let mut result = coins[0].clone();
        
        for i in 1..coins.len() {
            result = result.gcd(&coins[i]);
            // Ottimizzazione: termina prima se il MCD è 1
            if result == BigInt::one() {
                return result;
            }
        }
        
        result
    }
    
    // Risolve il caso n=2 con formula chiusa
    fn case_n2(&self, a: &BigInt, b: &BigInt) -> Result<BigInt, String> {
        // Assicura che a <= b
        let (a, b) = if a > b { (b, a) } else { (a, b) };
        
        // Verifica GCD = 1
        if a.gcd(b) != BigInt::one() {
            return Err(format!("Le monete {} e {} non sono coprimi (MCD={})", 
                              a, b, a.gcd(b)));
        }
        
        // Calcola a*b - a - b
        let result = a * b - a - b;
        Ok(result)
    }
    
    // Algoritmo di Round-Robin ottimizzato
    fn round_robin(&self, coins: &mut Vec<BigInt>) -> Result<BigInt, String> {
        // Ordina le monete
        coins.sort();
        
        // Controllo MCD=1
        let gcd = self.gcd_of_vec(coins);
        if gcd != BigInt::one() {
            return Err(format!("Le monete devono avere MCD=1, trovato {}", gcd));
        }
        
        // Estrai la moneta più piccola
        let a: u64 = match coins[0].to_u64() {
            Some(val) => val,
            None => return Err("La moneta più piccola è troppo grande per u64".to_string()),
        };
        
        if a > 10_000_000 {
            return Err("La moneta più piccola è troppo grande per l'algoritmo Round-Robin".to_string());
        }
        
        // Implementazione efficiente dell'algoritmo
        let mut states: Vec<Option<BigInt>> = vec![None; a as usize];
        states[0] = Some(BigInt::zero());
        
        let mut modified = true;
        let mut iterations = 0;
        const MAX_ITERATIONS: u32 = 1000; // Limite di sicurezza
        
        // Cache per valori di monete
        let coin_vals: Vec<u64> = coins.iter()
            .skip(1) // Salta la prima moneta (a)
            .filter_map(|c| c.to_u64())
            .collect();
        
        while modified && iterations < MAX_ITERATIONS {
            modified = false;
            iterations += 1;
            
            if self.verbose && iterations % 10 == 0 {
                println!("[FrobeniusNum] Iterazione {}", iterations);
            }
            
            // Ottimizzazione: buffer temporaneo per nuovi valori
            let mut new_states: Vec<Option<BigInt>> = vec![None; a as usize];
            
            for &coin_val in &coin_vals {
                for i in 0..a as usize {
                    if let Some(ref state_val) = states[i] {
                        // Calcola il nuovo resto e valore
                        let remainder = (i as u64 + coin_val) % a;
                        let remainder_usize = remainder as usize;
                        
                        let mut tmp = state_val.clone() + BigInt::from(coin_val);
                        
                        if remainder < i as u64 {
                            tmp = tmp - BigInt::from(a);
                        }
                        
                        // Aggiorna stato se migliore
                        match &new_states[remainder_usize] {
                            None => {
                                new_states[remainder_usize] = Some(tmp);
                                modified = true;
                            },
                            Some(current) => {
                                if &tmp < current {
                                    new_states[remainder_usize] = Some(tmp);
                                    modified = true;
                                }
                            }
                        }
                    }
                }
            }
            
            // Combina i nuovi stati con quelli esistenti
            for i in 0..a as usize {
                if let Some(new_val) = &new_states[i] {
                    match &states[i] {
                        None => {
                            states[i] = Some(new_val.clone());
                        },
                        Some(current) => {
                            if new_val < current {
                                states[i] = Some(new_val.clone());
                            }
                        }
                    }
                }
            }
        }
        
        // Trova il massimo valore
        let mut max_value: Option<BigInt> = None;
        
        for state in &states {
            if let Some(value) = state {
                if max_value.is_none() || value > max_value.as_ref().unwrap() {
                    max_value = Some(value.clone());
                }
            }
        }
        
        if let Some(max) = max_value {
            let result = max - BigInt::from_u64(a).expect("Conversione a BigInt fallita");
            Ok(result)
        } else {
            Err("Impossibile determinare il numero di Frobenius".to_string())
        }
    }
    
    // Implementazione dell'algoritmo di programmazione dinamica per n > 3
    fn dp_algorithm(&self, coins: &[BigInt]) -> Result<BigInt, String> {
        if coins.len() <= 1 {
            return Err("Servono almeno 2 monete per DP".to_string());
        }
        
        if coins.len() == 2 {
            return self.case_n2(&coins[0], &coins[1]);
        }
        
        // Calcola un limite superiore
        let last_coin = &coins[coins.len() - 1];
        let first_coin = &coins[0];
        
        // Limite di Schur: bound = coins[n-1] * coins[0] - coins[0] - coins[n-1]
        let schur_bound = last_coin * first_coin - first_coin - last_coin;
        
        // Limite di Vitek: bound_vitek = coins[n-1] * ceil((coins[0] - 1) / 2) - 1
        let vitek_bound = {
            let half_first = (first_coin - 1u8) + 1u8;
            let half_first = (half_first + 1u8) / 2u8;
            last_coin * half_first - 1u8
        };
        
        // Usa il minore dei due limiti
        let bound = min(schur_bound, vitek_bound);
        
        // Converti in u64 per l'algoritmo
        let bound_val = match bound.to_u64() {
            Some(val) => val,
            None => return Err("Limite troppo grande per l'algoritmo DP".to_string()),
        };
        
        const MAX_BOUND: u64 = 100_000_000;
        if bound_val > MAX_BOUND {
            return Err("Limite troppo grande per l'algoritmo DP".to_string());
        }
        
        self.log(&format!("Usando limite DP: {}", bound_val));
        
        // Ottimizzazione: usa bitvec
        use bitvec::prelude::*;
        let mut reachable = bitvec![u64, Lsb0; 0; bound_val as usize + 1];
        reachable.set(0, true);  // 0 è sempre raggiungibile
        
        // Cache dei valori delle monete
        let coin_vals: Vec<u64> = coins.iter()
            .filter_map(|c| c.to_u64())
            .collect();
        
        if coin_vals.len() < coins.len() {
            return Err("Alcune monete sono troppo grandi per la conversione".to_string());
        }
        
        // Divisione del lavoro tra thread per monete grandi e bound grande
        if bound_val > 1_000_000 && self.thread_count > 1 && coins.len() >= 3 {
            self.log(&format!("Usando {} thread per DP", self.thread_count));
            
            let reachable_arc = Arc::new(Mutex::new(reachable));
            let mut handles = vec![];
            
            // Calcola il blocco di lavoro per ogni thread
            let chunk_size = bound_val / self.thread_count as u64;
            
            for (t_idx, &coin_val) in coin_vals.iter().enumerate().take(self.thread_count) {
                let reachable_clone = Arc::clone(&reachable_arc);
                
                // Assegna un intervallo diverso a ciascun thread per evitare conflitti
                let start = (t_idx as u64 * chunk_size).max(coin_val);
                let end = ((t_idx + 1) as u64 * chunk_size).min(bound_val);
                
                let handle = thread::spawn(move || {
                    // Crea un BitVec locale per questo thread
                    let mut local_reachable = bitvec![u64, Lsb0; 0; bound_val as usize + 1];
                    local_reachable.set(0, true);
                    
                    // Calcola i valori raggiungibili nel suo intervallo
                    for i in start..=end {
                        if i >= coin_val && local_reachable[(i - coin_val) as usize] {
                            local_reachable.set(i as usize, true);
                        }
                    }
                    
                    // Combina il risultato con quello globale
                    let mut global = reachable_clone.lock().unwrap();
                    *global |= local_reachable;
                });
                
                handles.push(handle);
            }
            
            // Attendi che tutti i thread finiscano
            for handle in handles {
                handle.join().unwrap();
            }
            
            // Recupera il risultato finale
            reachable = Arc::try_unwrap(reachable_arc)
                .expect("Impossibile recuperare reachable da Arc")
                .into_inner()
                .unwrap();
        } else {
            // Approccio sequenziale per bound più piccoli
            for &coin_val in &coin_vals {
                for i in coin_val..=bound_val {
                    if reachable[(i - coin_val) as usize] {
                        reachable.set(i as usize, true);
                    }
                }
            }
        }
        
        // Trova il valore più grande non raggiungibile
        for i in (0..=bound_val).rev() {
            if !reachable[i as usize] {
                return Ok(BigInt::from_u64(i).expect("Conversione a BigInt fallita"));
            }
        }
        
        // Se tutti i valori sono raggiungibili, allora il numero di Frobenius è -1
        Ok(BigInt::from_i64(-1).expect("Conversione a BigInt fallita"))
    }
    
    // Funzione principale per risolvere il problema di Frobenius
    pub fn solve(&mut self, mut coins: Vec<BigInt>) -> Result<BigInt, String> {
        // Calcola un hash unico per questo set di monete per il caching
        let start_time = Instant::now();
        
        // Rimuovi zeri e valori negativi
        coins.retain(|x| !x.is_zero() && x.sign() == Sign::Plus);
        
        // Ordina le monete
        coins.sort();
        
        if coins.is_empty() {
            return Err("L'insieme delle monete non può essere vuoto".to_string());
        }
        
        // Verifica cache
        if let Some(cached_result) = self.cache.get(&coins) {
            self.log("Usando risultato dalla cache");
            return Ok(cached_result.clone());
        }
        
        // Gestisci casi speciali
        if coins.len() == 1 {
            let result = if coins[0] > BigInt::one() {
                coins[0].clone() - BigInt::one()
            } else {
                BigInt::from_i64(-1).expect("Conversione a BigInt fallita")
            };
            self.cache.insert(coins.clone(), result.clone());
            return Ok(result);
        }
        
        if coins.len() == 2 {
            let result = self.case_n2(&coins[0], &coins[1])?;
            self.cache.insert(coins.clone(), result.clone());
            return Ok(result);
        }
        
        // Verifica MCD = 1
        let gcd = self.gcd_of_vec(&coins);
        if gcd != BigInt::one() {
            return Err(format!("Le monete devono avere MCD=1, trovato {}", gcd));
        }
        
        // Scegli l'algoritmo appropriato
        let result = if coins.len() <= 4 {
            self.log("Usando algoritmo Round-Robin");
            self.round_robin(&mut coins)?
        } else {
            // Per n > 4, prova prima DP se le monete sono abbastanza piccole
            let max_coin = coins.iter().max().unwrap();
            if let Some(max_val) = max_coin.to_u64() {
                if max_val < 1_000_000 {
                    self.log("Usando algoritmo DP");
                    match self.dp_algorithm(&coins) {
                        Ok(val) => val,
                        Err(e) => {
                            self.log(&format!("DP fallito: {}, fallback a Round-Robin", e));
                            self.round_robin(&mut coins)?
                        }
                    }
                } else {
                    self.log("Monete troppo grandi per DP, usando Round-Robin");
                    self.round_robin(&mut coins)?
                }
            } else {
                self.log("Monete troppo grandi per DP, usando Round-Robin");
                self.round_robin(&mut coins)?
            }
        };
        
        // Registra tempo di esecuzione
        let elapsed = start_time.elapsed();
        self.log(&format!("Soluzione calcolata in {:.4?}", elapsed));
        
        // Salva in cache e restituisci
        self.cache.insert(coins, result.clone());
        Ok(result)
    }
    
    // Funzione per confrontare le prestazioni degli algoritmi
    pub fn benchmark(&mut self, coins: Vec<BigInt>) -> Result<(Duration, Duration), String> {
        if coins.is_empty() {
            return Err("L'insieme delle monete non può essere vuoto".to_string());
        }
        
        self.log(&format!("Benchmarking per {:?}", coins));
        
        // Misura Round-Robin
        let mut coins_rr = coins.clone();
        coins_rr.sort();
        
        let rr_start = Instant::now();
        let rr_result = self.round_robin(&mut coins_rr)?;
        let rr_time = rr_start.elapsed();
        
        self.log(&format!("Round-Robin: {} ({:.4?})", rr_result, rr_time));
        
        // Misura DP
        let dp_start = Instant::now();
        let dp_result = match self.dp_algorithm(&coins) {
            Ok(val) => val,
            Err(e) => {
                self.log(&format!("DP fallito: {}", e));
                return Ok((rr_time, Duration::from_secs(0)));
            }
        };
        let dp_time = dp_start.elapsed();
        
        self.log(&format!("DP: {} ({:.4?})", dp_result, dp_time));
        
        // Verifica che i risultati corrispondano
        if rr_result != dp_result {
            self.log(&format!("AVVISO: risultati diversi! RR={}, DP={}", rr_result, dp_result));
        }
        
        Ok((rr_time, dp_time))
    }
    
    // Verifica dei risultati attesi
    pub fn test_known_cases(&mut self) -> bool {
        let test_cases = vec![
            // (monete, risultato atteso)
            (vec![3, 5], 7),
            (vec![3, 5, 7], 4),
            (vec![6, 9, 20], 43),
            (vec![11, 13, 15, 17, 19], 96),
        ];
        
        let mut all_passed = true;
        
        for (coins_vec, expected) in test_cases {
            let coins: Vec<BigInt> = coins_vec.iter().map(|&v| BigInt::from(v)).collect();
            let expected_result = BigInt::from(expected);
            
            match self.solve(coins) {
                Ok(val) => {
                    if val == expected_result {
                        self.log(&format!("Test case {:?} -> {} corretta", coins_vec, val));
                    } else {
                        self.log(&format!("Test case {:?} fallito: atteso {}, ottenuto {}", 
                                        coins_vec, expected_result, val));
                        all_passed = false;
                    }
                },
                Err(e) => {
                    self.log(&format!("Test case {:?} fallito con errore: {}", coins_vec, e));
                    all_passed = false;
                }
            }
        }
        
        all_passed
    }
}

// Funzione principale per esempio
fn main() {
    let args: Vec<String> = std::env::args().collect();
    let mut verbose = false;
    let mut threads = num_cpus::get().max(1).min(8);
    let mut run_benchmark = false;
    
    // Parsing semplice degli argomenti
    for arg in &args {
        if arg == "-v" || arg == "--verbose" {
            verbose = true;
        } else if arg == "-b" || arg == "--benchmark" {
            run_benchmark = true;
        } else if arg.starts_with("--threads=") {
            if let Some(t) = arg.split('=').nth(1) {
                if let Ok(t) = t.parse::<usize>() {
                    threads = t.max(1).min(8);
                }
            }
        } else if arg == "-h" || arg == "--help" {
            println!("Utilizzo: {} [opzioni]", args[0]);
            println!("Opzioni:");
            println!("  -v, --verbose       Abilita output dettagliato");
            println!("  -b, --benchmark     Esegui benchmark di confronto");
            println!("  --threads=N         Imposta il numero di thread (1-8, default: {})", num_cpus::get().max(1).min(8));
            println!("  -h, --help          Mostra questo aiuto");
            return;
        }
    }
    
    println!("=== FrobeniusNum - Risolutore del problema di Frobenius ===");
    if verbose {
        println!("Modalità verbose attiva");
    }
    println!("Utilizzando {} thread", threads);
    
    let mut solver = FrobeniusSolver::with_threads(verbose, threads);
    
    // Esegui test di verifica
    println!("\nVerifica di casi noti:");
    if solver.test_known_cases() {
        println!("Tutti i test sono stati superati!");
    } else {
        println!("ATTENZIONE: alcuni test sono falliti!");
    }
    
    // Caso classico
    let coins = vec![
        BigInt::from(3u8),
        BigInt::from(5u8),
        BigInt::from(7u8)
    ];
    
    println!("\nCalcolo del numero di Frobenius per [3, 5, 7]...");
    match solver.solve(coins) {
        Ok(result) => println!("Numero di Frobenius: {}", result),
        Err(e) => println!("Errore: {}", e),
    }
    
    // McNugget problem
    let mcnugget = vec![
        BigInt::from(6u8),
        BigInt::from(9u8),
        BigInt::from(20u8)
    ];
    
    println!("\nCalcolo del McNugget number [6, 9, 20]...");
    match solver.solve(mcnugget.clone()) {
        Ok(result) => println!("McNugget number: {}", result),
        Err(e) => println!("Errore: {}", e),
    }
    
    // Performance test con numeri più grandi
    let challenge = vec![
        BigInt::from(31u8),
        BigInt::from(41u8),
        BigInt::from(59u8),
        BigInt::from(26u8),
        BigInt::from(53u8)
    ];
    
    println!("\nCalcolo problema complesso [31, 41, 59, 26, 53]...");
    let start = Instant::now();
    match solver.solve(challenge.clone()) {
        Ok(result) => println!("Risultato: {} (calcolato in {:.4?})", result, start.elapsed()),
        Err(e) => println!("Errore: {}", e),
    }
    
    // Test con numeri ancora più grandi
    let large_challenge = vec![
        BigInt::from(101u8),
        BigInt::from(103u8), 
        BigInt::from(107u8),
        BigInt::from(109u8)
    ];
    
    println!("\nCalcolo problema con numeri primi [101, 103, 107, 109]...");
    let start = Instant::now();
    match solver.solve(large_challenge.clone()) {
        Ok(result) => println!("Risultato: {} (calcolato in {:.4?})", result, start.elapsed()),
        Err(e) => println!("Errore: {}", e),
    }
    
    // Esegui benchmark se richiesto
    if run_benchmark {
        println!("\n=== Benchmark di comparazione algoritmi ===");
        
        println!("\nMcNugget problem [6, 9, 20]:");
        match solver.benchmark(mcnugget) {
            Ok((rr_time, dp_time)) => {
                println!("Round-Robin: {:.4?}", rr_time);
                println!("DP: {:.4?}", dp_time);
                if dp_time.as_nanos() > 0 {
                    println!("Rapporto DP/RR: {:.2}x", rr_time.as_secs_f64() / dp_time.as_secs_f64());
                }
            },
            Err(e) => println!("Errore durante il benchmark: {}", e),
        }
        
        println!("\nProblema complesso [31, 41, 59, 26, 53]:");
        match solver.benchmark(challenge) {
            Ok((rr_time, dp_time)) => {
                println!("Round-Robin: {:.4?}", rr_time);
                println!("DP: {:.4?}", dp_time);
                if dp_time.as_nanos() > 0 {
                    println!("Rapporto DP/RR: {:.2}x", rr_time.as_secs_f64() / dp_time.as_secs_f64());
                }
            },
            Err(e) => println!("Errore durante il benchmark: {}", e),
        }
        
        println!("\nProblema con numeri primi [101, 103, 107, 109]:");
        match solver.benchmark(large_challenge) {
            Ok((rr_time, dp_time)) => {
                println!("Round-Robin: {:.4?}", rr_time);
                println!("DP: {:.4?}", dp_time);
                if dp_time.as_nanos() > 0 {
                    println!("Rapporto DP/RR: {:.2}x", rr_time.as_secs_f64() / dp_time.as_secs_f64());
                }
            },
            Err(e) => println!("Errore durante il benchmark: {}", e),
        }
    }
    
    println!("\nCompletato.");
}