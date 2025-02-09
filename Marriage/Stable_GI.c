/*************************************************************************
**************************************************************************
**                                                                      **
**  PROGRAMMA......:  Stable_GI.c                                       **
**  AUTORE.........:  M.A.W. 1968                                       **
**  LICENZA........:  Public Domain                                     **
**  DATA...........:  31/mar/2020 19:18                                 **
**  VERSIONE.......:  1.0                                               **
**                                                                      **
**  Descrizione....:  Originale implementazione dell'algoritmo          **
**                    di Gusfield & Irving per i matrimoni stabili.     **
**************************************************************************
**
**
**************************************************************************
*************************************************************************/

#include "Stable_GI.h"

/************************************************************************/
marriage_t Top(void)
{
    return SM.Stack[SM.stk_ptr-1];
}
/************************************************************************/
boole_t IsInStack(const marriage_t d)
{
    return SM.b_stack[d];
}
/************************************************************************/
marriage_t Pop(void)
{
    marriage_t retval = BLANK;

    if (SM.stk_ptr > 0)
    {
        retval = SM.Stack[--SM.stk_ptr];
        SM.b_stack[retval] = FALSE;
    }

    return (retval);
}
/************************************************************************/
void Push(const marriage_t d)
{
    if (SM.stk_ptr < 2*PROBLEM_SIZE)
    {
        SM.b_stack[d] = TRUE;
        SM.Stack[SM.stk_ptr++] = d;
    }
}
/************************************************************************/
void DisplayPrefs(void)
{
    size_t i, j;
    node_t *p;
    char buff[BUFF_SIZE];

    memset(buff, ' ', 80);

    puts("***** Uomini              Donne");
    for (i = 0; i < PROBLEM_SIZE; ++i)
    {
        j = 0;
        p = SM.M_pref[i].first->next;
        printf("**%2zu:", i+1);
        while (p != SM.M_pref[i].first)
        {
            sprintf(buff + j, "%2d", p->val+1);
            p = p->next;
            j += 2;
        }
        buff[j] = ' ';

        j = 20;
        p = SM.F_pref[i].first->next;
        while (p != SM.F_pref[i].first)
        {
            sprintf(buff + j, "%2d", p->val+1);
            p = p->next;
            j += 2;
        }
        puts(buff);
        memset(buff, ' ', 80);
    }
    puts("");
}
/************************************************************************/
void DisplayStableCouples(void)
{
    size_t i, j;

	puts(H_SEP_STAR);
	puts("Matrice delle coppie stabili (m,w)");
	puts(H_SEP_STAR);
	printf("     ");
	for (i = 0; i < PROBLEM_SIZE; ++i)
	{
		printf("%2u", i+1);
	}
	printf(" <- Donne\n");

    for (i = 0; i < PROBLEM_SIZE; ++i)
    {
		printf("**%2u:", i+1);
		for (j = 0; j < PROBLEM_SIZE; ++j)
		{
			printf("%2u", SM.StableCouples[i][j]);
		}
		puts("");
	}

	puts(H_SEP_STAR);
}
/************************************************************************/
void DisplaySolution(marriage_t sol[PROBLEM_SIZE], const char *label)
{
    size_t i;

    printf("%s = ", label);
    for (i = 0; i < PROBLEM_SIZE; ++i)
    {
        printf("%2d", sol[i]+1);
    }
    puts("");
}
/************************************************************************/
void InitData(void)
{
    size_t i, j;
    node_t *p;
    size_t size = 2 * (PROBLEM_SIZE+1) * PROBLEM_SIZE * sizeof(node_t);

    p = (node_t *)malloc(size);

    for (i = 0; i < PROBLEM_SIZE; ++i)
    {
        /* Nodo dummy in testa alla lista */
        SM.M_pref[i].first = p;
        p->val  = BLANK;
        p->prev = p;
        p->next = p+1;
        ++p;

        for (j = 0; j < PROBLEM_SIZE; ++j)
        {
            SM.F_LUT[i][M_choice[i][j]] = p;
            p->val = M_choice[i][j];
            p->prev = p-1;
            p->next = p+1;
            ++p;
        }
        /*
        ** L'ultimo nodo punta circolarmente al nodo dummy di testa.
        ** Si noti che invece SM.M_pref[i].first->prev risulta del tutto
        ** inutilizzato.
        */
        (p-1)->next = SM.M_pref[i].first;
        SM.M_pref[i].last  = p-1;
        SM.M_pref[i].total = PROBLEM_SIZE;
    }

    for (i = 0; i < PROBLEM_SIZE; ++i)
    {
        /* Nodo dummy in testa alla lista */
        SM.F_pref[i].first = p;
        p->val  = BLANK;
        p->prev = p;
        p->next = p+1;
        ++p;

        for (j = 0; j < PROBLEM_SIZE; ++j)
        {
            SM.M_LUT[i][F_choice[i][j]] = p;
            p->val = F_choice[i][j];
            p->prev = p-1;
            p->next = p+1;
            ++p;
        }
        (p-1)->next = SM.F_pref[i].first;
        SM.F_pref[i].last  = p-1;
        SM.F_pref[i].total = PROBLEM_SIZE;
    }
}
/************************************************************************/
void Delete(const marriage_t m, const marriage_t w)
{
    node_t *p;

    //printf("Delete(%u,%u)\n", m+1, w+1);

    /* Cancella la donna w dalla lista di m */
    p = SM.F_LUT[m][w];
    SM.M_pref[m].total -= 1;
    /* Si usa la tecnica dei dancing links di Knuth */
    p->prev->next = p->next;
    if (p->next != SM.M_pref[m].first)
    {
        p->next->prev = p->prev;
    }
	else
	{
		SM.M_pref[m].last = p->prev;
	}

    /* Cancella l'uomo m dalla lista di w */
    p = SM.M_LUT[w][m];
    SM.F_pref[w].total -= 1;
    p->prev->next = p->next;
    if (p->next != SM.F_pref[w].first)
    {
        p->next->prev = p->prev;
    }
	else
	{
		SM.F_pref[w].last = p->prev;
	}
}
/************************************************************************/
/************************************************************************/
void FindGSLists(void)
{
    node_t *p;
    size_t i;
    marriage_t w0, w2, m0, m2;

    /* Produce le MGS-List */
    for (i = 0; i < PROBLEM_SIZE; ++i)
    {
        Push(i);
        SM.Solution[i] = BLANK;
    }

    while (SM.stk_ptr > 0)
    {
        m0 = Pop();
        w0 = SM.M_pref[m0].first->next->val;
        m2 = SM.Solution[w0];

        if (BLANK != m2)
        {
            Push(m2);
        }

        SM.Solution[w0] = m0;

        p = SM.F_pref[w0].last;
        m2 = p->val;
        while (m2 != m0)
        {
            Delete(m2, w0);
            p = p->prev;
            m2 = p->val;
        }
    }

    /*
    ** Il secondo passaggio produce le GS-List invertendo i proponenti
    ** e usando le MGS-Lists come input.
    */
    for (i = 0; i < PROBLEM_SIZE; ++i)
    {
        Push(i);
        SM.F_Optimal[i] = BLANK;
		/*
		** Converte l'array temporaneo di reverse lookup nella corrispondente
		** soluzione man-oriented e aggiorna il lookup delle coppie.
		*/
		SM.M_Optimal[SM.Solution[i]] = i;
		SM.StableCouples[SM.Solution[i]][i] = TRUE;
    }
	/*
	** Prepara l'array di lavoro SM.Solution per la discesa alla
	** ricerca delle rotazioni.
	*/
	memcpy(SM.Solution, SM.M_Optimal, PROBLEM_SIZE);

    while (SM.stk_ptr > 0)
    {
        w0 = Pop();
        m0 = SM.F_pref[w0].first->next->val;
        w2 = SM.F_Optimal[m0];

        if (BLANK != w2)
        {
            Push(w2);
        }

        SM.F_Optimal[m0] = w0;

        p = SM.M_pref[m0].last;
        w2 = p->val;
        while (w2 != w0)
        {
            Delete(m0, w2);
            p = p->prev;
            w2 = p->val;
        }
    }
}

