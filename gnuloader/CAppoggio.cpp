/***************************************************************************
                          main.h  -  description
                             -------------------
    begin                : Thu May 17 2001
    copyright            : (C) 2001 by aurion s.r.l
    email                : fabio.giovagnini@aurion-tech.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

//-----------------------------------------------------------------------------
#include <stdio.h>
#include "WTDDLibreria.h"
#include "CAppoggio.h"
//extern unsigned long NByteTot;

//-----------------------------------------------------------------------------
// NOTA riguardo la classe Record
// i record di dati possono avere la seguente forma:
//
//               2 * ToNum(xx)
//           _______________
//	S1xxyyyyaabb...zzcc
//               _________
//                2*(ToNum(xx) - 3)
//
//               2 * ToNum(xx)
//           _________________
//	S2xxyyyyyyaabb...zzcc
//               ___________
//                2*(ToNum(xx) - 4)
//
//               2 * ToNum(xx)
//           ___________________
//	S3xxyyyyyyyyaabb...zzcc
//               _____________
//                2*(ToNum(xx) - 5)
//
//	xx = numero di byte; yyyy = indirizzo; aabb...zz = bytes dei dati; cc = checksum
//-----------------------------------------------------------------------------
// debug 		fprintf(stdout,"%s,%s,%d.\r\n",__FILE__,__FUNCTION__,__LINE__);
//-----------------------------------------------------------------------------
// Questa versione e' stata fatta per lavorare con multi quanti.

Record::Record(void)
{
Tipo = NONE;
NumOfByte = 0;
BaseAddr = 0;
NumOfData = 0;
Data = NULL;
CheckSum = 0;
}
//-----------------------------------------------------------------------------
Record::~Record(void)
{
if (Data != NULL)
   {
   delete Data;
   Data = NULL;
   }
}
//-----------------------------------------------------------------------------
Record::Record(const KwtddString Str)
{
	KwtddString App,S0,S1,S2,S3,S7,S8,S9;
	S0.Push('S');S0.Push('0');S0.Push('\0');
	S1.Push('S');S1.Push('1');S1.Push('\0');
	S2.Push('S');S2.Push('2');S2.Push('\0');
	S3.Push('S');S3.Push('3');S3.Push('\0');
	S7.Push('S');S7.Push('7');S7.Push('\0');
	S8.Push('S');S8.Push('8');S8.Push('\0');
	S9.Push('S');S9.Push('9');S9.Push('\0');

	App.Push(((KwtddString) Str)[0]);
	App.Push(((KwtddString) Str)[1]);
	App.Push('\0');

	KwtddString AddrType(App);

	if (App == S0) Tipo = HEADER;
	else if ((App == S2) || (App == S1) || (App == S3)) Tipo = DATA;   // S1 0x0000 0xffff , S2 0x010000 0x1ffff , S3 0xFFFFFFFF
	else if ((App == S7) || (App == S8) || (App == S9)) Tipo = FINAL;
	else Tipo = NONE;

	App.Clear();
	App.Push(((KwtddString) Str)[2]);
	App.Push(((KwtddString) Str)[3]);
	App.Push('\0');

	NumOfByte = (unsigned char) wtddahextol((&(App[0])));

	if (AddrType == S1) NumOfData = (unsigned char) (NumOfByte - 3);
	else if (AddrType == S2) NumOfData = (unsigned char) (NumOfByte - 4);
	else if (AddrType == S3) NumOfData = (unsigned char) (NumOfByte - 5);
	else NumOfData = (unsigned char) (0);
	
	Data = new unsigned char[NumOfData];

	if (AddrType == S1) {
		App.Clear();
		App.Push(((KwtddString) Str)[4]);
		App.Push(((KwtddString) Str)[5]);
		App.Push(((KwtddString) Str)[6]);
		App.Push(((KwtddString) Str)[7]);
		App.Push('\0');
		BaseAddr = (unsigned long) wtddahextol((&(App[0])));
		int i;
		for (i = 0; i < NumOfData; i++) {
			App.Clear();
			App.Push(((KwtddString) Str)[8 + (i * 2)]);
			App.Push(((KwtddString) Str)[8 + (i * 2) + 1]);
			App.Push('\0');
			Data[i] = (unsigned char) wtddahextol((&(App[0])));
			}

		App.Clear();
		App.Push(((KwtddString) Str)[8 + (i * 2)]);
		App.Push(((KwtddString) Str)[8 + (i * 2) + 1]);
		App.Push('\0');
		CheckSum = (unsigned char) wtddahextol((&(App[0])));
		}
	else if (AddrType == S2) {
		App.Clear();
		App.Push(((KwtddString) Str)[4]);
		App.Push(((KwtddString) Str)[5]);
		App.Push(((KwtddString) Str)[6]);
		App.Push(((KwtddString) Str)[7]);

		App.Push(((KwtddString) Str)[8]);
		App.Push(((KwtddString) Str)[9]);
		App.Push('\0');
		BaseAddr = (unsigned long) wtddahextol((&(App[0])));
		int i;
		for (i = 0; i < NumOfData; i++) {
			App.Clear();
			App.Push(((KwtddString) Str)[10 + (i * 2)]);
			App.Push(((KwtddString) Str)[10 + (i * 2) + 1]);
			App.Push('\0');
			Data[i] = (unsigned char) wtddahextol((&(App[0])));
			}

		App.Clear();
		App.Push(((KwtddString) Str)[10 + (i * 2)]);
		App.Push(((KwtddString) Str)[10 + (i * 2) + 1]);
		App.Push('\0');
		CheckSum = (unsigned char) wtddahextol((&(App[0])));
		}
	else if (AddrType == S3) {
		App.Clear();
		App.Push(((KwtddString) Str)[4]);
		App.Push(((KwtddString) Str)[5]);
		App.Push(((KwtddString) Str)[6]);
		App.Push(((KwtddString) Str)[7]);
		App.Push(((KwtddString) Str)[8]);
		App.Push(((KwtddString) Str)[9]);
		App.Push(((KwtddString) Str)[10]);
		App.Push(((KwtddString) Str)[11]);
		App.Push('\0');
		BaseAddr = (unsigned long) wtddahextol((&(App[0])));
		int i;
		for (i = 0; i < NumOfData; i++) {
			App.Clear();
			App.Push(((KwtddString) Str)[12 + (i * 2)]);
			App.Push(((KwtddString) Str)[12 + (i * 2) + 1]);
			App.Push('\0');
			Data[i] = (unsigned char) wtddahextol((&(App[0])));
			}

		App.Clear();
		App.Push(((KwtddString) Str)[12 + (i * 2)]);
		App.Push(((KwtddString) Str)[12 + (i * 2) + 1]);
		App.Push('\0');
		CheckSum = (unsigned char) wtddahextol((&(App[0])));
		}
}
//-----------------------------------------------------------------------------
Record& Record::operator =(const Record& R)
{
Tipo = R.Tipo;
NumOfByte = R.NumOfByte;
BaseAddr = R.BaseAddr;
NumOfData = R.NumOfData;
if (Data != NULL)
   {
   delete Data;
   Data = NULL;
   }

Data = new unsigned char[NumOfData];
for (int i = 0; i < NumOfData; i++)
    Data[i] = R.Data[i];
CheckSum = R.CheckSum;

return (*this);
}
//-----------------------------------------------------------------------------
bool Record::SendRecord(arnComPort *HPt)
{
if (Tipo == DATA)
   {
   char Tx_Buffer[64],Rx_Buffer[64];

   Tx_Buffer[0] = (char) ('P');
   Tx_Buffer[1] = (char) ((unsigned char) (NumOfData + 5));
   HPt->PurgeRx();
   HPt->PurgeTx();
   if (!HPt->SendData(Tx_Buffer,2))
      return false;
   if (!HPt->ReceiveData(Rx_Buffer,1))
      return false;
   if (Rx_Buffer[0] != 'K')
          return false;

   Tx_Buffer[2] = (char) ((BaseAddr & 0xFF000000) >> 24);
   Tx_Buffer[3] = (char) ((BaseAddr & 0x00FF0000) >> 16);
   Tx_Buffer[4] = (char) ((BaseAddr & 0x0000FF00) >> 8);
   Tx_Buffer[5] = (char) ((BaseAddr & 0x000000FF));

   for (int i = 0; i < NumOfData; i++)
       Tx_Buffer[6 + i] = (char) (Data[i]);

  Tx_Buffer[6 + NumOfData] = 0;
  for (unsigned char count = 0; count < (NumOfData + 6); count++)
      Tx_Buffer[6 + NumOfData] += Tx_Buffer[count];

  HPt->PurgeRx();
  HPt->PurgeTx();
  if (!HPt->SendData((Tx_Buffer + 2),(NumOfData + 5)))
          return false;
  while(!HPt->ReceiveData(Rx_Buffer,1));
  if (Rx_Buffer[0] != 0x03)
     return false;
  }
else if (Tipo == FINAL)
   {
   char Tx_Buffer[2],Rx_Buffer[2];
   // ******** record finale E0xFF
   Tx_Buffer[0] = (char) ('E');
   Tx_Buffer[1] = (char) (0xFF);

   HPt->PurgeRx();
   HPt->PurgeTx();
   if (!HPt->SendData(Tx_Buffer,2))
      return false;
   if (!HPt->ReceiveData(Rx_Buffer,1))
      return false;
   if (Tx_Buffer[0] != Rx_Buffer[0])
      return false;

   }
else if (Tipo == HEADER)
   return false;
else if (Tipo == NONE)
   return false;

return true;
}
//-----------------------------------------------------------------------------
bool Record::SendData(arnComPort *HPt)
{
char Tx_Buffer,Rx_Buffer;

for (int i = 0; i < NumOfData; i++)
    {
    Tx_Buffer = (char) (Data[i]);
    HPt->PurgeRx();
    HPt->PurgeTx();
    if (!HPt->SendData(&Tx_Buffer,1))
       return false;

    if (!HPt->ReceiveData(&Rx_Buffer,1))
       return false;

    if (Tx_Buffer != Rx_Buffer)
       return false;
    }
return true;
}
//-----------------------------------------------------------------------------





















DataStream::DataStream(targetdatasize_t datasize, targetpagesize_t pagesize)
{
	targetdatasize = datasize;
	targetpagesize = pagesize;
	numofquantum = 0;
	Block8bit = NULL;
}

DataStream::~DataStream(void)
{
	if (Block8bit != NULL) {
		for (unsigned short i = 0; i < numofquantum; i++) if (Block8bit[i]->Data != NULL) delete [] Block8bit[i]->Data;
		delete [] Block8bit;
		Block8bit = 0;
		}
}


bool DataStream::SendByteFormBlock8bit(arnComPort *HPt)
{
	char Tx_Buffer,Rx_Buffer;
	for (unsigned short v = 0; v < numofquantum; v++) {
		if (Block8bit[v]->ptr > Block8bit[v]->Size) Block8bit[v]->ptr = Block8bit[v]->Size;
		for (unsigned long i = 0; i < Block8bit[v]->ptr; i++) {
			Tx_Buffer = (char) (Block8bit[v]->Data[i]);
			HPt->PurgeRx();
			HPt->PurgeTx();
			if (!HPt->SendData(&Tx_Buffer,1)) return false;
			if (!HPt->ReceiveData(&Rx_Buffer,1)) return false;
			if (Tx_Buffer != Rx_Buffer) return false;
			fprintf(stdout,"%d byte inviati su %d bytes totali\r",(int)i,(int)Block8bit[v]->ptr);
			}
		}
	return true;
}


































DataStreamPage32byte::DataStreamPage32byte(targetdatasize_t datasize)
	: DataStream(datasize,_tps32)
{
	page32bytesNumber  = 0;
	pages = NULL;
}

DataStreamPage32byte::~DataStreamPage32byte(void)
{
	if (pages) delete [] pages;
	pages = NULL;
	page32bytesNumber  = 0;
}

bool DataStreamPage32byte::ConvFromRecordQuantum8bit(Record* RecArr,int RecArrSize,
                unsigned short numofquan,
                unsigned long* startaddr_arr,
                unsigned long* size_arr)
{
	unsigned long* startaddr_array;
	unsigned long* size_array;

	if ((numofquan < 1) || (!startaddr_arr) || (!size_arr)) throw ("DataStream: Error in constructor parameter definition");
	numofquantum = numofquan;
	startaddr_array = new unsigned long [numofquantum];
	if (!startaddr_array) throw ("DataStream: startaddr_array : Error in constructor parameter allocation");
	size_array = new unsigned long [numofquantum];
	if (!size_array) throw ("DataStream: size_array : Error in constructor parameter allocation");
	for (unsigned short i = 0; i < numofquantum; i++) {
		startaddr_array[i] = startaddr_arr[i];
		size_array[i] = size_arr[i];
		page32bytesNumber +=  (size_array[i] / PAGESIZE32);
		}
	Block8bit = NULL;
//   page32bytesNumber  = PAGESNUMBER;
	switch (targetdatasize) {
		case _8bit:
			Block8bit = new quantum8bit_t*[numofquantum];
			if (!Block8bit) throw ("DataStream: Block8bit : Error in constructor parameter allocation");
        		for (unsigned short i = 0; i < numofquantum; i++) {
        			Block8bit[i] = new quantum8bit_t();
        			if (!Block8bit[i]) throw ("DataStream: Block8bit : Error in constructor parameter allocation");
        			Block8bit[i]->BaseAddr = startaddr_array[i];
        			Block8bit[i]->Size = size_array[i];
        			Block8bit[i]->Data = new unsigned char[Block8bit[i]->Size];
        			if (!Block8bit[i]->Data) throw ("DataStream: Block8bit : Error in constructor parameter allocation");
        			Block8bit[i]->ptr = 0;
        			for (unsigned long j = 0; j < Block8bit[i]->Size; j++)  Block8bit[i]->Data[j] = 0xFF;
        			}
        		break;
		default : break;
		}
	if (startaddr_array) delete [] startaddr_array;
	if (size_array) delete [] size_array;


	unsigned long Off = 0 ;
	unsigned long BAddr = 0;
	RecordArricchito* RecL = new  RecordArricchito();
	if (!RecL) throw ("DataStream: RecL : Error in constructor parameter allocation");
	for (int i = 0; i < RecArrSize; i++) {
		(*((Record*)RecL)) = RecArr[i];
		BAddr = RecL->GetBaseAddr();
		for (unsigned short v = 0; v < numofquantum; v++) {
			if ((BAddr >= Block8bit[v]->BaseAddr) && (((double)BAddr) < (((double)(Block8bit[v]->BaseAddr)) + Block8bit[v]->Size))) {
				Off = BAddr - Block8bit[v]->BaseAddr;
				if ((Off > Block8bit[v]->Size) || (RecL->GetTipo() != DATA)) continue;
				for (int j = 0; j <  RecL->GetNumOfData(); j++)
					if ((Off + j) <= Block8bit[v]->Size) {
						Block8bit[v]->Data[Off + j] = RecL->GetData((unsigned char)j);
						Block8bit[v]->ptr = Off + j;
						}
				Block8bit[v]->ptr++;
				break;
				}
			}
		}

	unsigned long acc = 0;//Block8bit[0]->BaseAddr;
	pages = new page32bytes[page32bytesNumber];
	if (!pages) throw ("DataStream: pages : Error in constructor parameter allocation");
	for (unsigned long i = 0; i < page32bytesNumber;) {
		pages[i].Addr = acc;
		for (unsigned short v = 0; v < numofquantum; v++) {
			if ((pages[i].Addr >= Block8bit[v]->BaseAddr) && (((double)(pages[i].Addr)) < (((double)(Block8bit[v]->BaseAddr)) + Block8bit[v]->Size))) {
				for (int j = 0; j < PAGESIZE32; j++) pages[i].Data[j] = Block8bit[v]->Data[acc - Block8bit[v]->BaseAddr + j];
				acc += PAGESIZE32;
				i++;
				break;
				}
			else if (pages[i].Addr < Block8bit[v]->BaseAddr) {
				acc = Block8bit[v]->BaseAddr;
				}
			}
		}
	delete RecL;
	return true;
}

bool DataStreamPage32byte::IsDummyPage(unsigned long Num)
{
	for (int i = 0; i < PAGESIZE32; i++) if (pages[Num].Data[i] != 0xFF) return false;
	return true;
}

bool DataStreamPage32byte::SendPage(arnComPort *HPt,unsigned long Num)
{
	pages[Num].ntwrite = 0;
	if (IsDummyPage(Num)) return true;

	char Tx_Buffer[64],Rx_Buffer[64];

	Tx_Buffer[0] = (char) ('P');
	Tx_Buffer[1] = (char) ((unsigned char) (PAGESIZE32 + 5));
	HPt->PurgeRx();
	HPt->PurgeTx();
	if (!HPt->SendData(Tx_Buffer,2)) {return false;}

	if (!HPt->ReceiveData(Rx_Buffer,1)) {
		return false;
		}
	if (Rx_Buffer[0] != 'K') {return false;}

	Tx_Buffer[2] = (char) ((pages[Num].Addr & 0xFF000000) >> 24);
	Tx_Buffer[3] = (char) ((pages[Num].Addr & 0x00FF0000) >> 16);
	Tx_Buffer[4] = (char) ((pages[Num].Addr & 0x0000FF00) >> 8);
	Tx_Buffer[5] = (char) ((pages[Num].Addr & 0x000000FF));
//	test test test
//	if (pages[Num].Addr ==0x00410000) {
//		unsigned char ppp= 0;
//		ppp++;
//		}

	for (int i = 0; i < PAGESIZE32; i++) Tx_Buffer[6 + i] = (char) (pages[Num].Data[i]);

	Tx_Buffer[6 + PAGESIZE32] = 0;
	for (unsigned char count = 0; count < (PAGESIZE32 + 6); count++) Tx_Buffer[6 + PAGESIZE32] += Tx_Buffer[count];

	HPt->PurgeRx();
	HPt->PurgeTx();
	if (!HPt->SendData((Tx_Buffer + 2),(PAGESIZE32 + 5))) {return false;}
	Rx_Buffer[0] = 0;
	unsigned char cntt = 3;
 	while((Rx_Buffer[0] != ((char)ADDROUTOFRANGE)) &&
		(Rx_Buffer[0] != ((char)PRGNOTPAGE)) &&
		(Rx_Buffer[0] != ((char)PRGOKPAGE)) &&
		(Rx_Buffer[0] != 'C') &&
		(cntt)
		) {
		if (HPt->ReceiveData(Rx_Buffer,1)) {
		/*if ((Rx_Buffer[0] != ((char)PRGNOTPAGE)) &&
			(Rx_Buffer[0] != ((char)PRGOKPAGE))&&
			(Rx_Buffer[0] != 'C') &&
			(Rx_Buffer[0] != ((char)ADDROUTOFRANGE))
			) pages[Num].ntwrite = Rx_Buffer[0];
		*/
			if (Rx_Buffer[0] == 'N') {
				if (HPt->ReceiveData(Rx_Buffer + 1,1)) {
					if (Rx_Buffer[1] == 'T') {
						if (HPt->ReceiveData(Rx_Buffer + 2,1)) {
							pages[Num].ntwrite = Rx_Buffer[2];
							}
						}
					}
				}
			}
		else cntt--;
		}
	if ((!cntt) || (Rx_Buffer[0] == 'C') || (((unsigned char) Rx_Buffer[0]) == PRGNOTPAGE)) {return false;}
	else if (((unsigned char) Rx_Buffer[0]) == ADDROUTOFRANGE) {
		HPt->ReceiveData(Rx_Buffer + 1,4);
		return false;
		}
	return true;
}


