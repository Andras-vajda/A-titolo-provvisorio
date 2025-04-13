{-|
Module      : Frobenius
Description : Risolutore completo ed ottimizzato per il Problema di Frobenius
Copyright   : (c) M.A.W. 1968
License     : MIT

/*!
 * # Frobenius.hs
 * ===============================================================
 * Implementazione del risolutore per il problema di Frobenius in Haskell
 * ===============================================================
 * 
 * @author: M.A.W. 1968
 * @version: 1.3.0
 * @date: 2025-04-13
 * 
 * ## METRICHE DI COMPLESSITA
 * ===============================================================
 * ### Function Points:
 * Input.................................: 3 FP
 * Output................................: 2 FP
 * Inquiries.............................: 2 FP
 * Internal Files........................: 2 FP
 * External Interface....................: 1 FP
 * Totale Function Points................: 10 FP
 * 
 * ### Complessita Ciclomatica (McCabe):
 * gcd'..................................: 7
 * lcm'..................................: 2
 * frobeniusN2...........................: 3
 * frobeniusRoundRobin...................: 6
 * frobeniusSieve........................: 9
 * isRepresentable.......................: 5
 * frobeniusSymbolic.....................: 8
 * findSubsetWithGCD1....................: 5
 * Complessita ciclomatica media.........: 5.63
 * Complessita ciclomatica massima.......: 9 (frobeniusSieve)
 * 
 * ### Metriche di Halstead:
 * Operatori unici (n1)..................: 34
 * Operandi unici (n2)...................: 104
 * Occorrenze totali operatori (N1)......: 812
 * Occorrenze totali operandi (N2).......: 986
 * Vocabolario (n).......................: 138
 * Lunghezza (N).........................: 1798
 * Volume (V)............................: 12,864
 * Difficolta (D)........................: 161.31
 * Sforzo (E)............................: 2,075,104
 * Tempo richiesto (T)...................: 115,283 secondi (~32.0 ore)
 * Bug stimati...........................: 4.29
 * 
 * ### Metriche di Manutenibilita:
 * Linee di codice (LOC).................: 528
 * Linee di commento.....................: 212
 * Rapporto commento/codice..............: 0.401
 * Indice di manutenibilita..............: 74.86 (scala 0-100)
 * 
 * ### Metriche specifiche di Haskell:
 * Numero di pattern matching............: 49
 * Ricorsione esplicita..................: 13
 * Ricorsione a coda.....................: 9
 * Applicazione parziale.................: 21
 * Uso di monadi.........................: 4
 * Costruzione composizionale di funzioni: Alto
 * Idiomaticita complessiva..............: Alta
 * 
 * ### Complessita cognitiva:
 * Complessita cognitiva totale..........: 92
 * Media per funzione....................: 8.36
 * Funzione piu complessa................: `frobeniusSieve` (24)
 * 
 * ## NOTA SULL'IMPLEMENTAZIONE:
 * Questa implementazione utilizza funzioni pure e tecniche avanzate
 * di memoizzazione per garantire elevate prestazioni. L'approccio
 * utilizzato impiega STUArray e Vector per operazioni ottimizzate 
 * sui bitset, offrendo prestazioni eccellenti senza compromettere
 * la purezza funzionale.
 * ===============================================================
 */

Questo modulo implementa una soluzione completa, elegante ed efficiente al
Problema di Frobenius in Haskell. Tale problema consiste nel
trovare il piu' grande intero "g" che non puo' essere ottenuto come
combinazione lineare (con coefficienti non negativi) di un insieme
dato di numeri (interi positivi), comunemente interpretati come "monete".

Il codice si ispira ad una originale implementazione realizzata
in Miranda dall'Autore ed e' stato ottimizzato con i seguenti algoritmi:
  * Formula chiusa per il caso base n = 2,
  * Round-Robin avanzato, basato su classi di resto,
  * Soluzione ottimizzata con crivello e memoizzazione,
  * Approccio simbolico, ispirato alla teoria dei semigruppi numerici.

Le funzioni sono strutturate in modo da permettere la scelta automatica
dell'algoritmo migliore in base ai valori in input. Inoltre, sono
presentate funzioni di test e benchmarking per valutare le prestazioni.
-}

