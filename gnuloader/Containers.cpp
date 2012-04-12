/*
	CONTAINERS.CPP
	TECH-PRO snc , 1999, 2000
        Autore : Sergio Bianchi
        TECH-PRO Applications Kernel (Tutti i diritti riservati)
*/

//---------------------------------------------------------------------------


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Containers.h"

wtddException::wtddException(const char *why)
{
  printf("Begin Constructor\n");
  m_why = (char *)malloc( strlen( why ) + 1 );
  if( m_why != NULL ) {
    strcpy( m_why, why );
    printf( " Error Message: \"%s\"\n", GetMsgError() );
  }
  printf("End Constructor\n");
}


wtddException::~wtddException()
{
  printf("Begin Destructor\n");
  if( m_why != NULL ) {
    free( m_why );
    m_why = NULL;
  }
  printf("End Destructor\n");
}


//////////////////////////////// KaShiftReg ////////////////////////////////////

template <class A> KaShiftReg<A>::KaShiftReg(int size)
{
   if (size < 1) throw EKaShiftReg("Size");
   arrsize = size;
   arrpos = 0;
   array = new A[arrsize];   // Run Preset before operate !!!
}

template <class A> KaShiftReg<A>::KaShiftReg(const A& elem, int size)
{
   if (size < 1) throw EKaShiftReg("SizeP");
   arrsize = size;
   arrpos = 0;
   array = new A[arrsize];
   for (int i = 0; i < arrsize; i++) array[i] = elem;
}

template <class A> KaShiftReg<A>::KaShiftReg(const KaShiftReg<A>& ashreg)
{
   arrsize = ashreg.arrsize;
   arrpos = ashreg.arrpos;
   array = new A[arrsize];
   for (int i = 0; i < arrsize; i++) array[i] = ashreg.array[i];
}

template <class A> void KaShiftReg<A>::Preset(const A& elem)
{
   for (int i = 0; i < arrsize; i++) array[i] = elem;
}

template <class A> A& KaShiftReg<A>::operator [](int i)
{
   if (i >= arrsize || i < 0) throw EKaShiftReg("[]Range");
   int p = arrpos + arrsize - (i + 1);
   if (p >= arrsize) p -= arrsize;
   return array[p];
}

template <class A> A& KaShiftReg<A>::operator ()(int i)
{
   if (i >= arrsize || i < 0) throw EKaShiftReg("()Range");
   int p = arrpos + i;
   if (p >= arrsize) p -= arrsize;
   return array[p];
}

template <class A> KaShiftReg<A>& KaShiftReg<A>::operator = (const KaShiftReg<A>& ashreg)
{
   if (&ashreg == this) return *this;
   delete [] array;
   arrsize = ashreg.arrsize;
   arrpos = ashreg.arrpos;
   array = new A[arrsize];
   for (int i = 0; i < arrsize; i++) array[i] = ashreg.array[i];
   return *this;
}

template <class A> void KaShiftReg<A>::SetSize(int size)
{
   if (size < 1) throw EKaShiftReg("SetSize");
   A *temp = new A[size];
   if(size > arrsize) {
      for (int i = arrsize; i < size; i++) temp[i] = array[arrpos];
      for (int i = 0; i < arrsize; i++) temp[i + size - arrsize] = this->operator()(i);
   }
   else {
      for (int i = 0; i < size; i++) temp[i] = this->operator()(i);
   }
   delete [] array;
   array = temp;
   arrsize = size;
   arrpos = 0;
}

///////////////////////////////// KaBuffer //////////////////////////////////////

template <class B> KaBuffer<B>::KaBuffer(int size, bool ovlap)
{
   if (size < 2) throw EKaBuffer("Size");
   arrsize = size + 1;
   inptr = 0;
   outptr = 0;
   enaboverlap = ovlap;
   overlap = false;
   array = new B[arrsize];    // Run Preset before operate !!!
}

template <class B> KaBuffer<B>::KaBuffer(const B& elem, int size, bool ovlap)
{
   if (size < 2) throw EKaBuffer("Size");
   arrsize = size + 1;
   inptr = 0;
   outptr = 0;
   enaboverlap = ovlap;
   overlap = false;
   array = new B[arrsize];
   for (int i = 0; i < arrsize; i++) array[i] = elem;
}

template <class B> KaBuffer<B>::KaBuffer(const KaBuffer<B>& abuffer)
{
   arrsize = abuffer.arrsize;
   inptr = abuffer.inptr;
   outptr = abuffer.outptr;
   enaboverlap = abuffer.enaboverlap;
   overlap = abuffer.overlap;
   array = new B[arrsize];
   for (int i = 0; i < arrsize; i++) array[i] = abuffer.array[i];
}

template <class B> void KaBuffer<B>::Preset(const B& elem)
{
   for (int i = 0; i < arrsize; i++) array[i] = elem;
}

