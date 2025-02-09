# 📜 Algoritmi per le Partizioni - Implementazioni e Benchmark

Questa cartella contiene una serie di algoritmi per la generazione e l'analisi delle **partizioni di numeri interi**, con particolare attenzione a metodi ottimizzati e a confronto di prestazioni.

## 📂 Contenuto della Cartella

### 🔹 Algoritmi di Generazione

- **AccAsc.c** – Implementa gli algoritmi di **Zoghbi & Stojmenovic** e **Kelleher & O'Sullivan** per la generazione di partizioni.
- **Hpart.c** – Implementa l'algoritmo **H di Knuth**, che genera una partizione in esattamente *n* parti.

### 🔹 Tabelle e LUT

- **gen\_GPN.c** – Generatore di **LUT (Lookup Table)** per i **numeri pentagonali generalizzati** (*GPN*), utilizzati per migliorare l'efficienza del metodo Malenfant.
- **gpn\_table.h** – LUT pre-generata di **numeri pentagonali generalizzati** per l'implementazione del metodo Malenfant.

### 🔹 Benchmark e Comparazioni

- **Malenfant.c** – Genera una **tabella comparativa** per valutare l’efficienza del metodo di Malenfant rispetto ad altre strategie.

### 🔹 Extra e Curiosità

- **Susy77.c** – Risolve il **964° "Quesito con la Susi"**, pubblicato su *La Settimana Enigmistica* N° 4577 del 12 dicembre 2019, in occasione del **4513° concorso settimanale**.

---

## 📖 Licenza

🆓 **Tutto il materiale qui contenuto è rilasciato con licenza MIT.**

🚀 **Buono studio e buon coding!**


