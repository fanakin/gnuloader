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

#ifndef CAppoggioH
#define CAppoggioH

#include "arnComPort.h"

#include "Containers.h"
#include "realtemplateclasses.h"

typedef enum{HEADER,DATA,FINAL,NONE} TipoRecord;
typedef struct _InfoRecord {
                           TipoRecord Tipo;
                           unsigned char NumOfByte;
                           } InfoRecord;



class Record
{
private:
protected:
	TipoRecord Tipo;
	unsigned char NumOfByte;
	unsigned long BaseAddr;
	unsigned char NumOfData;
	unsigned char *Data;
	unsigned char CheckSum;
public:
	Record(const KwtddString Str);
	Record(void);
	~Record(void);
	Record& operator =(const Record& R);
	TipoRecord GetTipo(void) {return Tipo;}
	unsigned char GetData(unsigned char i) {return Data[i];}
	unsigned char GetNumOfData(void) {return NumOfData;}
	bool SendRecord(arnComPort *HPt);
	bool SendData(arnComPort *HPt);
};



typedef enum _targetdatasize_t {_8bit,_16bit,_32bit,_nbiterr} targetdatasize_t;
typedef enum _targetpagesize_t {_tps32,_tps128,_tps256,_tpserr} targetpagesize_t;
typedef struct _quantum8bit_t {
      //_quantum8bit_t* next;
      unsigned long BaseAddr;
      unsigned long Size;
      unsigned char* Data;
      unsigned long ptr;
      } quantum8bit_t;
#define PAGESIZE32     32
typedef struct _page32bytes {
      unsigned long Addr;
      unsigned long ntwrite;
      unsigned char Data[PAGESIZE32];
      } page32bytes;
#define PAGESIZE128     128
typedef struct _page128bytes {
      unsigned long Addr;
      unsigned long ntwrite;
      unsigned char Data[PAGESIZE128];
      } page128bytes;

#define PAGESIZE256     256
typedef struct _page256bytes {
      unsigned long Addr;
      unsigned long ntwrite;
      unsigned char Data[PAGESIZE256];
      } page256bytes;

class RecordArricchito : public Record {
   private:
   protected:
   public:
   RecordArricchito(const KwtddString Str) : Record(Str) {;}
   RecordArricchito(void) : Record() {;}
   ~RecordArricchito(void) {;}
   unsigned long GetBaseAddr(void) {return BaseAddr;}
   };

class DataStream {
   private:
      #define PRGOKPAGE	((unsigned char)0xF0)
      #define PRGNOTPAGE	((unsigned char)0xF1)
      #define ADDROUTOFRANGE	((unsigned char)0xF2)

   protected:
      targetdatasize_t targetdatasize;
      targetpagesize_t targetpagesize;
      unsigned short numofquantum;
      quantum8bit_t** Block8bit;


   public:
     DataStream(targetdatasize_t datasize = _8bit, targetpagesize_t pagesize = _tps32);
     virtual ~DataStream(void);
	 virtual bool IsDummyPage(unsigned long Num) = 0;
     virtual bool ConvFromRecordQuantum8bit(Record* RecArr,int RecArrSize,
                unsigned short numofquan,
                unsigned long* startaddr_arr,
                unsigned long* size_arr) = 0;
     virtual bool SendPage(arnComPort *HPt,unsigned long Num) = 0;
     virtual bool SendEndSignal(arnComPort *HPt) = 0;
     virtual unsigned long GetpageNumber(void) = 0;
     virtual unsigned long Getpagentwrite(unsigned long Num) = 0;
     virtual unsigned long Getpagebaseaddr(unsigned long Num) = 0;
	 virtual unsigned char* Getpagedata(unsigned long Num) = 0;
     bool SendByteFormBlock8bit(arnComPort *HPt);
     unsigned long GetBlock8Bit_ptr(unsigned short i) {
                if (i >= numofquantum) return 0x0L;
                if (Block8bit && Block8bit[i]) return Block8bit[i]->ptr;
                else return 0x0L;
                }
     inline targetdatasize_t getTargetDataSize(void) {return targetdatasize;}
     inline targetpagesize_t getTargetPageSize(void) {return targetpagesize;}
};

