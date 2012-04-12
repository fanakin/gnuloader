/*
	CONTAINERS.H
	TECH-PRO snc  1999,2000
        Autore : Sergio Bianchi
        TECH-PRO Applications Kernel (Tutti i diritti riservati)
*/

//---------------------------------------------------------------------------

#ifndef ContainersH
#define ContainersH
#include <q3cstring.h>
//////////////////////////// Exceptions classes ////////////////////////////////
// needs qt Q3CString
////////////////////////////////////////////////////////////////////////////////

class wtddException
{
  protected:
    char *m_why;

  public:
    wtddException(const char *why );
    virtual ~wtddException();
    virtual char *GetMsgError() { return m_why; }

};


class EKaShiftReg : public wtddException  {
    public:
	   EKaShiftReg(Q3CString Mess) : wtddException(Mess) { }
} ;

class EKaBuffer : public wtddException  {
    public:
	   EKaBuffer(Q3CString Mess) : wtddException(Mess) { }
} ;

class EKaArray : public wtddException  {
    public:
	   EKaArray(Q3CString Mess) : wtddException(Mess) { }
} ;

class EKaArrayID : public wtddException  {
    public:
	   EKaArrayID(Q3CString Mess) : wtddException(Mess) { }
} ;

class EKaArrayS : public wtddException  {
    public:
	   EKaArrayS(Q3CString Mess) : wtddException(Mess) { }
} ;

class EKaPArray : public wtddException  {
    public:
	   EKaPArray(Q3CString Mess) : wtddException(Mess) { }
} ;

class EKaPArrayID : public wtddException  {
    public:
	   EKaPArrayID(Q3CString Mess) : wtddException(Mess) { }
} ;

class EKaPArrayS : public wtddException  {
    public:
	   EKaPArrayS(Q3CString Mess) : wtddException(Mess) { }
} ;

class EKlList : public wtddException  {
    public:
	   EKlList(Q3CString Mess) : wtddException(Mess) { }
} ;

class EKlDList : public wtddException  {
    public:
	   EKlDList(Q3CString Mess) : wtddException(Mess) { }
} ;

class EKlTree : public wtddException  {
    public:
	   EKlTree(Q3CString Mess) : wtddException(Mess) { }
} ;

class EKlPTree : public wtddException  {
    public:
	   EKlPTree(Q3CString Mess) : wtddException(Mess) { }
} ;

/*
   Classi Containers Template con parametri int
   Incompatibili con precedenti versioni con parametri unsigned int

   Nomenclatura : Ka = Basato su array; Kl = Basato su link
                  "Name" il container contiene il dato
                  P"Name" il container contiene il puntatore al dato ed �                          responsabile della costruzione e distruzione del dato

   I containers basati su array sono veloci nella indicizzazione del dato ma nel
   ridimensionamento devono ricopiare l' intero array ogni volta.
   I containers basati su link sono lenti nella indicizzazione ed allocazione
   ma non necessitano di periodici ridimensionamenti.
   I containers contenenti il dato sono pi veloci per strutture piccole o dati
   elementari mentre per grosse strutture dove si richiede l' ordinamento o lo
   spostamento dei dati sono pi efficenti i containers di puntatori.
   La scelta del tipo va quindi valutata caso per caso.

   Nei containers basati su array non esiste una classe Stack pura ma si parte da
   una classe Array comprendente sia gli operatori di indicizzazione che quelli di Stack
   permettendo cos�il completo accesso allo Stack.
   Nelle code array (LIFO) si �mantenuta la terminologia Push / Pop intendendo con
   Push l' ingresso nella coda e con Pop l' uscita.

*/

///////////////////////////////// KaShiftReg ///////////////////////////////////
//  Registro a scorrimento; Elementi indicizzati
//  Basato su array che contiene il dato
//  Presuppone l' esistenza di un operatore di assegnamento "="
////////////////////////////////////////////////////////////////////////////////

