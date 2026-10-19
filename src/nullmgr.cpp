// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "nullmgr.h"
#include "_keyboar.h"
#include "conquer.h"
#include "data.h"
#include "dbgprint.h"
#include "init.h"
#include "language\language.h"
#include "misc.h"
#include "modemreg.h"
#include "msgbox.h"
#include "msgloop.h"
#include "ownrdraw.h"
#include "session.h"
#include "wincomm.h"
#include "stringtable.h"
#include "stimer.h"
#define ESC		         27
#define NOKEY           0xffff
#define INIT_COMMAND_RETRIES	2
#define DEFAULT_TIMEOUT		90
#define DETECT_DEFAULT_TIMEOUT		180

#define DEFAULT_TIMEOUT		90

#include <math.h>

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007EEFC8;
extern const double DAT_007EEFD0;

// FUNCTION: 0x005F13D0 $E33
double DAT_00AC12E0 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x005F1400 $E36
double DAT_00AC1338 = DAT_00AC12E0 / DAT_007EEFC8;

// FUNCTION: 0x005F1420 $E39
double DAT_00AC1320 = DAT_007E1718 / DAT_00AC1338;

// FUNCTION: 0x005F1440 $E42
double DAT_00AC1318 = DAT_007E1720 * DAT_007EEFD0;

// FUNCTION: 0x005F1460 $E45
double DAT_00AC12F0 = DAT_007E1728 * DAT_007EEFD0;

// FUNCTION: 0x005F1480 $E48
double DAT_00AC1270 = DAT_007E1730 * DAT_007EEFD0;

// FUNCTION: 0x005F14A0 $E51
int DAT_00AC1340 = (int)(FUN_004CAD50(DAT_00AC1270 - DAT_00AC12F0) * DAT_007E1738 * DAT_00AC12E0);

// FUNCTION: 0x005F14E0 $E54
double DAT_00AC12F8 = FUN_004CADE0(DAT_00AC1340 * DAT_007E1740);

// FUNCTION: 0x005F1510 $E57
double DAT_00AC1310 = FUN_004CADE0((DAT_00AC1340 * 2) / DAT_00AC12E0);

// FUNCTION: 0x005F1540 $E60
int DAT_00AC1334 = (int)(DAT_00AC1340 * 4 + DAT_007E1738);

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x005F1570 $E63
ShortPair DAT_00AC12D8;

// FUNCTION: 0x005F1580 $E66
IntTriple DAT_00AC1300;

// FUNCTION: 0x005F15C0
NullModemClass::NullModemClass (int numsend, int numreceive, int maxlen,
	unsigned short magicnum) :
	BASECLASS(),
	NumSend(numsend),
	NumReceive(numreceive),
	MaxLen(maxlen),
	MagicNum(magicnum),
	ModemGuardTime(60),
	RetryDelta(60)
{
	PortHandle = INVALID_HANDLE_VALUE;

	Connection = NULL;

	NumSend = numsend;
	NumReceive = numreceive;
	MaxLen = maxlen;
	MagicNum = magicnum;

	RXBuf = 0;
	BuildBuf = 0;

	EchoSize = 500;
	EchoBuf = 0;

	OldIRQPri = -1;

	ModemVerboseOn = false;
	ModemEchoOn = false;
	ModemWaitCarrier = 3000;
	ModemHangupDelay = 1200;
	ModemGuardTime = 60;
	ModemEscapeCode = '+';

	SendOverflows = 0;
	ReceiveOverflows = 0;
	CRCErrors = 0;

	NumConnections = 0;

	RetryDelta = TIMER_SECOND;
	MaxRetries = -1;
	Timeout = 20 * TIMER_SECOND;

	ModemRegistry = NULL;
}

// FUNCTION: 0x005F1650
NullModemClass::~NullModemClass (void)
{
	if (ModemRegistry != NULL) {
		delete ModemRegistry;
	}
	Delete_Connection();

	Shutdown();

	if (SerialPort != NULL) {
		delete SerialPort;
		SerialPort = NULL;
	}
}