class DataStreamPage32byte : public DataStream{
   private:
      #define PRGOKPAGE	((unsigned char)0xF0)
      #define PRGNOTPAGE	((unsigned char)0xF1)
      #define ADDROUTOFRANGE	((unsigned char)0xF2)

      page32bytes* pages;
      unsigned long page32bytesNumber;
   protected:

   public:
     DataStreamPage32byte(targetdatasize_t datasize = _8bit);
      ~DataStreamPage32byte(void);
      virtual bool IsDummyPage(unsigned long Num);
      virtual bool ConvFromRecordQuantum8bit(Record* RecArr,int RecArrSize,
                unsigned short numofquan,
                unsigned long* startaddr_arr,
                unsigned long* size_arr);
      virtual bool SendPage(arnComPort *HPt,unsigned long Num);
      virtual bool SendEndSignal(arnComPort *HPt);
      virtual unsigned long GetpageNumber(void) {return page32bytesNumber;}
      virtual unsigned long Getpagentwrite(unsigned long Num) {return pages[Num].ntwrite;}
      virtual unsigned long Getpagebaseaddr(unsigned long Num) {return pages[Num].Addr;}
      virtual unsigned char* Getpagedata(unsigned long Num) {return pages[Num].Data;}
   };

class DataStreamPage128byte : public DataStream{
   private:
      #define PRGOKPAGE	((unsigned char)0xF0)
      #define PRGNOTPAGE	((unsigned char)0xF1)
      #define ADDROUTOFRANGE	((unsigned char)0xF2)

      page128bytes* pages;
      unsigned long page128bytesNumber;
   protected:

   public:
     DataStreamPage128byte(targetdatasize_t datasize = _8bit);
      ~DataStreamPage128byte(void);
      virtual bool IsDummyPage(unsigned long Num);
      virtual bool ConvFromRecordQuantum8bit(Record* RecArr,int RecArrSize,
                unsigned short numofquan,
                unsigned long* startaddr_arr,
                unsigned long* size_arr);
      virtual bool SendPage(arnComPort *HPt,unsigned long Num);
      virtual bool SendEndSignal(arnComPort *HPt);
      virtual unsigned long GetpageNumber(void) {return page128bytesNumber;}
      virtual unsigned long Getpagentwrite(unsigned long Num) {return pages[Num].ntwrite;}
      virtual unsigned long Getpagebaseaddr(unsigned long Num) {return pages[Num].Addr;}
      virtual unsigned char* Getpagedata(unsigned long Num) {return pages[Num].Data;}
   };

class DataStreamPage256byte : public DataStream{
   private:
      #define PRGOKPAGE	((unsigned char)0xF0)
      #define PRGNOTPAGE	((unsigned char)0xF1)
      #define ADDROUTOFRANGE	((unsigned char)0xF2)

      page256bytes* pages;
      unsigned long page256bytesNumber;
   protected:

   public:
     DataStreamPage256byte(targetdatasize_t datasize = _8bit);
      ~DataStreamPage256byte(void);
      virtual bool IsDummyPage(unsigned long Num);
      virtual bool ConvFromRecordQuantum8bit(Record* RecArr,int RecArrSize,
                unsigned short numofquan,
                unsigned long* startaddr_arr,
                unsigned long* size_arr);
      virtual bool SendPage(arnComPort *HPt,unsigned long Num);
      virtual bool SendEndSignal(arnComPort *HPt);
      virtual unsigned long GetpageNumber(void) {return page256bytesNumber;}
      virtual unsigned long Getpagentwrite(unsigned long Num) {return pages[Num].ntwrite;}
      virtual unsigned long Getpagebaseaddr(unsigned long Num) {return pages[Num].Addr;}
      virtual unsigned char* Getpagedata(unsigned long Num) {return pages[Num].Data;}
   };

#endif
