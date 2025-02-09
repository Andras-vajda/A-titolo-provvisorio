# 📜 Problema dei Ménages - Implementazioni e Algoritmi

Questo archivio contiene la versione aggiornata degli esempi correlati alla serie di articoli del blog sul **problema dei Ménages**.

## 📂 Struttura delle Cartelle

```
Menage.zip
|
+---Menage............ Esempi dai primi due articoli.
|
+---APR............... Algoritmo di Rohl-Ganapathi-Rama.
|   +---VS
|       +---Release
+---ER................ Algoritmo di Effler-Ruskey.
|   +---VS
|       +---Release
+---MT................ Algoritmi di Mikawa-Tanaka.
|   +---VS
|       +---Release
|
+---Common............ File comuni per APR, ER, MT.
|
+---Deranged_3........ File di progetto Visual Studio.
|
+---Extra............. Algoritmi Myrvold-Ruskey e HRW.
```

## 📜 Descrizione delle Cartelle

- **Menage**: Contiene i principali due sorgenti di esempio correlati alla serie di articoli.
- **APR, ER, MT**: Ospitano i sorgenti delle reference implementation per i rispettivi algoritmi:
  - *APR* (Rohl-Ganapathi-Rama)
  - *ER* (Effler-Ruskey)
  - *MT* (Mikawa-Tanaka)
- **Common**: Contiene file condivisi tra le tre implementazioni.
- **Deranged_3**: Soluzione unica per Visual Studio 2010.
- **Extra**: Contiene tre ulteriori sorgenti relativi agli algoritmi:
  - *MR_perm.c*: Ranking e unranking di Myrvold-Ruskey.
  - *HRW_gen.c, HRW_dec.c*: Algoritmo di Holroyd-Ruskey-Williams per la codifica e decodifica di permutazioni con ordinamento "Bellringer".

## ⚠️ Avvertenze

- Le cartelle *APR*, *ER* e *MT* contengono i file di progetto compressi per **Digital Mars** e **Open Watcom**.
- La cartella **Deranged_3** ospita la soluzione per **Visual Studio 2010**.
- Gli eseguibili sono generati automaticamente nelle sottodirectory `VS\Release` delle rispettive cartelle di progetto.
- **Supporto a CMake** previsto per sistemi Linux.

---

## 📖 Licenza

🆓 **Tutto il materiale didattico originale qui contenuto viene rilasciato con licenza MIT.**

---

## 📊 Statistiche sui File Sorgenti

| LOC  | Parole | Caratteri | File |
|------|--------|-----------|------------------|
|  102 |   304  |   3340    | Menage/menage.c |
|  396 |  1309  |  12067    | Menage/menage2.c |
|  162 |   538  |   6017    | APR/derange_APR.c |
|  150 |   530  |   5275    | ER/derange_ER.c |
|  107 |   307  |   3539    | ER/misc_ER.c |
|  334 |  1249  |  10642    | MT/derange_MT.c |
|  190 |   754  |   7156    | MT/misc_MT.c |
|  154 |  1006  |   9983    | Common/binom.c |
|  339 |   894  |   9494    | Extra/hrw_dec.c |
|  264 |   812  |   7504    | Extra/hrw_gen.c |
|  258 |   749  |   7491    | Extra/MR_perm.c |
|------|--------|-----------|------------------|
| **2456** | **8452** | **82508** | **TOTAL** |

🚀 **Buono studio e buon coding!**


