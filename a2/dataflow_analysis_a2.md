# Very Busy Expressions

![image](https://github.com/user-attachments/assets/98b05235-cca0-4e98-aefb-14d4e39698ed)


#### Framework

| Proprietà                  | Descrizione                                                                                                                                                                                                                                                                                                                               |
| -------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Domain**                 | Insieme di espressioni                                                                                                                                                                                                                                                                                                                    |
| **Direction**              | Backwards: Le informazioni sulle Very Busy Expressions vengono aggiornate dall'exit point verso punti più recenti del CFG. Se la valutazione di un'espressione incontra in punto più in alto del CFG una redefinizione di un suo operando, non può essere considerato un VBE. Tale informazione viene eventualmente passato al punto `p`. |
| **Transfer function**      | Trattandosi di una Backward analysis:<br>`IN[B] = GEN[B] U (OUT[B] - KILL[B])`                                                                                                                                                                                                                                                            |
| **Meet operator**          | $\bigcap$. ${OUT}[B] = \bigcap {IN}[{successors}]$                                                                                                                                                                                                                                                                                        |
| **Boundary condition**     | `OUT[EXIT] = ∅` In quanto non ci sono espressioni da valutare dopo il blocco exit.                                                                                                                                                                                                                                                        |
| **Initial Interior Point** | `IN[B] = OUT[B] = U` Si parte dal presupposto che qualsiasi espressione può essere un VBE. Nel corso dell'analisi gli insiemi vengono rifiniti tramite la funzione di trasferimento per poi ottenere esclusivamente i VBE.                                                                                                                |

#### Analisi

Iterazione 1 (unica iterazione)

| Blocco | IN\[B]      | OUT\[B] |
| ------ | ----------- | ------- |
| ENTRY  | ∅           | ∅       |
| BB2    | {b-a}       | {b-a}   |
| BB3    | {b-a ; a-b} | {a-b}   |
| BB4    | {a-b}       | ∅       |
| BB5    | {b-a}       | ∅       |
| BB6    | ∅           | {a-b}   |
| BB7    | {a-b}       | ∅       |
| EXIT   | ∅           | ∅       |

---

# Dominator analysis

![{D81722E7-C6BF-4E2E-8012-B055005A14ED}](https://github.com/user-attachments/assets/032d10cf-57e9-441f-8dd1-d332fb0e9be6)


**Framework**

| **Proprietà**              | **Descrizione**                                                                                                                                             |
| -------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Domain**                 | Insieme di basic blocks                                                                                                                                     |
| **Direction**              | Forward<br><br>Man mano che si scorrono i blocchi incontrati a partire dall'entry point, questi vengono annotati fino ad arrivare al punto d'interesse.     |
| **Transfer Function**      | `OUT[B] = GEN[B] U IN[B]` <br><br>Considero Il blocco stesso (GEN\[B]) e i blocchi incontrati in precedenza (IN\[B]).                                       |
| **Meet Operator**          | $\cap$. ${IN}[B] = \bigcap {OUT}[{predecessors}]$<br><br>Per ogni blocco considero soltanto i blocchi incontrati in precedenza in comune per ogni percorso. |
| **Boundary condition**     | `IN[ENTRY] = ∅`<br>`OUT[ENTRY] = ENTRY`<br><br>L'entry point non è preceduto da nessun blocco, se non sé stesso.                                            |
| **Initial Interior Point** | `OUT[B] = B` <br><br>Per definizione, un nodo domina a prescindere sé stesso.                                                                               |

#### Analisi

Iterazione 1

| **Blocco** | IN\[B] | OUT\[B] |
| ---------- | ------ | ------- |
| A          | ∅      | A       |
| B          | A      | B       |
| C          |        | C       |
| D          |        | D       |
| E          |        | E       |
| F          |        | F       |
| G          |        | G       |

Iterazione 2

| **Blocco** | IN\[B]                 | OUT\[B] |
| ---------- | ---------------------- | ------- |
| A          | ∅                      | A       |
| B          | A                      | B,A     |
| C          | A                      | C.A     |
| D          | C,A                    | D,C,A   |
| E          | C,A                    | E,C,A   |
| F          | {D,C,A} $\cap$ {E,C,A} | F,C,A   |
| G          | {B,A} $\cap$ {F,C,A}   | G,A     |

## Constant Propagation

![cfg](https://github.com/user-attachments/assets/5cce3344-9ced-4b28-880f-02d7166b8229)

**Framework**

| **Proprietà**              | **Descrizione**                                                                                                                                             |
| -------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Domain**                 | Insieme di (\<variable>,\<const-value>)                                                                                                                                    |
| **Direction**              | Forward: <br> $out[b]=f_b(in[b])$ <br> $in[b]=\land \ out[pred(b)]$ |
| **Transfer Function**      | $f_b(x)=Gen_b \ \cup \ (x-Kill_b)$
| **Meet Operator**          | $\cap$
| **Boundary condition**     | $out[entry] = \emptyset$               |
| **Initial Interior Point** | $out[b] = U$                                                  |

#### Analisi

![itertable](https://github.com/user-attachments/assets/46947309-0962-431a-9eb1-b3a2c63b4f5c)
