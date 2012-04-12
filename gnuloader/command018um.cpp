/***************************************************************************
                          command018um.cpp  -  description
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
#include <string.h>
#include "CommonFunctions.h"
#include "commfunc.h"
#include "command018um.h"

Command018Handler::Command018Handler (arnComPort* aprt)
{
	aport = aprt;
	aport->PurgeRx();
}

Command018Handler::~Command018Handler (void)
{
}

Res_t* Command018Handler::ManageCommand(unsigned char cmd,void* hpar, void* lpar)
{
	Command.type = cmd_to_type(cmd);
	if (Command.type == c018_NONE) return 0;
	if (!prepareCmd(cmd,hpar,lpar)) return 0;
	if (!sendCmd()) return 0;
	if (!receiveRes()) return 0;
	return &Response;
}

c018_t Command018Handler::cmd_to_type(unsigned char cmd)
{
	c018_t res;
	switch (cmd) {
		case INQ_SEL_CMD__SUPPORTED_DEVICE_INQUIRY: res = c018_1_BYTE; break;
		case INQ_SEL_CMD__DEVICE_SELECTION: res = c018_N_BYTE;  break;
		case INQ_SEL_CMD__CLOCK_MODE_INQUIRY: res = c018_1_BYTE;  break;
		case INQ_SEL_CMD__CLOCK_MODE_SELECTION: res = c018_N_BYTE;  break;
		case INQ_SEL_CMD__MULTIPLICATION_RATIO_INQUIRY: res = c018_1_BYTE;  break;
		case INQ_SEL_CMD__OP_CLOCK_FREQ_INQUIRY: res = c018_1_BYTE;  break;
		case INQ_SEL_CMD__USER_BOOTMAT_INFO_INQUIRY: res = c018_1_BYTE;  break;
		case INQ_SEL_CMD__USERMAT_INFO_INQUIRY: res = c018_1_BYTE;  break;
		case INQ_SEL_CMD__BLOCK_FOR_ERASING_INFO_INQUIRY: res = c018_1_BYTE;  break;
		case INQ_SEL_CMD__PROGRAMMING_UNIT_INQUIRY: res = c018_1_BYTE;  break;
		case INQ_SEL_CMD__NEW_BITRATE_SELECTION: res = c018_N_BYTE;  break;
		case INQ_SEL_CMD__TRANS_TO_PROG_ERASE_STATE: res = c018_1_BYTE;  break;
		case INQ_SEL_CMD__BOOTPROGRAM_STATUS_INQUIRY: res = c018_1_BYTE;  break;

		case PRG_ERS_CMD__USER_BOOTMAT_PRG_SELECTION: res = c018_1_BYTE;  break;
		case PRG_ERS_CMD__USER_MAT_PRG_SELECTION: res = c018_1_BYTE;  break;
		case PRG_ERS_CMD__128_BYTE_PROGRAMMING: res = c018_N_BYTE;  break;
		case PRG_ERS_CMD__ERASING_SELECTION: res = c018_1_BYTE;  break;
		case PRG_ERS_CMD__BLOCK_ERASING: res = c018_N_BYTE;  break;
		case PRG_ERS_CMD__MEMORY_READ: res = c018_N_BYTE;  break;
		case PRG_ERS_CMD__USER_BOOTMAT_SUM_CHECK: res = c018_1_BYTE;  break;
		case PRG_ERS_CMD__USER_MAT_SUM_CHECK: res = c018_1_BYTE;  break;
		case PRG_ERS_CMD__USER_BOOTMAT_BLK_CHECK: res = c018_1_BYTE;  break;
		case PRG_ERS_CMD__USER_MAT_BLK_CHECK: res = c018_1_BYTE;  break;
		default : res = c018_NONE; break;
		}
	return res;
}

r018_t Command018Handler::cmd_to_res(unsigned char cmd)
{
	r018_t res;
	switch (cmd) {
		case INQ_SEL_CMD__SUPPORTED_DEVICE_INQUIRY: res = r018_N_BYTE; break;
		case INQ_SEL_CMD__DEVICE_SELECTION: res = r018_1_BYTE;  break;
		case INQ_SEL_CMD__CLOCK_MODE_INQUIRY: res = r018_N_BYTE;  break;
		case INQ_SEL_CMD__CLOCK_MODE_SELECTION: res = r018_1_BYTE;  break;
		case INQ_SEL_CMD__MULTIPLICATION_RATIO_INQUIRY: res = r018_N_BYTE;  break;
		case INQ_SEL_CMD__OP_CLOCK_FREQ_INQUIRY: res = r018_N_BYTE;  break;
		case INQ_SEL_CMD__USER_BOOTMAT_INFO_INQUIRY: res = r018_N_BYTE;  break;
		case INQ_SEL_CMD__USERMAT_INFO_INQUIRY: res = r018_N_BYTE;  break;
		case INQ_SEL_CMD__BLOCK_FOR_ERASING_INFO_INQUIRY: res = r018_N_BYTE;  break;
		case INQ_SEL_CMD__PROGRAMMING_UNIT_INQUIRY: res = r018_N_BYTE;  break;
		case INQ_SEL_CMD__NEW_BITRATE_SELECTION: res = r018_1_BYTE;  break;
		case INQ_SEL_CMD__TRANS_TO_PROG_ERASE_STATE: res = r018_1_BYTE;  break;
		case INQ_SEL_CMD__BOOTPROGRAM_STATUS_INQUIRY: res = r018_N_BYTE;  break;

		case PRG_ERS_CMD__USER_BOOTMAT_PRG_SELECTION: res = r018_1_BYTE;  break;
		case PRG_ERS_CMD__USER_MAT_PRG_SELECTION: res = r018_1_BYTE;  break;
		case PRG_ERS_CMD__128_BYTE_PROGRAMMING: res = r018_1_BYTE;  break;
		case PRG_ERS_CMD__ERASING_SELECTION: res = r018_1_BYTE;  break;
		case PRG_ERS_CMD__BLOCK_ERASING: res = r018_1_BYTE;  break;
		case PRG_ERS_CMD__MEMORY_READ: res = r018_N_BYTE;  break;
		case PRG_ERS_CMD__USER_BOOTMAT_SUM_CHECK: res = r018_N_BYTE;  break;
		case PRG_ERS_CMD__USER_MAT_SUM_CHECK: res = r018_N_BYTE;  break;
		case PRG_ERS_CMD__USER_BOOTMAT_BLK_CHECK: res = r018_1_BYTE;  break;
		case PRG_ERS_CMD__USER_MAT_BLK_CHECK: res = r018_1_BYTE;  break;
		default : res = r018_NONE; break;
		}
	return res;
}

bool Command018Handler::prepareCmd(unsigned char cmd,void* hpar, void* lpar)
{
	bool res = false;
	unsigned char chks;
	switch (cmd) {
		case INQ_SEL_CMD__SUPPORTED_DEVICE_INQUIRY:
			Command.Command = INQ_SEL_CMD__SUPPORTED_DEVICE_INQUIRY;
			Response.type = r018_N_BYTE;
			Response.Response = 0x30;
			res = true;
			break;
		case INQ_SEL_CMD__DEVICE_SELECTION:
			Command.Command = INQ_SEL_CMD__DEVICE_SELECTION;
			Command.body.N_BytesC.Size = *((unsigned char*)lpar); // 4
			memcpy(Command.body.N_BytesC.Data,(char*)hpar,Command.body.N_BytesC.Size); //"0a01"
			chks = 0;
			chks += INQ_SEL_CMD__DEVICE_SELECTION;
			chks += Command.body.N_BytesC.Size;
			for (unsigned int i = 0; i < Command.body.N_BytesC.Size; i++) chks += Command.body.N_BytesC.Data[i];
			Command.body.N_BytesC.chks = ((unsigned short)0x0100) - chks;
			//
			Response.type = r018_1_BYTE;
			Response.Response = 0x06;
			res = true;
			break;
		case INQ_SEL_CMD__CLOCK_MODE_INQUIRY:
			Command.Command = INQ_SEL_CMD__CLOCK_MODE_INQUIRY;
			Response.type = r018_N_BYTE;
			Response.Response = 0x31;
			res = true;
			break;
		case INQ_SEL_CMD__CLOCK_MODE_SELECTION: {
			unsigned char clockmode = (*((unsigned char*)lpar)); 
			Command.Command = INQ_SEL_CMD__CLOCK_MODE_SELECTION;
			Command.body.N_BytesC.Size = 1;
			Command.body.N_BytesC.Data[0] = clockmode;
			chks = 0;
			chks += INQ_SEL_CMD__CLOCK_MODE_SELECTION;
			chks += Command.body.N_BytesC.Size;
			for (unsigned int i = 0; i < Command.body.N_BytesC.Size; i++) chks += Command.body.N_BytesC.Data[i];
			Command.body.N_BytesC.chks = ((unsigned short)0x0100) - chks;
			//
			Response.type = r018_1_BYTE;
			Response.Response = 0x06;
			res = true;
			}
			break;
		case INQ_SEL_CMD__MULTIPLICATION_RATIO_INQUIRY:
			Command.Command = INQ_SEL_CMD__MULTIPLICATION_RATIO_INQUIRY;
			Response.type = r018_N_BYTE;
			Response.Response = 0x32;
			res = true;
			break;
		case INQ_SEL_CMD__OP_CLOCK_FREQ_INQUIRY:
			Command.Command = INQ_SEL_CMD__OP_CLOCK_FREQ_INQUIRY;
			Response.type = r018_N_BYTE;
			Response.Response = 0x33;
			res = true;
			break;
		case INQ_SEL_CMD__NEW_BITRATE_SELECTION:
			Command.Command = INQ_SEL_CMD__NEW_BITRATE_SELECTION;
			Command.body.N_BytesC.Size = 7;
			Command.body.N_BytesC.Data[0] = *((unsigned char*)hpar); // Bit Rate 19200 / 100 = 192 (0x00c0); 38400 / 100 = 384 (0x0180)
			Command.body.N_BytesC.Data[1] = *((unsigned char*)(hpar) + 1);
			Command.body.N_BytesC.Data[2] = *((unsigned char*)(hpar) + 2);//0x06; // Input Frequency 16 Mhz (1600 decine di KHz (0x0640))
			Command.body.N_BytesC.Data[3] = *((unsigned char*)(hpar) + 3);//0x40;
			Command.body.N_BytesC.Data[4] = *((unsigned char*)(hpar) + 4);//0x01; // Number of Multiplication Ratios
			Command.body.N_BytesC.Data[5] = *((unsigned char*)(hpar) + 5);//0x01; // Operating
			Command.body.N_BytesC.Data[6] = *((unsigned char*)(hpar) + 6);//0x01; // Peripheral
			chks = 0;
			chks += INQ_SEL_CMD__NEW_BITRATE_SELECTION;
			chks += Command.body.N_BytesC.Size;
			for (unsigned int i = 0; i < Command.body.N_BytesC.Size; i++) chks += Command.body.N_BytesC.Data[i];
			Command.body.N_BytesC.chks = ((unsigned short)0x0100) - chks;
			//
			Response.type = r018_1_BYTE;
			Response.Response = 0x06;
			res = true;
			break;
		case INQ_SEL_CMD__TRANS_TO_PROG_ERASE_STATE:
			Command.Command = INQ_SEL_CMD__TRANS_TO_PROG_ERASE_STATE;
			Response.type = r018_1_BYTE;
			Response.Response = 0x06;
			res = true;
			break;
		case PRG_ERS_CMD__USER_BOOTMAT_PRG_SELECTION:
			Command.Command = PRG_ERS_CMD__USER_BOOTMAT_PRG_SELECTION;
			Response.type = r018_1_BYTE;
			Response.Response = 0x06;
			res = true;
			break;
		case PRG_ERS_CMD__USER_MAT_PRG_SELECTION:
			Command.Command = PRG_ERS_CMD__USER_MAT_PRG_SELECTION;
			Response.type = r018_1_BYTE;
			Response.Response = 0x06;
			res = true;
			break;
		case INQ_SEL_CMD__USERMAT_INFO_INQUIRY:
			Command.Command = INQ_SEL_CMD__USERMAT_INFO_INQUIRY;
			Response.type = r018_N_BYTE;
			Response.Response = 0x35;
			res = true;
			break;
		case INQ_SEL_CMD__BOOTPROGRAM_STATUS_INQUIRY:
			Command.Command = INQ_SEL_CMD__BOOTPROGRAM_STATUS_INQUIRY;
			Response.type = r018_N_BYTE;
			Response.Response = 0x5F;
			res = true;
			break;
		default : break;
		}
	return res;
}

bool Command018Handler::sendCmd(void)
{
	bool res = false;
	unsigned char Tx_Buffer;
	switch (Command.type) {
		case c018_1_BYTE:
			Tx_Buffer = Command.Command;
			aport->PurgeRx();
			if (!aport->SendData((char*)&Tx_Buffer,1)) {
				fprintf(stderr,"Errore trasmissione comando:0x%x02\r\n",Command.Command);
				return false;
				}
			res = true;
			break;
		case c018_N_BYTE:
			Tx_Buffer = Command.Command;
			aport->PurgeRx();
			if (!aport->SendData((char*)&Tx_Buffer,1)) {
				fprintf(stderr,"Errore trasmissione comando:0x%x02\r\n",Command.Command);
				return false;
				}
			Tx_Buffer = Command.body.N_BytesC.Size;
			aport->PurgeRx();
			if (!aport->SendData((char*)&Tx_Buffer,1)) {
				fprintf(stderr,"Errore trasmissione size:0x%x02\r\n",Command.Command);
				return false;
				}
			for (unsigned int i = 0; i < Command.body.N_BytesC.Size; i++) {
				Tx_Buffer = Command.body.N_BytesC.Data[i];
				aport->PurgeRx();
				if (!aport->SendData((char*)&Tx_Buffer,1)) {
					fprintf(stderr,"Errore trasmissione data:0x%x02\r\n",Command.Command);
					return false;
					}
				}
			Tx_Buffer = Command.body.N_BytesC.chks;
			aport->PurgeRx();
			if (!aport->SendData((char*)&Tx_Buffer,1)) {
				fprintf(stderr,"Errore trasmissione chks:0x%x02\r\n",Command.Command);
				return false;
				}
			res = true;
			break;
		default : break;
		}
	return res;
}

bool Command018Handler::receiveRes(void)
{
	bool res = false;
	unsigned char Rx_Buffer;
	unsigned char chks;
	switch (Response.type) {
		case r018_N_BYTE: {
			chks = 0;
			unsigned char i;
			if (Command.Command == INQ_SEL_CMD__SUPPORTED_DEVICE_INQUIRY) {
				Rx_Buffer = 0xFF;
/*				if ((!aport->ReceiveData((char*)&Rx_Buffer,1))) {
					fprintf(stderr,"Errore ricezione risposta comando:0x%02x\r\n",Command.Command);
					return false;
					}
				fprintf(stderr,"0x%02x\r\n",Rx_Buffer);*/
				if ((!aport->ReceiveData((char*)&Rx_Buffer,1))) {
					fprintf(stderr,"Errore ricezione risposta comando:0x%02x\r\n",Command.Command);
					return false;
					}
				if (Rx_Buffer != Response.Response) {
					fprintf(stderr,"Errore ricezione risposta comando:0x%02x. Codice risposta errato.\r\n",Command.Command);
					fprintf(stderr,"0x%02x\r\n",Rx_Buffer);
					fprintf(stderr,"0x%02x\r\n",Response.Response);
					fprintf(stderr,"0x%02x\r\n",Command.Command);
					return false;
					}
				chks += Rx_Buffer;
				if ((!aport->ReceiveData((char*)&Rx_Buffer,1))) {
					fprintf(stderr,"Errore ricezione risposta comando:0x%02x. size non pervenuta.\r\n",Command.Command);
					return false;
					}
				Response.body.N_BytesR.Size = Rx_Buffer;
				chks += Rx_Buffer;
				if ((!aport->ReceiveData((char*)&Rx_Buffer,1))) {
					fprintf(stderr,"Errore ricezione risposta comando:0x%02x. size non pervenuta.\r\n",Command.Command);
					return false;
					}
				chks += Rx_Buffer;
				if ((!aport->ReceiveData((char*)&Rx_Buffer,1))) {
					fprintf(stderr,"Errore ricezione risposta comando:0x%02x. size non pervenuta.\r\n",Command.Command);
					return false;
					}
				chks += Rx_Buffer;
				for (i = 0; i < (Response.body.N_BytesR.Size - 2); i++) {
					if ((!aport->ReceiveData((char*)&Rx_Buffer,1))) {
						fprintf(stderr,"Errore ricezione risposta comando:0x%02x. errore sui data.\r\n",Command.Command);
						return false;
						}
					Response.body.N_BytesR.Data[i] = Rx_Buffer;
					chks += Rx_Buffer;
					}
				if ((!aport->ReceiveData((char*)&Rx_Buffer,1))) {
					fprintf(stderr,"Errore ricezione risposta comando:0x%02x. size non pervenuta.\r\n",Command.Command);
					return false;
					}
				chks += Rx_Buffer;
				if (chks != 0) {
					fprintf(stderr,"Errore ricezione risposta comando:0x%02x.Chks errata.\r\n",Command.Command);
					return false;
					}
				}
			else if ((Command.Command == INQ_SEL_CMD__CLOCK_MODE_INQUIRY) ||
				(Command.Command == INQ_SEL_CMD__USERMAT_INFO_INQUIRY) ||
				(Command.Command == INQ_SEL_CMD__MULTIPLICATION_RATIO_INQUIRY) ||
				(Command.Command == INQ_SEL_CMD__OP_CLOCK_FREQ_INQUIRY)
				) {
				Rx_Buffer = 0xFF;
				if ((!aport->ReceiveData((char*)&Rx_Buffer,1))) {
					fprintf(stderr,"Errore ricezione risposta comando:0x%02x\r\n",Command.Command);
					return false;
					}
				if (Rx_Buffer != Response.Response) {
					fprintf(stderr,"Errore ricezione risposta comando:0x%02x. Codice risposta errato.\r\n",Command.Command);
					fprintf(stderr,"0x%02x\r\n",Rx_Buffer);
					fprintf(stderr,"0x%02x\r\n",Response.Response);
					fprintf(stderr,"0x%02x\r\n",Command.Command);
					return false;
					}
				chks += Rx_Buffer;
				if ((!aport->ReceiveData((char*)&Rx_Buffer,1))) {
					fprintf(stderr,"Errore ricezione risposta comando:0x%02x. size non pervenuta.\r\n",Command.Command);
					return false;
					}
				Response.body.N_BytesR.Size = Rx_Buffer;
				//fprintf(stderr,"Response.body.N_BytesR.Size:0x%02x\r\n",Response.body.N_BytesR.Size);
				chks += Rx_Buffer;
				for (i = 0; i < Response.body.N_BytesR.Size; i++) {
					if ((!aport->ReceiveData((char*)&Rx_Buffer,1))) {
						fprintf(stderr,"Errore ricezione risposta comando:0x%02x. errore sui data.\r\n",Command.Command);
						return false;
						}
//fprintf(stderr,"\tData[%d]:0x%02x\r\n",i,Rx_Buffer);
					Response.body.N_BytesR.Data[i] = Rx_Buffer;
					chks += Rx_Buffer;
					}
				if ((!aport->ReceiveData((char*)&Rx_Buffer,1))) {
					fprintf(stderr,"Errore ricezione risposta comando:0x%02x. size non pervenuta.\r\n",Command.Command);
					return false;
					}
				chks += Rx_Buffer;
				if (chks != 0) {
					fprintf(stderr,"Errore ricezione risposta comando:0x%02x.Chks errata.\r\n",Command.Command);
					return false;
					}
				}
			else if (Command.Command == INQ_SEL_CMD__BOOTPROGRAM_STATUS_INQUIRY) {
				unsigned char lRx_Buffer[5] = {0xff,0xff,0xff,0xff,0xff};
				if ((!aport->ReceiveData((char*)&lRx_Buffer[0],1))) {
					fprintf(stderr,"Errore ricezione risposta comando:0x%02x\r\n",Command.Command);
					return false;
					}
				if ((!aport->ReceiveData((char*)&lRx_Buffer[1],1))) {
					fprintf(stderr,"Errore ricezione risposta comando:0x%02x\r\n",Command.Command);
					return false;
					}
				if ((!aport->ReceiveData((char*)&lRx_Buffer[2],1))) {
					fprintf(stderr,"Errore ricezione risposta comando:0x%02x\r\n",Command.Command);
					return false;
					}
				if ((!aport->ReceiveData((char*)&lRx_Buffer[3],1))) {
					fprintf(stderr,"Errore ricezione risposta comando:0x%02x\r\n",Command.Command);
					return false;
					}
				if ((!aport->ReceiveData((char*)&lRx_Buffer[4],1))) {
					fprintf(stderr,"Errore ricezione risposta comando:0x%02x\r\n",Command.Command);
					return false;
					}
				if (lRx_Buffer[0] != Response.Response) {
					fprintf(stderr,"Errore ricezione risposta comando:0x%02x. Codice risposta errato.\r\n",Command.Command);
					return false;
					}
				chks = lRx_Buffer[0];
				Response.body.N_BytesR.Size = lRx_Buffer[1];
				//fprintf(stderr,"Response.body.N_BytesR.Size:0x%02x\r\n",Response.body.N_BytesR.Size);
				chks += lRx_Buffer[1];
				for (i = 0; i < Response.body.N_BytesR.Size; i++) {
					Response.body.N_BytesR.Data[i] = lRx_Buffer[2 + i];
					fprintf(stderr,"\tData[%d]:0x%02x\r\n",i,Response.body.N_BytesR.Data[i]);
					chks += Response.body.N_BytesR.Data[i];
					}
/*				chks += lRx_Buffer[4];
				if (chks != 0) {
					fprintf(stderr,"Errore ricezione risposta comando:0x%02x.Chks errata.\r\n",Command.Command);
					return false;
					}*/
				}
			}
			res = true;
			break;
		case r018_1_BYTE:
			if ((!aport->ReceiveData((char*)&Rx_Buffer,1))) {
				fprintf(stderr,"Errore ricezione risposta comando:0x%02x. ack non pervenuta.\r\n",Command.Command);
				return false;
				}
			if (Response.Response != Rx_Buffer) {
				Response.type = r018_ERROR;
				Response.body.ErrorR.Err = Rx_Buffer;
				if ((!aport->ReceiveData((char*)&Rx_Buffer,1))) {
					fprintf(stderr,"Errore ricezione risposta comando:0x%02x. err code non pervenuto.\r\n",Command.Command);
					return false;
					}
				Response.body.ErrorR.Code = Rx_Buffer;
				}
			res = true;
			break;
		default : break;
		}
	return res;
}

