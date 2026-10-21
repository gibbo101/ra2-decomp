// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "wincomm.h"
#include "dbgprint.h"
#include "globals.h"
#include "stimer.h"
#include "timer.h"
#include <cstdio>

// FUNCTION: GAMEMD 0x00774120
WinModemClass::WinModemClass(void)
{
	SerialBuffer = new unsigned char [SIZE_OF_WINDOWS_SERIAL_BUFFER];

	SerialBufferReadPtr	= 0;
	SerialBufferWritePtr	= 0;

	WaitingForSerialCharRead	= FALSE;
	WaitingForSerialCharWrite	= FALSE;

	AbortFunction = NULL;
	EchoFunction  = NULL;

	FramingErrors=0;
	IOErrors=0;
	BufferOverruns=0;
	InBufferOverflows=0;
	ParityErrors=0;
	OutBufferOverflows=0;

	PortHandle = INVALID_HANDLE_VALUE;
}

// FUNCTION: GAMEMD 0x00774170
WinModemClass::~WinModemClass(void)
{
	if (PortHandle != INVALID_HANDLE_VALUE){
		Serial_Port_Close();
	}

	if (SerialBuffer){
		delete [] SerialBuffer;
	}
}

// FUNCTION: GAMEMD 0x007741C0
HKEY Get_Registry_Sub_Key (HKEY base_key, char const *search_key, bool close)
{
	char		class_string[1024];
	DWORD		string_size = sizeof(class_string);
	DWORD		num_sub_keys;
	DWORD		longest_sub_key_name;
	DWORD		longest_class_string;
	DWORD		num_value_entries;
	DWORD		longest_value_name_length;
	DWORD		longest_value_data_length;
	DWORD		security_descriptor_length;
	FILETIME	last_write_time;
	HKEY		result_key;
	DWORD		sub_key_buffer_size;

	char		*sub_key_buffer;
	char		*sub_key_class;

	if (RegQueryInfoKey (base_key,
							&class_string[0],
							&string_size,
							NULL,
							&num_sub_keys,
							&longest_sub_key_name,
							&longest_class_string,
							&num_value_entries,
							&longest_value_name_length,
							&longest_value_data_length,
							&security_descriptor_length,
							&last_write_time) != ERROR_SUCCESS) return(0);

	sub_key_buffer_size = longest_sub_key_name+16;
	sub_key_buffer = new char [sub_key_buffer_size];
	sub_key_class	= new char [longest_class_string+1];

	for (DWORD key_num=0 ; key_num<num_sub_keys ; key_num++){
		*sub_key_buffer = 0;
		longest_sub_key_name = sub_key_buffer_size;
		RegEnumKeyEx(base_key,
						key_num,
						sub_key_buffer,
						&longest_sub_key_name,
						NULL,
						sub_key_class,
						&longest_class_string,
						&last_write_time);

		if (!stricmp(search_key, sub_key_buffer)){
			if (RegOpenKeyEx(	base_key,
									sub_key_buffer,
									NULL,
									KEY_READ,
									&result_key) == ERROR_SUCCESS){
				if (close){
					RegCloseKey(base_key);
				}
				delete [] sub_key_buffer;
				delete [] sub_key_class;
				return(result_key);
			}
		}
	}

	if (close){
		RegCloseKey(base_key);
	}
	delete [] sub_key_buffer;
	delete [] sub_key_class;
	return(0);
}

// FUNCTION: GAMEMD 0x00774310
bool Get_Modem_Name_From_Registry(char *buffer, int buffer_len)
{
	HKEY	key;
	char	modem_name[256];
	DWORD	modem_name_size = sizeof(modem_name);

	key = Get_Registry_Sub_Key (HKEY_LOCAL_MACHINE, "Enum", FALSE);
	if (!key) return(FALSE);

	key = Get_Registry_Sub_Key (key, "Root", TRUE);
	if (!key) return(FALSE);

	key = Get_Registry_Sub_Key (key, "Modem", TRUE);
	if (!key) return(FALSE);

	key = Get_Registry_Sub_Key (key, "0000", TRUE);
	if (!key) return(FALSE);

	if (RegQueryValueEx(key, "FriendlyName", NULL, NULL, (unsigned char*)&modem_name[0], &modem_name_size) != ERROR_SUCCESS){
		RegCloseKey(key);
		return(FALSE);
	}

	RegCloseKey(key);
	memcpy (buffer, modem_name, MIN(buffer_len, (int)modem_name_size));
	return(TRUE);
}