template <class B> void KaBuffer<B>::EnableOverlap(bool ovlap)
{
   enaboverlap = ovlap;
   if (!enaboverlap) overlap = false;
}

template <class B> bool KaBuffer<B>::IsFull()
{
   if ((inptr > outptr) && ((inptr - outptr) == (arrsize - 1))) return true;
   else if (outptr > inptr && (outptr - inptr) == 1) return true;
   else return false;
}

template <class B> void KaBuffer<B>::Push(const B& elem)
{
   if (enaboverlap) {
      array[inptr] = elem;
      if (++inptr >= arrsize) inptr = 0;
      if (inptr == outptr) {
         ++outptr;
         overlap = true;
      }
      if (outptr >= arrsize) outptr = 0;
   }
   else {
      array[inptr] = elem;
      if (++inptr >= arrsize) inptr = 0;
      if (inptr == outptr) {
         if (inptr > 0) --inptr;
         else inptr = arrsize - 1;
         throw EKaBuffer("Overlap");
      }
   }
}

template <class B> bool KaBuffer<B>::PushNoDup(const B& elem)
{
   int ptr = outptr;
   bool insert = true;

   while (ptr != inptr) {
      if (array[ptr] == elem) {
         insert = false;
         break;
      }
      if (++ptr >= arrsize) ptr = 0;
   }
   if (insert) Push(elem);
   return insert;
}

template <class B> bool KaBuffer<B>::PushNoDupBack(const B& elem)
{
   int ptr = outptr;
   int i, j;
   bool insert = true;

   while (ptr != inptr) {
      if (array[ptr] == elem) {
        insert = false;
        i = ptr;
        break;
      }
      if (++ptr >= arrsize) ptr = 0;
   }
   if (insert) Push(elem);
   else {
      j = i;
      while (i != inptr) {
         if (++j >= arrsize) j = 0;
         if (j != inptr)
            array[i] = array[j];
         else
            array[i] = elem;
         i = j;
      }
   }
   return insert;
}

template <class B> const B&  KaBuffer<B>::Pop()
{
   if (inptr == outptr) throw EKaBuffer("Empty");
   int i = outptr;
   if (++outptr >= arrsize) outptr = 0;
   return array[i];
}

template <class B> B&  KaBuffer<B>::Pos(int i)
{
   if (i >= arrsize || i < 0) throw EKaBuffer("<< Range");
   return array[i];                       // Vedi note in .H !!!
}

template <class B> B&  KaBuffer<B>::operator ()(int i)
{
   if (i >= arrsize || i < 0) throw EKaBuffer("(1)Range");
   int p = outptr + i;
   if (p >= arrsize) p -= arrsize;
   if (inptr > outptr) {
      if (p < inptr && p >= outptr) return array[p];
      else throw EKaBuffer("(2)Range");
   }
   else if (outptr > inptr) {
      if (p >= inptr && p < outptr) throw EKaBuffer("(3)Range");
      else return array[p];
   }
   else throw EKaBuffer("(4)Range");
}

template <class B> B&  KaBuffer<B>::operator [](int i)
{
   if (i >= arrsize || i < 0) throw EKaBuffer("[]1 Range");
   int p = inptr + arrsize - (i + 1);
   if (p >= arrsize) p -= arrsize;
   if (inptr > outptr) {
      if (p < inptr && p >= outptr) return array[p];
      else throw EKaBuffer("[]2 Range");
   }
   else if (outptr > inptr) {
      if (p >= inptr && p < outptr) throw EKaBuffer("[]3 Range");
      else return array[p];

   }
   else throw EKaBuffer("[]4 Range");
}

template <class B> KaBuffer<B>& KaBuffer<B>::operator = (const KaBuffer<B>& abuffer)
{
   if (&abuffer == this) return *this;
   delete [] array;
   arrsize = abuffer.arrsize;
   inptr = abuffer.inptr;
   outptr = abuffer.outptr;
   enaboverlap = abuffer.enaboverlap;
   overlap = abuffer.overlap;
   array = new B[arrsize];
   for (int i = 0; i < arrsize; i++) array[i] = abuffer.array[i];
   return *this;
}

template <class B> void KaBuffer<B>::Clear()
{
   outptr = inptr = 0;
   overlap = false;
}

template <class B> int KaBuffer<B>::Count()
{
   if (inptr > outptr) return inptr - outptr;
   else if (outptr > inptr) return arrsize - (outptr - inptr);
   else return 0;
}

template <class B> void KaBuffer<B>::SetSize(int size)
{
   if (size < 2) throw EKaBuffer("SetSize");
   int c = Count();
   if (c > size) c = size;
   B *temp = new B[size + 1];
   for (int i = 0; i < c; i++) temp[i] = this->operator()(i);
   delete [] array;
   array = temp;
   arrsize = size + 1;
   inptr = c;
   outptr = 0;                            // Vedi note in .H !!!
}