Res_t* Command018Handler::WritePage(unsigned long address, unsigned char* bff, unsigned short len)
{
	unsigned char Tx_Buffer,Rx_Buffer;
	// prepare
	Command.type = cmd_to_type(PRG_ERS_CMD__128_BYTE_PROGRAMMING);
	if (Command.type == c018_NONE) return 0;
	Command.Command = PRG_ERS_CMD__128_BYTE_PROGRAMMING;
	Command.body.PageProg.chks = Command.Command;
	Command.body.PageProg.Address = address;
	unsigned char* ptmp = ((unsigned char*)(&(Command.body.PageProg.Address)));
	for (unsigned int i = 0; i < 4; i++) {
		Command.body.PageProg.chks += ptmp[i];
		}
	memset(Command.body.PageProg.Data,0xff,DATA_LEN);
	for (unsigned int i = 0; i < len/*128*/; i++) {
		if (i < len) Command.body.PageProg.Data[i] = bff[i];
		Command.body.PageProg.chks += Command.body.PageProg.Data[i];
		}
	Command.body.PageProg.chks = ((unsigned short)0x0100) - Command.body.PageProg.chks;
	//
	Response.type = r018_1_BYTE;
	Response.Response = 0x06;
	// FINE --- prepare

	// send
	Tx_Buffer = Command.Command;
	aport->PurgeRx();
	if (!aport->SendData((char*)&Tx_Buffer,1)) {
		fprintf(stderr,"Errore trasmissione comando:0x%x02\r\n",Command.Command);
		return 0;
		}
	//fprintf(stdout,"0x%02x..\r\n",Tx_Buffer);
	//fprintf(stdout,"\r\n");
	unsigned long ltp = lmirror(Command.body.PageProg.Address);
	ptmp = ((unsigned char*)(&ltp));
	for (unsigned int i = 0; i < 4; i++) {
		Tx_Buffer = ptmp[i];
		aport->PurgeRx();
		if (!aport->SendData((char*)&Tx_Buffer,1)) {
			fprintf(stderr,"Errore trasmissione address:0x%x02\r\n",Command.Command);
			return 0;
			}
		//fprintf(stdout,"0x%02x..\r\n",Tx_Buffer);
		}
	//fprintf(stdout,"\r\n");
	for (unsigned int i = 0; i < len; i++) {
		Tx_Buffer = Command.body.PageProg.Data[i];
		aport->PurgeRx();
		if (!aport->SendData((char*)&Tx_Buffer,1)) {
			fprintf(stderr,"Errore trasmissione data:0x%x02\r\n",Command.Command);
			return 0;
			}
		//fprintf(stdout,"0x%02x..\r\n",Tx_Buffer);
		}
	//fprintf(stdout,"\r\n");
	Tx_Buffer = Command.body.PageProg.chks;
	aport->PurgeRx();
	if (!aport->SendData((char*)&Tx_Buffer,1)) {
		fprintf(stderr,"Errore trasmissione chks:0x%x02\r\n",Command.Command);
		return 0;
		}
	//fprintf(stdout,"0x%02x..\r\n",Tx_Buffer);
	// FINE --- send
	//fprintf(stdout,"\r\n\r\n\r\n");

	// receice RES
	Rx_Buffer = 0xff;
	if ((!aport->ReceiveData((char*)&Rx_Buffer,1))) {
		fprintf(stderr,"Errore ricezione risposta comando:0x%02x. ack non pervenuta.\r\n",Command.Command);
		return 0;
		}
	//fprintf(stderr,"0x%02x..\r\n",Rx_Buffer);
	//fprintf(stderr,"0x%02x..\r\n",Response.Response);
	if (Response.Response != Rx_Buffer) {
		Response.type = r018_ERROR;
		Response.body.ErrorR.Err = Rx_Buffer;
		Rx_Buffer = 0xff;
		if ((!aport->ReceiveData((char*)&Rx_Buffer,1))) {
			fprintf(stderr,"Errore ricezione risposta comando:0x%02x. err code non pervenuto.\r\n",Command.Command);
			return 0;
			}
		fprintf(stderr,"0x%02x..\r\n",Rx_Buffer);
		Response.body.ErrorR.Code = Rx_Buffer;
		return 0;
		}
	// FINE --- receice RES
	
	return &Response;
}