template <class A> class KaShiftReg  {

  protected:
   A  *array;
   int arrsize;
   int arrpos;

  public:
   A inpreg;      // con ShInpReg1() inserisce inpreg in r[0]

   KaShiftReg(int size = 5); // effettuare Preset prima di operare !!!
   KaShiftReg(const A& elem, int size); // preset con elem
   KaShiftReg(const KaShiftReg<A>& ashreg);
   inline ~KaShiftReg() { delete [] array; }
   inline void ShLoop() {if (++arrpos >= arrsize) arrpos = 0;} // 1 shift in modo loop
   inline void ShInpReg() {array[arrpos] = inpreg; if (++arrpos >= arrsize) arrpos = 0;}
   // 1 shift in modo inpreg
   void Preset(const A& elem); // inizializza il registro con elem
   A& operator [](int i);  // indirizza il registro dall' inizio
   A& operator ()(int i);  // indirizza il registro dalla fine
   KaShiftReg<A>& operator = (const KaShiftReg<A>& ashreg);
   inline int Size() {return arrsize;}   // dimensione del buffer
   void SetSize(int size); // imposta nuova dimensione buffer
   // tronca i dati se riduce la dimensione
};


///////////////////////////////// KaBuffer //////////////////////////////////////
//  Buffer circolare; FIFO limitato; Coda limitata ; Elementi indicizzati
//  Basato su array che contiene il dato
//  Presuppone l' esistenza di un operatore di assegnamento "="
//  Presuppone l' esistenza di un operatore di confronto "=="
//  KaBuffer non autoincrementa le proprie dimensioni sebbene queste siano modificabili.
//  In caso di superamento se abilitato overlap ricopre i dati se no lancia eccezione.
////////////////////////////////////////////////////////////////////////////////

template <class B> class KaBuffer  {

  protected:
   B  *array;
   int arrsize;
   int inptr;
   int outptr;
   bool enaboverlap;
   bool overlap;

  public:
   KaBuffer(int size = 5, bool ovlap = true); // effettuare Preset per operare con Pos !!!
   KaBuffer(const B& elem, int size = 5, bool ovlap = true);  // preset con elem
   KaBuffer(const KaBuffer<B>& abuffer);
   inline ~KaBuffer() { delete [] array; }
   void Preset(const B& elem); // inizializza il buffer con elem
   inline int GetInPtr() { return  inptr; }
   inline int GetOutPtr() { return  outptr; }
   void EnableOverlap(bool ovlap);
   inline bool Overlap() { return overlap; }
   inline bool IsEmpty() { return (inptr == outptr); }
   bool IsFull();
   void Push(const B& elem);        // inserisce un elemento comunque
   bool PushNoDup(const B& elem);   // inserisce un elemento se non esiste gia'
   bool PushNoDupBack(const B& elem); // come PushNoDup ma porta indietro elem se
   //  esiste gi� es. :  E -> ABCDEFG = EABCDFG
   //  usato ad es. come memoria ultimi files aperti
   const B& Pop();                  // estrae un elemento
   B& Pos(int i);  // indirizzo assoluto del buffer
   // se eseguito setsize o non eseguito il preset pu�accedere ad aree non inizializzate !!!
   B& operator [](int i);  // indirizzo relativo da In
   B& operator ()(int i);  // indirizzo relativo da Out
   KaBuffer<B>& operator = (const KaBuffer<B>& abuffer);
   void Clear();
   int Count();            // elementi presenti nel buffer
   inline int Size() { return arrsize - 1; }   // dimensione del buffer
   void SetSize(int size); // imposta nuova dimensione buffer
   // tronca i dati se riduce la dimensione
   // vedi nota su Pos(int i)
};

//////////////////////////////////// KaArray ///////////////////////////////////
//  Array ridimensionabile; Stack; LIFO; Elementi indicizzati
//  Basato su array che contiene il dato
//  Push produce ridimensionamento automatico di step elementi
//  Presuppone l' esistenza di un operatore di assegnamento "="
////////////////////////////////////////////////////////////////////////////////

template <class T> class KaArray  {

  protected:
   T *array;
   int arrsize;
   int arrstep;
   int arrpos;

  public:
   KaArray(int size = 4, int step = 4);
   KaArray(const KaArray<T>& anarray);
   inline  ~KaArray() { delete [] array; }
   void Push(const T& elem);
   const T& Pop();
   inline const T& Peek() { return this->operator[](arrpos - 1); }
   T& operator [](int i);
   T& operator ()(int i);
   KaArray<T>& operator = (const KaArray<T>& anarray);
   void Clear(bool freemem = false);
   inline int Count() { return arrpos; }
   inline int Size() { return arrsize; }
   inline int Step() { return arrstep; }
   void SetSize(int size);
   void SetStep(int step);
};

////////////////////////////////////// KaArrayID ///////////////////////////////
//  Array + INSERT + DELETE
//  Presuppone l' esistenza di un operatore di confronto "=="
//  Basato su array che contiene il dato
//  Usato Per KSet
////////////////////////////////////////////////////////////////////////////////