// FUNCTION: 0x005F16E0
int NullModemClass::Init (int port, int irq, char *dev_name, int baud, char parity, int wordlen, int stopbits)
{
	DebugString("NullModemClass::Init\n");

	int com;

	irq = irq;

	if (SerialPort != NULL) {
		PortHandle = SerialPort->Get_Port_Handle();
	} else {
		PortHandle = INVALID_HANDLE_VALUE;
	}

	if (!Connection) {
		DebugString("Creating connection class\n");
		Connection = new NullModemConnClass (NumSend, NumReceive, MaxLen,
			MagicNum);

		Connection->Set_Retry_Delta (RetryDelta);
		Connection->Set_Max_Retries (MaxRetries);
		Connection->Set_TimeOut (Timeout);

		RXSize = Connection->Actual_Max_Packet() * NumReceive;
		RXBuf = new char [RXSize];

		BuildBuf = new char [MaxLen];

		EchoBuf = new char [ EchoSize ];
	} else {
		DebugString("Connection already exists\n");
	}

	RXCount = 0;
	EchoCount = 0;

	switch (port) {
		case 0x3f8:
			com = WinModemClass::COM1;
			break;

		case 0x2f8:
			com = WinModemClass::COM2;
			break;

		case 0x3e8:
			com = WinModemClass::COM3;
			break;

		case 0x2e8:
			com = WinModemClass::COM4;
			break;

		default:
			com = WinModemClass::COM5;
			break;
	}

	int	i;

	if (!SerialPort) {
		SerialPort = new WinModemClass;
	}

	static char com_ids[9][5]={
		"COM1",
		"COM2",
		"COM3",
		"COM4",
		"COM5",
		"COM6",
		"COM7",
		"COM8",
		"COM9"
	};

	char *device;

	switch ( port ) {
		case 0x3f8:
			device = com_ids[0];
			break;

		case 0x2f8:
			device = com_ids[1];
			break;

		case 0x3e8:
			device = com_ids[2];
			break;

		case 0x2e8:
			device = com_ids[3];
			break;

		case 1:
			device = dev_name;

			if (ModemRegistry){
				delete ModemRegistry;
				ModemRegistry = NULL;
			}
			for ( i=0 ; i<10 ; i++ ){
				ModemRegistry = new ModemRegistryEntryClass (i);
				if (ModemRegistry->Get_Modem_Name()){
					if (!strcmp (dev_name, ModemRegistry->Get_Modem_Name() )){
						device = ModemRegistry->Get_Modem_Device_Name();
						break;
					}
				}
				delete ModemRegistry;
				ModemRegistry = NULL;
			}
			break;

		default:
			device = NULL;
	}

	PortHandle = SerialPort->Serial_Port_Open (device, baud, parity, wordlen, stopbits);
	if (PortHandle == INVALID_HANDLE_VALUE) {
		return(false);
	}

	DebugString("Ready to initialise connection\n");

	Connection->Init (PortHandle);

	NumConnections = 1;

	return(true);
}

// FUNCTION: 0x005F1960
int NullModemClass::Delete_Connection( void )
{
	if (Connection) {
		delete Connection;
		Connection = NULL;
	}

	if (RXBuf) {
		delete [] RXBuf;
		RXBuf = NULL;
	}

	if (BuildBuf) {
		delete [] BuildBuf;
		BuildBuf = NULL;
	}

	if (EchoBuf) {
		delete [] EchoBuf;
		EchoBuf = NULL;
	}

	NumConnections = 0;

	return( true );
}

// FUNCTION: 0x005F19C0
int NullModemClass::Init_Send_Queue( void )
{
	if ( Connection ) {
		Connection->Queue->Init_Send_Queue();
	}

	return(true);
}

