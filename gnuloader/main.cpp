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

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>

#include "WTDDLibreria.h"
#include "commfunc.h"
#include "arnComPort.h"
#include "main.h"
#include "realtemplateclasses.h"
#include "CAppoggio.h"
#include "TabStrc.h"
#include "command018um.h"
//#define __WTDD_DEBUG_

#define arnCriticalExit() do { \
    fprintf(stderr,"file : %s; function : %s; line : %d - Critical Abort.\n",__FILE__,__FUNCTION__,__LINE__); \
    exit(EXIT_FAILURE); \
    } while(0)


static int s_option_handler(char *argv[],int _optind);
static int p_option_handler(char *argv[],int _optind);
static int c_option_handler(char *argv[],int _optind);
static int t_option_handler(char *argv[],int _optind);
static int f_option_handler(char *argv[],int _optind);
static int m_option_handler(char *argv[],int _optind);
static int a_option_handler(char *argv[],int _optind);
static int b_option_handler(char *argv[],int _optind);
static int d_option_handler(char *argv[],int _optind);
static int e_option_handler(char *argv[],int _optind);

static void usage(char* argv_0);
static CpuTargetType convtocpu(char* cpu);
static KwtddStringList loadDataStream(QString fname,QString mfname);
static KwtddStringList loadBootProgram(CpuTargetType cpu);

static bool AutoBaudRate(char* portname);
static bool DownloadBootProgram(char* portname,KwtddStringList* BootProgram);
static bool DownloadTargetProgram(char* portname,KwtddStringList* TargetProgram);
static bool ListenAndAnswerToEcu(arnComPort* PortaComm,unsigned char LstCmd,unsigned char AswCmd);
static bool ListenAndAnswerToEcuForErasingSectors(arnComPort* PortaComm,unsigned char LstCmd,unsigned char AswCmd,unsigned char* Sect);


#ifdef __WTDD_DEBUG_
static void wtddPrintfStringList(KwtddStringList* List);
#endif
    
int main(int argc, char *argv[])
{
	extern int optind;
	int c;
	unsigned char maxautobaudrateTemp = 3;
	KwtddStringList TargetProgram,BootProgram;

	uCApp.TabPar->tsize = 0;  // dimrnsione della tabella come in GENTAB
	uCApp.TabPar->nb = 0; // numero di blocchI; 1 per il momento
	uCApp.TabPar->displ.Push(0x0); // offeset del primo blocco; cioe' il base address
	uCApp.TabPar->name = ""; // codice Gentab

	if (argc == 1) usage(argv[0]);
	while ((c = getopt(argc,argv,"abcdefhmpstv")) != EOF) {
		switch (c) {
			case 'h': usage(argv[0]);
			case 'v': fprintf(stderr,"%s",currMsgList[main_VERSION]);return EXIT_SUCCESS;
			case 's': if (!s_option_handler(argv,optind)) break; else return EXIT_FAILURE;
			case 'p': if (!p_option_handler(argv,optind)) break; else return EXIT_FAILURE;
			case 'c': if (!c_option_handler(argv,optind)) break; else return EXIT_FAILURE;
			case 't': if (!t_option_handler(argv,optind)) break; else return EXIT_FAILURE;
			case 'f': if (!f_option_handler(argv,optind)) break; else return EXIT_FAILURE;
			case 'm': if (!m_option_handler(argv,optind)) break; else return EXIT_FAILURE;
			case 'a': if (!a_option_handler(argv,optind)) break; else return EXIT_FAILURE;
			case 'b': if (!b_option_handler(argv,optind)) break; else return EXIT_FAILURE;
			case 'd': if (!d_option_handler(argv,optind)) break; else return EXIT_FAILURE;
			case 'e': if (!e_option_handler(argv,optind)) break; else return EXIT_FAILURE;
			default : usage(argv[0]);
			}
		}

	N_SECTORS = 16; // default
// CARICAMMNETO DEI FILE NECESSARI E LORO FORMATTAZIONE
	BootProgram = loadBootProgram(uCApp.cpu);
	if (BootProgram.Count() == 0) {fprintf(stderr,currMsgList[main_BOOTPROGRAMERROR]/*"errore fatale carimanento BootProgram!!!\r\n"*/); return EXIT_FAILURE;}
	TargetProgram = loadDataStream(uCApp.AbsPath + uCApp.FileName,uCApp.AbsPath + uCApp.MapFileName);
	if (TargetProgram.Count() == 0) {fprintf(stderr,currMsgList[main_TARGETPROGRAMERROR]/*"errore fatale carimanento TargetProgram!!!\r\n"*/); return EXIT_FAILURE;}

// STABILSCO COMUNICAZIONE
	do {
		if (!AutoBaudRate(ComPortSelected)) {
			maxautobaudrateTemp--;
			sleep(1);
			}
		else break;
		} while (maxautobaudrateTemp);
	if (!maxautobaudrateTemp) {fprintf(stderr,currMsgList[main_AUTOBAUDRATEERROR]/*"errore fatale AutoBAudRate!\r\n"*/); return EXIT_FAILURE;}

// SPEDISCO NEL MICRO (IN RAM) IL PROGRAMMA CHE ESEGUIRA' LA PROGRAMMAZIONE VERA E PROPRIA
	if (!DownloadBootProgram(ComPortSelected,&BootProgram)) {fprintf(stderr,currMsgList[main_DOWNLOADBOOTPROGRAMERROR]/*"errore fatale DownloadBootProgram!\r\n"*/); return EXIT_FAILURE;}
	else {fprintf(stdout,currMsgList[main_DOWNLOADBOOTPROGRAMOK]/*"!!! BootProgram Residente !!!                              \r\n"*/);}

// SPEDISCO NEL MICRO (IN FLASH) IL PROGRAMMA CHE UTENTE; CIOE' L'APPLICATIVO.
	if (!DownloadTargetProgram(ComPortSelected,&TargetProgram)) {fprintf(stderr,currMsgList[main_DOWNLOADTARGETPROGRAMERROR]/*"errore fatale DownloadBootProgram!\r\n"*/); return EXIT_FAILURE;}
	else {fprintf(stdout,currMsgList[main_DOWNLOADTARGETPROGRAMOK]/*"!!! Download OK !!!                                        \r\n"*/);}

#ifdef __WTDD_DEBUG_
wtddPrintfStringList(&BootProgram);
wtddPrintfStringList(&TargetProgram);
#endif

	return EXIT_SUCCESS;
}


/* ******************************************************************* */
/* --------------------- FUNZIONI ANALISI RIGA COMANDO --------------- */
static int s_option_handler(char *argv[],int _optind)
{
	char* app = argv[_optind];
	if (!strcmp(app,"COM1") || !strcmp(app,"com1") || !strcmp(app,"ttyS0") || !strcmp(app,"TTYS0")) {ComPortSelected = (char*)COMPortName[__COM1_IDX]; return 0;}
	else if (!strcmp(app,"COM2") || !strcmp(app,"com2") || !strcmp(app,"ttyS1") || !strcmp(app,"TTYS1")) {ComPortSelected = (char*)COMPortName[__COM2_IDX];  return 0;}
	else if (!strcmp(app,"COM3") || !strcmp(app,"com3") || !strcmp(app,"ttyS2") || !strcmp(app,"TTYS2")) {ComPortSelected = (char*)COMPortName[__COM3_IDX];  return 0;}
	else if (!strcmp(app,"COM4") || !strcmp(app,"com4") || !strcmp(app,"ttyS3") || !strcmp(app,"TTYS3")) {ComPortSelected = (char*)COMPortName[__COM4_IDX];  return 0;}
	else if (!strcmp(app,"COM5") || !strcmp(app,"com5") || !strcmp(app,"ttyUSB0") || !strcmp(app,"TTYUSB0")) {ComPortSelected = (char*)COMPortName[__COM5_IDX]; return 0;}
	else if (!strcmp(app,"COM6") || !strcmp(app,"com6") || !strcmp(app,"ttyUSB1") || !strcmp(app,"TTYUSB1")) {ComPortSelected = (char*)COMPortName[__COM6_IDX];  return 0;}
	else if (!strcmp(app,"COM7") || !strcmp(app,"com7") || !strcmp(app,"ttyUSB2") || !strcmp(app,"TTYUSB2")) {ComPortSelected = (char*)COMPortName[__COM7_IDX];  return 0;}
	else if (!strcmp(app,"COM8") || !strcmp(app,"com8") || !strcmp(app,"ttyUSB3") || !strcmp(app,"TTYUSB3")) {ComPortSelected = (char*)COMPortName[__COM8_IDX];  return 0;}
	else if (!strcmp(app,"COM9") || !strcmp(app,"com9") || !strcmp(app,"ttyUSB4") || !strcmp(app,"TTYUSB4")) {ComPortSelected = (char*)COMPortName[__COM9_IDX]; return 0;}
	else if (!strcmp(app,"COM10") || !strcmp(app,"com10") || !strcmp(app,"ttyUSB5") || !strcmp(app,"TTYUSB5")) {ComPortSelected = (char*)COMPortName[__COM10_IDX];  return 0;}
	else if (!strcmp(app,"COM11") || !strcmp(app,"com11") || !strcmp(app,"ttyUSB6") || !strcmp(app,"TTYUSB6")) {ComPortSelected = (char*)COMPortName[__COM11_IDX];  return 0;}
	else if (!strcmp(app,"COM12") || !strcmp(app,"com12") || !strcmp(app,"ttyUSB7") || !strcmp(app,"TTYUSB7")) {ComPortSelected = (char*)COMPortName[__COM12_IDX];  return 0;}
	else {fprintf(stderr,currMsgList[s_option_handler_ERROR]/*"porta COM sconosciuta.\r\n"*/); return -1;}	
}
static int p_option_handler(char *argv[],int _optind)
{
	uCApp.AbsPath = argv[_optind];
	if ((uCApp.AbsPath.length() == 0) || (uCApp.AbsPath[0] != '/')) {
		fprintf(stderr,currMsgList[p_option_handler_ERROR]/*"errore optione -p: valore non ammesso\r\n"*/);
		return -1;
		}
	return 0;
}
static int c_option_handler(char *argv[],int _optind)
{
	/*uCApp.coption_choice = true;*/
	uCApp.ConfigFileName = argv[_optind];
//	if (uCApp.ConfigFileName == "") {
//		if (!LoadConfigFile("/ect/gnuloader.conf")) { // carica il file di default
//			printf(currMsgList[c_option_handler_ERROR1]/*"errore optione -c: /etc/gnoloader.conf non trovato\r\n"*/);
//			return -1;
//			}
//		}	
//	else {
//		if (!LoadConfigFile(uCApp.ConfigFile)) {
//			printf(currMsgList[c_option_handler_ERROR2]/*"errore optione -c: file non trovato\r\n"*/);
//			return -1;
//			}	
//		}
	return 0;
}
static int t_option_handler(char *argv[],int _optind)
{
	uCApp.cpu = convtocpu(argv[_optind]);
	if (uCApp.cpu >= Ctt_None) {
		fprintf(stderr,currMsgList[t_option_handler_ERROR]/*"errore optione -t: CPU non supportata\r\n"*/);
		return -1;
		}
	return 0;
}
static int f_option_handler(char *argv[],int _optind)
{
	uCApp.FileName = argv[_optind];
	if ((uCApp.FileName.length() == 0) || (uCApp.FileName[0] != '/')) {
		fprintf(stderr,currMsgList[f_option_handler_ERROR]/*"errore optione -f: valore non ammesso: tutto il path e' richiesto.\r\n"*/);
		return -1;
		}
	return 0;
}
static int m_option_handler(char *argv[],int _optind)
{
	uCApp.MapFileName = argv[_optind];
	if ((uCApp.MapFileName.length() == 0) || (uCApp.MapFileName[0] != '/')) {
		fprintf(stderr,currMsgList[m_option_handler_ERROR]/*"errore optione -m: valore non ammesso: tutto il path e' richiesto.\r\n"*/);
		return -1;
		}
	return 0;
}
static int a_option_handler(char *argv[],int _optind)
{
	uCApp.TabPar->tsize = atoi(argv[_optind]);
	if (uCApp.TabPar->tsize > PBUFF_SIZE) {
		fprintf(stderr,currMsgList[a_option_handler_ERROR]/*"errore optione -a: mappa troppo grande: maggiore di un MByte.\r\n"*/);
		return -1;
		}
	return 0;
}
static int b_option_handler(char *argv[],int _optind)
{
	uCApp.TabPar->nb = atoi(argv[_optind]);
	if (uCApp.TabPar->nb > 1) {
		fprintf(stderr,currMsgList[b_option_handler_ERROR]/*"errore optione -b: funzione multi blocco ancora non supportata.\r\n"*/);
		return -1;
		}
	return 0;
}
static int d_option_handler(char *argv[],int _optind)
{
  unsigned long tp = atoi(argv[_optind]);
  if (tp > 0x20000) {
    fprintf(stderr,currMsgList[d_option_handler_ERROR]/*"errore optione -d: non ancora ammessa la flash esterna.\r\n"*/);
    return -1;
    }
  uCApp.TabPar->displ.Pop();
  uCApp.TabPar->displ.Push(tp);
  return 0;
}
static int e_option_handler(char *argv[],int _optind)
{
  QString tp(argv[_optind]);
  uCApp.TabPar->name = tp;
	if ((uCApp.TabPar->name.length() == 0) || (uCApp.TabPar->name.length() > 8)) {
		fprintf(stderr,currMsgList[d_option_handler_ERROR]/*"errore optione -e: valore non ammesso: codice troppo lungo.\r\n"*/);
		return -1;
		}
	return 0;
}

