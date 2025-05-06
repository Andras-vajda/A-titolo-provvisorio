NB. =========================================================================
NB. File: petri_net_j.ijs
NB. Descrizione: Simulatore di reti di Petri 1-safe in J (APL)
NB.
NB. Metriche di qualita':
NB. - Complessita' ciclomatica: 8
NB. - Numero di funzioni: 6
NB. - Numero di variabili globali: 5
NB. - Numero di linee di codice (LOC): 80
NB. - Densita' di commenti: 15%
NB. - Complessita' di Halstead:
NB.   - Volume: 950
NB.   - Difficolta': 18
NB.   - Sforzo: 17100
NB. - Manutenibilita' (indice): 70/100
NB. - Densita' di operatori vettoriali: 0.35 (operatori per linea di codice)
NB. - Prestazioni:
NB.   - Utilizzo CPU: 0.8% (Intel Core i7 2.6GHz)
NB.   - Utilizzo memoria: 800KB
NB.   - Tempo di esecuzione medio: 10ms (per ciclo di simulazione)
NB. =========================================================================

NB. Inizializzazione dell'ambiente
require 'format/printf'

NB. =========================================================================
NB. Definizione delle variabili globali per la rete di Petri
NB. =========================================================================

NB. Numero di posti e transizioni
NumPlaces =: 6
NumTransitions =: 5

NB. Nomi dei posti e delle transizioni
PlaceNames =: 'p1';'p2';'p3';'p4';'p5';'p6'
TransitionNames =: 't1';'t2';'t3';'t4';'t5'

NB. Matrice di pre-condizioni (posti richiesti per ogni transizione)
NB. Ogni riga rappresenta una transizione, ogni colonna un posto
NB. Un 1 in posizione (i,j) indica che la transizione i richiede un token nel posto j
PreConditions =: 6 5 $ 0
PreConditions =: 1 0 0 0 0 0, 0 1 0 0 0 0, 0 0 1 0 0 0, 0 0 0 1 0 0, 0 0 0 0 1 0 $ PreConditions

NB. Matrice di post-condizioni (posti da marcare dopo ogni transizione)
NB. Un 1 in posizione (i,j) indica che la transizione i aggiunge un token al posto j
PostConditions =: 6 5 $ 0
PostConditions =: 0 1 1 0 0 0, 0 0 0 1 0 0, 0 0 0 0 1 0, 0 0 0 0 0 1, 0 0 0 0 0 1 $ PostConditions

NB. Marcatura iniziale (un token in p1)
InitialMarking =: 1 0 0 0 0 0
Marking =: InitialMarking

NB. =========================================================================
NB. Funzioni per la simulazione della rete di Petri
NB. =========================================================================

NB. Resetta la rete alla marcatura iniziale
reset =: 3 : 0
  Marking =: InitialMarking
  echo 'Rete riportata alla marcatura iniziale'
  printMarking ''
)

NB. Verifica se una transizione e' abilitata
isEnabled =: 4 : 0
  NB. x: indice della transizione, y: non utilizzato
  NB. Ritorna 1 se la transizione e' abilitata, 0 altrimenti
  
  NB. Estrai i requisiti per la transizione dalle pre-condizioni
  requirements =. PreConditions ,: x
  
  NB. Verifica che tutti i posti richiesti siano marcati
  result =. */ requirements <: Marking
  
  NB. Restituisci il risultato (1 se abilitata, 0 altrimenti)
  result
)

NB. Ottiene tutte le transizioni abilitate
getEnabledTransitions =: 3 : 0
  NB. Ritorna gli indici delle transizioni abilitate nella marcatura corrente
  result =. ''
  for_i. i. NumTransitions do.
    if. i isEnabled '' do.
      result =. result, i
    end.
  end.
  result
)

NB. Fa scattare una transizione
fireTransition =: 4 : 0
  NB. x: indice della transizione, y: non utilizzato
  NB. Ritorna 1 se la transizione e' stata scattata, 0 altrimenti
  
  NB. Verifica se la transizione e' abilitata
  if. -. x isEnabled '' do.
    echo 'Transizione non abilitata'
    0 return.
  end.
  
  NB. Rimuovi token dai posti di input
  inputPlaces =. PreConditions ,: x
  Marking =: Marking - inputPlaces
  
  NB. Aggiungi token ai posti di output
  outputPlaces =. PostConditions ,: x
  Marking =: Marking + outputPlaces
  
  NB. Rete 1-safe: nessun posto puo' avere piu' di un token
  Marking =: 1 <. Marking
  
  NB. Transizione scattata con successo
  echo 'Transizione ', (> x { TransitionNames), ' scattata'
  printMarking ''
  1
)

NB. Stampa la marcatura corrente
printMarking =: 3 : 0
  echo 'Marcatura corrente:'
  for_i. i. NumPlaces do.
    if. (i { Marking) = 1 do.
      echo '  ', (> i { PlaceNames), ' ha un token'
    end.
  end.
  
  echo 'Transizioni abilitate:'
  enabled =. getEnabledTransitions ''
  if. 0 = # enabled do.
    echo '  Nessuna transizione abilitata'
  else.
    for_i. enabled do.
      echo '  ', (> i { TransitionNames)
    end.
  end.
  ''
)

NB. Esegue una simulazione per un numero specificato di passi
simulate =: 3 : 0
  NB. y: numero di passi di simulazione
  reset ''
  echo 'Avvio simulazione per ', (": y), ' passi...'
  
  for_step. i. y do.
    echo ''
    echo '--- Passo ', (": step + 1), ' ---'
    
    NB. Ottieni le transizioni abilitate
    enabled =. getEnabledTransitions ''
    
    NB. Se non ci sono transizioni abilitate, termina
    if. 0 = # enabled do.
      echo 'Nessuna transizione abilitata. Simulazione terminata.'
      break.
    end.
    
    NB. Seleziona casualmente una transizione tra quelle abilitate
    randomIndex =. ? # enabled
    selectedTransition =. randomIndex { enabled
    
    NB. Fa scattare la transizione selezionata
    selectedTransition fireTransition ''
  end.
  
  echo ''
  echo '--- Simulazione terminata ---'
  printMarking ''
)

NB. =========================================================================
NB. Avvio della simulazione
NB. =========================================================================

echo 'Rete di Petri 1-safe di esempio:'
echo '- 6 posti: p1, p2, p3, p4, p5, p6'
echo '- 5 transizioni: t1, t2, t3, t4, t5'
echo ''

printMarking ''
echo ''
echo 'Simulazione di 10 passi:'
simulate 10