// FUNCTION: 0x005F19E0
DetectPortType NullModemClass::Detect_Port( SerialSettingsType *settings )
{
	static char com_ids[9][5]={
		"COM1",
		"COM2",
		"COM3",
		"COM4",
		"COM5",
		"COM6",
		"COM7",
		"COM8",
		"COM9"
	};

	int	i;

	if (!SerialPort) {
		SerialPort = new WinModemClass;
	}else{
		SerialPort->Serial_Port_Close();
	}

	int baud = settings->Baud;

	char *device;

	switch ( settings->Port ) {
		case 0x3f8:
			device = com_ids[0];
			break;

		case 0x2f8:
			device = com_ids[1];
			break;

		case 0x3e8:
			device = com_ids[2];
			break;

		case 0x2e8:
			device = com_ids[3];
			break;

		case 1:
			device = settings->ModemName;

			if (ModemRegistry){
				delete ModemRegistry;
				ModemRegistry = NULL;
			}
			for ( i=0 ; i<10 ; i++){
				ModemRegistry = new ModemRegistryEntryClass (i);
				if (ModemRegistry->Get_Modem_Name()){
					if (!strcmp (device, ModemRegistry->Get_Modem_Name() )){
						device = ModemRegistry->Get_Modem_Device_Name();
						break;
					}
				}
				delete ModemRegistry;
				ModemRegistry = NULL;
			}
			break;

		default:
			return(PORT_INVALID);
	}

	HANDLE porthandle = SerialPort->Serial_Port_Open (device, baud, 0, 8, 1);

	if (porthandle == INVALID_HANDLE_VALUE){
		return(PORT_INVALID);
	}

	SerialPort->Serial_Port_Close();
	return(PORT_VALID);
}

// FUNCTION: 0x005F1B70
void NullModemClass::Shutdown ( void )
{
	if (PortHandle != INVALID_HANDLE_VALUE && SerialPort) {
		SerialPort->Serial_Port_Close();
		delete SerialPort;
		SerialPort = NULL;
		PortHandle = INVALID_HANDLE_VALUE;
		Delete_Connection();
	}
}

// FUNCTION: 0x005F1BC0
void NullModemClass::Set_Timing (unsigned int retrydelta,
	unsigned int maxretries, unsigned int timeout, bool)
{
	RetryDelta = retrydelta;
	MaxRetries = maxretries;
	Timeout = timeout;

	Connection->Set_Retry_Delta (RetryDelta);
	Connection->Set_Max_Retries (MaxRetries);
	Connection->Set_TimeOut (Timeout);
}

// FUNCTION: 0x005F1C30
int NullModemClass::Get_Message (void *buf, int *buflen)
{
	if (NumConnections == 0) {
		return( false );
	}
	return( Connection->Get_Packet( buf, buflen ) );
}

// FUNCTION: 0x005F1C60
int NullModemClass::Service (void)
{
	int pos;
	int i;
	unsigned short length;
	SerialHeaderType *header;
	SerialCRCType *crc;
	char moredata = 0;

	if (NumConnections == 0) {
		return( false );
	}

	RXCount += SerialPort->Read_From_Serial_Port((unsigned char*)(RXBuf + RXCount), int(RXSize - RXCount) );

	if ( RXCount < (PACKET_SERIAL_OVERHEAD_SIZE + 1) ) {
		return( Connection->Service() );
	}

	pos = -1;
	for (i = 0; i <= RXCount - (int)sizeof( short ); i++) {
		if ( *((unsigned short *)(RXBuf + i)) == PACKET_SERIAL_START ) {
			pos = i;
			break;
		}
	}

	if (pos==-1) {
		memmove (RXBuf, RXBuf + i, sizeof( short ) - 1);
		RXCount = sizeof( short ) - 1;
		return( Connection->Service() );
	}

	if ( (RXCount - pos) < sizeof( SerialHeaderType ) ) {
		memmove (RXBuf, RXBuf + pos, RXCount - pos);
		RXCount -= pos;
		return(Connection->Service());
	}

	header = (SerialHeaderType *)(RXBuf + pos);

	if ( header->MagicNumber2 != PACKET_SERIAL_VERIFY ) {
		pos += sizeof ( short );
		memmove (RXBuf, RXBuf + pos, RXCount - pos);
		RXCount -= pos;
		return(Connection->Service());
	}

	length = header->Length;

	if (length > MaxLen) {
#if (CONN_DEBUG)
		printf( "length too lonnng\n" );
#endif

		DebugString("Serial packet length too long by %d bytes\n", length - MaxLen);

		pos += sizeof ( short );
		memmove (RXBuf, RXBuf + pos, RXCount - pos);
		RXCount -= pos;
		return(Connection->Service());
	}

	if ( (pos + length + PACKET_SERIAL_OVERHEAD_SIZE) > (unsigned)RXCount) {
		if ( moredata ) {
		}

		if (pos) {
			memmove (RXBuf, RXBuf + pos, RXCount - pos);
			RXCount -= pos;
		}
		return(Connection->Service());
	}

	crc = (SerialCRCType *)(RXBuf + pos + sizeof( SerialHeaderType ) + length);
	if (NullModemConnClass::Compute_CRC(RXBuf + pos +
		sizeof( SerialHeaderType ), length) != crc->SerialCRC) {
		CRCErrors++;

#if (CONN_DEBUG)
		printf( "CRC check failed\n" );
#endif

		DebugString("Serial packet failed CRC check\n");

		pos += sizeof ( short );
		memmove (RXBuf, RXBuf + pos, RXCount - pos);
		RXCount -= pos;
		return(Connection->Service());
	}

#if (0)
 Debug_Modem_Dump = true;
 Smart_Printf( "Received tick=%d, Protocol=%d \n",  TickCount, Session.CommProtocol );
 Hex_Dump_Data( (RXBuf + pos), (sizeof( SerialHeaderType) + length) );
 Debug_Modem_Dump = false;
#endif

	if (!Connection->Receive_Packet(RXBuf + pos + sizeof( SerialHeaderType ), length)) {
		ReceiveOverflows++;
	}

#if (0)
	else {
 Debug_Modem_Dump = true;
 Smart_Printf( "Received Packet \n" );
 Debug_Modem_Dump = false;
	}
#endif

#if (0)
	Hex_Dump_Data( (RXBuf + pos), (PACKET_SERIAL_OVERHEAD_SIZE + length) );
#endif

	pos += (PACKET_SERIAL_OVERHEAD_SIZE + length);
	memmove (RXBuf, RXBuf + pos, RXCount - pos);
	RXCount -= pos;

	return(Connection->Service());
}