// FUNCTION: GAMEMD 0x00774410
HANDLE WinModemClass::Serial_Port_Open (char *device_name, int baud, int parity, int wordlen, int stopbits)
{
	HANDLE			com_handle;
	DCB				device_control;
	COMMTIMEOUTS	timeouts;
#if (0)
	static char com_ids[8][5]={
		"COM1",
		"COM2",
		"COM3",
		"COM4",
		"COM5",
		"COM6",
		"COM7",
		"COM8"
	};
#endif

	char devname[266]={"\\\\.\\"};
	strcat (devname, device_name);

	Serial_Port_Close();

	DebugString("Opening serial port %s\n", devname);
	com_handle = CreateFile (devname, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

	if (com_handle == INVALID_HANDLE_VALUE){
		DebugString("Failed to get valid port handle. Error: %s\n", Last_Error_Text());
		return(com_handle);
	}

	DebugString("Port opened OK\n");
	PortHandle = com_handle;

	SetupComm(com_handle, SIZE_OF_WINDOWS_SERIAL_BUFFER, SIZE_OF_WINDOWS_SERIAL_BUFFER);

	PurgeComm (PortHandle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

	ReadOverlap.Internal 		= 0;
	ReadOverlap.InternalHigh	= 0;
	ReadOverlap.Offset			= 0;
	ReadOverlap.OffsetHigh		= 0;
	ReadOverlap.hEvent			= CreateEvent (NULL, TRUE, TRUE, NULL);

	WriteOverlap.Internal 		= 0;
	WriteOverlap.InternalHigh	= 0;
	WriteOverlap.Offset			= 0;
	WriteOverlap.OffsetHigh		= 0;
	WriteOverlap.hEvent			= CreateEvent (NULL, TRUE, TRUE, NULL);

	if (GetCommState (com_handle , &device_control)){
		device_control.BaudRate = baud;
		device_control.Parity 	= parity;
		device_control.ByteSize = (char)wordlen;
		device_control.StopBits	= (char)(stopbits-1);

		device_control.fBinary			= TRUE;
		device_control.fParity			= FALSE;
		device_control.fOutxCtsFlow 	= TRUE;
		device_control.fRtsControl		= RTS_CONTROL_HANDSHAKE;
		device_control.fErrorChar		= FALSE;
		device_control.fOutxDsrFlow	= FALSE;
		device_control.fDtrControl		= DTR_CONTROL_ENABLE;
		device_control.fOutX				= FALSE;
		device_control.fInX				= FALSE;
		device_control.fAbortOnError	= FALSE;
#if 0

		if (!flowcontrol){
			device_control.fOutxCtsFlow 	= FALSE;
			device_control.fRtsControl		= RTS_CONTROL_DISABLE;
		}
#endif

		if ( !SetCommState (com_handle , &device_control)){
			DebugString("SetCommState returned error %s.\n", Last_Error_Text());
		}else{
			DeviceControl = device_control;
		}
	}else{
		DebugString("GetCommState returned error code %s.\n", Last_Error_Text());
	}

	timeouts.ReadIntervalTimeout 			= 1000;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant	= 3000;
	timeouts.WriteTotalTimeoutMultiplier= 500;
	timeouts.WriteTotalTimeoutConstant	= 1000;

	if ( !SetCommTimeouts(com_handle, &timeouts) ){
		DebugString("SetCommTimeouts returned error code %s.\n", Last_Error_Text());
	}

	Set_Serial_DTR(true);

	DWORD error;
	COMSTAT status;
	if ( ClearCommError(PortHandle, &error, &status) ) {
		InQueue = status.cbInQue;
		OutQueue = status.cbOutQue;
		if (error != 0) {
			Debug_Status(error, status);
		}
	}

	return(com_handle);
}

// FUNCTION: GAMEMD 0x00774670
bool WinModemClass::Enter_Failsafe_Mode(void)
{
	DWORD error;
	DWORD mask;
	COMSTAT status;
	DCB device_control;

	DebugString("WinModemClass - Entering failsafe mode\n");

	PurgeComm(PortHandle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

	ResetEvent(ReadOverlap.hEvent);
	ResetEvent(WriteOverlap.hEvent);

	WaitingForSerialCharRead = false;
	WaitingForSerialCharWrite = false;

	if (GetCommState(PortHandle, &device_control)) {
		device_control.BaudRate = 9600;
		device_control.Parity = 0;
		device_control.ByteSize = 8;
		device_control.StopBits = 0;

		device_control.fBinary			= TRUE;
		device_control.fParity			= FALSE;
		device_control.fOutxCtsFlow		= FALSE;
		device_control.fRtsControl		= RTS_CONTROL_ENABLE;
		device_control.fErrorChar		= FALSE;
		device_control.fOutxDsrFlow		= FALSE;
		device_control.fDtrControl		= DTR_CONTROL_ENABLE;
		device_control.fOutX			= FALSE;
		device_control.fInX				= FALSE;
		device_control.fAbortOnError	= FALSE;

		if (!SetCommState(PortHandle, &device_control)) {
			DebugString("SetCommState returned error %s.\n", Last_Error_Text());
			return(false);
		}
	} else {
		DebugString("GetCommState returned error %s.\n", Last_Error_Text());
	}

	GetCommMask(PortHandle, &mask);

	Set_Serial_DTR(true);

	Get_Modem_Status();

	if (!EscapeCommFunction(PortHandle, SETRTS)) {
		DebugString("Enter_Failsafe_Mode - EscapeCommFunction failed to set state of RTS line. Error code %d, %s\n", GetLastError(), Last_Error_Text());
	}

	if (ClearCommError(PortHandle, &error, &status)) {
		InQueue = status.cbInQue;
		OutQueue = status.cbOutQue;
		if ( error != 0) {
			Debug_Status(error, status);
		}
	}

	return(true);
}

// FUNCTION: GAMEMD 0x007747C0
void WinModemClass::Leave_Failsafe_Mode(void)
{
	DWORD error;
	COMSTAT status;

	DebugString("WinModemClass - Leaving failsafe mode\n");

	PurgeComm(PortHandle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

	ResetEvent(ReadOverlap.hEvent);
	ResetEvent(WriteOverlap.hEvent);

	WaitingForSerialCharRead = false;
	WaitingForSerialCharWrite = false;

	if (!SetCommState(PortHandle, &DeviceControl)) {
		DebugString("SetCommState returned error %s.\n", Last_Error_Text());
	}

	if (!EscapeCommFunction(PortHandle, SETRTS)) {
		DebugString("Leave_Failsafe_Mode - EscapeCommFunction failed to set state of RTS line. Error code %d, %s\n", GetLastError(), Last_Error_Text());
	}

	if (ClearCommError(PortHandle, &error, &status)) {
		InQueue = status.cbInQue;
		OutQueue = status.cbOutQue;
		if (error != 0) {
			Debug_Status(error, status);
		}
	}

	Set_Serial_DTR(true);
}

// FUNCTION: GAMEMD 0x007748A0
void WinModemClass::Set_Modem_Dial_Type(WinCommDialMethodType method)
{
	DialingMethod = method;
}

// FUNCTION: GAMEMD 0x007748B0
unsigned WinModemClass::Get_Modem_Status(void)
{
	DWORD		modem_stat = 0;
	unsigned	int return_stat = 0;

	GetCommModemStatus(PortHandle, &modem_stat);

	if (MS_CTS_ON  & modem_stat) return_stat |= CTS_SET;
	if (MS_DSR_ON  & modem_stat) return_stat |= DSR_SET;
	if (MS_RING_ON & modem_stat) return_stat |= RI_SET;
	if (MS_RLSD_ON & modem_stat) return_stat |= CD_SET;

	return(return_stat);
}

// FUNCTION: GAMEMD 0x00774900
void WinModemClass::Set_Serial_DTR(bool state)
{
	if (state){
		if (EscapeCommFunction(PortHandle, SETDTR)) {
			return;
		}
	} else {
		if (EscapeCommFunction(PortHandle, CLRDTR)) {
			return;
		}
	}
	DebugString("Set_Serial_DTR - EscapeCommFunction failed with error code %d, %s\n", GetLastError(), Last_Error_Text());
}

// FUNCTION: GAMEMD 0x00774950
void WinModemClass::Serial_Port_Close (void)
{
	if (PortHandle != INVALID_HANDLE_VALUE){
		DebugString("Closing serial port\n");
		CloseHandle(PortHandle);
		PortHandle = INVALID_HANDLE_VALUE;
	}
}

// FUNCTION: GAMEMD 0x00774980
bool WinModemClass::Read_Serial_Chars (void)
{
	DWORD	bytes_read;
	bool	read_result;
	BOOL	overlap_result;
	DWORD	total_bytes_read=0;
	DWORD	bytes_to_read;
	DWORD	i;

	if (WaitingForSerialCharRead){
		bytes_read = 0;
		overlap_result = GetOverlappedResult(PortHandle, &ReadOverlap, &bytes_read, FALSE);

		if (overlap_result){
			WaitingForSerialCharRead = FALSE;

			if (bytes_read){
				for (i=0 ; i<bytes_read ; i++){
					*(SerialBuffer + SerialBufferWritePtr++) = TempSerialBuffer[i];
					SerialBufferWritePtr &= SIZE_OF_WINDOWS_SERIAL_BUFFER - 1;
				}
				total_bytes_read += bytes_read;
			}
		}else{
			if (GetLastError() == ERROR_IO_INCOMPLETE){
				return(FALSE);
			}
			DebugString("GetOverlappedResult in WinModemClass::Read_Serial_Chars returned error %s\n", Last_Error_Text());
		}
	}

	ResetEvent(ReadOverlap.hEvent);

	DWORD 	error;
	COMSTAT	status;

	bytes_to_read = 1;

	if (ClearCommError(PortHandle, &error,	&status)){
		InQueue 	= status.cbInQue;
		OutQueue	= status.cbOutQue;
		bytes_to_read = status.cbInQue;

		if (error){
			Debug_Status(error, status);
			bytes_to_read = status.cbInQue;
		}
		bytes_to_read = MIN((int)bytes_to_read, SIZE_OF_WINDOWS_SERIAL_BUFFER);
	}

	if (!bytes_to_read) bytes_to_read++;

	do{
		bytes_read = 0;
		read_result = ReadFile(PortHandle ,TempSerialBuffer ,bytes_to_read ,&bytes_read, &ReadOverlap) != 0;

		if (!read_result){
			if (GetLastError() == ERROR_IO_PENDING){
				WaitingForSerialCharRead = TRUE;
			} else {
				DebugString("Reading from port handle failed with error %s\n", Last_Error_Text());
			}

		}else{
			if (bytes_read){
				for (i=0 ; i<bytes_read ; i++){
					*(SerialBuffer + SerialBufferWritePtr++) = TempSerialBuffer[i];
					SerialBufferWritePtr &= SIZE_OF_WINDOWS_SERIAL_BUFFER - 1;
				}
				total_bytes_read += bytes_read;
			}
		bytes_to_read = 1;
		}

	} while (read_result == TRUE);

	return(total_bytes_read != 0);
}

// FUNCTION: GAMEMD 0x00774B30
int WinModemClass::Read_From_Serial_Port (unsigned char *dest_ptr, int buffer_len)
{
	int	bytes_read;
	int	bytes_to_copy = 0;
	unsigned char *original_dest = dest_ptr;

	bytes_read = Read_Serial_Chars();

	if (bytes_read){
		bytes_to_copy = SerialBufferWritePtr - SerialBufferReadPtr;
		if (bytes_to_copy <0 ) bytes_to_copy += SIZE_OF_WINDOWS_SERIAL_BUFFER;
		if (bytes_to_copy>buffer_len) bytes_to_copy = buffer_len;

		for (int i=0 ; i<bytes_to_copy ; i++){
			if (EchoFunction && *(SerialBuffer + SerialBufferReadPtr) !=13 ){
				EchoFunction(*(SerialBuffer + SerialBufferReadPtr));
			}

			*dest_ptr++ = *(SerialBuffer + SerialBufferReadPtr++);
			SerialBufferReadPtr &= SIZE_OF_WINDOWS_SERIAL_BUFFER-1;
		}
	}

#ifdef LOG_MODEM

	if (bytes_read){
		char *outstr = new char[bytes_read+1];
		memcpy (outstr, original_dest, bytes_read);
		outstr[bytes_read] = 0;
		OutputDebugString (outstr);

		int	handle;

		handle = open("COMMLOG.TXT",O_WRONLY | O_CREAT | O_APPEND | O_TEXT);

		if (handle != -1){
			write(handle, original_dest, bytes_read);
			close(handle);
		}
	}
#endif

	return(bytes_to_copy);
}

// FUNCTION: GAMEMD 0x00774BC0
void WinModemClass::Wait_For_Serial_Write(void)
{
	DWORD						bytes_written;
	BOOL						overlap_result;
	BOOL 						wait_send;
	CDTimerClass<SystemTimerClass>	timer;

	if (WaitingForSerialCharWrite){
		timer = (TIMER_SECOND*5);

		do{
			wait_send = FALSE;
			overlap_result = GetOverlappedResult(PortHandle, &WriteOverlap, &bytes_written, FALSE);

			if (!overlap_result){
				if (GetLastError() == ERROR_IO_INCOMPLETE){
					wait_send = TRUE;
				}
			}

		}while (wait_send && timer());

		WaitingForSerialCharWrite = FALSE;
	}

	ResetEvent(WriteOverlap.hEvent);
}

// FUNCTION: GAMEMD 0x00774C70
void WinModemClass::Write_To_Serial_Port (unsigned char *buffer, int length)
{
	DWORD	bytes_written;
	BOOL	write_result;

#ifdef LOG_MODEM

	if (length){
		char *outstr = new char[length+1];
		memcpy (outstr, buffer, length);
		outstr[length] = 0;
		OutputDebugString (outstr);

		int	handle;

		handle = open("COMMLOG.TXT",O_WRONLY | O_CREAT | O_APPEND | O_TEXT);

		if (handle != INVALID_HANDLE_VALUE){
			write(handle, buffer, length);
			close(handle);
		}
	}
#endif

	Wait_For_Serial_Write();

	write_result = WriteFile (PortHandle, buffer, length, &bytes_written, &WriteOverlap);

	if (!write_result){
		if (GetLastError() == ERROR_IO_PENDING){
			WaitingForSerialCharWrite = TRUE;
		} else {
			DebugString("Writing to port handle failed with error %s\n", Last_Error_Text());
		}
	}
}

// FUNCTION: GAMEMD 0x00774CD0
int WinModemClass::Get_Modem_Result(int delay, char *buffer, int buffer_len, int inter_char_delay)
{
	CDTimerClass<SystemTimerClass>	timer;
	CDTimerClass<SystemTimerClass>	utimer;
	int						dest_ptr;
	char						*cr_ptr;
	char						*lf_ptr;
	int						copy_bytes;

#if 0
	char abuffer [128];
	sprintf (abuffer, "Wincomm - delay = %d, buffer = %p, buffer_len = %d.\n", delay, buffer, buffer_len);
#endif

	memset(buffer, 0 ,buffer_len);
	dest_ptr = 0;

	utimer = inter_char_delay;

	do{
		cr_ptr 	= NULL;
		timer = (delay);

		do{
			if (AbortFunction){
				int abort = AbortFunction();
				if (abort != COMMSUCCESS) return(abort);
			}

			if (!GameInFocus){
				return(0);
			}

			copy_bytes = Read_From_Serial_Port((unsigned char*)(buffer + dest_ptr), (int)buffer_len-dest_ptr < 0 ? 0 : (int)buffer_len-dest_ptr);
			if (copy_bytes) {
				dest_ptr += copy_bytes;
				utimer = inter_char_delay;
			}
		} while (timer &&
					dest_ptr < buffer_len &&
					(!strchr (buffer, 13) || (inter_char_delay != -1 && utimer) ) );

		buffer[buffer_len - 1] = 0;

		cr_ptr = strstr(buffer,"AT");
		if (cr_ptr){
			if (*buffer == 'A' && *(buffer+1) == 'T' && strchr(buffer,13)){
				cr_ptr = strchr(buffer,13);
			}
		}

		if (!cr_ptr && (*buffer==13 || *buffer==10)){
			cr_ptr = strchr(buffer,13);
			lf_ptr = strchr(buffer,10);
			if (!cr_ptr || (lf_ptr && lf_ptr < cr_ptr)){
				cr_ptr = strchr(buffer,10);
			}
		}

		if (cr_ptr){
			while (*cr_ptr == 13 || *cr_ptr == 10){
				cr_ptr++;
			}

			if (cr_ptr != buffer){
				copy_bytes = (int)cr_ptr - (int)buffer;
				memmove(buffer, cr_ptr, buffer_len - copy_bytes);
				dest_ptr -= copy_bytes;
			}
		}

	}while (cr_ptr);

	if (strrchr(buffer, 13)){
		*(strrchr(buffer, 13)) = 0;
	}

	return(timer);
}

// FUNCTION: GAMEMD 0x00774F50
void WinModemClass::Dial_Modem(char *dial_number)
{
	char	dial_string[80];

	DebugString("Creating dial string\n");
	strcpy (dial_string, "ATD");
	if (DialingMethod == WC_TOUCH_TONE){
		strcat(dial_string, "T");
		DebugString("Dial type is tone\n");
	}else{
		DebugString("Dial type is pulse\n");
		strcat(dial_string, "P");
	}

	DebugString("Appending dial string\n");
	strcat (dial_string, dial_number);
	DebugString("Appending carriage return\n");
	strcat (dial_string, "\r");

	DebugString("Send to modem: %s\n", dial_string);
	Write_To_Serial_Port ((unsigned char*)dial_string, strlen(dial_string));
	Wait_For_Serial_Write();
}

// FUNCTION: GAMEMD 0x00775110
int WinModemClass::Send_Command_To_Modem(char const *command, char terminator, char *buffer, int buflen, int delay, int retries, int inter_char_delay)
{
	int				times;
	unsigned	char	tmp_string[80];
	char				tmp_buff[80];
	char				term_string[2];
	int				time;

	term_string[0]	= terminator;
	term_string[1]	= 0;

	strcpy((char*)tmp_string, command);
	strcat((char*)tmp_string, term_string);

	unsigned char	nothing_buff[80];
	Read_From_Serial_Port(nothing_buff,80);
	Sleep (100);
	Read_From_Serial_Port(nothing_buff,80);

	for (times = 0 ; times<retries; times++){
		CDTimerClass<SystemTimerClass> timer = delay;

		Write_To_Serial_Port (tmp_string, strlen((char*)tmp_string));
		Wait_For_Serial_Write();

		if (timer <= 0) {
			return(0);
		}

		int modem_delay;
		if (timer < 1) {
			modem_delay = 1;
		} else {
			modem_delay = timer;
		}

		memset(tmp_buff, 0, 80);
		time = Get_Modem_Result(modem_delay, tmp_buff, 80, inter_char_delay);

		if (!strcmp(tmp_buff,"0")) return(MODEM_CMD_0);
		if (strstr(tmp_buff,"OK")) return(MODEM_CMD_OK);
		if (strstr(tmp_buff,"ERROR")) return(MODEM_CMD_ERROR);

		if (strlen(tmp_buff)==3){
			if ( (tmp_buff[0] >= '0' && tmp_buff[0] <='9') &&
				  (tmp_buff[1] >= '0' && tmp_buff[1] <='9') &&
				  (tmp_buff[2] >= '0' && tmp_buff[2] <='9')) {
				strncpy(buffer, tmp_buff, MIN(buflen, 80));
				return(MODEM_CMD_OK);
			}
		}

		strncpy(buffer, tmp_buff, MIN(buflen, 80));

		Sleep (100);
		Write_To_Serial_Port((unsigned char*)"\r",1);
		Wait_For_Serial_Write();
		Sleep (100);
	}

	return(0);
}

// FUNCTION: GAMEMD 0x007754A0
void WinModemClass::Set_Echo_Function( void ( *func )( char c) )
{
	EchoFunction = func;
}

// FUNCTION: GAMEMD 0x007754B0
void WinModemClass::Set_Abort_Function(int (*func)(void))
{
	AbortFunction = func;
}

// FUNCTION: GAMEMD 0x007754C0
HANDLE WinModemClass::Get_Port_Handle(void)
{
	return(PortHandle);
}

// FUNCTION: GAMEMD 0x007754D0
void WinModemClass::Debug_Status(DWORD error, COMSTAT status)
{
	if (error) {
		if (CE_FRAME & error) {
			DebugString("Communications 'Framing' error detected.\n");
			FramingErrors++;
		}

		if (CE_BREAK & error) {
			DebugString("Communications 'Break' detected.\n");
		}

		if (CE_IOE & error) {
			DebugString("Communications 'IO' error detected.\n");
			IOErrors++;
		}

		if (CE_OVERRUN & error) {
			DebugString("Communications 'Receive Overrun' error detected.\n");
			BufferOverruns++;
		}

		if (CE_RXOVER & error) {
			DebugString("Communications 'Receive Queue Overflow' error detected.\n");
			InBufferOverflows++;
		}

		if (CE_RXPARITY & error) {
			DebugString("Communications 'Parity' error detected.\n");
			ParityErrors++;
		}

		if (CE_TXFULL & error) {
			DebugString("Communications 'Transmit Queue Full' error detected.\n");
			OutBufferOverflows++;
		}
	}

	if (status.fCtsHold) {
		DebugString("Comm status - Tx waiting for CTS signal\n");
	}

	if (status.fDsrHold) {
		DebugString("Comm status - Tx waiting for DSR signal\n");
	}

	if (status.fRlsdHold) {
		DebugString("Comm status - Tx waiting for RLSD signal\n");
	}

	if (status.fXoffHold) {
		DebugString("Comm status - Tx waiting, XOFF char received\n");
	}

	if (status.fXoffSent) {
		DebugString("Comm status - Tx waiting, XOFF char sent\n");
	}

	if (status.fEof) {
		DebugString("Comm status - EOF character sent\n");
	}

	if (status.fTxim) {
		DebugString("Comm status - Character waiting for Tx\n");
	}
}

// SYNTHETIC: GAMEMD 0x00775610
// ??_GWinModemClass@@UAEPAXI@Z

int DAT_00B72F28;
int DAT_00B72F2C;
int DAT_00B72F30;
int DAT_00B72F34;

// FUNCTION: GAMEMD 0x00775670
void FUN_00775670(void)
{
	DAT_00B72F28 = 0;
	DAT_00B72F2C = 0;
	DAT_00B72F30 = 0;
	DAT_00B72F34 = 0;
}