bool DataStreamPage32byte::SendEndSignal(arnComPort *HPt)
{
   char Tx_Buffer[2],Rx_Buffer[2];
   // ******** record finale E0xFF
   Tx_Buffer[0] = (char) ('E');
   Tx_Buffer[1] = (char) (0xFF);

   HPt->PurgeRx();
   HPt->PurgeTx();
   if (!HPt->SendData(Tx_Buffer,2)) return false;
   if (!HPt->ReceiveData(Rx_Buffer,1)) return false;
   if (Tx_Buffer[0] != Rx_Buffer[0]) return false;
   return true;
}




DataStreamPage128byte::DataStreamPage128byte(targetdatasize_t datasize)
	: DataStream(datasize,_tps128)
{
	page128bytesNumber  = 0;
	pages = NULL;
}

DataStreamPage128byte::~DataStreamPage128byte(void)
{
	if (pages) delete [] pages;
	pages = NULL;
	page128bytesNumber  = 0;
}

bool DataStreamPage128byte::ConvFromRecordQuantum8bit(Record* RecArr,int RecArrSize,
                unsigned short numofquan,
                unsigned long* startaddr_arr,
                unsigned long* size_arr)
{
	unsigned long* startaddr_array;
	unsigned long* size_array;

	if ((numofquan < 1) || (!startaddr_arr) || (!size_arr)) throw ("DataStream: Error in constructor parameter definition");
	numofquantum = numofquan;
	startaddr_array = new unsigned long [numofquantum];
	if (!startaddr_array) throw ("DataStream: startaddr_array : Error in constructor parameter allocation");
	size_array = new unsigned long [numofquantum];
	if (!size_array) throw ("DataStream: size_array : Error in constructor parameter allocation");
	for (unsigned short i = 0; i < numofquantum; i++) {
		startaddr_array[i] = startaddr_arr[i];
		size_array[i] = size_arr[i];
		page128bytesNumber +=  (size_array[i] / PAGESIZE128);
		}
	Block8bit = NULL;
//   page128bytesNumber  = PAGESNUMBER;
	switch (targetdatasize) {
		case _8bit:
			Block8bit = new quantum8bit_t*[numofquantum];
			if (!Block8bit) throw ("DataStream: Block8bit : Error in constructor parameter allocation");
        		for (unsigned short i = 0; i < numofquantum; i++) {
        			Block8bit[i] = new quantum8bit_t();
        			if (!Block8bit[i]) throw ("DataStream: Block8bit : Error in constructor parameter allocation");
        			Block8bit[i]->BaseAddr = startaddr_array[i];
        			Block8bit[i]->Size = size_array[i];
        			Block8bit[i]->Data = new unsigned char[Block8bit[i]->Size];
        			if (!Block8bit[i]->Data) throw ("DataStream: Block8bit : Error in constructor parameter allocation");
        			Block8bit[i]->ptr = 0;
        			for (unsigned long j = 0; j < Block8bit[i]->Size; j++)  Block8bit[i]->Data[j] = 0xFF;
        			}
        		break;
		default : break;
		}
	if (startaddr_array) delete [] startaddr_array;
	if (size_array) delete [] size_array;


	unsigned long Off = 0 ;
	unsigned long BAddr = 0;
	RecordArricchito* RecL = new  RecordArricchito();
	if (!RecL) throw ("DataStream: RecL : Error in constructor parameter allocation");
	for (int i = 0; i < RecArrSize; i++) {
		(*((Record*)RecL)) = RecArr[i];
		BAddr = RecL->GetBaseAddr();
		for (unsigned short v = 0; v < numofquantum; v++) {
			if ((BAddr >= Block8bit[v]->BaseAddr) && (((double)BAddr) < (((double)(Block8bit[v]->BaseAddr)) + Block8bit[v]->Size))) {
				Off = BAddr - Block8bit[v]->BaseAddr;
				if ((Off > Block8bit[v]->Size) || (RecL->GetTipo() != DATA)) continue;
				for (int j = 0; j <  RecL->GetNumOfData(); j++)
					if ((Off + j) <= Block8bit[v]->Size) {
						Block8bit[v]->Data[Off + j] = RecL->GetData((unsigned char)j);
						Block8bit[v]->ptr = Off + j;
						}
				Block8bit[v]->ptr++;
				break;
				}
			}
		}
	unsigned long acc = 0;//Block8bit[0]->BaseAddr;
	pages = new page128bytes[page128bytesNumber];
	if (!pages) throw ("DataStream: pages : Error in constructor parameter allocation");
	for (unsigned long i = 0; i < page128bytesNumber;) {
		pages[i].Addr = acc;
		for (unsigned short v = 0; v < numofquantum; v++) {
			if ((pages[i].Addr >= Block8bit[v]->BaseAddr) && (((double)(pages[i].Addr)) < (((double)(Block8bit[v]->BaseAddr)) + Block8bit[v]->Size))) { // somma maggiore di 0xffffffff
				for (int j = 0; j < PAGESIZE128; j++) pages[i].Data[j] = Block8bit[v]->Data[acc - Block8bit[v]->BaseAddr + j];
				acc += PAGESIZE128;
				i++;
				break;
				}
			else if (pages[i].Addr < Block8bit[v]->BaseAddr)
				acc = Block8bit[v]->BaseAddr;
			}
		}
	delete RecL;
	return true;
}