// FUNCTION: 0x005F1EE0
unsigned int NullModemClass::Response_Time(void)
{
	if (Connection)
		return( Connection->Queue->Avg_Response_Time() );
	else
		return(0);
}

// FUNCTION: 0x005F1F00
void NullModemClass::Reset_Response_Time(bool zero)
{
	if (Connection)
		Connection->Queue->Reset_Response_Time(true);
}

// FUNCTION: 0x005F1F20
void * NullModemClass::Oldest_Send(void)
{
	int i;
	SendQueueType *send_entry;
	CommHeaderType *packet;
	void *buf = NULL;

	for (i = 0; i < Connection->Queue->Num_Send(); i++) {
		send_entry = Connection->Queue->Get_Send(i);
		if (send_entry) {
			packet = (CommHeaderType *)send_entry->Buffer;
			if (packet->Code == ConnectionClass::PACKET_DATA_ACK && send_entry->IsACK == 0) {
				buf = send_entry->Buffer;
				break;
			}
		}
	}

	return(buf);
}

// FUNCTION: 0x005F1F70
void NullModemClass::Configure_Debug(int, int type_offset, int type_size,
	char **names, int namestart, int namecount)
{
	if (Connection)
		Connection->Queue->Configure_Debug (type_offset, type_size, names,
			namestart, namecount);
}

extern char ModemRXString[80];

namespace OwnerDraw {
	HWND Custom_Message_Box(wchar_t const * btn1txt, wchar_t const * btn2txt, bool * cancelled);
}

