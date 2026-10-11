// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "data.h"
#include <new.h>

HINSTANCE LanguageResources;

// FUNCTION: 0x004A3890
void * Load_Alloc_Data(FileClass & file)
{
	void * ptr = NULL;
	if (file.Is_Available()) {
		int size = file.Size();

		ptr = new char[size];
		if (ptr != NULL) {
			file.Read(ptr, size);
		}
	}
	return(ptr);
}

// FUNCTION: 0x004A3A10
int Load_Uncompress(FileClass & file, Buffer & uncomp_buff, Buffer & dest_buff, void * reserved_data)
{
	unsigned short	size;
	void	* sptr = uncomp_buff.Get_Buffer();
	void	* dptr = dest_buff.Get_Buffer();
	int	opened = false;
	CompHeaderType	header;

	if (!file.Is_Open()) {
		if (!file.Open()) {
			return(0);
		}
		opened = true;
	}

	file.Read(&size, sizeof(size));

	file.Read(&header, sizeof(header));
	size -= (unsigned short)sizeof(header);

	if (header.Skip) {
		size -= header.Skip;
		if (reserved_data) {
			file.Read(reserved_data, header.Skip);
		} else {
			file.Seek(header.Skip, SEEK_CUR);
		}
		header.Skip = 0;
	}

	if (uncomp_buff.Get_Buffer() == dest_buff.Get_Buffer()) {
		sptr = (char *)sptr + uncomp_buff.Get_Size()-(size+sizeof(header));
	}

	memmove(sptr, &header, sizeof(header));
	file.Read((char *)sptr + sizeof(header), size);

	size = (unsigned short) Uncompress_Data(sptr, dptr);

	if (opened) {
		file.Close();
	}
	return((int)size);
}

// FUNCTION: 0x004A3B40
void const * Fetch_Resource(LPCSTR resname, LPCSTR restype)
{
	HRSRC handle = FindResource(LanguageResources, MAKEINTRESOURCE(resname), restype);
	if (handle == NULL) {
		return(NULL);
	}

	HGLOBAL rhandle = LoadResource(LanguageResources, handle);
	if (rhandle == NULL) {
		return(NULL);
	}

	return(LockResource(rhandle));
}

// FUNCTION: 0x004A3B80
int Load_Picture(FileClass & file, Buffer & scratchbuf, Buffer & destbuf, unsigned char * palette, PicturePlaneType )
{
	return(Load_Uncompress(file, scratchbuf, destbuf,  palette ) / 8000);
}

// FUNCTION: 0x004A3BB0
void * Hires_Load(FileClass & file)
{
	int 	length;
	void * return_ptr;

	if (file.Is_Available()) {
		length = file.Size();
		return_ptr = new char[length];
		file.Read(return_ptr, length);
		return(return_ptr);

	} else {
		return(NULL);
	}
}