bool DataStreamPage128byte::IsDummyPage(unsigned long Num)
{
	for (int i = 0; i < PAGESIZE128; i++) if (pages[Num].Data[i] != 0xFF) return false;
	return true;
}

bool DataStreamPage128byte::SendPage(arnComPort *HPt,unsigned long Num)
{
	pages[Num].ntwrite = 0;
	if (IsDummyPage(Num)) return true;

	char Tx_Buffer[512],Rx_Buffer[64];

	Tx_Buffer[0] = (char) ('P');
	Tx_Buffer[1] = (char) ((unsigned char) (PAGESIZE128 + 5));
	HPt->PurgeRx();
	HPt->PurgeTx();
	if (!HPt->SendData(Tx_Buffer,2)) {
		return false;
		}

	if (!HPt->ReceiveData(Rx_Buffer,1)) {
		return false;
		}
	if (Rx_Buffer[0] != 'K') {
		return false;
		}

	Tx_Buffer[2] = (char) ((pages[Num].Addr & 0xFF000000) >> 24);
	Tx_Buffer[3] = (char) ((pages[Num].Addr & 0x00FF0000) >> 16);
	Tx_Buffer[4] = (char) ((pages[Num].Addr & 0x0000FF00) >> 8);
	Tx_Buffer[5] = (char) ((pages[Num].Addr & 0x000000FF));
//	test test test
//	if (pages[Num].Addr ==0x00410000) {
//		unsigned char ppp= 0;
//		ppp++;
//		}

	for (int i = 0; i < PAGESIZE128; i++) Tx_Buffer[6 + i] = (char) (pages[Num].Data[i]);

	Tx_Buffer[6 + PAGESIZE128] = 0;
	for (unsigned char count = 0; count < (PAGESIZE128 + 6); count++) Tx_Buffer[6 + PAGESIZE128] += Tx_Buffer[count];

	HPt->PurgeRx();
	HPt->PurgeTx();
	if (!HPt->SendData((Tx_Buffer + 2),(PAGESIZE128 + 5))) {
		return false;
		}
	Rx_Buffer[0] = 0;
	unsigned char cntt = 3;
 	while((Rx_Buffer[0] != ((char)ADDROUTOFRANGE)) &&
		(Rx_Buffer[0] != ((char)PRGNOTPAGE)) &&
		(Rx_Buffer[0] != ((char)PRGOKPAGE)) &&
		(Rx_Buffer[0] != 'C') &&
		(cntt)
		) {
		if (HPt->ReceiveData(Rx_Buffer,1)) {
		/*if ((Rx_Buffer[0] != ((char)PRGNOTPAGE)) &&
			(Rx_Buffer[0] != ((char)PRGOKPAGE))&&
			(Rx_Buffer[0] != 'C') &&
			(Rx_Buffer[0] != ((char)ADDROUTOFRANGE))
			) pages[Num].ntwrite = Rx_Buffer[0];
		*/
			if (Rx_Buffer[0] == 'N') {
				if (HPt->ReceiveData(Rx_Buffer + 1,1)) {
					if (Rx_Buffer[1] == 'T') {
						if (HPt->ReceiveData(Rx_Buffer + 2,1)) {
							pages[Num].ntwrite = Rx_Buffer[2];
							}
						}
					}
				}
			}
		else cntt--;
		}
	if ((!cntt) || (Rx_Buffer[0] == 'C') || (((unsigned char) Rx_Buffer[0]) == PRGNOTPAGE)) {
		return false;
		}
	else if (((unsigned char) Rx_Buffer[0]) == ADDROUTOFRANGE) {
		HPt->ReceiveData(Rx_Buffer + 1,4);
		return false;
		}
	return true;
}