/************************************************************************/
void FindRotations(void)
{
    struct {
        marriage_t m, w;
    } Rho[PROBLEM_SIZE];
    size_t rotation = 1;
    marriage_t m0   = 0;
    marriage_t m2, w0;
    char buff[8];
    node_t *p;

    while (m0 < PROBLEM_SIZE)
    {
        if (SM.stk_ptr == 0)
        {
            while ((SM.Solution[m0] == SM.F_Optimal[m0]) && (m0 < PROBLEM_SIZE))
            {
                ++m0;
            }
            if (m0 < PROBLEM_SIZE) Push(m0);
        }

        if (SM.stk_ptr > 0)
        {
            size_t i;
            size_t idx = PROBLEM_SIZE-1;

            m0 = Top();
			w0 = SM.M_pref[m0].first->next->next->val;
            m2 = SM.F_pref[w0].last->val;

            while (!IsInStack(m2))
            {
                Push(m2);
				w0 = SM.M_pref[m2].first->next->next->val;
            	m2 = SM.F_pref[w0].last->val;
            }

            m0 = Pop();
            Rho[idx].m   = m0;
            Rho[idx--].w = SM.Solution[m0];
            while (m0 != m2)
            {
                m0 = Pop();
                Rho[idx].m   = m0;
                Rho[idx--].w = SM.Solution[m0];
            }
            ++idx;

            /* Output rho(n) */
            printf("rho%u: (%u,%u)", rotation, Rho[idx].m+1, Rho[idx].w+1);
            for (i = idx+1; i < PROBLEM_SIZE; ++i)
            {
                printf(",(%u,%u)", Rho[i].m+1, Rho[i].w+1);
            }
            puts("");

            /* Apply rotation */
            SM.Solution[Rho[PROBLEM_SIZE-1].m] = Rho[idx].w;
            for (i = idx; i < (PROBLEM_SIZE -1); ++i)
            {
                SM.Solution[Rho[i].m] = Rho[i+1].w;
            }

            /* Update preference lists */
            p = SM.F_pref[Rho[idx].w].last;
            m2 = p->val;
            while (m2 != Rho[PROBLEM_SIZE-1].m)
            {
                Delete(m2, Rho[idx].w);
                p = p->prev;
                m2 = p->val;
            }
            for (i = idx+1; i < PROBLEM_SIZE; ++i)
            {
                p = SM.F_pref[Rho[i].w].last;
                m2 = p->val;
                while (m2 != Rho[i-1].m)
                {
                    Delete(m2, Rho[i].w);
                    p = p->prev;
                    m2 = p->val;
                }
            }

            sprintf(buff, "M%u", rotation);
            DisplaySolution(SM.Solution, buff);
            /*DisplayPrefs();*/

            ++rotation;
        }
    }
}
/************************************************************************/
/************************************************************************/
/************************************************************************/
int main(void)
{
    SM.stk_ptr = 0;

    InitData();
    puts("***** Liste di preferenze date:");
    DisplayPrefs();

    FindGSLists();
    puts("***** Soluzioni estremali:");
    DisplaySolution(SM.M_Optimal, "M0");
    DisplaySolution(SM.F_Optimal, "Mz");

    puts("\n***** Liste di preferenze ridotte:");
    DisplayPrefs();

	DisplayStableCouples();

	FindRotations();

/* TBD:
1) Serve un backup delle liste di preferenze dentro FindRotations;

2) Aggiornare l'elenco dele coppie stabili e l'array globale delle rotazioni in FindRotations;

3) Costruire G_overstrike con una doppia scansione delle GS-lists, prima banalmente
   per i nodi di tipo 1, poi facendo scorrere ogni rho_n a sx di una posizione per costruire
   le nuove coppie m,w(i+1) e cercare eventuali cancellati m' a DX di m nella pref list
   di w(i+1), che non compaiano in altre rotazioni.
   Per ciascuno di essi, RLUT e si marca w(i+1) come tipo 2 etichetta rho_n nella pref list di m'.
   Per ogni coppia esplicitamente elencata in una rotazione, conviene marcare sia l'uomo
   che la donna con la label di tale rho_n. Questo facilita il secondo passaggio.

4) Aggiungere campi label e boole_t type1, type2 a node_t.

5) G_overstrike va rappresentato come LISTE DI ADIACENZA con un array di puntatori
   nel quale ad ogni locazione corrisponde una delle rotazioni rho. Le liste puntate da
   ciascuna delle locazioni iniziali in GAdjLists[] sono composte da nodi contenenti
   il solito puntatore next e come payload un indice dell'array SM.Rho[][].
*/

    return EXIT_SUCCESS;
}
/************************************************************************/
/************************************************************************/
/************************************************************************/

/************************************************************************/
/* EOF: Stable_GI.c                                                     */
/************************************************************************/