template <class T> class KaArrayID : public KaArray<T>  {

  public:

   inline KaArrayID(int size = 4, int step = 4) : KaArray<T>(size, step) { }
   inline KaArrayID(const KaArray<T>& anarray) : KaArray<T>(anarray) { }
   inline  ~KaArrayID() { }
   bool PushNoDup(const T& elem);
   bool operator == (const KaArrayID<T>& anarray);
   void Insert(const T& elem, int i);  // Inserisce elem alla posizione i
   void Delete(int i);                 // Elimina l' elemento alla posiz. i
};

////////////////////////////////////// KaArrayS ///////////////////////////////
//  Array + SORT + HEAP
//  Basato su array che contiene il dato
//  presuppone l' esistenza di un operatore di confronto "<"
//  Implementa uno Heap e una coda con priorit�////////////////////////////////////////////////////////////////////////////////

template <class T> class KaArrayS : public KaArrayID<T>  {
  private:

   int heapsize;
   void Heapify(int i);   // funzioni di servizio per Heap
   inline int Parent(int i) { return ((i - 1) >> 1); }
   inline int Left(int i) { return ((i << 1) + 1); }
   inline int Right(int i) { return ((i + 1) << 1); }
   int Partition(int p, int r);  // funzioni di servizio per SortQuick
   void QuickSort(int p, int r);

  public:

   inline KaArrayS(int size = 4, int step = 4) : KaArrayID<T>(size, step) {}
   inline KaArrayS(const KaArray<T>& anarray) : KaArrayID<T>(anarray) {}
   inline  ~KaArrayS() {}
   void SortSel();   // Ordinamento per selezione  K*N^2 test; K*N switch (per grandi sizeof())
   void BuildHeap(); // Costruisce uno Heap
   void SortHeap();  // Ordinamento per Heap  K*N*LogN
   T HeapExtractMax(); // Valore massimo della coda
   void HeapInsert(const T& elem); // Inserisce un valore nella coda
   inline void SortQuick() { QuickSort(0, this->arrpos - 1); }
   // Ordinamento per QuickSort K*N*LogN medio (da sistemare :implementazione ricorsiva!)
};


//////////////////////////////////// KaPArray ///////////////////////////////////
//  Array ridimensionabile; Stack; LIFO; Elementi indicizzati
//  Basato su array che contiene il puntatore al dato
//  Push produce ridimensionamento automatico di step elementi
//  Presuppone l' esistenza di un operatore di assegnamento "="
////////////////////////////////////////////////////////////////////////////////

template <class T> class KaPArray  {

  protected:

   T 	**array;
   int arrsize;
   int arrpos;
   int arrstep;

  public:

   KaPArray(int size = 4, int step = 4);
   KaPArray(const KaPArray<T>& anarray);
   ~KaPArray();
   T& operator [](int i);
   T& operator ()(int i);
   void Push(const T& elem);
   T Pop();
   inline const T& Peek() {return this->operator[](arrpos - 1);}
   KaPArray<T>& operator = (const KaPArray<T>& anarray);
   void Clear(bool freemem = false);
   inline int Count() { return arrpos; }
   inline int Size() { return arrsize; }
   inline int Step() {return arrstep;}
   void SetSize(int size);
   void SetStep(int step);
};

////////////////////////////////////// KaPArrayID //////////////////////////////
//  Array + INSERT + DELETE
//  Presuppone l' esistenza di un operatore di confronto "=="
//  Basato su array che contiene il puntatore al dato
////////////////////////////////////////////////////////////////////////////////

template <class T> class KaPArrayID : public KaPArray<T>  {

  public:

   inline KaPArrayID(int size = 4, int step = 4) : KaPArray<T>(size, step) {}
   inline KaPArrayID(const KaPArray<T>& anarray) : KaPArray<T>(anarray) {}
   inline  ~KaPArrayID() {}
   bool PushNoDup(const T& elem);
   bool operator == (const KaPArrayID<T>& anarray);
   void Insert(const T& elem, int i);
   void Delete(int i);
};

////////////////////////////////////// KaPArrayS ///////////////////////////////
//  Array + SORT + HEAP
//  Basato su array che contiene il puntatore al dato
//  Presuppone l' esistenza di un operatore di confronto "<"
//  Implementa uno Heap e una coda con priorit�////////////////////////////////////////////////////////////////////////////////