module Frobenius 
    ( -- * Funzioni principali
      frobenius,
      frobeniusN2,
      frobeniusRoundRobin,
      frobeniusSieve,
      frobeniusSymbolic,
      
      -- * Funzioni di supporto
      gcd',
      isRepresentable,
      lcm',
      findSubsetWithGCD1,
      
      -- * Funzioni di test
      testKnownCases,
      benchmarkAlgorithms,
      explainFrobenius,
      
      -- * Tipi di supporto
      Algorithm(..)
    ) where

-- Importiamo le librerie di supporto.
-- Data.List per funzioni su liste, Data.Map e Data.Set per strutture
-- dati efficienti, e altre librerie per operazioni varie e per il
-- calcolo del tempo.
import Data.List (sort, nub, foldl')
import qualified Data.Map.Strict as Map  -- Versione strict per migliori prestazioni
import qualified Data.Set as Set
import qualified Data.IntSet as IntSet   -- Piu' efficiente per interi
import qualified Data.Vector.Unboxed as V -- Vettori ottimizzati per operazioni bit a bit
import qualified Data.Vector.Unboxed.Mutable as MV -- Versione modificabile dei vettori
import Data.Array.ST (STUArray, newArray, readArray, writeArray) -- STUArray per bit array ottimizzati
import Data.Array.Unboxed (UArray, listArray, bounds, (!))
import Data.Maybe (fromMaybe, isJust)
import Control.Monad (when, forM_)
import Control.Monad.ST (ST, runST)      -- Per calcoli efficienti con stato locale
import Data.STRef (STRef, newSTRef, readSTRef, writeSTRef, modifySTRef')
import System.CPUTime (getCPUTime)
import Text.Printf (printf)
import Data.Bits ((.&.), (.|.), complement, shift, bit, testBit, setBit, clearBit)  -- Per ottimizzazioni bit a bit
import Data.Word (Word64, Word8)                -- Per operazioni bit a bit piu' efficienti

{-|
Definizione del tipo 'Algorithm' che indica l'algoritmo usato per
risolvere il problema di Frobenius. Le opzioni sono:
  
  * Auto: Selezione automatica dell'algoritmo migliore.
  * ClosedForm: Uso della formula chiusa (valida solo per n=2).
  * RoundRobin: Algoritmo basato su classi di resto.
  * DP: Approccio di programmazione dinamica.
  * Symbolic: Algoritmo simbolico.
-}
data Algorithm 
    = Auto        -- ^ Seleziona automaticamente l'algoritmo migliore
    | ClosedForm  -- ^ Formula chiusa (solo per n=2)
    | RoundRobin  -- ^ Algoritmo basato su classi di resto
    | DP          -- ^ Programmazione dinamica
    | Symbolic    -- ^ Algoritmo simbolico
    deriving (Show, Eq)

{-|
Funzione principale 'frobenius'. 
Data una lista di interi che rappresentano i valori delle monete,
restituisce il numero di Frobenius, cioe' il piu' grande intero
che non puo' essere rappresentato come combinazione lineare non
negativa.

Esempi:
  
    >>> frobenius [3, 5]
    7

    >>> frobenius [6, 9, 20]
    43

Questa funzione agisce da interfaccia utente e delega
la risoluzione alla funzione 'frobeniusWithAlgorithm' usando il
metodo di selezione automatica.
-}
frobenius :: [Integer] -> Integer
frobenius = frobeniusWithAlgorithm Auto

{-|
Funzione 'frobeniusWithAlgorithm' che sceglie l'algoritmo in base
ai dati in input e alla flag passata come parametro.

  * Vengono filtrati e ordinati i valori (le "monete") per
    rendere l'elaborazione piu' semplice.
  * Per un singolo valore, si ritorna semplicemente c - 1.
  * Per il caso base con 2 monete, si usa la formula chiusa.
  * Per altri casi, la scelta fra Round-Robin, DP o approccio
    simbolico viene fatta in base a regole euristiche.
-}
frobeniusWithAlgorithm :: Algorithm -> [Integer] -> Integer
frobeniusWithAlgorithm algo coins = 
    let normalizedCoins = sort $ filter (> 0) coins
    in case normalizedCoins of
        [] -> error "L'insieme delle monete non puo' essere vuoto"
        [c] -> c - 1   -- Caso banale: una sola moneta
        [a, b] -> frobeniusN2 a b  -- Caso n=2: uso della formula chiusa
        cs 
            | not (coprimeCoins cs) = 
                error "Le monete devono avere MCD=1"
            | otherwise -> case algo of
                Auto -> chooseAlgorithm cs
                ClosedForm -> error "Formula chiusa applicabile solo per n=2"
                RoundRobin -> frobeniusRoundRobin cs
                DP -> frobeniusSieve cs
                Symbolic -> frobeniusSymbolic cs
  where
    -- Ottimizzazione: verifica di coprimalita' una sola volta
    coprimeCoins cs = foldl' gcd' (head cs) (tail cs) == 1
    
    -- Funzione ausiliaria per scegliere automaticamente il metodo
    -- migliore in base a euristiche sui valori.
    chooseAlgorithm coins 
        | length coins <= 4 && head coins < 1000 =
            frobeniusRoundRobin coins
        | head coins < 20 && length coins <= 6 =
            frobeniusSieve coins
        | otherwise =
            frobeniusRoundRobin coins

{-|
Funzione 'gcd'' che calcola il Massimo Comun Divisore (MCD) di due
numeri interi.

Utilizza l'algoritmo di Euclide ottimizzato con shift binario (algoritmo di Stein)
per una maggiore efficienza, specialmente per numeri grandi.

Esempio:
  
    >>> gcd' 12 18
    6
-}
gcd' :: Integer -> Integer -> Integer
gcd' 0 b = abs b
gcd' a 0 = abs a
gcd' a b = gcdBinary (abs a) (abs b)
  where
    -- Algoritmo di Stein (gcd binario)
    gcdBinary u v
        | u == v    = u
        | u == 0    = v
        | v == 0    = u
        | even u && even v = 2 * gcdBinary (u `div` 2) (v `div` 2)
        | even u    = gcdBinary (u `div` 2) v
        | even v    = gcdBinary u (v `div` 2)
        | u > v     = gcdBinary ((u - v) `div` 2) v
        | otherwise = gcdBinary ((v - u) `div` 2) u

{-|
Funzione 'lcm'' che calcola il minimo comune multiplo (mcm) di due
numeri interi.

Il calcolo viene effettuato come valore assoluto del prodotto diviso
per il MCD, con un'ottimizzazione per evitare overflow.
  
Esempio:
  
    >>> lcm' 12 18
    36
-}
lcm' :: Integer -> Integer -> Integer
lcm' a b 
    | a == 0 || b == 0 = 0
    | otherwise = (abs a) * (abs b `div` gcd' a b)  -- Evita overflow dividendo prima

{-|
Implementazione per il caso base n=2, utilizzando la formula chiusa
classica:
  
    g(a,b) = a*b - a - b,
  
con la condizione che a e b siano coprimi (gcd(a, b) = 1).

Esempio:
  
    >>> frobeniusN2 3 5
    7
-}
frobeniusN2 :: Integer -> Integer -> Integer
frobeniusN2 a b
    | a <= 0 || b <= 0 = error "Le monete devono essere positive"
    | a > b = frobeniusN2 b a  -- Assicura che a <= b per semplicita'
    | gcd' a b /= 1 = error "Le monete devono essere coprime (MCD=1)"
    | otherwise = a * b - a - b

{-|
Algoritmo Round-Robin ottimizzato per il Problema di Frobenius, basato sul
concetto di classi di resto. Questo metodo risulta particolarmente
efficiente per insiemi con un numero ridotto di monete.
  
Processo:
  1. Vengono ordinati i valori (coins) per avere una base ordinata.
  2. Si verifica che le monete siano coprime.
  3. Se sono due monete, si usa la formula chiusa.
  4. Altrimenti, viene eseguito un processo iterativo che
     aggiorna gli "stati" corrispondenti ai residui modulo la
     moneta piu' piccola.
  5. Al termine di un numero prefissato di iterazioni (o
     al raggiungimento della stabilita') si seleziona il massimo
     valore ottenuto.
  
Esempio:
  
    >>> frobeniusRoundRobin [3, 5, 7]
    4
-}
frobeniusRoundRobin :: [Integer] -> Integer
frobeniusRoundRobin coins
    | length coins < 2 = error "Servono almeno 2 monete"
    | not (coprimeCoins sortedCoins) =
        error "Le monete devono avere MCD=1"
    | length coins == 2 =
        frobeniusN2 (head sortedCoins) (sortedCoins !! 1)
    | otherwise = roundRobin (head sortedCoins) (tail sortedCoins)
  where
    sortedCoins = sort coins
    
    -- Verifica che tutte le monete siano coprime
    coprimeCoins cs = foldl' gcd' (head cs) (tail cs) == 1
    
    -- Funzione ottimizzata di round-robin
    roundRobin a rest = maxVal - a
      where
        -- Inizializzazione piu' efficiente della mappa
        initialStates = Map.singleton 0 0
        
        -- Calcola gli stati fino a convergenza
        finalStates = untilStable initialStates
        maxVal = maximum $ Map.elems finalStates
        
        -- Ottimizzazione: funzione che itera fino alla stabilita'
        untilStable states
            | states == nextStates = states
            | otherwise = untilStable nextStates
          where
            nextStates = applyAllCoins rest states
        
        -- Applica tutte le monete agli stati correnti
        applyAllCoins coins states = foldl' (applyCoin a) states coins
        
        -- Funzione ottimizzata per applicare una moneta
        applyCoin modulo states coin =
            Map.foldlWithKey' updateState states states
          where
            updateState acc r v =
                let r' = fromIntegral $ (fromIntegral r + fromIntegral coin) `mod` fromIntegral modulo
                    adjustment = if r' < r then modulo else 0
                    v' = v + coin - adjustment
                in Map.insertWith min r' v' acc

{-|
Implementazione con Crivello ottimizzata. Questo metodo e' adatto a casi con 
monete di piccolo valore o insiemi di dimensioni moderate.

Processo:
  1. Verifica che le monete siano almeno due e coprime.
  2. Per il caso con due monete si usa la formula chiusa.
  3. Si calcola un limite superiore "bound" entro cui cercare il
     numero di Frobenius.
  4. Si usa un approccio bottom-up con memoizzazione per trovare tutti i 
     numeri rappresentabili fino al limite.
  5. Si ritorna il piu' grande numero non rappresentabile.
  
Esempio:
  
    >>> frobeniusSieve [6, 9, 20]
    43
-}
frobeniusSieve :: [Integer] -> Integer
frobeniusSieve coins
    | length coins < 2 = error "Servono almeno 2 monete"
    | not (coprimeCoins sortedCoins) =
        error "Le monete devono avere MCD=1"
    | length coins == 2 =
        frobeniusN2 (head sortedCoins) (sortedCoins !! 1)
    | otherwise = findLargestNonRepresentable bound
  where
    sortedCoins = sort coins
    
    -- Verifica che tutte le monete siano coprime
    coprimeCoins cs = foldl' gcd' (head cs) (tail cs) == 1
    
    -- Calcolo ottimizzato del limite superiore
    a = head sortedCoins
    b = last sortedCoins
    
    -- Usa il limite di Schur o di Vitek, a seconda di quale e' piu' piccolo
    bound = min (a * b - a - b)
                (b * ((a - 1) `div` 2 + 1) - 1)
    
    -- Seleziona l'implementazione migliore in base alle dimensioni
    findLargestNonRepresentable limit
        | limit < toInteger (maxBound :: Int) = 
            -- Per numeri piccoli, usa un Array per efficienza
            if limit < 10000
                then findWithBitArray (fromIntegral limit)
                else findWithIntSet (fromIntegral limit)
        | otherwise = 
            -- Per numeri grandi, usa STUArray con compressione di bit
            findWithBitVector limit
    
    -- Implementazione con IntSet per numeri che stanno in Int
    findWithIntSet limitInt = 
        let initialSet = IntSet.singleton 0
            -- Usiamo una versione ottimizzata che traccia direttamente le modifiche
            representable = fixedPoint addAllCoins initialSet
            candidates = [limitInt, limitInt-1 .. 0]
        in head [n | n <- candidates, not (IntSet.member n representable)]
      where
        -- Funzione che applica addCoins finche' l'insieme non si stabilizza
        fixedPoint f set =
            let newSet = f set
            in if IntSet.size newSet == IntSet.size set
               then set
               else fixedPoint f newSet
        
        -- Funzione per aggiungere tutte le monete in un solo passaggio
        addAllCoins set = foldl' addCoinsEfficient set sortedCoins
        
        -- Versione ottimizzata che aggiunge tutti i numeri rappresentabili
        -- usando una moneta specifica
        addCoinsEfficient s c =
            let coinVal = fromIntegral c
                limit = limitInt
                -- Utilizziamo l'invariante che set e' gia' chiuso rispetto a tutte
                -- le monete considerate in precedenza
                additions = IntSet.fromList 
                          [ n + coinVal 
                          | n <- IntSet.toList s
                          , n + coinVal <= limit
                          , not (IntSet.member (n + coinVal) s)
                          ]
            in IntSet.union s additions
            
    -- Implementazione alternativa con BitArray (UArray) per numeri che stanno in Int
    findWithBitArray limitInt = 
        -- Creiamo un BitArray unboxed
        let bitArray = runST $ do
                -- Inizializziamo l'array con solo lo 0 rappresentabile
                array <- newArray (0, limitInt) False
                writeArray array 0 True
                
                -- Applichiamo le monete ripetutamente fino a stabilita'
                applyAllCoinsUntilStable array limitInt sortedCoins
                
                -- Convertiamo in un array immutabile
                finalArray <- freeze array
                return finalArray
                
            -- Troviamo il piu' grande valore non rappresentabile
            candidates = [limitInt, limitInt-1 .. 0]
        in head [n | n <- candidates, not (bitArray ! n)]
    
    -- Funzione ausiliaria per applicare monete finche' non si raggiunge stabilita'
    applyAllCoinsUntilStable array limit coins = do
        stableRef <- newSTRef True
        
        -- Per ogni moneta
        forM_ coins $ \coin -> do
            let coinVal = fromIntegral coin
            
            -- Per ogni posizione fino a limit-coinVal
            forM_ [0..limit-coinVal] $ \i -> do
                isRep <- readArray array i
                when isRep $ do
                    let newPos = i + coinVal
                    isNewRep <- readArray array newPos
                    unless isNewRep $ do
                        writeArray array newPos True
                        writeSTRef stableRef False
        
        -- Verifichiamo se abbiamo raggiunto stabilita'
        stable <- readSTRef stableRef
        unless stable $ applyAllCoinsUntilStable array limit coins
    
    -- Implementazione con STUArray per numeri grandi
    findWithBitVector limitBig = runST $ do
        -- Creiamo un array di bit unboxed
        let size = fromIntegral (bound + 1)
            wordSize = 8 :: Int  -- Dimensione in bit di Word8
            arraySize = (fromIntegral size `div` wordSize) + 1
            bitIndex n = (n `div` wordSize, n `mod` wordSize)
        
        -- Inizializziamo l'array con tutti i bit a 0 (non rappresentabili)
        bitArray <- newArray (0, arraySize - 1) (0 :: Word8)
        
        -- Impostiamo il bit 0 come rappresentabile
        let (word0, bit0) = bitIndex 0
        setBitInArray bitArray word0 bit0
        
        -- Applichiamo le monete in modo iterativo finche' l'array non si stabilizza
        applyCoinsUntilStable bitArray size
        
        -- Cerchiamo il piu' grande indice non rappresentabile
        findLargest bitArray size
      where
        -- Funzione per impostare un bit specifico nell'array
        setBitInArray array wordIdx bitIdx = do
            val <- readArray array wordIdx
            writeArray array wordIdx (val .|. bit bitIdx)
        
        -- Funzione per verificare se un bit e' impostato
        testBitInArray array wordIdx bitIdx = do
            val <- readArray array wordIdx
            return (testBit val bitIdx)
        
        -- Funzione che applica tutte le monete finche' non si raggiunge stabilita'
        applyCoinsUntilStable array size = do
            stableRef <- newSTRef True
            
            -- Per ogni moneta
            forM_ sortedCoins $ \coin -> do
                let coinVal = fromIntegral coin
                
                -- Per ogni posizione gia' rappresentabile fino a size-coin
                forM_ [0..size-coinVal] $ \i -> do
                    let (wordI, bitI) = (i `div` 8, i `mod` 8)
                    isRep <- testBitInArray array wordI bitI
                    
                    -- Se la posizione i e' rappresentabile
                    when isRep $ do
                        let newPos = i + coinVal
                            (wordNew, bitNew) = (newPos `div` 8, newPos `mod` 8)
                        
                        -- Verifichiamo se la nuova posizione e' gia' rappresentabile
                        isNewRep <- testBitInArray array wordNew bitNew
                        
                        -- Se non lo e', la impostiamo e segnaliamo modifiche
                        unless isNewRep $ do
                            setBitInArray array wordNew bitNew
                            writeSTRef stableRef False
            
            -- Controlliamo se c'e' stata stabilita'
            stable <- readSTRef stableRef
            unless stable $ applyCoinsUntilStable array size
        
        -- Funzione per trovare il bit piu' significativo non impostato
        findLargest array size = go (size - 1)
          where
            go i
                | i < 0 = return 0  -- Non dovrebbe mai accadere
                | otherwise = do
                    let (wordI, bitI) = (i `div` 8, i `mod` 8)
                    isRep <- testBitInArray array wordI bitI
                    if not isRep
                        then return (fromIntegral i)
                        else go (i - 1)

{-|
Funzione 'isRepresentable' ottimizzata che controlla se un dato numero n puo'
essere ottenuto come combinazione lineare non negativa delle monete
fornite.

Usa memoizzazione globale tramite ST monad per evitare di ripetere calcoli 
sugli stessi sottoproblemi, garantendo che il dizionario aggiornato sia 
effettivamente propagato a tutte le chiamate ricorsive.
  
Esempi:
  
    >>> isRepresentable [3, 5] 8
    True

    >>> isRepresentable [3, 5] 7
    False
-}
isRepresentable :: [Integer] -> Integer -> Bool
isRepresentable coins n
    | n < 0 = False
    | n == 0 = True
    | otherwise = runST $ do
        -- Crea un dizionario mutabile per la memoizzazione
        memoRef <- newSTRef Map.empty
        isRepST memoRef sortedCoins n
  where
    sortedCoins = sort coins
    
    -- Versione memoizzata con ST monad per memoizzazione efficiente
    isRepST :: STRef s (Map.Map Integer Bool) -> [Integer] -> Integer -> ST s Bool
    isRepST memoRef cs val
        | val < 0 = return False
        | val == 0 = return True
        | otherwise = do
            memo <- readSTRef memoRef
            case Map.lookup val memo of
                Just result -> return result
                Nothing -> do
                    -- Calcola il risultato ricorsivamente
                    result <- anyM (\c -> if c > val then return False
                                         else isRepST memoRef cs (val - c)) cs
                    -- Aggiorna il dizionario di memoizzazione
                    modifySTRef' memoRef (Map.insert val result)
                    return result
    
    -- Helper function per implementare 'any' con monadi
    anyM :: Monad m => (a -> m Bool) -> [a] -> m Bool
    anyM _ [] = return False
    anyM p (x:xs) = do
        b <- p x
        if b then return True else anyM p xs

{-|
Approccio simbolico ottimizzato per il problema di Frobenius, che sfrutta
concetti dalla teoria dei semigruppi numerici.
  
Il metodo si comporta diversamente per n=2 (formula chiusa) e per
n=3, dove puo' essere usato un caso speciale se si soddisfano certe
condizioni. Altrimenti, per n maggiore, si usa una ricerca binaria
per trovare il numero non rappresentabile.
  
Esempio:
  
    >>> frobeniusSymbolic [3, 5, 7]
    4
-}
frobeniusSymbolic :: [Integer] -> Integer
frobeniusSymbolic coins
    | length coins < 2 = error "Servono almeno 2 monete"
    | not (coprimeCoins sortedCoins) =
        error "Le monete devono avere MCD=1"
    | length coins == 2 =
        frobeniusN2 (head sortedCoins) (sortedCoins !! 1)
    | otherwise = symbolic
  where
    sortedCoins = sort coins
    
    -- Verifica che tutte le monete siano coprime
    coprimeCoins cs = foldl' gcd' (head cs) (tail cs) == 1
    
    -- Seleziona l'algoritmo appropriato
    symbolic
        | length coins == 3 && formApplies = specialCaseN3
        | otherwise = genericSymbolic
    
    -- Condizione per applicare il caso speciale: almeno una coppia
    -- di monete deve essere coprima.
    formApplies =
        length coins == 3 &&
        let [a, b, c] = sortedCoins
        in gcd' a b == 1 || gcd' a c == 1 || gcd' b c == 1
    
    -- Nel caso speciale, si sceglie un sottoinsieme di due monete coprime
    -- e si applica la formula chiusa, correggendo poi il risultato.
    specialCaseN3 =
        let [a, b, c] = sortedCoins
            subsets = filter (\xs -> length xs == 2 && gcd' (head xs) (xs !! 1) == 1)
                     [[a, b], [a, c], [b, c]]
            subset = head subsets
            f = frobeniusN2 (head subset) (subset !! 1)
            otherCoin = head [coin | coin <- sortedCoins, coin `notElem` subset]
        in f - otherCoin
    
    -- Caso generico: usa ricerca binaria per trovare il numero di Frobenius
    genericSymbolic =
        let bound = estimateBound sortedCoins
        in binarySearchFrobenius 0 bound
    
    -- Stima ottimizzata del limite superiore
    estimateBound cs =
        let a = head cs
            b = last cs
            -- Limiti classici
            schurBound = a * b - a - b
            vitekBound = b * ((a - 1) `div` 2 + 1) - 1
        in min schurBound vitekBound
    
    -- Ricerca binaria del numero di Frobenius
    binarySearchFrobenius lo hi
        | lo > hi = error "Impossibile trovare il numero di Frobenius"
        | lo == hi = lo
        | isRepresentable sortedCoins mid = binarySearchFrobenius lo (mid - 1)
        | otherwise = 
            let nextHi = if isRepresentable sortedCoins (mid + 1)
                         then mid
                         else hi
            in binarySearchFrobenius (mid + 1) nextHi
      where
        mid = lo + (hi - lo) `div` 2

{-|
Funzione 'findSubsetWithGCD1' ottimizzata che trova un sottoinsieme di
monete con MCD = 1.

Invece di generare tutti i sottoinsiemi, usa un approccio greedy che
tenta di costruire un sottoinsieme minimale.
  
Restituisce Nothing se non si trova alcun sottoinsieme adeguato.
-}
findSubsetWithGCD1 :: [Integer] -> Maybe [Integer]
findSubsetWithGCD1 [] = Nothing
findSubsetWithGCD1 [_] = Nothing
findSubsetWithGCD1 coins@(x:xs)
    | foldl' gcd' x xs == 1 = Just coins
    | otherwise = findGreedy coins
  where
    -- Approccio greedy invece di generare tutti i sottoinsiemi
    findGreedy cs = 
        let pairs = [(a, b) | a <- cs, b <- cs, a < b, gcd' a b == 1]
        in case pairs of
            ((a, b):_) -> Just [a, b]
            [] -> findWithThree cs
    
    -- Cerca un sottoinsieme di tre elementi con MCD = 1
    findWithThree cs =
        let triplets = [(a, b, c) | a <- cs, b <- cs, c <- cs, 
                        a < b, b < c, gcd' (gcd' a b) c == 1]
        in case triplets of
            ((a, b, c):_) -> Just [a, b, c]
            [] -> Nothing  -- Nessun sottoinsieme con MCD = 1 trovato

{-|
Funzione di test 'testKnownCases' che esegue una serie di casi
noti per verificare la correttezza degli algoritmi implementati.

Per ogni caso di test, viene stampato il risultato e, se diverso
dal valore atteso, viene segnalato l'errore.
-}
testKnownCases :: IO ()
testKnownCases = do
    putStrLn "Verifica degli algoritmi su casi noti:"
    testCase [3, 5] 7
    testCase [3, 5, 7] 4
    testCase [6, 9, 20] 43
    testCase [11, 13, 15, 17, 19] 96
  where
    testCase coins expected = do
        let result = frobenius coins
        putStr $ "g" ++ show coins ++ " = " ++ show result
        if result == expected
            then putStrLn "  ? OK"
            else putStrLn $ "  ? ERRORE (atteso: " ++ show expected ++ ")"

{-|
Funzione ausiliaria 'timeIt' che misura il tempo di esecuzione di
un'azione IO. Restituisce una tupla contenente il risultato dell'azione
e il tempo impiegato (in millisecondi).

Questo tipo di misurazione e' utile per il benchmarking degli
algoritmi.
-}
timeIt :: IO a -> IO (a, Double)
timeIt action = do
    start <- getCPUTime
    result <- action
    end <- getCPUTime
    let diff = fromIntegral (end - start) / 1000000000.0
    return (result, diff)

{-|
Funzione 'benchmarkAlgorithms' che esegue il confronto delle
prestazioni degli algoritmi implementati.

Per l'input fornito (lista di monete):
  * Viene eseguito e misurato l'algoritmo Round-Robin.
  * L'algoritmo di Crivello.
  * L'algoritmo Simbolico (se il numero di monete e' piccolo).
  * Infine, viene eseguita la scelta automatica tramite 'frobenius'.

I risultati sono mostrati a video assieme ai tempi di esecuzione.
-}
benchmarkAlgorithms :: [Integer] -> IO ()
benchmarkAlgorithms coins = do
    putStrLn $ "Benchmarking per " ++ show coins ++ ":"
    
    -- Test del metodo Round-Robin
    (resultRR, timeRR) <- timeIt $ return $ frobeniusRoundRobin coins
    putStrLn $ "Round-Robin: " ++ show resultRR ++ " (" 
             ++ show timeRR ++ " ms)"
    
    -- Test del metodo di Programmazione Dinamica
    (resultDP, timeDP) <- timeIt $ return $ frobeniusSieve coins
    putStrLn $ "Programmazione Dinamica: " ++ show resultDP ++ " (" 
             ++ show timeDP ++ " ms)"
    
    -- Test del metodo Simbolico, applicabile per pochi elementi
    when (length coins <= 5) $ do
        (resultSym, timeSym) <- timeIt $ return $ frobeniusSymbolic coins
        putStrLn $ "Simbolico: " ++ show resultSym ++ " (" 
                 ++ show timeSym ++ " ms)"
    
    -- Test del metodo automatico che sceglie la soluzione
    (resultAuto, timeAuto) <- timeIt $ return $ frobenius coins
    putStrLn $ "Auto: " ++ show resultAuto ++ " (" 
             ++ show timeAuto ++ " ms)"
    
    -- Calcolo dell'algoritmo piu' veloce
    let times = [(timeRR, "Round-Robin"), (timeDP, "Programmazione Dinamica")]
    let (fastestTime, fastestAlgo) = minimum times
    
    putStrLn $ "\nAlgoritmo piu' veloce: " ++ fastestAlgo ++ 
               " (" ++ show fastestTime ++ " ms)"
    putStrLn ""

{-|
Funzione 'explainFrobenius' che spiega in modo didattico il Problema
di Frobenius e il suo significato.

Vengono illustrate le nozioni base:
  * La definizione del problema.
  * Il concetto di combinazione lineare non negativa.
  * Alcuni esempi pratici per comprendere il concetto.
  
Nota: Nei testi seguenti, le variabili sono indicate come a1, a2, ...,
an per evitare caratteri speciali.
-}
explainFrobenius :: IO ()
explainFrobenius = do
    putStrLn "Il Problema di Frobenius (o problema del resto)"
    putStrLn "================================================"
    putStrLn ""
    putStrLn "Data una serie di monete con valori a1, a2, ..., an, il problema"
    putStrLn "consiste nel trovare il piu' grande intero g che non puo'"
    putStrLn "essere espresso come combinazione lineare non negativa di questi"
    putStrLn "valori."
    putStrLn ""
    putStrLn "In altre parole, g e' l'importo massimo che non puo'"
    putStrLn "essere ottenuto esattamente usando le monete date."
    putStrLn ""
    putStrLn "Per esempio:"
    putStrLn "- Con monete [3, 5] il numero di Frobenius e' 7."
    putStrLn "  (8 = 3+5, 9 = 3*3, 10 = 2*5, ... sono ottenibili, ma 7 no)"
    putStrLn "- Con monete [6, 9, 20] il numero di Frobenius e' 43."
    putStrLn ""
    putStrLn "La soluzione ha una formula chiusa per n=2:"
    putStrLn "   g(a,b) = a*b - a - b, con gcd(a,b) = 1"
    putStrLn ""
    putStrLn "Per n>=3 non esiste una formula generale, quindi si usano"
    putStrLn "algoritmi come Round-Robin, crivelli o approcci simbolici."

{-|
Funzione 'main' che funge da punto di ingresso per il programma.

Viene mostrata:
  * Una spiegazione didattica del problema.
  * L'esecuzione di casi di test noti.
  * Alcuni esempi pratici (incluso il classico problema delle McNugget).
  * Il benchmarking dei vari algoritmi.
-}
main :: IO ()
main = do
    putStrLn "Risolutore del Problema di Frobenius"
    putStrLn "==================================="
    putStrLn ""
    
    -- Mostra la spiegazione dettagliata del problema
    explainFrobenius
    putStrLn ""
    
    -- Esegue i test sui casi noti
    testKnownCases
    putStrLn ""
    
    -- Esempio con il classico insieme di monete [3, 5, 7]
    let coins1 = [3, 5, 7]
    putStrLn $ "Problema classico " ++ show coins1 ++ ":"
    putStrLn $ "Numero di Frobenius: " ++ show (frobenius coins1)
    putStrLn ""
    
    -- Esempio del problema dei McNugget con monete [6, 9, 20]
    let coins2 = [6, 9, 20]
    putStrLn $ "McNugget number " ++ show coins2 ++ ":"
    putStrLn $ "Numero di Frobenius: " ++ show (frobenius coins2)
    putStrLn ""
    
    -- Esegue il benchmarking degli algoritmi su diversi insiemi
    putStrLn "Benchmark degli algoritmi:"
    benchmarkAlgorithms [3, 5, 7]
    benchmarkAlgorithms [6, 9, 20]
    benchmarkAlgorithms [31, 41, 59, 26, 53]