////////////////////////////////////// KaArray /////////////////////////////////

template <class T> KaArray<T>::KaArray(int size, int step)
{
   if (size < 1) throw EKaArray("Size");
   if (step < 1) throw EKaArray("Step");
   arrsize = size;
   arrstep = step;
   arrpos = 0;
   array = new T[arrsize];
}

template <class T> KaArray<T>::KaArray(const KaArray<T>& anarray)
{
   arrsize = anarray.arrsize;
   arrstep = anarray.arrstep;
   arrpos = anarray.arrpos;
   array = new T[arrsize];
   for (int i = 0; i < arrpos; i++) array[i] = anarray.array[i];
}

template <class T> void KaArray<T>::Push(const T& elem)
{
   if (arrpos < arrsize) {
        array[arrpos++] = elem;
   }
   else {
        T *temp;
        arrsize += arrstep;
    	temp = new T[arrsize];
	for (int i = 0; i < arrpos; i++) temp[i] = array[i];
      	delete [] array;
      	array = temp;
        array[arrpos++] = elem;
   }
}

template <class T> const T& KaArray<T>::Pop()
{
   if (arrpos == 0) throw EKaArray("Pop Underflow");
   return array[--arrpos];
}

template <class T> void KaArray<T>::Clear(bool freemem)
{
   arrpos = 0;
   if (freemem) {
      delete [] array;
      arrsize = 4;
      arrstep = 4;
      array = new T[arrsize];
   }
}

template <class T> T& KaArray<T>::operator [](int i)
{
   if (i >= arrpos || i < 0) throw EKaArray("[]Range");
   return array[i];
}

template <class T> T& KaArray<T>::operator ()(int i)
{
   if (i >= arrpos || i < 0) throw EKaArray("()Range");
   return array[arrpos - (i + 1)];
}

template <class T> KaArray<T>& KaArray<T>::operator = (const KaArray<T>& anarray)
{
   if (&anarray == this) return *this;
   delete [] array;
   arrstep = anarray.arrstep;
   arrsize = anarray.arrsize;
   arrpos = anarray.arrpos;
   array = new T[arrsize];
   for (int i = 0; i < arrpos; i++) array[i] = anarray.array[i];
   return *this;
}

template <class T> void KaArray<T>::SetSize(int size)
{
   if (size > arrsize) {
        T *temp;
        arrsize = size;
        temp = new T[arrsize];
        for (int i = 0; i < arrpos; i++) temp[i] = array[i];
      	delete [] array;
      	array = temp;
   }
}

template <class T> void KaArray<T>::SetStep(int step)
{
   if (step < 1) throw EKaArray("SetStep");
   arrstep = step;
}

///////////////////////////////////// KaArrayID ////////////////////////////////

template <class T> void  KaArrayID<T>::Insert(const T& elem, int i)
{
   if (i < 0) throw EKaArrayID("-Insert");
   if (i < this->arrpos) {
      int k = this->arrpos - 1;
      Push(this->array[k]);
      for (int j = k; j > i; j--) this->array[j] = this->array[j - 1];
      this->array[i] = elem;
   }
   else if (i == this->arrpos) Push(elem);
   else throw EKaArrayID("+Insert");
}

template <class T> bool KaArrayID<T>::PushNoDup(const T& elem)
{
   bool insert = true;
   for (int i = 0; i < this->arrpos; i++) {
      if (this->array[i] == elem) {
        insert = false;
        break;
      }
   }
   if (insert) Push(elem);
   return insert;
}

template <class T> bool KaArrayID<T>::operator == (const KaArrayID<T>& anarray)
{
   if (this->arrpos != anarray.arrpos) return false;
   for (int i = 0; i < this->arrpos; i++)
      if (!(this->array[i] == anarray.array[i])) return false;
   return true;
}

template <class T> void  KaArrayID<T>::Delete(int i)
{
   if (i < 0) throw EKaArrayID("-Delete");
   if (i >= this->arrpos) throw EKaArrayID("+Delete");
   int k = this->arrpos - 1;
   for (int j = i; j < k; j++) this->array[j] = this->array[j + 1];
   this->Pop();
}

///////////////////////////////////// KaArrayS /////////////////////////////////


template <class T> void  KaArrayS<T>::SortSel()
{
   T temp;
   int i, j, min;

   for (i = 0; i < this->arrpos; i++) {
       min = i;
       for (j = i; j < this->arrpos; j++)
          if (this->array[j] < this->array[min]) min = j;
       temp = this->array[i];
       this->array[i] = this->array[min];
       this->array[min] = temp;
   }
}

template <class T> void  KaArrayS<T>::Heapify(int i)
{
   int max, l, r, ci;
   ci = i;
   for (;;) {
     l = Left(ci);
     r = Right(ci);
     if (l < heapsize && this->array[ci] < this->array[l]) max = l;
     else max = ci;
     if (r < heapsize && this->array[max] < this->array[r]) max = r;
     if (max != ci) {
        T temp = this->array[ci];
        this->array[ci] = this->array[max];
        this->array[max] = temp;
        ci = max;
     }
     else break;
   }
}