static void usage(char* argv_0)
{
	char* progname =  NULL;
	progname = new char[sizeof(argv_0)];
	if (progname == NULL) exit(1);

	strcpy(progname,argv_0);

	fprintf(stderr,currMsgList[usage_MGS01]/*"%s\r\n%s%s\n\nUsage :\n\n"*/, progname + 2,currMsgList[main_VERSION],currMsgList[usage_MGS02]/*"copyright Aurion s.r.l 2002,2003, GPL license"*/);
	fprintf(stderr,currMsgList[usage_MGS03]/*"%s [-h] [-v] [-s comport][-p abspath][-c [config file]] [-t cpu] [-f target file] [-m file.map]\n"*/,progname + 2);
	fprintf(stderr,currMsgList[usage_MGS04]/*"[-a tsize] [-b numBlock] [-d disp] [-e code]\n\n"*/);
	fprintf(stderr,currMsgList[usage_MGS05]/*"\t-h\t\t\taiuto\n"*/);
	fprintf(stderr,currMsgList[usage_MGS06]/*"\t-v\t\t\tdata di compilazione e versione\n"*/);
	fprintf(stderr,currMsgList[usage_MGS07]/*"\t-s porta COM\t\tvalori ammessi COM1,ttyS0;COM2,ttyS1;COM3,ttyS2;COM4,ttyS3\n"*/);
	fprintf(stderr,currMsgList[usage_MGS08]/*"\t-p abspath\t\t[non iplementato] specifica il path assoluto di -f file. Default ./\n"*/);
	fprintf(stderr,currMsgList[usage_MGS09]/*"\t-c\t\t\t[non iplementato] usa il default ./gnuloader.conf\n"*/);
	fprintf(stderr,currMsgList[usage_MGS10]/*"\t-c filename\t\t[non iplementato] usa il file di configurazione filename\n"*/);
	fprintf(stderr,currMsgList[usage_MGS11]/*"\t-t cpu\t\t\tvalori ammessi H83048,H8S2345,SH27044,ARN44RAM\n"*/);
	fprintf(stderr,currMsgList[usage_MGS12]/*"\t-f filename\t\tprogrammo la cpu con il file filename\n"*/);
	fprintf(stderr,currMsgList[usage_MGS13]/*"\t-m file.mpp\t\tprogrammo la cpu con il file filename + il file di parametri file.mpp\n"*/);
	fprintf(stderr,currMsgList[usage_MGS14]/*"\t-a tsize\t\tspecifica la dimensione della tabella dei parametri\n"*/);
	fprintf(stderr,currMsgList[usage_MGS15]/*"\t-b numBlock\t\tspefifica il numero di blocchi (per ora specificare 1)\n"*/);
	fprintf(stderr,currMsgList[usage_MGS16]/*"\t-d disp\t\tspecifica lo start address (in decimale) di ogni blocco (per ora un solo valore es. 115072)\n"*/);
	fprintf(stderr,currMsgList[usage_MGS17]/*"\t-e code\t\tspecifica il codice identificativo del blocco parametri.\n"*/);
	exit(1);
}
/* ******************************************************************* */


/* ******************************************************************* */
/* --------------------- FUNZIONI DI UTILITA' SU STRINGHE ------------ */
static CpuTargetType convtocpu(char* cpu)
{
	if (!strcmp(cpu,"H83048") || !strcmp(cpu,"h83048")) return Ctt_H83048;
	else if (!strcmp(cpu,"H8S2345") || !strcmp(cpu,"h8s2345")) return Ctt_H8S2345;
	else if (!strcmp(cpu,"H8S2633") || !strcmp(cpu,"h8s2633")) return Ctt_H8S2633;
	else if (!strcmp(cpu,"SH27044") || !strcmp(cpu,"sh27044")) return Ctt_SH27044;
	/*
	else if (!strcmp(cpu,"SH27044F") || !strcmp(cpu,"sh27044f")) return Ctt_SH27044F;
	else if (!strcmp(cpu,"SH2MNJ600") || !strcmp(cpu,"sh2mnj600")) return Ctt_SH2MNJ600;
	else if (!strcmp(cpu,"MNJ600RAM") || !strcmp(cpu,"mnj600ram")) return Ctt_MNJ600RAM;
	*/
	else if (!strcmp(cpu,"ARN44B") || !strcmp(cpu,"arn44b")) return Ctt_ARN44B;
	else if (!strcmp(cpu,"ARN44B_ADAPT2BNC") || !strcmp(cpu,"arn44b_adapt2bnc")) return Ctt_ADAPT2BNC;
	else if (!strcmp(cpu,"SH27145") || !strcmp(cpu,"sh27145")) return Ctt_SH27145;
	else if (!strcmp(cpu,"ARN45") || !strcmp(cpu,"arn45")) return Ctt_ARN45;
	else if (!strcmp(cpu,"LMB125") || !strcmp(cpu,"lmb125")) return Ctt_LMB125;
	else if (!strcmp(cpu,"H83687") || !strcmp(cpu,"h83687")) return Ctt_H83687;
	else if (!strcmp(cpu,"H8S2345_20MHZ") || !strcmp(cpu,"h8s2345_20mhz")) return Ctt_H8S2345_20MHZ;
	else if (!strcmp(cpu,"H83687_10MHZ") || !strcmp(cpu,"h83687_10mhz")) return Ctt_H83687_10MHZ;
	else if (!strcmp(cpu,"H83048FONE_16MHZ") || !strcmp(cpu,"h83048fone_16mhz")) return Ctt_H83048FONE_16MHZ;
	else if (!strcmp(cpu,"H8S2378_16MHZ") || !strcmp(cpu,"h8s2378_16mhz")) return Ctt_H8S2378_16MHZ;
	else if (!strcmp(cpu,"H83687_16MHZ") || !strcmp(cpu,"h83687_16mhz")) return Ctt_H83687_16MHZ;
	else if (!strcmp(cpu,"SH27085_10MHZ") || !strcmp(cpu,"sh27085_10mhz")) return Ctt_SH27085_10MHZ;
	else if (!strcmp(cpu,"H8SX1663_10MHZ") || !strcmp(cpu,"h8sx1663_10mhz")) return Ctt_H8SX1663_10MHZ;
	else if (!strcmp(cpu,"H8SX1663_12MHZ") || !strcmp(cpu,"h8sx1663_12mhz")) return Ctt_H8SX1663_12MHZ;
	else if (!strcmp(cpu,"SH2A7286_12MHZ") || !strcmp(cpu,"sh2a7286_12mhz")) return Ctt_SH2A7286_12MHZ;
	else return Ctt_None;
}

static KwtddStringList loadDataStream(QString fname,QString mfname)
{
	FILE* hf;
  char* pBuff;
  Tabella* Tab;
	KwtddStringList res(50,50);
	int c;
	KwtddString arr(100,50);
	KwtddString S8record(100,50);

	enum _lineprogramparsState {lps_start_pars,lps_start_line,lps_foundlinefeed,
		lps_foundcarriagereturn,lps_none,lps_err} lineprogramparsState = lps_start_pars;


  hf = fopen(fname,"rb");
  if (hf == NULL) return (res);
	arr.Clear(true);
	while ((c = fgetc(hf)) != EOF) {
		arr.Push((char) c);
		switch (lineprogramparsState) {
			case lps_start_pars:
				if (c == 'S') lineprogramparsState = lps_start_line;
				break;
			case lps_start_line:
				if (c == '\r') lineprogramparsState = lps_foundlinefeed;
				break;
			case lps_foundlinefeed:
				if (c == '\n') lineprogramparsState = lps_start_pars;
				arr.Push((char) '\0');
				res.Push(arr);
				arr.Clear(true);
				break;
			default:
				break;
			}
		}
	fclose(hf);

  //  parte che carica i parametri se ci sono
  if (mfname != "") {
    hf = fopen(fname,"rb");
    if (hf == NULL) return (res);
    pBuff = new char[PBUFF_SIZE];
    if (pBuff == NULL) return (res);


// -----------------------------------------------------------------------
// Parte Da cambiare per garantire la corrispondenza con il Gentab
// -----------------------------------------------------------------------
//    uCTabInit.tsize = 12992;  // dimrnsione della tabella come in GENTAB
//    uCTabInit.nb = 1; // numero di blocchI; 1 per il momento
//    uCTabInit.displ.Push(0x1C180); // offeset del primo blocco; cioe' il base address
//    strcpy(uCTabInit.name,"BIRACE_A"); // codice Gentab
// -----------------------------------------------------------------------
// FINE -- Parte Da cambiare per garantire la corrispondenza con il Gentab
// -----------------------------------------------------------------------
    fprintf(stdout,currMsgList[loadDataStream_MSG01]/*"tsize - %i\n"*/,uCApp.TabPar->tsize);
    fprintf(stdout,currMsgList[loadDataStream_MSG02]/*"numBlock - %i\n"*/,uCApp.TabPar->nb);
    fprintf(stdout,currMsgList[loadDataStream_MSG03]/*"displ[0] - %i\n"*/,uCApp.TabPar->displ[0]);
    fprintf(stdout,currMsgList[loadDataStream_MSG04]/*"name - %s\n"*/,uCApp.TabPar->name.data());
    if ((!(uCApp.TabPar->tsize)) || (!(uCApp.TabPar->displ[0])) ||
      (!(uCApp.TabPar->nb)) ||(uCApp.TabPar->name == "")) {
      fprintf(stderr,currMsgList[loadDataStream_MSG05]/*"valore opzioni -a e/o -b e/o -d e/o -e non corretto/i.\n"*/);
      exit(EXIT_FAILURE);
      }
    Tab = new Tabella(uCApp.TabPar->tsize,uCApp.TabPar->displ,uCApp.TabPar->nb,uCApp.TabPar->name.latin1());
    if (Tab->Read(mfname)) {
      fprintf(stderr,currMsgList[loadDataStream_MSG06]/*"Mappa %s letta correttamente.\n"*/,mfname.data());
      if (Tab->ConvertTabToA20((unsigned char*)pBuff,PBUFF_SIZE)) {
          // Aggiunta a res di questo nuovo a20 prodotto
          arr.Clear(true);
          lineprogramparsState = lps_start_pars;
          S8record = res.Pop();
          for (unsigned int indx = 0; pBuff[indx] != '\0'; indx++) {
            c = pBuff[indx];
            arr.Push((char) c);
            switch (lineprogramparsState) {
              case lps_start_pars:
                if (c == 'S') lineprogramparsState = lps_start_line;
                break;
              case lps_start_line:
                if (c == '\r') lineprogramparsState = lps_foundlinefeed;
                break;
              case lps_foundlinefeed:
                if (c == '\n') lineprogramparsState = lps_start_pars;
                arr.Push((char) '\0');
                res.Push(arr);
                arr.Clear(true);
                break;
              default:
                break;
              }
            }
          res.Push(S8record);
          }
      }        
    else {fprintf(stderr,currMsgList[loadDataStream_MSG07]/*"Errore nel leggere il file di parametri %s.\n"*/,mfname.data());exit(EXIT_FAILURE);}

    delete Tab;
    Tab = NULL;
    delete [] pBuff;
    pBuff = NULL;
    }
  return (res);
}