bool DataStreamPage128byte::SendEndSignal(arnComPort *HPt)
{
   char Tx_Buffer[2],Rx_Buffer[2];
   // ******** record finale E0xFF
   Tx_Buffer[0] = (char) ('E');
   Tx_Buffer[1] = (char) (0xFF);

   HPt->PurgeRx();
   HPt->PurgeTx();
   if (!HPt->SendData(Tx_Buffer,2)) return false;
   if (!HPt->ReceiveData(Rx_Buffer,1)) return false;
   if (Tx_Buffer[0] != Rx_Buffer[0]) return false;
   return true;
}


// pagine 256 bytes

DataStreamPage256byte::DataStreamPage256byte(targetdatasize_t datasize)
	: DataStream(datasize,_tps256)
{
	page256bytesNumber  = 0;
	pages = NULL;
}

DataStreamPage256byte::~DataStreamPage256byte(void)
{
	if (pages) delete [] pages;
	pages = NULL;
	page256bytesNumber  = 0;
}

bool DataStreamPage256byte::ConvFromRecordQuantum8bit(Record* RecArr,int RecArrSize,
                unsigned short numofquan,
                unsigned long* startaddr_arr,
                unsigned long* size_arr)
{
	unsigned long* startaddr_array;
	unsigned long* size_array;

	if ((numofquan < 1) || (!startaddr_arr) || (!size_arr)) throw ("DataStream: Error in constructor parameter definition");
	numofquantum = numofquan;
	startaddr_array = new unsigned long [numofquantum];
	if (!startaddr_array) throw ("DataStream: startaddr_array : Error in constructor parameter allocation");
	size_array = new unsigned long [numofquantum];
	if (!size_array) throw ("DataStream: size_array : Error in constructor parameter allocation");
	for (unsigned short i = 0; i < numofquantum; i++) {
		startaddr_array[i] = startaddr_arr[i];
		size_array[i] = size_arr[i];
		page256bytesNumber +=  (size_array[i] / PAGESIZE256);
		}
	Block8bit = NULL;
//   page256bytesNumber  = PAGESNUMBER;
	switch (targetdatasize) {
		case _8bit:
			Block8bit = new quantum8bit_t*[numofquantum];
			if (!Block8bit) throw ("DataStream: Block8bit : Error in constructor parameter allocation");
        		for (unsigned short i = 0; i < numofquantum; i++) {
        			Block8bit[i] = new quantum8bit_t();
        			if (!Block8bit[i]) throw ("DataStream: Block8bit : Error in constructor parameter allocation");
        			Block8bit[i]->BaseAddr = startaddr_array[i];
        			Block8bit[i]->Size = size_array[i];
        			Block8bit[i]->Data = new unsigned char[Block8bit[i]->Size];
        			if (!Block8bit[i]->Data) throw ("DataStream: Block8bit : Error in constructor parameter allocation");
        			Block8bit[i]->ptr = 0;
        			for (unsigned long j = 0; j < Block8bit[i]->Size; j++)  Block8bit[i]->Data[j] = 0xFF;
        			}
        		break;
		default : break;
		}
	if (startaddr_array) delete [] startaddr_array;
	if (size_array) delete [] size_array;


	unsigned long Off = 0 ;
	unsigned long BAddr = 0;
	RecordArricchito* RecL = new  RecordArricchito();
	if (!RecL) throw ("DataStream: RecL : Error in constructor parameter allocation");
	for (int i = 0; i < RecArrSize; i++) {
		(*((Record*)RecL)) = RecArr[i];
		BAddr = RecL->GetBaseAddr();
		for (unsigned short v = 0; v < numofquantum; v++) {
			if ((BAddr >= Block8bit[v]->BaseAddr) && (((double)BAddr) < (((double)(Block8bit[v]->BaseAddr)) + Block8bit[v]->Size))) {
				Off = BAddr - Block8bit[v]->BaseAddr;
				if ((Off > Block8bit[v]->Size) || (RecL->GetTipo() != DATA)) continue;
				for (int j = 0; j <  RecL->GetNumOfData(); j++)
					if ((Off + j) <= Block8bit[v]->Size) {
						Block8bit[v]->Data[Off + j] = RecL->GetData((unsigned char)j);
						Block8bit[v]->ptr = Off + j;
						}
				Block8bit[v]->ptr++;
				break;
				}
			}
		}
	unsigned long acc = 0;//Block8bit[0]->BaseAddr;
	pages = new page256bytes[page256bytesNumber];
	if (!pages) throw ("DataStream: pages : Error in constructor parameter allocation");
	for (unsigned long i = 0; i < page256bytesNumber;) {
		pages[i].Addr = acc;
		for (unsigned short v = 0; v < numofquantum; v++) {
			if ((pages[i].Addr >= Block8bit[v]->BaseAddr) && (((double)(pages[i].Addr)) < (((double)(Block8bit[v]->BaseAddr)) + Block8bit[v]->Size))) { // somma maggiore di 0xffffffff
				for (int j = 0; j < PAGESIZE256; j++) pages[i].Data[j] = Block8bit[v]->Data[acc - Block8bit[v]->BaseAddr + j];
				acc += PAGESIZE256;
				i++;
				break;
				}
			else if (pages[i].Addr < Block8bit[v]->BaseAddr)
				acc = Block8bit[v]->BaseAddr;
			}
		}
	delete RecL;
	return true;
}