template <class T> void  KaArrayS<T>::BuildHeap()
{
   heapsize = this->arrpos;
   int k = this->arrpos - 1;
   for (int i = k; i >= 0; i--) Heapify(i);
}

template <class T> void  KaArrayS<T>::SortHeap()
{
   BuildHeap();
   // heapsize = arrpos; effettuato in BuildHeap
   int k = this->arrpos - 1;
   for (int i = k; i >= 1; i--) {
      T temp = this->array[0];
      this->array[0] = this->array[i];
      this->array[i] = temp;
      --heapsize;
      Heapify(0);
   }
}

template <class T> T KaArrayS<T>::HeapExtractMax()
{
   if (this->arrpos < 1) throw EKaArrayS("PriQueue Empty");
   T temp = this->array[0];
   this->array[0] = this->array[this->arrpos-1];
   this->Pop();
   heapsize = this->arrpos;
   Heapify(0);
   return temp;
}

template <class T> void KaArrayS<T>::HeapInsert(const T& elem)
{
   int j;
   Push(elem);
   int i = this->arrpos - 1;
   while (i > 0 && this->array[(j = Parent(i))] < elem) {
      this->array[i] = this->array[j];
      i = j;
   }
   this->array[i] = elem;
}

template <class T> int KaArrayS<T>::Partition(int p, int r)
{
   T x = this->array[p];
   int i = p - 1;
   int j = r + 1;
   for (;;) {
      while(x < this->array[--j]);
      while(this->array[++i] < x);
      if (i < j) {
         T temp = this->array[i];
         this->array[i] = this->array[j];
         this->array[j] = temp;
      }
      else return j;
   }
}

template <class T> void  KaArrayS<T>::QuickSort(int p, int r)
{
   if (p < r) {
      int q = Partition(p, r);
      QuickSort(p, q);
      QuickSort(q + 1, r);
   }
}

////////////////////////////////////// KaPArray /////////////////////////////////

template <class T> KaPArray<T>::KaPArray(int size, int step )
{
   if (size < 1) throw EKaPArray("Size");
   if (step < 1) throw EKaPArray("Step");
   arrsize = size;
   arrstep = step;
   arrpos = 0;
   array = new T*[arrsize];
   for (int i = 0; i < arrsize; i++) array[i] = NULL;
}

template <class T> KaPArray<T>::KaPArray(const KaPArray<T>& anarray)
{
   arrsize = anarray.arrsize;
   arrstep = anarray.arrstep;
   arrpos = anarray.arrpos;
   array = new T*[arrsize];
   for (int i = arrpos; i < arrsize; i++) array[i] = NULL;
   for (int i = 0; i < arrpos; i++) {
      T *p = new T(*(anarray.array[i]));
      array[i] = p;
   }
}

template <class T> KaPArray<T>::~KaPArray()
{
   for (int i = 0; i < arrsize; i++) if (array[i] != NULL) delete array[i];
   delete [] array;
}

template <class T> void KaPArray<T>::Clear(bool freemem)
{
   for (int i = 0; i < arrsize; i++) if (array[i] != NULL) delete array[i];
   arrpos = 0;
   if (freemem) {
      delete [] array;
      arrsize = 4;
      arrstep = 4;
      array = new T*[arrsize];
   }
   for (int i = 0; i < arrsize; i++) array[i] = NULL;
}

template <class T> void KaPArray<T>::Push(const T& elem)
{
   if (arrpos < arrsize) {
      T *p = new T(elem);
      array[arrpos++] = p;
   }
   else {
      T **temp;
      arrsize += arrstep;
      temp = new T*[arrsize];
      for (int i = arrpos; i < arrsize; i++) temp[i] = NULL;
      for (int i = 0; i < arrpos; i++) temp[i] = array[i];
      delete [] array;
      array = temp;
      T *p = new T(elem);
      array[arrpos++] = p;
   }
}

template <class T> T KaPArray<T>::Pop()
{
   if (arrpos == 0) throw EKaPArray("Pop Underflow");
   T p(*array[--arrpos]);
   delete array[arrpos];
   array[arrpos] = NULL;
   return p;
}

template <class T> T& KaPArray<T>::operator [](int i)
{
   if (i >= arrpos || i < 0) throw EKaPArray("[]Range");
   return *array[i];
}

template <class T> T& KaPArray<T>::operator ()(int i)
{
   if (i >= arrpos || i < 0) throw EKaPArray("()Range");
   return *array[arrpos - (i + 1)];
}