template <class T> class KaPArrayS : public KaPArrayID<T>  {
  private:

   int heapsize;
   void Heapify(int i);   // funzioni di servizio per Heap
   inline int Parent(int i) { return ((i - 1) >> 1); }
   inline int Left(int i) { return ((i << 1) + 1); }
   inline int Right(int i) { return ((i + 1) << 1); }
   int Partition(int p, int r);  // funzioni di servizio per SortQuick
   void QuickSort(int p, int r);

  public:

   inline KaPArrayS(int size = 4, int step = 4) : KaPArrayID<T>(size, step) {}
   inline KaPArrayS(const KaPArray<T>& anarray) : KaPArrayID<T>(anarray) {}
   inline  ~KaPArrayS() {}
   void SortSel();   // Ordinamento per selezione (mantenuta per simmetria con KaArrayS)
   void BuildHeap(); // Costruisce uno Heap
   void SortHeap();  // Ordinamento per Heap        K*N*LogN
   T HeapExtractMax(); // Valore massimo della coda
   void HeapInsert(const T& elem); // Inserisce un valore nella coda
   inline void SortQuick() { QuickSort(0, this->arrpos - 1); }
   // Ordinamento per QuickSort K*N*LogN medio (da sistemare :implementazione ricorsiva!)
};

/////////////////////////////////// NODO LISTA  ////////////////////////////////

template <class N> struct KNode {
   N item;
   KNode<N> *next;
};

///////////////////////////////////// KlList ////////////////////////////////////
//  Lista; LIFO; Stack
//  Basato su link
////////////////////////////////////////////////////////////////////////////////

template <class N> class KlList {

   protected:

      KNode<N> *first;

   public:

      KlList();
      KlList(const KlList<N>& alist);
      ~KlList();
      void Push(const N& elem);
      N Pop();
      inline N& Peek() { return first->item; }
      bool IsEmpty();
      void Clear();
      KlList<N>& operator = (const KlList<N>& alist);
};


//////////////////////////// NODO LISTA DOPPIA ENTRATA /////////////////////////

template <class D> struct KDNode {
   D item;
   KDNode<D> *right;
   KDNode<D> *left;
};

///////////////////////////////////// KlDList //////////////////////////////////
//  Lista a Doppia Entrata; Stack - LIFO, Buffer - FIFO, Shift Register (es. LPop->RPush)
//  Basato su link
////////////////////////////////////////////////////////////////////////////////

template <class D> class KlDList {

   protected:

      KDNode<D> *lfirst;
      KDNode<D> *rfirst;

   public:

      KlDList();
      KlDList(const KlDList<D>& adlist);
      ~KlDList();
      void LPush(D elem);
      void RPush(D elem);
      D LPop();
      D RPop();
      inline D& LPeek() { return lfirst->item; }
      inline D& RPeek() { return rfirst->item; }
      inline bool IsEmpty() {return ((lfirst == NULL) && (rfirst == NULL));}
      void Clear();
      KlDList<D>& operator = (const KlDList<D>& adlist);
};

////////////////////////////////// KTNode //////////////////////////////////////
//  Contiene il dato
//  Struttura minimale per nodo albero (derivare da questa nodi pi complessi)
////////////////////////////////////////////////////////////////////////////////

template <class C> struct KTNode  {
   C item;  // dato
   KTNode<C> *right;
   // usato come destro in alberi binari o ramo successivo in alberi con figli illimitati
   KTNode<C> *left;
   // usato come sinistro in alberi binari o primo figlio in alberi con figli illimitati
   KTNode<C> *parent;  // nodo genitore
};

////////////////////////////////// KPTNode //////////////////////////////////////
//  Contiene il puntatore al dato
//  Struttura minimale per nodo albero (derivare da questa nodi pi complessi)
////////////////////////////////////////////////////////////////////////////////

template <class C> struct KPTNode  {
   C *item;  // puntatore al dato
   KPTNode<C> *right;
   // usato come destro in alberi binari o ramo successivo in alberi con figli illimitati
   KPTNode<C> *left;
   // usato come sinistro in alberi binari o primo figlio in alberi con figli illimitati
   KPTNode<C> *parent;  // nodo genitore
};

/////////////////////////////////// KlTree /////////////////////////////////////
//  Contiene il dato
//  Albero generico con numero di figli per nodo illimitato
////////////////////////////////////////////////////////////////////////////////