// FUNCTION: 0x005F2950
DialStatusType NullModemClass::Dial_Modem(char * string, DialMethodType method, bool reconnect)
{
	bool process = true;
	int delay;
	DialStatusType dialstatus = DIAL_ERROR;
	wchar_t const * text_buffer;
	char buffer[80*3];

	DebugString("Dial_Modem\n");

	if (reconnect == true) {
#line 1409 "D:\\ra2mdpost\\NullMgr.CPP"
		text_buffer = StringTable::LoadString("TXT_MODEM_CONNERR_REDIALING", NULL, __FILE__, __LINE__);
	} else {
#line 1411 "D:\\ra2mdpost\\NullMgr.CPP"
		text_buffer = StringTable::LoadString("TXT_DIALING", NULL, __FILE__, __LINE__);
	}
	ModemDialogIsOpen = false;
#line 1415 "D:\\ra2mdpost\\NullMgr.CPP"
	HWND dialog = OwnerDraw::Custom_Message_Box(text_buffer, StringTable::LoadString("TXT_CANCEL", NULL, __FILE__, __LINE__), &ModemDialogIsOpen);

	if (dialog) {
		OwnerDraw::Display_Dialog(dialog);

		DebugString("Setting modem dial type\n");
		SerialPort->Set_Modem_Dial_Type((WinCommDialMethodType) method);

		DebugString("Clearing results buffer\n");
		SerialPort->Get_Modem_Result(TIMER_SECOND, buffer, 81, -1);

		DebugString("Calling dial function\n");
		SerialPort->Dial_Modem(string);

		SerialPort->Set_Abort_Function((int(*)(void))Abort_Modem);
		ModemDialogIsOpen = false;

		delay = ModemWaitCarrier;
		DebugString("Waiting for dial result\n");
		while (process == true) {

			if (OwnerDraw::Dialog_Message_Handler() != 1 && !ModemDialogIsOpen) {
				Title_Screen_Restore();
				memset(buffer, 0, sizeof(buffer));
				delay = SerialPort->Get_Modem_Result(delay, buffer, 81, TIMER_SECOND / 4);

				if (delay == -1000) {
					dialstatus = DIAL_CANCELED;
					if (SerialPort != NULL) {
						char escape[3] = "\r";
						SerialPort->Write_To_Serial_Port((unsigned char*)escape, sizeof(escape)-1);
					}
					DebugString("Dialling cancelled\n");
					process = false;
					break;
				}

				strupr(buffer);
				if (strlen(buffer)) {
					DebugString("From Modem: %s\n", buffer);
				}

				if ( strstr( buffer, "CON" ) != 0 ) {
					memset (ModemRXString, 0, 80);
					strncpy (ModemRXString, buffer, 79);
					dialstatus = DIAL_CONNECTED;
					DebugString("Dial result is %s\n", buffer);
					process = false;
				} else if ( strncmp( buffer, "BUSY", 4 ) == 0 ) {
					dialstatus = DIAL_BUSY;
					DebugString("Dial result is %s\n", buffer);
					process = false;
				} else if ( strncmp( buffer, "NO C", 4 ) == 0 ) {
					dialstatus = DIAL_NO_CARRIER;
					DebugString("Dial result is %s\n", buffer);
					process = false;
				} else if ( strncmp( buffer, "NO D", 4 ) == 0 ) {
					dialstatus = DIAL_NO_DIAL_TONE;
					DebugString("Dial result is %s\n", buffer);
					process = false;
				} else if ( strncmp( buffer, "ERRO", 4 ) == 0 ) {
					dialstatus = DIAL_ERROR;
					DebugString("Dial result is %s\n", buffer);
					process = false;
				}

				if (delay <= 0) {
					DebugString("NullModemClass::Dial_Modem has timed out\n");
					if (strlen(buffer)) {
						DebugString("Dial result is %s\n", buffer);
					}
					if (SerialPort != NULL) {
						char escape[3] = "\r";
						SerialPort->Write_To_Serial_Port((unsigned char*)escape, sizeof(escape)-1);
					}
					dialstatus = DIAL_TIMEOUT;
					process = false;
					break;
				}
			} else {
				dialstatus = DIAL_CANCELED;
				process = false;
				if (SerialPort != NULL) {
					char escape[3] = "\r";
					SerialPort->Write_To_Serial_Port((unsigned char *)escape, sizeof(escape)-1);
				}
				DebugString("Dialling cancelled\n");
				break;
			}
		}

		SerialPort->Set_Abort_Function(NULL);
		ModemDialogIsOpen = false;

		OwnerDraw::End_Dialog(dialog);
	}

	return( dialstatus );
}