template <class T> KaPArray<T>& KaPArray<T>::operator = (const KaPArray<T>& anarray)
{
   if (&anarray == this) return *this;
   for (int i = 0; i < arrsize; i++) if (array[i] != NULL) delete array[i];
   delete [] array;
   arrstep = anarray.arrstep;
   arrsize = anarray.arrsize;
   arrpos = anarray.arrpos;
   array = new T*[arrsize];
   for (int i = arrpos; i < arrsize; i++) array[i] = NULL;
   for (int i = 0; i < arrpos; i++) {
      T *p = new T(*(anarray.array[i]));
      array[i] = p;
   }
   return *this;
}

template <class T> void KaPArray<T>::SetSize(int size)
{
   if (size > arrsize) {
      T **temp;
      arrsize = size;
      temp = new T*[arrsize];
      for (int i = arrpos; i < arrsize; i++) temp[i] = NULL;
      for (int i = 0; i < arrpos; i++) temp[i] = array[i];
      delete [] array;
      array = temp;
   }
}

template <class T> void KaPArray<T>::SetStep(int step)
{
   if (step < 1) throw EKaPArray("SetStep");
   arrstep = step;
}

///////////////////////////////////// KaPArrayID ////////////////////////////////

template <class T> void  KaPArrayID<T>::Insert(const T& elem, int i)
{
   if (i < 0) throw EKaPArrayID("-Insert");
   if (i < this->arrpos) {
      if (this->arrpos >= this->arrsize) {
         T **temp;
         this->arrsize += this->arrstep;
         temp = new T*[this->arrsize];
         for (int j = this->arrpos; j < this->arrsize; j++) temp[j] = NULL;
         for (int j = 0; j < this->arrpos; j++) temp[j] = this->array[j];
         delete [] this->array;
         this->array = temp;
      }
      for (int j = this->arrpos - 1; j > i; j--) this->array[j] = this->array[j - 1];
      T *p = new T(elem);
      this->array[i] = p;
   }
   else if (i == this->arrpos) Push(elem);
   else throw EKaPArrayID("+Insert");
}

template <class T> void  KaPArrayID<T>::Delete(int i)
{
   if (i < 0) throw EKaPArrayID("-Delete");
   if (i >= this->arrpos) throw EKaPArrayID("-Delete");
   delete this->array[i];
   int k = this->arrpos - 1;
   for (int j = i; j < k; j++) this->array[j] = this->array[j + 1];
   this->array[--this->arrpos] = NULL;
}

template <class T> bool KaPArrayID<T>::operator == (const KaPArrayID<T>& anarray)
{
   if (this->arrpos != anarray.arrpos) return false;
   for (int i = 0; i < this->arrpos; i++)
      if (!(*(this->array[i]) == (*(anarray.array[i])))) return false;
   return true;
}

template <class T> bool KaPArrayID<T>::PushNoDup(const T& elem)
{
   bool insert = true;
   for (int i = 0; i < this->arrpos; i++) {
      if ((*(this->array[i])) == elem) {
        insert = false;
        break;
      }
   }
   if (insert) Push(elem);
   return insert;
}

///////////////////////////////////// KaPArrayS /////////////////////////////////


template <class T> void  KaPArrayS<T>::SortSel()
{
   T *temp;
   int i,j,min;

   for (i = 0; i < this->arrpos; i++) {
       min = i;
       for (j = i; j < this->arrpos; j++)
          if ((*(this->array[j])) < (*(this->array[min]))) min = j;
       temp = this->array[i];
       this->array[i] = this->array[min];
       this->array[min] = temp;
   }
}

template <class T> void  KaPArrayS<T>::Heapify(int i)
{
   int max, l, r, ci;
   ci = i;
   for (;;) {
     l = Left(ci);
     r = Right(ci);
     if (l < heapsize && (*(this->array[ci])) < (*(this->array[l]))) max = l;
     else max = ci;
     if (r < heapsize && (*(this->array[max])) < (*(this->array[r]))) max = r;
     if (max != ci) {
        T *temp = this->array[ci];
        this->array[ci] = this->array[max];
        this->array[max] = temp;
        ci = max;
     }
     else break;
   }
}

template <class T> void  KaPArrayS<T>::BuildHeap()
{
   heapsize = this->arrpos;
   int k = this->arrpos - 1;
   for (int i = k; i >= 0; i--) Heapify(i);
}

template <class T> void  KaPArrayS<T>::SortHeap()
{
   BuildHeap();
   // heapsize = arrpos; effettuato in BuildHeap
   int k = this->arrpos - 1;
   for (int i = k; i >= 1; i--) {
      T *temp = this->array[0];
      this->array[0] = this->array[i];
      this->array[i] = temp;
      --heapsize;
      Heapify(0);
   }
}

template <class T> T KaPArrayS<T>::HeapExtractMax()
{
   if (this->arrpos < 1) throw EKaPArrayS("PriQueue Empty");
   T temp = *(this->array[0]);
   T *tp = this->array[0];
   this->array[0] = this->array[this->arrpos-1];
   this->array[this->arrpos-1] = tp;
   this->Pop();
   heapsize = this->arrpos;
   Heapify(0);
   return temp;
}