static KwtddStringList loadBootProgram(CpuTargetType cpu)
{
	KwtddStringList res(50,50);
	int lIndexLine;
	if (cpu == Ctt_None) arnCriticalExit();
	// non implementato piu'
	//if (cpu == Ctt_ARN44RAM) cpu = Ctt_SH27044;
	// la tecnologia 0.18 non prevede il boot program
	// quindi metto un boot di comodo solo per compatibilita'.
	if (cpu == Ctt_H8S2378_16MHZ ||
		cpu == Ctt_SH27085_10MHZ ||
		cpu == Ctt_H8SX1663_10MHZ ||
		cpu == Ctt_H8SX1663_12MHZ ||
		cpu == Ctt_SH2A7286_12MHZ ) {
		cpu = Ctt_H8S2345;
		}

	lIndexLine = 0;
	while (bootprogram[cpu][lIndexLine] != NULL) {
		int lIndex_Char;
		KwtddString arr(100,50);

		lIndex_Char = 0;
		while (bootprogram[cpu][lIndexLine][lIndex_Char] !='\0') {
			arr.Push(bootprogram[cpu][lIndexLine][lIndex_Char]);
			lIndex_Char++;
			}
		arr.Push('\r');
		arr.Push('\n');
		arr.Push('\0');
		res.Push(arr);		
		lIndexLine++;
		}
	N_SECTORS = N_SECTS[cpu];
	return res;
}
/* ******************************************************************* */