// FUNCTION: 0x005F3170
bool NullModemClass::Hangup_Modem( void )
{
	int status;
	int delay;
	char buffer[81];
	char escape[4];

	if (PortHandle == INVALID_HANDLE_VALUE || SerialPort == NULL) {
		return(false);
	}

	Session.ModemService = false;

	status = Send_Modem_Command( "AT", '\r', buffer, 81, DEFAULT_TIMEOUT, 1 );

	if (status == MODEM_CMD_OK) {
		Session.ModemService = true;
		return( true );
	}

	SerialPort->Set_Serial_DTR(FALSE);
	Delay(3200/TIMER_SECOND);
	SerialPort->Set_Serial_DTR(TRUE);

	status = Send_Modem_Command( "AT", '\r', buffer, 81, DEFAULT_TIMEOUT, 1 );

	if (status == MODEM_CMD_OK) {
		Session.ModemService = true;
		return( true );
	}

	delay = ModemGuardTime;
	while ( delay > 0 ) {
		delay = SerialPort->Get_Modem_Result(delay, buffer, 81, -1);
	}

	escape[0] = ModemEscapeCode;
	escape[1] = ModemEscapeCode;
	escape[2] = ModemEscapeCode;
	escape[3] = 0;

	SerialPort->Write_To_Serial_Port((unsigned char*)escape, 3);

	delay = ModemGuardTime;
	while ( delay > 0 ) {
		delay = SerialPort->Get_Modem_Result(delay, buffer, 81, -1);

		if ( strncmp( buffer, "OK", 2 ) == 0 ) {
			break;
		}
	}

	status = Send_Modem_Command( "ATH", '\r', buffer, 81, ModemHangupDelay, 1 );

	if (status == MODEM_CMD_OK) {
	} else {
		Session.ModemService = true;
		return( false );
	}

	status = Send_Modem_Command( "ATZ", '\r', buffer, 81, TIMER_SECOND, 1 );

	if (status != MODEM_CMD_OK) {
		Session.ModemService = true;
		return( false );
	}

	Session.ModemService = true;
	return( true );
}

// FUNCTION: 0x005F3320
void NullModemClass::Setup_Modem_Echo( void ( *func )( char c) )
{
	SerialPort->Set_Echo_Function(func);
}

// FUNCTION: 0x005F3350
void NullModemClass::Print_EchoBuf( void )
{
	for (int i = 0; (unsigned)i < strlen(NullModem.EchoBuf); i++) {
		if (NullModem.EchoBuf[i] == '\r') {
			NullModem.EchoBuf[i] = 1;
		} else {
			if (NullModem.EchoBuf[i] == '\n') {
				NullModem.EchoBuf[i] = 2;
			}
		}
	}
}

// FUNCTION: 0x005F33A0
void NullModemClass::Reset_EchoBuf( void )
{
	*EchoBuf = 0;
	EchoCount = 0;
}

// FUNCTION: 0x005F33B0
int NullModemClass::Abort_Modem( PORT * )
{
	Call_Back();

	Windows_Message_Handler();

	unsigned int key = Keyboard->Check();

	if (key != KN_NONE) {
		Keyboard->Clear();
		switch ( (int)key ) {
			case (KN_ESC):
				ModemDialogIsOpen = true;
				return(-1000);
		}
	}
	return(ModemDialogIsOpen != 0 ? -1000 : 0);
}

// FUNCTION: 0x005F3400
void NullModemClass::Setup_Abort_Modem( void )
{
	SerialPort->Set_Abort_Function((int(*)(void))Abort_Modem);
	ModemDialogIsOpen = false;
}

// FUNCTION: 0x005F3420
void NullModemClass::Remove_Abort_Modem( void )
{
	SerialPort->Set_Abort_Function(NULL);
	ModemDialogIsOpen = false;
}

// FUNCTION: 0x005F3450
int NullModemClass::Get_Modem_Status( void )
{
	int modemstatus;
	int status;
	char buffer[81];

	modemstatus = SerialPort->Get_Modem_Status();

	status = Send_Modem_Command( "AT", '\r', buffer, 81, DEFAULT_TIMEOUT, 1 );

	if (status == MODEM_CMD_OK) {
		modemstatus &= (~WinModemClass::CD_SET);
	}

	return( modemstatus );
}

// FUNCTION: 0x005F34A0
int NullModemClass::Send_Modem_Command( char const *command, char terminator, char *buffer, int buflen, int delay, int retries )
{
	return(SerialPort->Send_Command_To_Modem(command, terminator, buffer, buflen, delay, retries, -1));
}

// FUNCTION: 0x005F34D0
int NullModemClass::Verify_And_Convert_To_Int( char *buffer )
{
	int value = 0;
	int len = strlen(buffer);

	for (int i = 0; i < len; i++) {
		if ( !isdigit( *(buffer + i) ) ) {
			value = -1;
			break;
		}
	}

	if (value == 0) {
		value = atoi( buffer );
	}

	return( value );
}