template <class T> void KaPArrayS<T>::HeapInsert(const T& elem)
{
   int j;
   Push(elem);
   int i = this->arrpos - 1;
   T *p = this->array[i];
   while (i > 0 && (*(this->array[(j = Parent(i))])) < elem) {
      this->array[i] = this->array[j];
      i = j;
   }
   this->array[i] = p;
}

template <class T> int KaPArrayS<T>::Partition(int p, int r)
{
   T *x = this->array[p];
   int i = p - 1;
   int j = r + 1;
   for (;;) {
      while(*x < (*(this->array[--j])));
      while((*(this->array[++i])) < *x);
      if (i < j) {
         T *temp = this->array[i];
         this->array[i] = this->array[j];
         this->array[j] = temp;
      }
      else return j;
   }
}

template <class T> void  KaPArrayS<T>::QuickSort(int p, int r)
{
   if (p < r) {
      int q = Partition(p, r);
      QuickSort(p, q);
      QuickSort(q + 1, r);
   }
}

///////////////////////////////////// KlList ///////////////////////////////////

template <class N> KlList<N>::KlList()
{
    first = NULL;
}

template <class N> KlList<N>::KlList(const KlList<N>& alist)
{
    KNode<N> *e;
    KNode<N> *t = alist.first;

    if (t != NULL) {
        e = new KNode<N>;
        first = e;
    }
    else first = NULL;
    while (t !=  NULL) {
        e->item = t->item;
        t = t->next;
        if (t != NULL) {
            e->next = new KNode<N>;
            e = e->next;
        }
        else e->next = NULL;
    }
}

template <class N> KlList<N>::~KlList()

{
   Clear();
}

template <class N> void KlList<N>::Push(const N& elem)
{
   KNode<N> *t = new KNode<N>;
   t->item = elem;
   t->next = first;
   first = t;
}

template <class N> N KlList<N>::Pop()
{
   KNode<N> *t = first;
   if (t == NULL) throw EKlList("Pop Underflow");
   first = t->next;
   N elem = t->item;
   delete t;
   return elem;
}

template <class N>  bool KlList<N>::IsEmpty()
{
   return (first == NULL);
}


template <class N> void KlList<N>::Clear()
{
   KNode<N> *t = first;
   while(t != NULL) {
      first = t->next;
      delete t;
      t = first;
   }
}

template <class N> KlList<N>& KlList<N>::operator = (const KlList<N>& alist)
{
    Clear();

    KNode<N> *e;
    KNode<N> *t = alist.first;

    if (t != NULL) {
        e = new KNode<N>;
        first = e;
    }
    else first = NULL;
    while (t !=  NULL) {
        e->item = t->item;
        t = t->next;
        if (t != NULL) {
            e->next = new KNode<N>;
            e = e->next;
        }
        else e->next = NULL;
    }

    return *this;
}

////////////////////////////////////  KlDList  /////////////////////////////////

template <class D> KlDList<D>::KlDList()
{
    lfirst = NULL;
    rfirst = NULL;
}

template <class D> KlDList<D>::KlDList(const KlDList<D>& adlist)
{
    KDNode<D> *e;
    KDNode<D> *t = adlist.lfirst;

    if (t != NULL) {
        e = new KDNode<D>;
        lfirst = e;
        e->left = NULL;
    }
    else {
        lfirst = NULL;
        rfirst = NULL;
    }
    while (t !=  NULL) {
        e->item = t->item;
        t = t->right;
        if (t != NULL) {
            e->right = new KDNode<D>;
            (e->right)->left = e;
            e = e->right;
        }
        else {
            e->right = NULL;
            rfirst = e;
        }
    }
}

template <class D> KlDList<D>::~KlDList()
{
   Clear();
}

template <class D> void KlDList<D>::LPush(D elem)
{
   KDNode<D> *p = new KDNode<D>;
   p->item  = elem;
   p->right = lfirst;
   p->left  = NULL;
   if (lfirst != NULL) lfirst->left = p;
   else rfirst = p;
   lfirst = p;
}

template <class D> void KlDList<D>::RPush(D elem)
{
   KDNode<D> *p = new KDNode<D>;
   p->item  = elem;
   p->left = rfirst;
   p->right  = NULL;
   if (rfirst != NULL) rfirst->right = p;
   else lfirst = p;
   rfirst = p;
}

template <class D> D KlDList<D>::LPop()
{
   KDNode<D> *p = lfirst;
   if (p == NULL) throw EKlDList("LPop Underflow");
   lfirst = p->right;
   if (lfirst != NULL) lfirst->left = NULL;
   else rfirst = NULL;
   D elem = p->item;
   delete p;
   return elem;
}