bool DataStreamPage256byte::IsDummyPage(unsigned long Num)
{
	for (int i = 0; i < PAGESIZE256; i++) if (pages[Num].Data[i] != 0xFF) return false;
	return true;
}

bool DataStreamPage256byte::SendPage(arnComPort *HPt,unsigned long Num)
{
	pages[Num].ntwrite = 0;
	if (IsDummyPage(Num)) return true;

	char Tx_Buffer[512],Rx_Buffer[64];

	Tx_Buffer[0] = (char) ('P');
	Tx_Buffer[1] = (char) ((unsigned char) (PAGESIZE256 + 5));
	HPt->PurgeRx();
	HPt->PurgeTx();
	if (!HPt->SendData(Tx_Buffer,2)) {
		return false;
		}

	if (!HPt->ReceiveData(Rx_Buffer,1)) {
		return false;
		}
	if (Rx_Buffer[0] != 'K') {
		return false;
		}

	Tx_Buffer[2] = (char) ((pages[Num].Addr & 0xFF000000) >> 24);
	Tx_Buffer[3] = (char) ((pages[Num].Addr & 0x00FF0000) >> 16);
	Tx_Buffer[4] = (char) ((pages[Num].Addr & 0x0000FF00) >> 8);
	Tx_Buffer[5] = (char) ((pages[Num].Addr & 0x000000FF));
//	test test test
//	if (pages[Num].Addr ==0x00410000) {
//		unsigned char ppp= 0;
//		ppp++;
//		}

	for (int i = 0; i < PAGESIZE256; i++) Tx_Buffer[6 + i] = (char) (pages[Num].Data[i]);

	Tx_Buffer[6 + PAGESIZE256] = 0;
	for (unsigned short count = 0; count < (PAGESIZE256 + 6); count++) Tx_Buffer[6 + PAGESIZE256] += Tx_Buffer[count];

	HPt->PurgeRx();
	HPt->PurgeTx();
	if (!HPt->SendData((Tx_Buffer + 2),(PAGESIZE256 + 5))) {
		return false;
		}
	Rx_Buffer[0] = 0;
	unsigned char cntt = 3;
 	while((Rx_Buffer[0] != ((char)ADDROUTOFRANGE)) &&
		(Rx_Buffer[0] != ((char)PRGNOTPAGE)) &&
		(Rx_Buffer[0] != ((char)PRGOKPAGE)) &&
		(Rx_Buffer[0] != 'C') &&
		(cntt)
		) {
		if (HPt->ReceiveData(Rx_Buffer,1)) {
		/*if ((Rx_Buffer[0] != ((char)PRGNOTPAGE)) &&
			(Rx_Buffer[0] != ((char)PRGOKPAGE))&&
			(Rx_Buffer[0] != 'C') &&
			(Rx_Buffer[0] != ((char)ADDROUTOFRANGE))
			) pages[Num].ntwrite = Rx_Buffer[0];
		*/
			if (Rx_Buffer[0] == 'N') {
				if (HPt->ReceiveData(Rx_Buffer + 1,1)) {
					if (Rx_Buffer[1] == 'T') {
						if (HPt->ReceiveData(Rx_Buffer + 2,1)) {
							pages[Num].ntwrite = Rx_Buffer[2];
							}
						}
					}
				}
			}
		else cntt--;
		}
	if ((!cntt) || (Rx_Buffer[0] == 'C') || (((unsigned char) Rx_Buffer[0]) == PRGNOTPAGE)) {
		return false;
		}
	else if (((unsigned char) Rx_Buffer[0]) == ADDROUTOFRANGE) {
		HPt->ReceiveData(Rx_Buffer + 1,4);
		return false;
		}
	return true;
}


bool DataStreamPage256byte::SendEndSignal(arnComPort *HPt)
{
   char Tx_Buffer[2],Rx_Buffer[2];
   // ******** record finale E0xFF
   Tx_Buffer[0] = (char) ('E');
   Tx_Buffer[1] = (char) (0xFF);

   HPt->PurgeRx();
   HPt->PurgeTx();
   if (!HPt->SendData(Tx_Buffer,2)) return false;
   if (!HPt->ReceiveData(Rx_Buffer,1)) return false;
   if (Tx_Buffer[0] != Rx_Buffer[0]) return false;
   return true;
}