template <class C> class KlTree  {

   protected:

      KTNode<C> *root;

   public:

      inline KlTree() { root = NULL; }
      KlTree(const KlTree<C>& atree);
      inline ~KlTree() { DeleteNode(root); }
      void CopySubTree(KTNode<C>* dest, const KTNode<C>* source);
      // i sottorami del nodo dest vengono distrutti e quindi aggiunte copie dei
      // sottorami di source. Il valore C ed i fratelli di dest restano inalterati
      KlTree<C>& operator =(const KlTree<C>& atree);
      inline bool IsEmpty() { return (root == NULL); }
      inline KTNode<C>* GetRoot() { return root; }
      void CreateRoot(C anitem); // crea il nodo radice facendo prima un Clear()
      inline void Clear() { DeleteNode(root); }
      void PushChild(KTNode<C> *anode, C anitem); // aggiunge un figlio al nodo (LIFO)
      C PopChild(KTNode<C> *anode);  // elimina il primo figlio del nodo (LIFO)
      inline C& Peek(KTNode<C> *anode) { return anode->item; }
      inline KTNode<C>* FirstChild(KTNode<C> *anode) { return anode->left; }
      inline KTNode<C>* NextSibling(KTNode<C> *anode) { return anode->right; }
      inline KTNode<C>* Parent(KTNode<C> *anode) { return anode->parent; }
      inline bool HasChilds(KTNode<C> *anode) { return (anode->left != NULL); }
      inline bool IsLastSibling(KTNode<C> *anode) { return (anode->right != NULL); }
      void DeleteNode(KTNode<C> *anode);     // elimina un nodo ed i suoi figli
      void DeleteChilds(KTNode<C> *anode);   // elimina i figli di un nodo
      int TreeCount(KTNode<C> *anode); // conta i nodi di un albero da anode compreso
      int Count(KTNode<C> *anode);     // conta i figli di un nodo
      KTNode<C>* Child(KTNode<C> *anode, int n);  // restituisce il figlio n
};

/////////////////////////////////// KlPTree /////////////////////////////////////
//  Contiene il puntatore al dato
//  Albero generico con numero di figli per nodo illimitato
//  Attenzione : nell' uso con gerarchie di classi le funzioni di
//  costruttore copia, CopySubTree  e  operator =
//  producono istanze della classe base ! Non usarle in questo caso
//  ma creare una funzione esterna alla classe per la copia
////////////////////////////////////////////////////////////////////////////////

template <class C> class KlPTree  {

   protected:

      KPTNode<C> *root;

   public:

      inline KlPTree() { root = NULL; }
      KlPTree(const KlPTree<C>& atree);
      inline ~KlPTree() { DeleteNode(root); }
      void CopySubTree(KPTNode<C>* dest, const KPTNode<C>* source);
      // i sottorami del nodo dest vengono distrutti e quindi aggiunte copie dei
      // sottorami di source. Il valore C ed i fratelli di dest restano inalterati
      KlPTree<C>& operator =(const KlPTree<C>& atree);
      inline bool IsEmpty() { return (root == NULL); }
      inline KPTNode<C>* GetRoot() { return root; }
      void CreateRoot(C *anitem); // crea il nodo radice facendo prima un Clear()
      inline void Clear() { DeleteNode(root); }
      void PushChild(KPTNode<C> *anode, C *anitem); // aggiunge un figlio al nodo (LIFO)
      void PopChild(KPTNode<C> *anode);  // elimina il primo figlio del nodo (LIFO)
      // per accedere al valore usare Peek(FirstChild(..))
      inline C* Peek(KPTNode<C> *anode) { return anode->item; }
      inline KPTNode<C>* FirstChild(KPTNode<C> *anode) { return anode->left; }
      inline KPTNode<C>* NextSibling(KPTNode<C> *anode) { return anode->right; }
      inline KPTNode<C>* Parent(KPTNode<C> *anode) { return anode->parent; }
      inline bool HasChilds(KPTNode<C> *anode) { return (anode->left != NULL); }
      inline bool IsLastSibling(KPTNode<C> *anode) { return (anode->right == NULL); }
      void DeleteNode(KPTNode<C> *anode);     // elimina un nodo ed i suoi figli
      void DeleteChilds(KPTNode<C> *anode);   // elimina i figli di un nodo
      int TreeCount(KPTNode<C> *anode); // conta i nodi di un albero da anode compreso
      int Count(KPTNode<C> *anode);     // conta i figli di un nodo
      KPTNode<C>* Child(KPTNode<C> *anode, int n);  // restituisce il figlio n
};




#endif


