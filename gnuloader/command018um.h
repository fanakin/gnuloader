/***************************************************************************
                          command018um.h  -  description
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

#ifndef Command018umH
#define Command018umH

#include "arnComPort.h"

typedef enum {
	c018_1_BYTE,
	c018_N_BYTE,
	c018_PAGEPROG,
	c018_NONE
	} c018_t;

typedef enum {
	r018_1_BYTE,
	r018_N_BYTE,
	r018_ERROR,
	r018_MEMREAD,
	r018_NONE
	} r018_t;

#define DATA_LEN		256
typedef union _cbody {
		struct {
			unsigned char Size;
			unsigned char Data[DATA_LEN];
			unsigned char chks;
			} N_BytesC;
		struct {
			unsigned long Address;
			unsigned char Data[DATA_LEN];
			unsigned char chks;
			} PageProg;
		} cbody_t;

typedef union _rbody {
		struct {
			unsigned char Size;
			unsigned char Data[DATA_LEN];
			unsigned char chks;
			} N_BytesR;
		struct {
			unsigned char Err;
			unsigned char Code;
			} ErrorR;
		struct {
			unsigned long Size;
			unsigned char Data[DATA_LEN];
			unsigned char chks;
			} MemReadR;
		} rbody_t;

typedef struct {
	c018_t type;
	unsigned char Command;
	cbody_t body;
	} Com_t;

typedef struct {
	r018_t type;
	unsigned char Response;
	rbody_t body;
	} Res_t;

#define INQ_SEL_CMD__SUPPORTED_DEVICE_INQUIRY		0x20
#define INQ_SEL_CMD__DEVICE_SELECTION				0x10
#define INQ_SEL_CMD__CLOCK_MODE_INQUIRY				0x21
#define INQ_SEL_CMD__CLOCK_MODE_SELECTION			0x11
#define INQ_SEL_CMD__MULTIPLICATION_RATIO_INQUIRY	0x22
#define INQ_SEL_CMD__OP_CLOCK_FREQ_INQUIRY			0x23
#define INQ_SEL_CMD__USER_BOOTMAT_INFO_INQUIRY		0x24
#define INQ_SEL_CMD__USERMAT_INFO_INQUIRY			0x25
#define INQ_SEL_CMD__BLOCK_FOR_ERASING_INFO_INQUIRY	0x26
#define INQ_SEL_CMD__PROGRAMMING_UNIT_INQUIRY		0x27
#define INQ_SEL_CMD__NEW_BITRATE_SELECTION			0x3F
#define INQ_SEL_CMD__TRANS_TO_PROG_ERASE_STATE		0x40
#define INQ_SEL_CMD__BOOTPROGRAM_STATUS_INQUIRY		0x4F

#define PRG_ERS_CMD__USER_BOOTMAT_PRG_SELECTION		0x42
#define PRG_ERS_CMD__USER_MAT_PRG_SELECTION			0x43
#define PRG_ERS_CMD__128_BYTE_PROGRAMMING			0x50
#define PRG_ERS_CMD__ERASING_SELECTION				0x48
#define PRG_ERS_CMD__BLOCK_ERASING					0x58
#define PRG_ERS_CMD__MEMORY_READ					0x52
#define PRG_ERS_CMD__USER_BOOTMAT_SUM_CHECK			0x4A
#define PRG_ERS_CMD__USER_MAT_SUM_CHECK				0x4B
#define PRG_ERS_CMD__USER_BOOTMAT_BLK_CHECK			0x4C
#define PRG_ERS_CMD__USER_MAT_BLK_CHECK				0x4D


class Command018Handler {
	private:
	protected:
		arnComPort* aport;
		Com_t Command;
		Res_t Response;
		virtual c018_t cmd_to_type(unsigned char cmd);
		virtual r018_t cmd_to_res(unsigned char cmd);
		virtual bool prepareCmd(unsigned char cmd,void* hpar, void* lpar);
		virtual bool sendCmd(void);
		virtual bool receiveRes(void);
	public:
		Command018Handler (arnComPort* aprt);
		virtual ~Command018Handler (void);
		Res_t* ManageCommand(unsigned char cmd,void* hpar = 0, void* lpar = 0);
		Res_t* WritePage(unsigned long address, unsigned char* bff, unsigned short len);
	};
#endif