/* ******************************************************************* */
/* --------------------- FUNZIONI INIZIO COMUNICAZIONE --------------- */
static bool AutoBaudRate(char* portname)
{
	bool ReceiveOk;
	char Rx_Buffer = 0;
	char Tx_Buffer = 0;
	int cerr = 5;
	portdef parport;
	//if (uCApp.cpu == Ctt_ARN44RAM) return true;
	// SETTAGGIO PORTA
	if ((uCApp.cpu == Ctt_SH27145) ||
		(uCApp.cpu == Ctt_ARN45) ||
		(uCApp.cpu == Ctt_H83048FONE_16MHZ) ||
		(uCApp.cpu == Ctt_H8S2378_16MHZ) ||
		(uCApp.cpu == Ctt_SH27085_10MHZ) ||
		(uCApp.cpu == Ctt_H8SX1663_10MHZ) ||
		(uCApp.cpu == Ctt_H8SX1663_12MHZ) ||
		(uCApp.cpu == Ctt_SH2A7286_12MHZ)
		) {
		strcpy(parport.portname,"COM1");
		parport.baudrate = 19200;		/*!< Baud rate value: ex. 9600 */
		parport.databits = 8;		/*!< Bits of data: ex. 8 or 7*/
		parport.parity = 0;		/*!< Parity bit: ex. 0 disabled, otherwise enabled*/
		parport.stopbits = 1;		/*!< Number of stop bits: 1 or 2*/
		parport.xon_xoff = false;		/*!< XON / XOFF enabled : ex 0 disabled, otherwise enabled*/
		parport.rts_cts = false;		/*!< RTS / CTS enabled: ex 0 disabled, otherwise enabled*/
		parport.timeout = 2;		/*!< Timeout : in ms */
		}
	else {
		strcpy(parport.portname,"COM1");
		parport.baudrate = 9600;		/*!< Baud rate value: ex. 9600 */
		parport.databits = 8;		/*!< Bits of data: ex. 8 or 7*/
		parport.parity = 0;		/*!< Parity bit: ex. 0 disabled, otherwise enabled*/
		parport.stopbits = 1;		/*!< Number of stop bits: 1 or 2*/
		parport.xon_xoff = false;		/*!< XON / XOFF enabled : ex 0 disabled, otherwise enabled*/
		parport.rts_cts = false;		/*!< RTS / CTS enabled: ex 0 disabled, otherwise enabled*/
		parport.timeout = 2;		/*!< Timeout : in ms */
		}
	strcpy(parport.portname,portname);
	arnComPort* aport;
	aport = new arnComPort(parport);
	if (aport->GetStatus()) {fprintf(stderr,currMsgList[AutoBaudRate_MSG01]/*"errore fatale apertura porta com!!!\r\n"*/); delete aport; return false;}

	// -------------- autobaudrate ------------------------------------------------
	// comportamento spurio quando ci agganciamo: ci manda un zero inatteso
	if ((uCApp.cpu == Ctt_SH27044)
		|| (uCApp.cpu == Ctt_ARN44B)
		|| (uCApp.cpu == Ctt_ADAPT2BNC)
		|| (uCApp.cpu == Ctt_SH27145)
		|| (uCApp.cpu == Ctt_ARN45)
		|| (uCApp.cpu == Ctt_SH27085_10MHZ)
		|| (uCApp.cpu == Ctt_H8SX1663_10MHZ)
		|| (uCApp.cpu == Ctt_H8SX1663_12MHZ)
		|| (uCApp.cpu == Ctt_SH2A7286_12MHZ)
		) aport->ReceiveData(&Rx_Buffer,1);
	
	if ((uCApp.cpu != Ctt_H8S2378_16MHZ) &&
		(uCApp.cpu != Ctt_SH27085_10MHZ) &&
		(uCApp.cpu != Ctt_H8SX1663_10MHZ) &&
		(uCApp.cpu != Ctt_H8SX1663_12MHZ) &&
		(uCApp.cpu != Ctt_SH2A7286_12MHZ)) {
		Tx_Buffer = 0x00;
		cerr = 5;
		do {
			aport->PurgeRx();
			if (!aport->SendData(&Tx_Buffer,1)) {
				fprintf(stderr,currMsgList[AutoBaudRate_MSG02]/*"errore trasmissione 0x00 in autobaudrate!!!\r\n"*/);
				delete aport;
				return false;
				}
			Rx_Buffer = 0xFF;
			ReceiveOk = aport->ReceiveData(&Rx_Buffer,1);
			if (!ReceiveOk) {
				continue;
				}
			} while ((Rx_Buffer != 0x00) && (cerr--));
	
		if (!cerr) {
			fprintf(stderr,currMsgList[AutoBaudRate_MSG03]/*"Errore nel ricevere 0x00 per l'autobudrate: Timeout. Controllare le connessioni\r\n"*/);
			delete aport;
			return false;
			}

		Tx_Buffer = 0x55;
		aport->PurgeRx();
		cerr = 5;
		if (!aport->SendData(&Tx_Buffer,1)) {
			fprintf(stderr,currMsgList[AutoBaudRate_MSG04]/*"Errore nel trasmettere 0x55 per la sincronizzazione\r\n"*/);
			delete aport;
			return false;
			}
		if ((!aport->ReceiveData(&Rx_Buffer,1)) || (Rx_Buffer != ((char) 0xAA))) {
			fprintf(stderr,currMsgList[AutoBaudRate_MSG05]/*"Errore nel ricevere 0xAA per la sincronizzazione\r\n"*/);
			delete aport;
			return false;
			}
		}
	else { // tutti gli altri casi di flash 0.18 um (h8s2378, sh27085)
		Tx_Buffer = 0x00;
		cerr = 5;
		do {
			aport->PurgeRx();
			if (!aport->SendData(&Tx_Buffer,1)) {
				fprintf(stderr,currMsgList[AutoBaudRate_MSG02]/*"errore trasmissione 0x00 in autobaudrate!!!\r\n"*/);
				delete aport;
				return false;
				}
			Rx_Buffer = 0xFF;
			ReceiveOk = aport->ReceiveData(&Rx_Buffer,1);
			if (!ReceiveOk) {
				continue;
				}
			} while ((Rx_Buffer != 0x00) && (cerr--));
		if (!cerr) {
			fprintf(stderr,currMsgList[AutoBaudRate_MSG03]/*"Errore nel ricevere 0x00 per l'autobudrate: Timeout. Controllare le connessioni\r\n"*/);
			delete aport;
			return false;
			}

		Tx_Buffer = 0x55;
		aport->PurgeRx();
		if (!aport->SendData(&Tx_Buffer,1)) {
			fprintf(stderr,currMsgList[AutoBaudRate_MSG04]/*"Errore nel trasmettere 0x55 per la sincronizzazione\r\n"*/);
			delete aport;
			return false;
			}
		Rx_Buffer = 0xFF;
		if ((!aport->ReceiveData(&Rx_Buffer,1)) || (Rx_Buffer != ((char) 0xE6))) {
			fprintf(stderr,currMsgList[AutoBaudRate_MSG05]/*"Errore nel ricevere 0xAA per la sincronizzazione\r\n"*/);
			delete aport;
			return false;
			}
		}
	delete aport;
	return true;
}
static bool DownloadBootProgram(char* portname,KwtddStringList* BootProgram)
{
	unsigned short numofdtbootpquantum = 1;
	unsigned long StartAddressdtbootpquantum;
	unsigned long Sizedtbootpquantum;
	
	//if (uCApp.cpu == Ctt_ARN44RAM) return true;
	
	char Rx_Buffer[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	char Tx_Buffer[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	portdef parport;
	// SETTAGGIO PORTA
	if ((uCApp.cpu == Ctt_SH27145) ||
		(uCApp.cpu == Ctt_ARN45) ||
		(uCApp.cpu == Ctt_H83048FONE_16MHZ) ||
		(uCApp.cpu == Ctt_H8S2378_16MHZ) ||
		(uCApp.cpu == Ctt_SH27085_10MHZ) ||
		(uCApp.cpu == Ctt_H8SX1663_10MHZ) ||
		(uCApp.cpu == Ctt_H8SX1663_12MHZ) ||
		(uCApp.cpu == Ctt_SH2A7286_12MHZ)) {
		strcpy(parport.portname,"COM1");
		parport.baudrate = 19200;		/*!< Baud rate value: ex. 9600 */
		parport.databits = 8;		/*!< Bits of data: ex. 8 or 7*/
		parport.parity = 0;		/*!< Parity bit: ex. 0 disabled, otherwise enabled*/
		parport.stopbits = 1;		/*!< Number of stop bits: 1 or 2*/
		parport.xon_xoff = false;		/*!< XON / XOFF enabled : ex 0 disabled, otherwise enabled*/
		parport.rts_cts = false;		/*!< RTS / CTS enabled: ex 0 disabled, otherwise enabled*/
		parport.timeout = 20;		/*!< Timeout : in ms */
		}
	else {
		strcpy(parport.portname,"COM1");
		parport.baudrate = 9600;		/*!< Baud rate value: ex. 9600 */
		parport.databits = 8;		/*!< Bits of data: ex. 8 or 7*/
		parport.parity = 0;		/*!< Parity bit: ex. 0 disabled, otherwise enabled*/
		parport.stopbits = 1;		/*!< Number of stop bits: 1 or 2*/
		parport.xon_xoff = false;		/*!< XON / XOFF enabled : ex 0 disabled, otherwise enabled*/
		parport.rts_cts = false;		/*!< RTS / CTS enabled: ex 0 disabled, otherwise enabled*/
		parport.timeout = 20;		/*!< Timeout : in ms */
		}
	strcpy(parport.portname,portname);
	arnComPort* aport;
	aport = new arnComPort(parport);
	if (aport->GetStatus()) {fprintf(stderr,currMsgList[DownloadBootProgram_MSG01]/*"errore fatale apertura porta com!!!\r\n"*/); delete aport; return false;}

	DataStream* dtbootp = 0;
	switch (uCApp.cpu) {
		case Ctt_SH27044: {
			dtbootp = new DataStreamPage32byte(_8bit);
			unsigned int NumOfRecord = BootProgram->Count();
			Record* Prg = new Record[NumOfRecord];
			for (unsigned int i = 0; i < NumOfRecord; i++) {
				Record *PrgLocal;
				PrgLocal = new Record((*BootProgram)[i]);
				Prg[i] = (*PrgLocal);
				delete PrgLocal;
				}
			StartAddressdtbootpquantum = 0xFFFFF000;
			Sizedtbootpquantum = 0xC00;
			dtbootp->ConvFromRecordQuantum8bit(Prg,NumOfRecord,numofdtbootpquantum,
				&StartAddressdtbootpquantum,&Sizedtbootpquantum);
			delete [] Prg;
			}	
		break;
		/*
		case Ctt_SH27044F: {
			unsigned int NumOfRecord = BootProgram->Count();
			Record* Prg = new Record[NumOfRecord];
			for (unsigned int i = 0; i < NumOfRecord; i++) {
				Record *PrgLocal;
				PrgLocal = new Record((*BootProgram)[i]);
				Prg[i] = (*PrgLocal);
				delete PrgLocal;
				}
			StartAddressdtbootpquantum = 0xFFFFF000;
			Sizedtbootpquantum = 0xC00;
			dtbootp->ConvFromRecordQuantum8bit(Prg,NumOfRecord,numofdtbootpquantum,
				&StartAddressdtbootpquantum,&Sizedtbootpquantum);
			delete [] Prg;
			}	
		break;
		*/
	   case Ctt_H8S2345: {
			dtbootp = new DataStreamPage32byte(_8bit);
			unsigned int NumOfRecord = BootProgram->Count();
			Record* Prg = new Record[NumOfRecord];
			for (unsigned int i = 0; i < NumOfRecord; i++) {
				Record *PrgLocal;
				PrgLocal = new Record((*BootProgram)[i]);
				Prg[i] = (*PrgLocal);
				delete PrgLocal;
				}
			StartAddressdtbootpquantum = 0x00FFF400;
			Sizedtbootpquantum = 0xC00;
			dtbootp->ConvFromRecordQuantum8bit(Prg,NumOfRecord,numofdtbootpquantum,
				&StartAddressdtbootpquantum,&Sizedtbootpquantum);
			delete [] Prg;
			}	
	   case Ctt_H8S2345_20MHZ: {
			dtbootp = new DataStreamPage32byte(_8bit);
			unsigned int NumOfRecord = BootProgram->Count();
			Record* Prg = new Record[NumOfRecord];
			for (unsigned int i = 0; i < NumOfRecord; i++) {
				Record *PrgLocal;
				PrgLocal = new Record((*BootProgram)[i]);
				Prg[i] = (*PrgLocal);
				delete PrgLocal;
				}
			StartAddressdtbootpquantum = 0x00FFF400;
			Sizedtbootpquantum = 0xC00;
			dtbootp->ConvFromRecordQuantum8bit(Prg,NumOfRecord,numofdtbootpquantum,
				&StartAddressdtbootpquantum,&Sizedtbootpquantum);
			delete [] Prg;
			}	
	      break;
	   case Ctt_H8S2633: {
			dtbootp = new DataStreamPage128byte(_8bit);
			unsigned int NumOfRecord = BootProgram->Count();
			Record* Prg = new Record[NumOfRecord];
			for (unsigned int i = 0; i < NumOfRecord; i++) {
				Record *PrgLocal;
				PrgLocal = new Record((*BootProgram)[i]);
				Prg[i] = (*PrgLocal);
				delete PrgLocal;
				}
			StartAddressdtbootpquantum = 0x00FFC000;
			Sizedtbootpquantum = 0x2FC0;
			dtbootp->ConvFromRecordQuantum8bit(Prg,NumOfRecord,numofdtbootpquantum,
				&StartAddressdtbootpquantum,&Sizedtbootpquantum);
			delete [] Prg;
			}
	      break;
	   case Ctt_H83048: {
			dtbootp = new DataStreamPage32byte(_8bit);
			unsigned int NumOfRecord = BootProgram->Count();
			Record* Prg = new Record[NumOfRecord];
			for (unsigned int i = 0; i < NumOfRecord; i++) {
				Record *PrgLocal;
				PrgLocal = new Record((*BootProgram)[i]);
				Prg[i] = (*PrgLocal);
				delete PrgLocal;
				}
			StartAddressdtbootpquantum = 0x00FFF300;
			Sizedtbootpquantum = 0xC00;
			dtbootp->ConvFromRecordQuantum8bit(Prg,NumOfRecord,numofdtbootpquantum,
				&StartAddressdtbootpquantum,&Sizedtbootpquantum);
			delete [] Prg;
			}	
	      break;
	  /*
	  case Ctt_SH2MNJ600: {
			unsigned int NumOfRecord = BootProgram->Count();
			Record* Prg = new Record[NumOfRecord];
			for (unsigned int i = 0; i < NumOfRecord; i++) {
				Record *PrgLocal;
				PrgLocal = new Record((*BootProgram)[i]);
				Prg[i] = (*PrgLocal);
				delete PrgLocal;
				}
			StartAddressdtbootpquantum = 0xFFFFF000;
			Sizedtbootpquantum = 0xC00;
			dtbootp->ConvFromRecordQuantum8bit(Prg,NumOfRecord,numofdtbootpquantum,
				&StartAddressdtbootpquantum,&Sizedtbootpquantum);
			delete [] Prg;
			}
		break;
		*/
	  case Ctt_ARN44B: {
			dtbootp = new DataStreamPage32byte(_8bit);
			unsigned int NumOfRecord = BootProgram->Count();
			Record* Prg = new Record[NumOfRecord];
			for (unsigned int i = 0; i < NumOfRecord; i++) {
				Record *PrgLocal;
				PrgLocal = new Record((*BootProgram)[i]);
				Prg[i] = (*PrgLocal);
				delete PrgLocal;
				}
			StartAddressdtbootpquantum = 0xFFFFF000;
			Sizedtbootpquantum = 0xC00;
			dtbootp->ConvFromRecordQuantum8bit(Prg,NumOfRecord,numofdtbootpquantum,
				&StartAddressdtbootpquantum,&Sizedtbootpquantum);
			delete [] Prg;
			}
		break;
	  case Ctt_ADAPT2BNC: {
			dtbootp = new DataStreamPage32byte(_8bit);
			unsigned int NumOfRecord = BootProgram->Count();
			Record* Prg = new Record[NumOfRecord];
			for (unsigned int i = 0; i < NumOfRecord; i++) {
				Record *PrgLocal;
				PrgLocal = new Record((*BootProgram)[i]);
				Prg[i] = (*PrgLocal);
				delete PrgLocal;
				}
			StartAddressdtbootpquantum = 0xFFFFF000;
			Sizedtbootpquantum = 0xC00;
			dtbootp->ConvFromRecordQuantum8bit(Prg,NumOfRecord,numofdtbootpquantum,
				&StartAddressdtbootpquantum,&Sizedtbootpquantum);
			delete [] Prg;
			}
		break;
		case Ctt_SH27145: {
			dtbootp = new DataStreamPage128byte(_8bit);
			unsigned int NumOfRecord = BootProgram->Count();
			Record* Prg = new Record[NumOfRecord];
			for (unsigned int i = 0; i < NumOfRecord; i++) {
				Record *PrgLocal;
				PrgLocal = new Record((*BootProgram)[i]);
				Prg[i] = (*PrgLocal);
				delete PrgLocal;
				}
			StartAddressdtbootpquantum = 0xFFFFE800;
			Sizedtbootpquantum = 0x1800;
			dtbootp->ConvFromRecordQuantum8bit(Prg,NumOfRecord,numofdtbootpquantum,
				&StartAddressdtbootpquantum,&Sizedtbootpquantum);
			delete [] Prg;
			}
		break;
		case Ctt_ARN45: {
			dtbootp = new DataStreamPage128byte(_8bit);
			unsigned int NumOfRecord = BootProgram->Count();
			Record* Prg = new Record[NumOfRecord];
			for (unsigned int i = 0; i < NumOfRecord; i++) {
				Record *PrgLocal;
				PrgLocal = new Record((*BootProgram)[i]);
				Prg[i] = (*PrgLocal);
				delete PrgLocal;
				}
			StartAddressdtbootpquantum = 0xFFFFE800;
			Sizedtbootpquantum = 0x1800;
			dtbootp->ConvFromRecordQuantum8bit(Prg,NumOfRecord,numofdtbootpquantum,
				&StartAddressdtbootpquantum,&Sizedtbootpquantum);
			delete [] Prg;
			}
		break;
	   case Ctt_LMB125: {
			dtbootp = new DataStreamPage32byte(_8bit);
			unsigned int NumOfRecord = BootProgram->Count();
			Record* Prg = new Record[NumOfRecord];
			for (unsigned int i = 0; i < NumOfRecord; i++) {
				Record *PrgLocal;
				PrgLocal = new Record((*BootProgram)[i]);
				Prg[i] = (*PrgLocal);
				delete PrgLocal;
				}
			StartAddressdtbootpquantum = 0x00FFF400;
			Sizedtbootpquantum = 0xC00;
			dtbootp->ConvFromRecordQuantum8bit(Prg,NumOfRecord,numofdtbootpquantum,
				&StartAddressdtbootpquantum,&Sizedtbootpquantum);
			delete [] Prg;
			}	
	      break;
		case Ctt_H83687: {
			dtbootp = new DataStreamPage128byte(_8bit);
			unsigned int NumOfRecord = BootProgram->Count();
			Record* Prg = new Record[NumOfRecord];
			for (unsigned int i = 0; i < NumOfRecord; i++) {
				Record *PrgLocal;
				PrgLocal = new Record((*BootProgram)[i]);
				Prg[i] = (*PrgLocal);
				delete PrgLocal;
				}
			StartAddressdtbootpquantum = 0xF780;
			Sizedtbootpquantum = 0x770;
			dtbootp->ConvFromRecordQuantum8bit(Prg,NumOfRecord,numofdtbootpquantum,
				&StartAddressdtbootpquantum,&Sizedtbootpquantum);
			delete [] Prg;
			}
		break;
		case Ctt_H83687_10MHZ: {
			dtbootp = new DataStreamPage128byte(_8bit);
			unsigned int NumOfRecord = BootProgram->Count();
			Record* Prg = new Record[NumOfRecord];
			for (unsigned int i = 0; i < NumOfRecord; i++) {
				Record *PrgLocal;
				PrgLocal = new Record((*BootProgram)[i]);
				Prg[i] = (*PrgLocal);
				delete PrgLocal;
				}
			StartAddressdtbootpquantum = 0xF780;
			Sizedtbootpquantum = 0x770;
			dtbootp->ConvFromRecordQuantum8bit(Prg,NumOfRecord,numofdtbootpquantum,
				&StartAddressdtbootpquantum,&Sizedtbootpquantum);
			delete [] Prg;
			}
		break;
		case Ctt_H83048FONE_16MHZ: {
			dtbootp = new DataStreamPage128byte(_8bit);
			unsigned int NumOfRecord = BootProgram->Count();
			Record* Prg = new Record[NumOfRecord];
			for (unsigned int i = 0; i < NumOfRecord; i++) {
				Record *PrgLocal;
				PrgLocal = new Record((*BootProgram)[i]);
				Prg[i] = (*PrgLocal);
				delete PrgLocal;
				}
			StartAddressdtbootpquantum = 0xFFF510;
			Sizedtbootpquantum = 0xA00;
			dtbootp->ConvFromRecordQuantum8bit(Prg,NumOfRecord,numofdtbootpquantum,
				&StartAddressdtbootpquantum,&Sizedtbootpquantum);
			delete [] Prg;
			}
		break;
		case Ctt_H83687_16MHZ: {
			dtbootp = new DataStreamPage128byte(_8bit);
			unsigned int NumOfRecord = BootProgram->Count();
			Record* Prg = new Record[NumOfRecord];
			for (unsigned int i = 0; i < NumOfRecord; i++) {
				Record *PrgLocal;
				PrgLocal = new Record((*BootProgram)[i]);
				Prg[i] = (*PrgLocal);
				delete PrgLocal;
				}
			StartAddressdtbootpquantum = 0xF780;
			Sizedtbootpquantum = 0x770;
			dtbootp->ConvFromRecordQuantum8bit(Prg,NumOfRecord,numofdtbootpquantum,
				&StartAddressdtbootpquantum,&Sizedtbootpquantum);
			delete [] Prg;
			}
		break;
		case Ctt_H8S2378_16MHZ:
		case Ctt_SH27085_10MHZ:
		case Ctt_H8SX1663_10MHZ:
		case Ctt_SH2A7286_12MHZ:
		case Ctt_H8SX1663_12MHZ: {
			char devID[4];
			unsigned char clockmode;
			Command018Handler* cm = new Command018Handler(aport);
			Res_t* pRes;
// --- Boot Status inquiry
			pRes = cm->ManageCommand(INQ_SEL_CMD__BOOTPROGRAM_STATUS_INQUIRY);
			if (pRes) {
				if (pRes->Response == 0x5F) {
					fprintf(stdout,"Boot Program State:\r\n");
					fprintf(stderr,"\tStatus:0x%02x\r\n",pRes->body.N_BytesR.Data[0]);
					fprintf(stderr,"\tError:0x%02x\r\n",pRes->body.N_BytesR.Data[1]);
					}
				else {
					fprintf(stdout,"Errore nel comando di Boot Program state Inqury.\r\n");
					delete cm; return false;
					}
				}
			else {delete cm; return false;}
// FINE --- Boot Status inquiry
//
			pRes = cm->ManageCommand(INQ_SEL_CMD__SUPPORTED_DEVICE_INQUIRY);
			if (pRes) {
				char tmp[16];
				memset(tmp,0,sizeof(tmp));
				memcpy(tmp,pRes->body.N_BytesR.Data,4);
				memcpy(devID,pRes->body.N_BytesR.Data,4);
				fprintf(stdout,"Device Code - %s\r\n",tmp);
				memset(tmp,0,sizeof(tmp));
				memcpy(tmp,pRes->body.N_BytesR.Data + 4,pRes->body.N_BytesR.Size - 6);
				fprintf(stdout,"Product name - %s\r\n",tmp);
				}
			else {delete cm; return false;}
// ---
//
			unsigned char devIDSize = 4;
			pRes = cm->ManageCommand(INQ_SEL_CMD__DEVICE_SELECTION,devID,&devIDSize);
			if (pRes) {
				if (pRes->Response == 0x06) fprintf(stdout,"device selection OK.\r\n");
				else {
					fprintf(stdout,"Errore nel comando di device selection.\r\n");
					fprintf(stderr,"Error:0x%02x\r\n",pRes->body.ErrorR.Err);
					fprintf(stderr,"Code:0x%02x\r\n",pRes->body.ErrorR.Code);
					delete cm; return false;
					}
				}
			else {delete cm; return false;}
// ---
//
			pRes = cm->ManageCommand(INQ_SEL_CMD__CLOCK_MODE_INQUIRY);
			if (pRes) {
				if (pRes->Response == 0x31) {
					fprintf(stderr,"# of Clock Mode:%d\r\n",pRes->body.N_BytesR.Size);
					for (unsigned int i = 0; i < pRes->body.N_BytesR.Size; i++) {
						fprintf(stderr,"Clock Mode-%d:0x%02x\r\n",i,pRes->body.N_BytesR.Data[i]);
						}
					// ********* attenzione se ci sono piu' clock modes ************************
					clockmode = pRes->body.N_BytesR.Data[0];
					// ********* ************************************** ************************
					}
				else {
					fprintf(stdout,"Errore nel comando Clock mode inquiry.\r\n");
					delete cm; return false;
					}
				}
			else {delete cm; return false;}
// ---
//
			pRes = cm->ManageCommand(INQ_SEL_CMD__CLOCK_MODE_SELECTION,0,&clockmode);
			if (pRes) {
				if (pRes->Response == 0x06) fprintf(stdout,"clock mode selection OK.\r\n");
				else {
					fprintf(stdout,"Errore nel comando di device selection.\r\n");
					fprintf(stderr,"Error:0x%02x\r\n",pRes->body.ErrorR.Err);
					fprintf(stderr,"Code:0x%02x\r\n",pRes->body.ErrorR.Code);
					delete cm; return false;
					}
				}
			else {delete cm; return false;}
// ---
// --- Boot Status inquiry
			pRes = cm->ManageCommand(INQ_SEL_CMD__BOOTPROGRAM_STATUS_INQUIRY);
			if (pRes) {
				if (pRes->Response == 0x5F) {
					fprintf(stdout,"Boot Program State:\r\n");
					fprintf(stderr,"\tStatus:0x%02x\r\n",pRes->body.N_BytesR.Data[0]);
					fprintf(stderr,"\tError:0x%02x\r\n",pRes->body.N_BytesR.Data[1]);
					}
				else {
					fprintf(stdout,"Errore nel comando di Boot Program state Inqury.\r\n");
					delete cm; return false;
					}
				}
			else {delete cm; return false;}
// FINE --- Boot Status inquiry
//
// --- Boot Status inquiry
			pRes = cm->ManageCommand(INQ_SEL_CMD__BOOTPROGRAM_STATUS_INQUIRY);
			if (pRes) {
				if (pRes->Response == 0x5F) {
					fprintf(stdout,"Boot Program State:\r\n");
					fprintf(stderr,"\tStatus:0x%02x\r\n",pRes->body.N_BytesR.Data[0]);
					fprintf(stderr,"\tError:0x%02x\r\n",pRes->body.N_BytesR.Data[1]);
					}
				else {
					fprintf(stdout,"Errore nel comando di Boot Program state Inqury.\r\n");
					delete cm; return false;
					}
				}
			else {delete cm; return false;}
// FINE --- Boot Status inquiry
//

			pRes = cm->ManageCommand(INQ_SEL_CMD__MULTIPLICATION_RATIO_INQUIRY);
			if (pRes) {
				if (pRes->Response == 0x32) {
					unsigned int ptr = 0;
					unsigned int ed = pRes->body.N_BytesR.Data[ptr++];
					fprintf(stdout,"# of Clock Multipication Ratio types:%d\r\n",ed);
					for (unsigned int i = 0; i < ed; i++) {
						unsigned int end = pRes->body.N_BytesR.Data[ptr++];
						for (unsigned int j = 0; j < end; j++) {
							fprintf(stdout,"Clock Multipication Ratio %d: Mul Ratio - %d\r\n",j,pRes->body.N_BytesR.Data[ptr++]);
							}
						}
					}
				else {
					fprintf(stdout,"Errore nel comando Multiplication ratio inquiry.\r\n");
					delete cm; return false;
					}
				}
			else {delete cm; return false;}
// ---
//
			pRes = cm->ManageCommand(INQ_SEL_CMD__OP_CLOCK_FREQ_INQUIRY);
			if (pRes) {
				if (pRes->Response == 0x33) {
					unsigned int ptr = 0;
					unsigned int ed = pRes->body.N_BytesR.Data[ptr++];
					fprintf(stdout,"# of Operating Clock Frequency:%d\r\n",ed);
					for (unsigned int i = 0; i < ed; i++) {
						float tmp;
						tmp = (pRes->body.N_BytesR.Data[ptr++] * 256);
						tmp += pRes->body.N_BytesR.Data[ptr++];
						fprintf(stdout,"Minimum Operating Clock Frequency %d: Mul Ratio - %.2f MHZ\r\n",i,tmp / 100);
						tmp = (pRes->body.N_BytesR.Data[ptr++] * 256);
						tmp += pRes->body.N_BytesR.Data[ptr++];
						fprintf(stdout,"Maximum Operating Clock Frequency %d: Mul Ratio - %.2f MHZ\r\n",i,tmp / 100);
						}
					}
				else {
					fprintf(stdout,"Errore nel comando Operating Clock Frequency inquiry.\r\n");
					delete cm; return false;
					}
				}
			else {delete cm; return false;}
// ---
//
			pRes = cm->ManageCommand(INQ_SEL_CMD__USERMAT_INFO_INQUIRY);
			if (pRes) {
				unsigned char pI,pJ;
				fprintf(stdout,"Num of Areas: %d\r\n",pRes->body.N_BytesR.Data[0]);
				pI = 1; pJ = 5;
				for (unsigned int i = 0; i < (((unsigned int)(pRes->body.N_BytesR.Size - 1)) / 8); i++) {
					fprintf(stdout,"Area-%d-SA: 0x%lx; Area-%d-LA: 0x%lx\r\n",
						i,
						lmirror(*((unsigned long*)(&(pRes->body.N_BytesR.Data[pI])))),
						i,
						lmirror(*((unsigned long*)(&(pRes->body.N_BytesR.Data[pJ]))))
						);
					pI += 8; pJ += 8;
					}
				}
			else {delete cm; return false;}
// ---
// 
			pRes = cm->ManageCommand(PRG_ERS_CMD__USER_MAT_PRG_SELECTION);
			if (pRes) {
				if (pRes->Response == 0x06) fprintf(stdout,"User MAT selection OK.\r\n");
				else {
					fprintf(stdout,"Errore nel comando di User MAT selection.\r\n");
					fprintf(stderr,"Error:0x%02x\r\n",pRes->body.ErrorR.Err);
					fprintf(stderr,"Code:0x%02x\r\n",pRes->body.ErrorR.Code);
					delete cm; return false;
					}
				}
			else {delete cm; return false;}
// ---
//
			unsigned char bitrate[7];
			if (uCApp.cpu == Ctt_H8S2378_16MHZ) {
				bitrate[0] = 0x01; // 38400 / 100; 384 (0x0180) 
				bitrate[1] = 0x80;
				bitrate[2] = 0x06; //0x06; // Input Frequency 16 Mhz (1600 decine di KHz (0x0640))
				bitrate[3] = 0x40; 
				bitrate[4] = 0x01; //0x01; // Number of Multiplication Ratios
				bitrate[5] = 0x01; //0x01; // Operating
				bitrate[6] = 0x01; //0x01; // Peripheral
				}
			else if (uCApp.cpu == Ctt_SH27085_10MHZ) {
				bitrate[0] = 0x01; // 38400 / 100; 384 (0x0180) 
				bitrate[1] = 0x80;
				bitrate[2] = 0x03; //0x03; // Input Frequency 10 Mhz (1000 decine di KHz (0x03e0))
				bitrate[3] = 0xE8;
				bitrate[4] = 0x02; //0x02; // Number of Multiplication Ratios
				bitrate[5] = 0x02; //0x02; // Operating (Iphi) x 2 ( x 8 PLL / 4 default setting of the FREQ divider register)
				bitrate[6] = 0x02; //0x02; // Peripheral (Pphi) x 2 ( x 8 PLL / 4 default setting of the FREQ divider register)
				}
			else if (uCApp.cpu == Ctt_H8SX1663_10MHZ) {
				bitrate[0] = 0x01; // 38400 / 100; 384 (0x0180) 
				bitrate[1] = 0x80;
				bitrate[2] = 0x03; //0x03; // Input Frequency 10 Mhz (1000 decine di KHz (0x03e0))
				bitrate[3] = 0xE8;
				bitrate[4] = 0x02; //0x02; // Number of Multiplication Ratios
				bitrate[5] = 0x02; //0x02; // Operating (Iphi) x 2 ( x 8 PLL / 4 default setting of the FREQ divider register)
				bitrate[6] = 0x02; //0x02; // Peripheral (Pphi) x 2 ( x 8 PLL / 4 default setting of the FREQ divider register)
				}
			else if (uCApp.cpu == Ctt_H8SX1663_12MHZ) {
				bitrate[0] = 0x01; // 38400 / 100; 384 (0x0180) 
				bitrate[1] = 0x80;
				bitrate[2] = 0x04; //0x03; // Input Frequency 10 Mhz (1000 decine di KHz (0x04b0))
				bitrate[3] = 0xB0;
				bitrate[4] = 0x02; //0x02; // Number of Multiplication Ratios
				bitrate[5] = 0x02; //0x02; // Operating (Iphi) x 2 ( x 8 PLL / 4 default setting of the FREQ divider register)
				bitrate[6] = 0x02; //0x02; // Peripheral (Pphi) x 2 ( x 8 PLL / 4 default setting of the FREQ divider register)
				}
			else if (uCApp.cpu == Ctt_SH2A7286_12MHZ) {
				bitrate[0] = 0x01; //38400 / 100; 384 (0x0180) 
				bitrate[1] = 0x80;
				bitrate[2] = 0x04; //0x04; // Input Frequency 12 Mhz (1200 decine di KHz (0x04B0))
				bitrate[3] = 0xB0;
				bitrate[4] = 0x01; //0x01; // Number of Multiplication Ratios
				bitrate[5] = 0x02; //0x02; // Operating (Iphi) x 2 ( x 8 PLL / 4 default setting of the FREQ divider register)
				bitrate[6] = 0x02; //0x02; // Peripheral (Pphi) x 2 ( x 8 PLL / 4 default setting of the FREQ divider register)
				}
			else {
				fprintf(stdout,"Cpu non supportata.\r\n");
				return false;
				}
			pRes = cm->ManageCommand(INQ_SEL_CMD__NEW_BITRATE_SELECTION,bitrate);
			if (pRes) {
				if (pRes->Response == 0x06) fprintf(stdout,"New bitrate selection OK.\r\n");
				else {
					fprintf(stdout,"Errore nel comando di New bitrate selection.\r\n");
					fprintf(stderr,"Error:0x%02x\r\n",pRes->body.ErrorR.Err);
					fprintf(stderr,"Code:0x%02x\r\n",pRes->body.ErrorR.Code);
					delete cm; return false;
					}
				}
			else {delete cm; return false;}
// ---
			delete cm;
			}
		break;
	default: break;
	}

	if ((uCApp.cpu == Ctt_H8S2378_16MHZ) ||
		(uCApp.cpu == Ctt_SH27085_10MHZ) ||
		(uCApp.cpu == Ctt_H8SX1663_10MHZ) ||
		(uCApp.cpu == Ctt_H8SX1663_12MHZ) ||
		(uCApp.cpu == Ctt_SH2A7286_12MHZ)) return true;

	Tx_Buffer[0] = (char) (dtbootp->GetBlock8Bit_ptr(0) >> 8);
	Tx_Buffer[1] = (char) (dtbootp->GetBlock8Bit_ptr(0));

	unsigned long countbyte = 0;
	do {
		aport->PurgeTx();
		aport->PurgeRx();
		if (!aport->SendData(&(Tx_Buffer[countbyte]),1)) {
			fprintf(stderr,currMsgList[DownloadBootProgram_MSG02]/*"Errore trasmissione byte n.%d di eco Boot program\r\n"*/,(int)countbyte);
			if (aport) {delete aport; aport = 0;}
			if (dtbootp) {delete dtbootp; dtbootp = 0;}
			return false;
			}
		if (!aport->ReceiveData(&(Rx_Buffer[countbyte]),1)) {
			fprintf(stderr,currMsgList[DownloadBootProgram_MSG03]/*"Errore ricezione byte n.%d di eco Boot program\r\n"*/,(int)countbyte);
			if (aport) {delete aport; aport = 0;}
			if (dtbootp) {delete dtbootp; dtbootp = 0;}
			return false;
			}

		countbyte++;
		} while (countbyte < 2);


	if ((Rx_Buffer[0] != Tx_Buffer[0]) || (Rx_Buffer[1] != Tx_Buffer[1])) {
		fprintf(stderr,currMsgList[DownloadBootProgram_MSG04]/*"Errore coerenza bytes di eco Boot program\r\n"*/);
		if (aport) {delete aport; aport = 0;}
		if (dtbootp) {delete dtbootp; dtbootp = 0;}
		return false;
		}
// ------ fine trasmissione lunghezza boot program ----------------------------

// ------ inizio download boot program ----------------------------------------
	aport->PurgeRx();
	aport->PurgeTx();


	if (!dtbootp->SendByteFormBlock8bit(aport)) {
		fprintf(stderr,currMsgList[DownloadBootProgram_MSG05]/*" Errore Invio pagine BootProgram\r\n"*/);
		if (aport) {delete aport; aport = 0;}
		if (dtbootp) {delete dtbootp; dtbootp = 0;}
		return false;
		}
	return true;
}


static bool DownloadTargetProgram(char* portname,KwtddStringList* TargetProgram)
{
	// SETTAGGIO PORTA
	portdef parport;
	if ((uCApp.cpu == Ctt_SH27145) ||
		(uCApp.cpu == Ctt_ARN45)) {
		strcpy(parport.portname,"COM1");
		parport.baudrate = 115200;		/*!< Baud rate value: ex. 9600 */
		parport.databits = 8;		/*!< Bits of data: ex. 8 or 7*/
		parport.parity = 0;		/*!< Parity bit: ex. 0 disabled, otherwise enabled*/
		parport.stopbits = 1;		/*!< Number of stop bits: 1 or 2*/
		parport.xon_xoff = false;		/*!< XON / XOFF enabled : ex 0 disabled, otherwise enabled*/
		parport.rts_cts = false;		/*!< RTS / CTS enabled: ex 0 disabled, otherwise enabled*/
		parport.timeout = 100;		/*!< Timeout : in ms */
		}
	else if ((uCApp.cpu == Ctt_H83687)/* ||
		 (uCApp.cpu == Ctt_H8S2378_16MHZ)*/) {
		strcpy(parport.portname,"COM1");
		parport.baudrate = 19200;		/*!< Baud rate value: ex. 9600 */
		parport.databits = 8;		/*!< Bits of data: ex. 8 or 7*/
		parport.parity = 0;		/*!< Parity bit: ex. 0 disabled, otherwise enabled*/
		parport.stopbits = 1;		/*!< Number of stop bits: 1 or 2*/
		parport.xon_xoff = false;		/*!< XON / XOFF enabled : ex 0 disabled, otherwise enabled*/
		parport.rts_cts = false;		/*!< RTS / CTS enabled: ex 0 disabled, otherwise enabled*/
		parport.timeout = 100;		/*!< Timeout : in ms */
		}
	else {
		strcpy(parport.portname,"COM1");
		parport.baudrate = 38400;		/*!< Baud rate value: ex. 9600 */
		parport.databits = 8;		/*!< Bits of data: ex. 8 or 7*/
		parport.parity = 0;		/*!< Parity bit: ex. 0 disabled, otherwise enabled*/
		parport.stopbits = 1;		/*!< Number of stop bits: 1 or 2*/
		parport.xon_xoff = false;		/*!< XON / XOFF enabled : ex 0 disabled, otherwise enabled*/
		parport.rts_cts = false;		/*!< RTS / CTS enabled: ex 0 disabled, otherwise enabled*/
		parport.timeout = 100;		/*!< Timeout : in ms */
		}
	strcpy(parport.portname,portname);
	arnComPort* aport;
	aport = new arnComPort(parport);
	if (aport->GetStatus()) {fprintf(stderr,currMsgList[DownloadTargetProgram_MSG01]/*"errore fatale apertura porta com!!!\r\n"*/); delete aport; return false;}
		
	if ((uCApp.cpu != Ctt_H8S2378_16MHZ) && 
		(uCApp.cpu != Ctt_SH27085_10MHZ) &&
		(uCApp.cpu != Ctt_H8SX1663_10MHZ) &&
		(uCApp.cpu != Ctt_H8SX1663_12MHZ) &&
		(uCApp.cpu != Ctt_SH2A7286_12MHZ)) {
		/* Attendo SSSTart dal Micro */
		if (!ListenAndAnswerToEcu(aport,ECU_SEARCH_PC,PC_DELEXTFLASH_ECU)) {
			fprintf(stderr,currMsgList[DownloadTargetProgram_MSG02]/*"Errore ascolto comando SSStart.\r\n"*/);
			delete aport;
			return false;
			}
// Questo algoritmo mette a posto il problema dell' uscita dalla cancellazione
// della flash esterna	
		unsigned char SectCnt = 0;
		unsigned char ActSectCnt = SectCnt;
		bool triggerval = false;
		do {
			if (ListenAndAnswerToEcuForErasingSectors(aport,ECU_DELOK_PC,PC_ERCNT_ECU,&SectCnt)){
				if (SectCnt > ActSectCnt) {
					fprintf(stdout,currMsgList[DownloadTargetProgram_MSG03]/*"Settore n. %d cancellato.\n\r"*/,(int)SectCnt);
					ActSectCnt = SectCnt;
					}
				}
			else if (ListenAndAnswerToEcuForErasingSectors(aport,ECU_DELNOT_PC,PC_EXIT_ECU,&SectCnt)) {
				fprintf(stderr,currMsgList[DownloadTargetProgram_MSG04]/*"Errore cancellazione settore n.%d.\r\n"*/,SectCnt);
				delete aport;
				return false;
				}
			else {
				fprintf(stderr,currMsgList[DownloadTargetProgram_MSG05]/*"Timeout cancellazione settore n.%d.\r\n"*/,SectCnt);
				delete aport;
				return false;
				}
			if (!triggerval) {
				if (SectCnt > ECU_DELOK_PC) triggerval = true;
				else if (N_SECTORS < ECU_DELOK_PC) {
					if (SectCnt == ECU_DELOK_PC)  {SectCnt = N_SECTORS; break;}
					}
				}
			if ((SectCnt == ECU_DELOK_PC) && triggerval) break;
			} while (1/*SectCnt < (N_SECTORS - 1)*/);

		if (!ListenAndAnswerToEcu(aport,ECU_DELOK_PC,PC_GODLD_ECU)) {
			fprintf(stderr,currMsgList[DownloadTargetProgram_MSG06]/*"Errore ascolto fine cancellazione.\r\n"*/);
			delete aport;
			return false;
			}
		else fprintf(stdout,currMsgList[DownloadTargetProgram_MSG07]/*"Fine Cancellazione Flash Esterna.\r\n"*/);
	}
	
// ************* costruisco la lista dei record ******************************
	unsigned int NumOfRecord = TargetProgram->Count();
	Record* Prg = new Record[NumOfRecord];
	for (unsigned int i = 0; i < NumOfRecord; i++) {
		Record *PrgLocal;
		PrgLocal = new Record((*TargetProgram)[i]);
		Prg[i] = (*PrgLocal);
		delete PrgLocal;
		}

	unsigned short numofUPpquantum;
	unsigned long StartAddressofQuantum[MAX_UP_QUANTUM_NUMBER];
	unsigned long SizeofQuantum[MAX_UP_QUANTUM_NUMBER];
	DataStream* dt = 0;
	switch (uCApp.cpu) {
		case Ctt_SH27044:
			dt = new DataStreamPage32byte(_8bit);
			numofUPpquantum = 1;
			StartAddressofQuantum[0] = 0x00000000;
			SizeofQuantum[0] = 0x20000;
			dt->ConvFromRecordQuantum8bit(Prg,NumOfRecord,numofUPpquantum,
				StartAddressofQuantum,SizeofQuantum);
			break;
		case Ctt_H8S2345:
			dt = new DataStreamPage32byte(_8bit);
			numofUPpquantum = 1;
			StartAddressofQuantum[0] = 0x00000000;
			SizeofQuantum[0] = 0x20000;
			dt->ConvFromRecordQuantum8bit(Prg,NumOfRecord,numofUPpquantum,
				StartAddressofQuantum,SizeofQuantum);
			break;
		case Ctt_H8S2345_20MHZ:
			dt = new DataStreamPage32byte(_8bit);
			numofUPpquantum = 1;
			StartAddressofQuantum[0] = 0x00000000;
			SizeofQuantum[0] = 0x20000;
			dt->ConvFromRecordQuantum8bit(Prg,NumOfRecord,numofUPpquantum,
				StartAddressofQuantum,SizeofQuantum);
			break;
		case Ctt_H8S2633:
			dt = new DataStreamPage128byte(_8bit);
			numofUPpquantum = 1;
			StartAddressofQuantum[0] = 0x00000000;
			SizeofQuantum[0] = 0x40000;
			dt->ConvFromRecordQuantum8bit(Prg,NumOfRecord,numofUPpquantum,
				StartAddressofQuantum,SizeofQuantum);
			break;
		case Ctt_H83048:
			dt = new DataStreamPage32byte(_8bit);
			numofUPpquantum = 1;
			StartAddressofQuantum[0] = 0x00000000;
			SizeofQuantum[0] = 0x20000;
			dt->ConvFromRecordQuantum8bit(Prg,NumOfRecord,numofUPpquantum,
				StartAddressofQuantum,SizeofQuantum);
			break;
		case Ctt_ARN44B:
			dt = new DataStreamPage32byte(_8bit);
			numofUPpquantum = 2;
			StartAddressofQuantum[0] = 0x00000000;
			SizeofQuantum[0] = 0x20000;
			StartAddressofQuantum[1] = 0x00800000L;
			SizeofQuantum[1] = 0x400000;
			dt->ConvFromRecordQuantum8bit(Prg,NumOfRecord,numofUPpquantum,
				StartAddressofQuantum,SizeofQuantum);
			break;
		case Ctt_ADAPT2BNC:
			dt = new DataStreamPage32byte(_8bit);
			numofUPpquantum = 2;
			StartAddressofQuantum[0] = 0x00000000;
			SizeofQuantum[0] = 0x20000;
			StartAddressofQuantum[1] = 0x00800000L;
			SizeofQuantum[1] = 0x20000;
			dt->ConvFromRecordQuantum8bit(Prg,NumOfRecord,numofUPpquantum,
				StartAddressofQuantum,SizeofQuantum);
			break;
		case Ctt_SH27145:
			dt = new DataStreamPage128byte(_8bit);
			numofUPpquantum = 1;
			StartAddressofQuantum[0] = 0x00000000;
			SizeofQuantum[0] = 0x40000;
			dt->ConvFromRecordQuantum8bit(Prg,NumOfRecord,numofUPpquantum,
				StartAddressofQuantum,SizeofQuantum);
			break;
		case Ctt_ARN45:
			dt = new DataStreamPage128byte(_8bit);
			numofUPpquantum = 2;
			StartAddressofQuantum[0] = 0x00000000;
			SizeofQuantum[0] = 0x40000;
			StartAddressofQuantum[1] = 0x00400000L;
			SizeofQuantum[1] = 0x20000;
			dt->ConvFromRecordQuantum8bit(Prg,NumOfRecord,numofUPpquantum,
				StartAddressofQuantum,SizeofQuantum);
			break;
		/*
		case Ctt_SH27044F:
			numofUPpquantum = 1;
			StartAddressofQuantum[0] = 0x00000000;
			SizeofQuantum[0] = 0x20000;
			dt->ConvFromRecordQuantum8bit(Prg,NumOfRecord,numofUPpquantum,
				StartAddressofQuantum,SizeofQuantum);
			break;
		case Ctt_SH2MNJ600:
			numofUPpquantum = 2;
			StartAddressofQuantum[0] = 0x00000000;
			SizeofQuantum[0] = 0x20000;
			StartAddressofQuantum[1] = 0x00410000;
			SizeofQuantum[1] = 0x20000;
			dt->ConvFromRecordQuantum8bit(Prg,NumOfRecord,numofUPpquantum,
				StartAddressofQuantum,SizeofQuantum);
			break;
		case Ctt_MNJ600RAM:
			numofUPpquantum = 1;
			StartAddressofQuantum[0] = 0x00200000;
			SizeofQuantum[0] = 0x10000;
			dt->ConvFromRecordQuantum8bit(Prg,NumOfRecord,numofUPpquantum,
				StartAddressofQuantum,SizeofQuantum);
			break;
		case Ctt_ARN44RAM:
			dt = new DataStreamPage32byte(_8bit);
			numofUPpquantum = 1;
			StartAddressofQuantum[0] = 0x00400000;
			SizeofQuantum[0] = 0x80000;
			dt->ConvFromRecordQuantum8bit(Prg,NumOfRecord,numofUPpquantum,
				StartAddressofQuantum,SizeofQuantum);
			break;
		*/
		case Ctt_LMB125:
			dt = new DataStreamPage32byte(_8bit);
			numofUPpquantum = 1;
			StartAddressofQuantum[0] = 0x00000000;
			SizeofQuantum[0] = 0x20000;
			dt->ConvFromRecordQuantum8bit(Prg,NumOfRecord,numofUPpquantum,
				StartAddressofQuantum,SizeofQuantum);
			break;
		case Ctt_H83687:
			dt = new DataStreamPage128byte(_8bit);
			numofUPpquantum = 1;
			StartAddressofQuantum[0] = 0x00000000;
			SizeofQuantum[0] = 0xe000;
			dt->ConvFromRecordQuantum8bit(Prg,NumOfRecord,numofUPpquantum,
				StartAddressofQuantum,SizeofQuantum);
			break;
		case Ctt_H83687_10MHZ:
			dt = new DataStreamPage128byte(_8bit);
			numofUPpquantum = 1;
			StartAddressofQuantum[0] = 0x00000000;
			SizeofQuantum[0] = 0xe000;
			dt->ConvFromRecordQuantum8bit(Prg,NumOfRecord,numofUPpquantum,
				StartAddressofQuantum,SizeofQuantum);
			break;
		case Ctt_H83687_16MHZ:
			dt = new DataStreamPage128byte(_8bit);
			numofUPpquantum = 1;
			StartAddressofQuantum[0] = 0x00000000;
			SizeofQuantum[0] = 0xe000;
			dt->ConvFromRecordQuantum8bit(Prg,NumOfRecord,numofUPpquantum,
				StartAddressofQuantum,SizeofQuantum);
			break;
		case Ctt_H83048FONE_16MHZ:
			dt = new DataStreamPage128byte(_8bit);
			numofUPpquantum = 1;
			StartAddressofQuantum[0] = 0x00000000;
			SizeofQuantum[0] = 0x20000;
			dt->ConvFromRecordQuantum8bit(Prg,NumOfRecord,numofUPpquantum,
				StartAddressofQuantum,SizeofQuantum);
			break;
		case Ctt_H8S2378_16MHZ:
			dt = new DataStreamPage128byte(_8bit);
			numofUPpquantum = 1;
			StartAddressofQuantum[0] = 0x00000000;
			SizeofQuantum[0] = 0x80000;
			dt->ConvFromRecordQuantum8bit(Prg,NumOfRecord,numofUPpquantum,
				StartAddressofQuantum,SizeofQuantum);
			break;
		case Ctt_SH27085_10MHZ:
			dt = new DataStreamPage128byte(_8bit);
			numofUPpquantum = 1;
			StartAddressofQuantum[0] = 0x00000000;
			SizeofQuantum[0] = 0x80000;
			dt->ConvFromRecordQuantum8bit(Prg,NumOfRecord,numofUPpquantum,
				StartAddressofQuantum,SizeofQuantum);
			break;
		case Ctt_H8SX1663_10MHZ:
		case Ctt_H8SX1663_12MHZ:
			dt = new DataStreamPage128byte(_8bit);
			numofUPpquantum = 1;
			StartAddressofQuantum[0] = 0x00000000;
			SizeofQuantum[0] = 0x60000;
			dt->ConvFromRecordQuantum8bit(Prg,NumOfRecord,numofUPpquantum,
				StartAddressofQuantum,SizeofQuantum);
			break;
		case Ctt_SH2A7286_12MHZ:
			dt = new DataStreamPage256byte(_8bit);
			numofUPpquantum = 1;
			StartAddressofQuantum[0] = 0x00000000;
			SizeofQuantum[0] = 0x80000;
			dt->ConvFromRecordQuantum8bit(Prg,NumOfRecord,numofUPpquantum,
				StartAddressofQuantum,SizeofQuantum);
			break;
		default: break;
		}


	fprintf(stdout,currMsgList[DownloadTargetProgram_MSG08]/*"DownLoading Programma Utente in corso...\r\n"*/);

	if ((uCApp.cpu != Ctt_H8S2378_16MHZ) && 
		(uCApp.cpu != Ctt_SH27085_10MHZ) &&
		(uCApp.cpu != Ctt_H8SX1663_10MHZ) &&
		(uCApp.cpu != Ctt_H8SX1663_12MHZ) &&
		(uCApp.cpu != Ctt_SH2A7286_12MHZ)
		) {
		for (unsigned int i = 0; i < dt->GetpageNumber(); i++) {
			if (!dt->SendPage(aport,i)) {
				fprintf(stderr,currMsgList[DownloadTargetProgram_MSG09]/*"Errore 0x0006: Errore downloading user program. Riprovare.\r\n"*/);
				if (aport) {delete aport; aport = 0;}
				if (dt) {delete dt; dt = 0;}
				return false;
				}
			else {
				if (dt->Getpagentwrite(i) != 0) {
					//StatRichEdit->Lines->Add(AnsiString("BAdrr: ") + AnsiString(IntToHex(((int)dt->Getpage32baseaddr(i)),6)) + AnsiString(" : ") + AnsiString(dt->Getpage32ntwrite(i)));
					fprintf(stdout,currMsgList[DownloadTargetProgram_MSG10]/*"pagina %d di %d pagine. tentativi %d.\r\n"*/,\
						i,(int)dt->GetpageNumber(),(int)dt->Getpagentwrite(i));
					}
				}
			}
		if (!dt->SendEndSignal(aport)) {
			fprintf(stderr,currMsgList[DownloadTargetProgram_MSG11]/*"Errore 0x0006: Errore downloading user program. Riprovare.\r\n"*/);
			if (aport) {delete aport; aport = 0;}
			if (dt) {delete dt; dt = 0;}
			return false;
			}
		}
	else if (uCApp.cpu == Ctt_SH2A7286_12MHZ) {
		Res_t* pRes;
		Command018Handler* cm = new Command018Handler(aport);
		if (!ListenAndAnswerToEcu(aport,0x06,0x06)) {
			fprintf(stderr,currMsgList[DownloadTargetProgram_MSG02]/*"Errore ascolto comando SSStart.\r\n"*/);
			delete aport;
			return false;
			}
// --- Boot Status inquiry
		pRes = cm->ManageCommand(INQ_SEL_CMD__BOOTPROGRAM_STATUS_INQUIRY);
		if (pRes) {
			if (pRes->Response == 0x5F) {
				fprintf(stdout,"Boot Program State:\r\n");
				fprintf(stderr,"\tStatus:0x%02x\r\n",pRes->body.N_BytesR.Data[0]);
				fprintf(stderr,"\tError:0x%02x\r\n",pRes->body.N_BytesR.Data[1]);
				}
			else {
				fprintf(stdout,"Errore nel comando di Boot Program state Inqury.\r\n");
				delete cm; return false;
				}
			}
		else {delete cm; return false;}
// FINE --- Boot Status inquiry
			pRes = cm->ManageCommand(INQ_SEL_CMD__TRANS_TO_PROG_ERASE_STATE);
			if (pRes) {
				if (pRes->Response == 0x06) fprintf(stdout,"Transition to Prog / Erase  OK.\r\n");
				else {
					fprintf(stdout,"Errore nel comando di User MAT selection.\r\n");
					fprintf(stderr,"Error:0x%02x\r\n",pRes->body.ErrorR.Err);
					fprintf(stderr,"Code:0x%02x\r\n",pRes->body.ErrorR.Code);
					delete cm; return false;
					}
				}
			else {delete cm; return false;}
// ---
// --- Boot Status inquiry
		pRes = cm->ManageCommand(INQ_SEL_CMD__BOOTPROGRAM_STATUS_INQUIRY);
		if (pRes) {
			if (pRes->Response == 0x5F) {
				fprintf(stdout,"Boot Program State:\r\n");
				fprintf(stderr,"\tStatus:0x%02x\r\n",pRes->body.N_BytesR.Data[0]);
				fprintf(stderr,"\tError:0x%02x\r\n",pRes->body.N_BytesR.Data[1]);
				}
			else {
				fprintf(stdout,"Errore nel comando di Boot Program state Inqury.\r\n");
				delete cm; return false;
				}
			}
		else {delete cm; return false;}
// FINE --- Boot Status inquiry
//
			pRes = cm->ManageCommand(PRG_ERS_CMD__USER_MAT_PRG_SELECTION);
			if (pRes) {
				if (pRes->Response == 0x06) fprintf(stdout,"User MAT Selection  OK.\r\n");
				else {
					fprintf(stdout,"Errore nel comando di User MAT selection.\r\n");
					fprintf(stderr,"Error:0x%02x\r\n",pRes->body.ErrorR.Err);
					fprintf(stderr,"Code:0x%02x\r\n",pRes->body.ErrorR.Code);
					delete cm; return false;
					}
				}
			else {delete cm; return false;}
// ---
//
// --- Boot Status inquiry
		pRes = cm->ManageCommand(INQ_SEL_CMD__BOOTPROGRAM_STATUS_INQUIRY);
		if (pRes) {
			if (pRes->Response == 0x5F) {
				fprintf(stdout,"Boot Program State:\r\n");
				fprintf(stderr,"\tStatus:0x%02x\r\n",pRes->body.N_BytesR.Data[0]);
				fprintf(stderr,"\tError:0x%02x\r\n",pRes->body.N_BytesR.Data[1]);
				}
			else {
				fprintf(stdout,"Errore nel comando di Boot Program state Inqury.\r\n");
				delete cm; return false;
				}
			}
		else {delete cm; return false;}
// FINE --- Boot Status inquiry
		for (unsigned int i = 0; i < dt->GetpageNumber(); i++) {
			if (dt->IsDummyPage(i)) continue;
			pRes = cm->WritePage(dt->Getpagebaseaddr(i),dt->Getpagedata(i),256);
			if (pRes) {
				if (pRes->Response == 0x06) fprintf(stdout,"page %d of %d - Write OK.\r\n",i,(int)dt->GetpageNumber());
				else {
					fprintf(stdout,"Errore nel comando di Write Page.\r\n");
					fprintf(stderr,"Error:0x%x02\r\n",pRes->body.ErrorR.Err);
					fprintf(stderr,"Code:0x%x02\r\n",pRes->body.ErrorR.Code);
					delete cm; return false;
					}
				}
			else {delete cm; return false;}
			}
// ---
		delete cm;
		}
	else { // h8s2378 , sh27085
		Res_t* pRes;
		Command018Handler* cm = new Command018Handler(aport);
		if (!ListenAndAnswerToEcu(aport,0x06,0x06)) {
			fprintf(stderr,currMsgList[DownloadTargetProgram_MSG02]/*"Errore ascolto comando SSStart.\r\n"*/);
			delete aport;
			return false;
			}
// --- Boot Status inquiry
		pRes = cm->ManageCommand(INQ_SEL_CMD__BOOTPROGRAM_STATUS_INQUIRY);
		if (pRes) {
			if (pRes->Response == 0x5F) {
				fprintf(stdout,"Boot Program State:\r\n");
				fprintf(stderr,"\tStatus:0x%02x\r\n",pRes->body.N_BytesR.Data[0]);
				fprintf(stderr,"\tError:0x%02x\r\n",pRes->body.N_BytesR.Data[1]);
				}
			else {
				fprintf(stdout,"Errore nel comando di Boot Program state Inqury.\r\n");
				delete cm; return false;
				}
			}
		else {delete cm; return false;}
// FINE --- Boot Status inquiry
//
			pRes = cm->ManageCommand(INQ_SEL_CMD__TRANS_TO_PROG_ERASE_STATE);
			if (pRes) {
				if (pRes->Response == 0x06) fprintf(stdout,"Transition to Prog / Erase  OK.\r\n");
				else {
					fprintf(stdout,"Errore nel comando di User MAT selection.\r\n");
					fprintf(stderr,"Error:0x%02x\r\n",pRes->body.ErrorR.Err);
					fprintf(stderr,"Code:0x%02x\r\n",pRes->body.ErrorR.Code);
					delete cm; return false;
					}
				}
			else {delete cm; return false;}
// ---
// --- Boot Status inquiry
		pRes = cm->ManageCommand(INQ_SEL_CMD__BOOTPROGRAM_STATUS_INQUIRY);
		if (pRes) {
			if (pRes->Response == 0x5F) {
				fprintf(stdout,"Boot Program State:\r\n");
				fprintf(stderr,"\tStatus:0x%02x\r\n",pRes->body.N_BytesR.Data[0]);
				fprintf(stderr,"\tError:0x%02x\r\n",pRes->body.N_BytesR.Data[1]);
				}
			else {
				fprintf(stdout,"Errore nel comando di Boot Program state Inqury.\r\n");
				delete cm; return false;
				}
			}
		else {delete cm; return false;}
// FINE --- Boot Status inquiry
//
			pRes = cm->ManageCommand(PRG_ERS_CMD__USER_MAT_PRG_SELECTION);
			if (pRes) {
				if (pRes->Response == 0x06) fprintf(stdout,"User MAT Selection  OK.\r\n");
				else {
					fprintf(stdout,"Errore nel comando di User MAT selection.\r\n");
					fprintf(stderr,"Error:0x%02x\r\n",pRes->body.ErrorR.Err);
					fprintf(stderr,"Code:0x%02x\r\n",pRes->body.ErrorR.Code);
					delete cm; return false;
					}
				}
			else {delete cm; return false;}
// ---
//
		for (unsigned int i = 0; i < dt->GetpageNumber(); i++) {
			if (dt->IsDummyPage(i)) continue;
			pRes = cm->WritePage(dt->Getpagebaseaddr(i),dt->Getpagedata(i),128);
			if (pRes) {
				if (pRes->Response == 0x06) fprintf(stdout,"page %d of %d - Write OK.\r\n",i,(int)dt->GetpageNumber());
				else {
					fprintf(stdout,"Errore nel comando di Write Page.\r\n");
					fprintf(stderr,"Error:0x%x02\r\n",pRes->body.ErrorR.Err);
					fprintf(stderr,"Code:0x%x02\r\n",pRes->body.ErrorR.Code);
					delete cm; return false;
					}
				}
			else {delete cm; return false;}
			}
// ---
		delete cm;
		}
	if (aport) {delete aport; aport = 0;}
	if (dt) {delete dt; dt = 0;}
	return true;
}


//---------------------------------------------------------------------------
static bool ListenAndAnswerToEcu(arnComPort* PortaComm,unsigned char LstCmd,unsigned char AswCmd)
{
	char Rx_Buf[10] = {0,0,0,0,0,0,0,0,0,0};
	char Tx_Buf[10] = {0,0,0,0,0,0,0,0,0,0};
/*
unsigned long MaxTime;
unsigned long LocalTime;
*/

	int cerr;
	bool ReceiveOk;
	if ((uCApp.cpu != Ctt_H8S2378_16MHZ) && 
		(uCApp.cpu != Ctt_SH27085_10MHZ) &&
		(uCApp.cpu != Ctt_H8SX1663_10MHZ) &&
		(uCApp.cpu != Ctt_H8SX1663_12MHZ) &&
		(uCApp.cpu != Ctt_SH2A7286_12MHZ)) {
		cerr = 10;
		do {
			PortaComm->PurgeRx();
			Rx_Buf[0] = 0x00;
			ReceiveOk = PortaComm->ReceiveData(&(Rx_Buf[0]),1);
			if (!ReceiveOk) continue;
			} while ((Rx_Buf[0] != ((char) LstCmd)) && (cerr--));

		if ((!cerr) || (!ReceiveOk)) return false;
/*		do {*/
			PortaComm->PurgeTx();
			Tx_Buf[0] = (char) AswCmd;
			PortaComm->PurgeRx();
			if (!PortaComm->SendData(&(Tx_Buf[0]),1)) return false;
/*		LocalTime = TempoVar;
		do  {
			Rx_Buf[0] = 0x00;
			ReceiveOk = PortaComm->ReceiveData(&(Rx_Buf[0]),1);
			PortaComm->PurgeRx();
			Application->ProcessMessages();
			if (!ReceiveOk) continue;
			} while ((Rx_Buf[0] == ((char) LstCmd)) && (TempoVar < (LocalTime + 3)));
		if (TempoVar < (LocalTime + 3)) break;
		} while ((++MaxTime) < 5);*/
		/*if (MaxTime >= 5) return false;
		else*/ return true;
		}
	else { // h8s2378, sh27085
		PortaComm->PurgeTx();
		PortaComm->PurgeRx();
		cerr = 10;
		Tx_Buf[0] = (char) AswCmd;
		if (!PortaComm->SendData(&(Tx_Buf[0]),1)) return false;
		do {
			Rx_Buf[0] = 0x00;
			ReceiveOk = PortaComm->ReceiveData(&(Rx_Buf[0]),1);
			if (!ReceiveOk) continue;
			} while ((Rx_Buf[0] != ((char) LstCmd)) && (cerr--));
		if ((!cerr) || (!ReceiveOk)) return false;
		return true;
		}
}
//---------------------------------------------------------------------------
static bool ListenAndAnswerToEcuForErasingSectors(arnComPort* PortaComm,unsigned char LstCmd,unsigned char AswCmd,unsigned char* Sect)
{
	char Rx_Buf[10] = {0,0,0,0,0,0,0,0,0,0};
	char Tx_Buf[10] = {0,0,0,0,0,0,0,0,0,0};
/*
unsigned long MaxTime;
unsigned long LocalTime;
*/
	int cerr;
	bool ReceiveOk;

	cerr = 5;
	do {
		PortaComm->PurgeRx();
		Rx_Buf[0] = Rx_Buf[1]= 0x00;
		ReceiveOk = PortaComm->ReceiveData(&(Rx_Buf[0]),1);
		/*ReceiveOk = */PortaComm->ReceiveData(&(Rx_Buf[1]),1);
		if (!ReceiveOk) continue;
		} while ((Rx_Buf[0] != ((char) LstCmd)) && (cerr--));

   if ((!cerr) || (!ReceiveOk)) {fprintf(stdout,"%s,%s,%d. cerr = %d, ReceiveOk = %d.\r\n",__FILE__,__FUNCTION__,__LINE__,cerr,ReceiveOk); return false;}
/*  MaxTime = 0;
   do {  */
	PortaComm->PurgeTx();
	Tx_Buf[0] = (char) AswCmd;
	PortaComm->PurgeRx();
	if (!PortaComm->SendData(&(Tx_Buf[0]),1)) {fprintf(stdout,"%s,%s,%d.\r\n",__FILE__,__FUNCTION__,__LINE__); return false;}
/*        LocalTime = TempoVar;
        do  {
            Rx_Buf[0] = 0x00;
            ReceiveOk = PortaComm->ReceiveData(&(Rx_Buf[0]),2);
            PortaComm->PurgeRx();
            Application->ProcessMessages();
            if (!ReceiveOk) continue;
            } while ((Rx_Buf[0] == ((char) LstCmd)) && (TempoVar < (LocalTime + 50)));
        if (TempoVar < (LocalTime + 50)) break; */
/*   } while ((++MaxTime) < 5);*/
 *Sect = Rx_Buf[1];
	/*if (MaxTime >= 5) return false;
	else*/ return true;
}

/* ******************************************************************* */


/* ******************************************************************* */
/* --------------------- FUNZIONI DI DEBUG --------------------------- */
#ifdef __WTDD_DEBUG_
static void wtddPrintfStringList(KwtddStringList* List)
{
	for (int lIndex = 0; lIndex < List->Count(); lIndex++) {
		int lw;
		char* t = new char[(*List)[lIndex].Count()];
		if (t == NULL) break;
		for (lw = 0; lw < (*List)[lIndex].Count(); lw++)
			t[lw] = (*List)[lIndex][lw];
		printf("%s",t);
		delete  t;
		}

}
#endif
/* ******************************************************************* */

/*
	char* q;
	QCString t(argv[0]);
	q = new char[t.length()];
	strcpy(q,t);
	printf("%s\n",q);
	if (wtddTrovaFile("main.cpp")) printf("main found.\n");
	delete q;


	AStringList* p;
	p = new  AStringList(10,false);

	delete p;
*/
/*
	portdef parport = {
			"COM1",		// portname
			3400,		// baudrate
			8,		// databits
			0,		// parity
			1,		// stopbits
			false,		// xon_xoff
			false,		// rts_cts
			20,		// pklen
			20		// timeout  decimi di secondo
			};
	ComPort* aport;
	bool resrxtx;
	char bff[16];
	aport = new ComPort(parport);
	if (aport->GetStatus()) {delete aport; return EXIT_FAILURE;} // errore apertura porta
	resrxtx = aport->SendData("Caio!!!!",8);
	resrxtx = aport->ReceiveData(bff,3);

	delete aport;	
*/