template <class D> D KlDList<D>::RPop()
{
   KDNode<D> *p = rfirst;
   if (p == NULL) throw EKlList("RPop Underflow");
   rfirst = p->left;
   if (rfirst != NULL) rfirst->right = NULL;
   else lfirst = NULL;
   D elem = p->item;
   delete p;
   return elem;
}

template <class D> void KlDList<D>::Clear()
{
   KDNode<D> *p = lfirst;

   while (p != NULL) {
      lfirst = p->right;
      if (lfirst != NULL) lfirst->left = NULL;
      else rfirst = NULL;
      delete p;
      p = lfirst;
   }
}

template <class D> KlDList<D>& KlDList<D>::operator = (const KlDList<D>& adlist)
{
    Clear();

    KDNode<D> *e;
    KDNode<D> *t = adlist.lfirst;

    if (t != NULL) {
        e = new KDNode<D>;
        lfirst = e;
        e->left = NULL;
    }
    else {
        lfirst = NULL;
        rfirst = NULL;
    }
    while (t !=  NULL) {
        e->item = t->item;
        t = t->right;
        if (t != NULL) {
            e->right = new KDNode<D>;
            (e->right)->left = e;
            e = e->right;
        }
        else {
            e->right = NULL;
            rfirst = e;
        }
    }
    return *this;
}

////////////////////////////////////  KlTree  ///////////////////////////////////

template <class C> KlTree<C>::KlTree(const KlTree<C>& atree)
{
   root = NULL;
   if (atree.root != NULL) {
      root = new KTNode<C>;
      root->parent = NULL;
      root->left = NULL;
      root->right = NULL;
      root->item = atree.root->item;
      CopySubTree(root, atree.root);
   }
}

template <class C> void KlTree<C>::CopySubTree(KTNode<C> *dest, const KTNode<C> *source)
{
   KTNode<C> *nn, *pn;
   KTNode<C> *cn = source->left;
   if (cn == NULL) return;
   if (dest->left != NULL) DeleteChilds(dest);
   nn = new KTNode<C>;
   dest->left = nn;
   for (;;) {
      nn->parent = dest;
      nn->left = NULL;
      nn->right = NULL;
      nn->item = cn->item;
      pn = nn;
      CopySubTree(nn, cn);
      if (cn->right == NULL) break;
      cn = cn->right;
      nn = new KTNode<C>;
      pn->right = nn;
   }
}

template <class C> void KlTree<C>::CreateRoot(C anitem)
{
   Clear();
   root = new KTNode<C>;
   root->parent = NULL;
   root->left = NULL;
   root->right = NULL;
   root->item = anitem;
}

template <class C> KlTree<C>&  KlTree<C>::operator =(const KlTree<C>& atree)
{
   if (&atree == this) return *this;
   Clear();
   root = NULL;
   if (atree.root != NULL) {
      root = new KTNode<C>;
      root->parent = NULL;
      root->left = NULL;
      root->right = NULL;
      root->item = atree.root->item;
      CopySubTree(root, atree.root);
   }
   return *this;
}

template <class C> void KlTree<C>::PushChild(KTNode<C> *anode, C anitem)
{
   KTNode<C> *p;
   p = new KTNode<C>;
   p->parent = anode;
   p->left = NULL;
   p->item = anitem;
   p->right = anode->left;
   anode->left = p;
}

template <class C> C KlTree<C>::PopChild(KTNode<C> *anode)
{
   if (anode->left == NULL) throw EKlTree("No Child");
   C temp = anode->left->item;
   DeleteNode(anode->left);
   return temp;
}

template <class C> void KlTree<C>::DeleteNode(KTNode<C> *anode)
{
   if (anode == NULL) return;
   if (anode == root) {
      DeleteChilds(root);
      delete root;
      root = NULL;
   }
   else {
      DeleteChilds(anode);
      if (anode->parent->left == anode) {
         anode->parent->left = anode->right;
         delete anode;
      }
      else {
         KTNode<C> *p = anode->parent->left;
         KTNode<C> *s = NULL;
         while (p != anode) {
            if (p->right == NULL) throw EKlTree("Node not found");
            s = p;
            p = p->right;
         }
         s->right = p->right;
         delete anode;
      }
   }
}

template <class C> void KlTree<C>::DeleteChilds(KTNode<C> *anode)
{
   if (anode == NULL) return;
   KTNode<C> *p = anode->left;
   while (p != NULL) {
      DeleteNode(p);
      p = anode->left;
   }
}

template <class C> int KlTree<C>::TreeCount(KTNode<C> *anode)
{
   if (anode == NULL) return 0;
   int c = 1;
   KTNode<C> *p = anode->left;
   while (p != NULL) {
      c += TreeCount(p);
      p = p->right;
   }
   return c;
}

template <class C> int KlTree<C>::Count(KTNode<C> *anode)
{
   if (anode == NULL) return 0;
   int c = 0;
   KTNode<C> *p = anode->left;
   while (p != NULL) {
      ++c;
      p = p->right;
   }
   return c;
}

