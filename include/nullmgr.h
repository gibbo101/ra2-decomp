// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "connmgr.h"
#include "nullconn.h"
#include "session.h"
#include "special.h"

class GadgetClass;
class ModemRegistryEntryClass;

class NullModemClass : public ConnManClass
{
		typedef ConnManClass BASECLASS;

	public:
		enum SendModemEnum {
			MODEM_CMD_TIMEOUT = 0,
			MODEM_CMD_ERROR,
			MODEM_CMD_OK,
			MODEM_CMD_0,
		};

		char *BuildBuf;
		int MaxLen;

		char *EchoBuf;
		int EchoSize;
		int EchoCount;

		int OldIRQPri;

		int ModemVerboseOn;
		int ModemEchoOn;
		int ModemWaitCarrier;
		int ModemHangupDelay;
		int ModemGuardTime;
		char ModemEscapeCode;

		static void (*OrigAbortModemFunc)(int);
		static KeyNumType Input;
		static GadgetClass *Commands;

		NullModemClass (int numsend, int numreceive, int maxlen, unsigned short magicnum);
		virtual ~NullModemClass () override;

		int Init( int port, int irq, char *dev_name, int baud, char parity, int wordlength, int stopbits );
		int Delete_Connection( void );
		virtual int Num_Connections(void) override;
		virtual int Connection_ID(int ) override {return(0);}
		virtual int Connection_Index(int ) override {return(0);}
		int Init_Send_Queue( void );
		void Shutdown( void );

		virtual void Set_Timing (unsigned int retrydelta,
			unsigned int maxretries, unsigned int timeout, bool = true) override;
		virtual void Set_External_Timing (unsigned int retrydelta,
			unsigned int maxretries, unsigned int timeout) override { Set_Timing(retrydelta, maxretries, timeout, true); }

		int Send_Message (void *buf, int buflen, int ack_req = 1);
		int Get_Message (void *buf, int *buflen);

		virtual int Send_Private_Message (void *buf, int buflen,
			int ack_req = 1, int = CONNECTION_NONE) override
			{return(Send_Message(buf,buflen,ack_req));}
		virtual int Get_Private_Message (void *buf, int *buflen, int *) override
			{return(Get_Message(buf,buflen));}

		virtual int Service (void) override;

		int Num_Send(void);
		int Num_Receive(void);
		virtual unsigned int Response_Time(void) override;
		virtual void Reset_Response_Time(bool zero) override;
		void * Oldest_Send(void);
		virtual void Configure_Debug(int index, int type_offset, int type_size,
			char **names, int namestart, int namecount) override;
#ifdef _DEBUG
		virtual void Mono_Debug_Print(int index, int refresh = 0);
#endif

		virtual int Global_Num_Send(void) override {return(Num_Send());}
		virtual int Global_Num_Receive(void) override {return(Num_Receive());}
		virtual int Private_Num_Send(int = CONNECTION_NONE) override
			{return(Num_Send());}
		virtual int Private_Num_Receive(int = CONNECTION_NONE) override
			{return(Num_Receive());}

		DetectPortType Detect_Port( SerialSettingsType *settings );
		bool Detect_Modem( SerialSettingsType *settings, bool reconnect = 0 );
		DialStatusType Dial_Modem(char *string, DialMethodType method, bool reconnect = 0);
		DialStatusType Answer_Modem(bool reconnect = 0);
		bool Hangup_Modem(void);
		void Setup_Modem_Echo(void (*func)(char c));
		void Remove_Modem_Echo(void);
		void Print_EchoBuf(void);
		void Reset_EchoBuf(void);
		static int Abort_Modem(PORT *);
		void Setup_Abort_Modem(void);
		void Remove_Abort_Modem(void);

		int Change_IRQ_Priority(int irq);
		int Get_Modem_Status(void);
		int Send_Modem_Command( char const *command, char terminator, char *buffer, int buflen, int delay, int retries );
		int Verify_And_Convert_To_Int( char *buffer );

	private:

		ModemRegistryEntryClass *ModemRegistry;

		NullModemConnClass *Connection;
		int NumConnections;

		HANDLE	PortHandle;

		int NumSend;
		int NumReceive;
		unsigned short MagicNum;

		char *RXBuf;
		int RXSize;
		int RXCount;

		unsigned int RetryDelta;
		unsigned int MaxRetries;
		unsigned int Timeout;

		int SendOverflows;
		int ReceiveOverflows;
		int CRCErrors;
};

extern NullModemClass NullModem;

extern bool ModemDialogIsOpen;