template <class C> KTNode<C>* KlTree<C>::Child(KTNode<C> *anode, int n)
{
   if (anode == NULL || anode->left == NULL) throw EKlTree("No Child");
   int c = 0;
   KTNode<C> *p = anode->left;
   while (p != NULL) {
      if (n == c) return p;
      ++c;
      p = p->right;
   }
   throw EKlTree("Child not exist");
}


////////////////////////////////////  KlPTree  ///////////////////////////////////

template <class C> KlPTree<C>::KlPTree(const KlPTree<C>& atree)
{
   root = NULL;
   if (atree.root != NULL) {
      root = new KPTNode<C>;
      root->parent = NULL;
      root->left = NULL;
      root->right = NULL;
      root->item = new C(*(atree.root->item));
      CopySubTree(root, atree.root);
   }
}

template <class C> void KlPTree<C>::CopySubTree(KPTNode<C> *dest, const KPTNode<C> *source)
{
   KPTNode<C> *nn, *pn;
   KPTNode<C> *cn = source->left;
   if (cn == NULL) return;
   if (dest->left != NULL) DeleteChilds(dest);
   nn = new KPTNode<C>;
   dest->left = nn;
   for (;;) {
      nn->parent = dest;
      nn->left = NULL;
      nn->right = NULL;
      nn->item = new C(*(cn->item));
      pn = nn;
      CopySubTree(nn, cn);
      if (cn->right == NULL) break;
      cn = cn->right;
      nn = new KPTNode<C>;
      pn->right = nn;
   }
}

template <class C> void KlPTree<C>::CreateRoot(C *anitem)
{
   Clear();
   root = new KPTNode<C>;
   root->parent = NULL;
   root->left = NULL;
   root->right = NULL;
   root->item = anitem;
}

template <class C> KlPTree<C>&  KlPTree<C>::operator =(const KlPTree<C>& atree)
{
   if (&atree == this) return *this;
   Clear();
   root = NULL;
   if (atree.root != NULL) {
      root = new KPTNode<C>;
      root->parent = NULL;
      root->left = NULL;
      root->right = NULL;
      root->item = new C(*(atree.root->item));
      CopySubTree(root, atree.root);
   }
   return *this;
}

template <class C> void KlPTree<C>::PushChild(KPTNode<C> *anode, C *anitem)
{
   KPTNode<C> *p;
   p = new KPTNode<C>;
   p->parent = anode;
   p->left = NULL;
   p->item = anitem;
   p->right = anode->left;
   anode->left = p;
}

template <class C> void KlPTree<C>::PopChild(KPTNode<C> *anode)
{
   if (anode->left == NULL) throw EKlPTree("No Child");
   DeleteNode(anode->left);
}

template <class C> void KlPTree<C>::DeleteNode(KPTNode<C> *anode)
{
   if (anode == NULL) return;
   if (anode == root) {
      DeleteChilds(root);
      delete root->item;
      delete root;
      root = NULL;
   }
   else {
      DeleteChilds(anode);
      if (anode->parent->left == anode) {
         anode->parent->left = anode->right;
         delete anode->item;
         delete anode;
      }
      else {
         KPTNode<C> *p = anode->parent->left;
         KPTNode<C> *s = NULL;
         while (p != anode) {
            if (p->right == NULL) throw EKlPTree("Node not found");
            s = p;
            p = p->right;
         }
         s->right = p->right;
         delete anode->item;
         delete anode;
      }
   }
}

template <class C> void KlPTree<C>::DeleteChilds(KPTNode<C> *anode)
{
   if (anode == NULL) return;
   KPTNode<C> *p = anode->left;
   while (p != NULL) {
      DeleteNode(p);
      p = anode->left;
   }
}

template <class C> int KlPTree<C>::TreeCount(KPTNode<C> *anode)
{
   if (anode == NULL) return 0;
   int c = 1;
   KPTNode<C> *p = anode->left;
   while (p != NULL) {
      c += TreeCount(p);
      p = p->right;
   }
   return c;
}

template <class C> int KlPTree<C>::Count(KPTNode<C> *anode)
{
   if (anode == NULL) return 0;
   int c = 0;
   KPTNode<C> *p = anode->left;
   while (p != NULL) {
      ++c;
      p = p->right;
   }
   return c;
}

template <class C> KPTNode<C>* KlPTree<C>::Child(KPTNode<C> *anode, int n)
{
   if (anode == NULL || anode->left == NULL) throw EKlPTree("No Child");
   int c = 0;
   KPTNode<C> *p = anode->left;
   while (p != NULL) {
      if (n == c) return p;
      ++c;
      p = p->right;
   }
   throw EKlPTree("Child not exist");
}


////////////////////////////////////////////////////////////////////////////////
#define _INSTANTIATE_TEMPLATE_
#include "realtemplateclasses.h"

