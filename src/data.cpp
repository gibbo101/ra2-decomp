// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define DIRECTDRAW_VERSION 0x0300
#include "always.h"
#include <d3d.h>
#include "data.h"
#include "abuffer.h"
#include "ccfile.h"
#include "wstring.h"
#include "zbuffer.h"
#include <new.h>

HINSTANCE LanguageResources;

extern LPDIRECTDRAW2 DAT_008A0E00;
extern LPDIRECT3DDEVICE2 DAT_008A0DF8;
extern const GUID DAT_007F8130;
extern LPDIRECTDRAWSURFACE DAT_0089F970;

const DWORD DAT_0081FC60[3] = {0x5000, 0x24000, 0x5000};

class Texture3D {
	public:
		Texture3D(int width, int height, int type);
		int FUN_004A33A0(unsigned short * pixels);
		void FUN_004A3370(void);

		int Type;
		DDSURFACEDESC Desc;
		LPDIRECTDRAWSURFACE Surface;
		LPDIRECT3DTEXTURE2 Texture;
};

struct Triangle3D {
	void Set_Color(int a, int b, int c);
	void Set_Vertex(int index, float x, float y, float z, float u, float v);

	D3DTLVERTEX Vertex[3];
};

class Texture3DCache {
	public:
		Texture3DCache(void);
		void FUN_004A3640(void);
		void FUN_004A3650(void);
		HRESULT Flush(LPDIRECTDRAWSURFACE target);
		int Add(Triangle3D const & triangle);

		Triangle3D List[1024];
		int Count;
};

extern Texture3D * DAT_008A0E04;
extern Texture3D * DAT_008A0E08;
extern Texture3D * DAT_008A0E0C;

struct FileSystem {
	static void const * LoadFile(char const * name, bool flag);
	static void * LoadWholeFileEx(char const * name, bool & allocated);
};
void * __fastcall FUN_004A3970(Wstring const & name, bool * loaded);

// FUNCTION: 0x004A3270
Texture3D::Texture3D(int width, int height, int type)
{
	Type = type;
	Surface = NULL;
	Texture = NULL;
	memset(&Desc, '\0', sizeof(Desc));
	Desc.dwSize = sizeof(DDSURFACEDESC);
	Desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	Desc.ddsCaps.dwCaps = DAT_0081FC60[type];
	Desc.dwWidth = width;
	Desc.dwHeight = height;
	if (type == 1) {
		Desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_ZBUFFERBITDEPTH;
		Desc.dwZBufferBitDepth = 16;
	}

	if (DAT_008A0E00 != NULL) {
		if (DAT_008A0E00->CreateSurface(&Desc, &Surface, NULL) != 0) return;
	}

	if (Surface != NULL) {
		switch (type) {
			case 0:
			case 2:
				Surface->QueryInterface(DAT_007F8130, (void **)&Texture);
				break;

			case 1:
				DDBLTFX fx;
				memset(&fx, '\0', sizeof(fx));
				fx.dwSize = sizeof(fx);
				fx.dwFillDepth = 0xFFFFFFFF;
				if (Surface->Blt(NULL, NULL, NULL, DDBLT_DEPTHFILL, &fx) != 0) return;
				break;
		}
	}
}

// FUNCTION: 0x004A3370
void Texture3D::FUN_004A3370(void)
{
	if (Texture != NULL) {
		Texture->Release();
		Texture = NULL;
	}
	if (Surface != NULL) {
		Surface->Release();
		Surface = NULL;
	}
}

// FUNCTION: 0x004A33A0
int Texture3D::FUN_004A33A0(unsigned short * pixels)
{
	if (Surface == NULL) return(-1);

	if (Type == 2) {
		Texture3D * source = DAT_008A0E08;
		if (source == NULL || source->Surface == NULL) return(-1);

		RECT rect;
		rect.right = 256;
		rect.bottom = 256;
		DDBLTFX fx;
		memset(&fx, '\0', sizeof(fx));
		rect.left = 0;
		rect.top = 0;
		fx.dwSize = sizeof(fx);
		return(Surface->Blt(NULL, source->Surface, &rect, DDBLT_ASYNC, &fx));
	}

	DDSURFACEDESC desc;
	memset(&desc, '\0', sizeof(desc));
	desc.dwSize = sizeof(desc);
	if (Surface->Lock(NULL, &desc, DDLOCK_WAIT | DDLOCK_WRITEONLY | DDLOCK_NOSYSLOCK, NULL) != 0) return(1);

	switch (Type) {
		case 0:
			if (pixels != NULL) {
				memcpy(desc.lpSurface, pixels, Desc.dwHeight * desc.lPitch);
			}
			break;

		case 1:
			if (DepthBuffer != NULL && AlphaBuffer != NULL) {
				unsigned short * dest = (unsigned short *)desc.lpSurface;
				unsigned int width = (unsigned int)desc.lPitch >> 1;
				unsigned int height = DepthBuffer->Get_Buffer_Height();
				for (unsigned int y = 0; y < height; y++) {
					unsigned short * depth = (unsigned short *)DepthBuffer->Get_Buffer_Offset(Point2D(0, y - DepthBuffer->Bounds.Y));
					unsigned short * alpha = (unsigned short *)AlphaBuffer->Get_Buffer_Offset(Point2D(0, y - AlphaBuffer->Bounds.Y));
					if (depth == NULL || alpha == NULL) break;
					for (unsigned int x = width; x > 0; x--) {
						if (*alpha >= 8) {
							*dest = *depth;
						} else {
							*dest = 0;
						}
						dest++;
						depth = (unsigned short *)DepthBuffer->Wrap_Overflow((unsigned int)(depth + 1));
						alpha = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)(alpha + 1));
					}
				}
			}
			break;
	}

	return((Surface->Unlock(NULL) != 0) ? 2 : 0);
}

// FUNCTION: 0x004A35A0
void Triangle3D::Set_Color(int a, int b, int c)
{
	DWORD color = (((((0xFFFFFF00 | a) << 8) | (b & 0xFF)) << 8) | (c & 0xFF));
	Vertex[2].color = color;
	Vertex[1].color = color;
	Vertex[0].color = color;
	Vertex[2].specular = 0xFF000000;
	Vertex[1].specular = 0xFF000000;
	Vertex[0].specular = 0xFF000000;
}

// FUNCTION: 0x004A35F0
void Triangle3D::Set_Vertex(int index, float x, float y, float z, float u, float v)
{
	D3DTLVERTEX * vertex = &Vertex[index];
	vertex->sx = x;
	vertex->sy = y;
	vertex->sz = z;
	vertex->tu = u;
	vertex->tv = v;
	vertex->rhw = 1.0f;
}

// FUNCTION: 0x004A3630
Texture3DCache::Texture3DCache(void)
{
	Count = 0;
}

// FUNCTION: 0x004A3640
void Texture3DCache::FUN_004A3640(void)
{
	Count = 0;
}

// FUNCTION: 0x004A3650
void Texture3DCache::FUN_004A3650(void)
{
}

// FUNCTION: 0x004A3660
HRESULT Texture3DCache::Flush(LPDIRECTDRAWSURFACE target)
{
	int count = Count;
	Count = 0;

	if (target == NULL || DAT_008A0E04 == NULL || DAT_008A0E04->Texture == NULL) return(-1);
	if (DAT_008A0E08 == NULL || DAT_008A0E08->Surface == NULL) return(-1);

	HRESULT result;
	if (DAT_0089F970 != target) {
		if (DAT_0089F970 != NULL) {
			result = DAT_0089F970->DeleteAttachedSurface(0, DAT_008A0E08->Surface);
			if (result != 0) return(result);
			DAT_0089F970 = NULL;
		}
		result = target->AddAttachedSurface(DAT_008A0E08->Surface);
		if (result != 0) return(result);
		result = DAT_008A0DF8->SetRenderTarget(target, 0);
		if (result != 0) return(result);
		DAT_0089F970 = target;
	}

	LPDIRECT3DTEXTURE2 texture;
	if (DAT_008A0E0C != NULL) {
		texture = DAT_008A0E0C->Texture;
	} else {
		texture = DAT_008A0E04->Texture;
	}
	D3DTEXTUREHANDLE handle;
	result = texture->GetHandle(DAT_008A0DF8, &handle);
	if (result != 0) return(result);

	result = DAT_008A0DF8->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE, handle);
	if (result != 0) return(result);
	result = DAT_008A0DF8->SetRenderState(D3DRENDERSTATE_ZENABLE, 1);
	if (result != 0) return(result);
	result = DAT_008A0DF8->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 0);
	if (result != 0) return(result);

	result = DAT_008A0DF8->SetRenderState(D3DRENDERSTATE_TEXTUREMAG, D3DFILTER_LINEAR);
	result |= DAT_008A0DF8->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
	result |= DAT_008A0DF8->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
	result |= DAT_008A0DF8->SetRenderState((D3DRENDERSTATETYPE)29, 0);
	result |= DAT_008A0DF8->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 1);
	result |= DAT_008A0DF8->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);
	if (result != 0) return(result);

	for (int i = 0; i < count; i++) {
		result = DAT_008A0DF8->DrawPrimitive(D3DPT_TRIANGLELIST, D3DVT_TLVERTEX, &List[i], 3, 0);
		if (result != 0) break;
	}
	return(result);
}

// FUNCTION: 0x004A3840
int Texture3DCache::Add(Triangle3D const & triangle)
{
	if (Count == 1024) return(1);
	List[Count] = triangle;
	Count++;
	return(0);
}

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

// FUNCTION: 0x004A38D0
void * FileSystem::LoadWholeFileEx(char const * name, bool & allocated)
{
	allocated = false;
	void * loaded = (void *)LoadFile(name, false);
	if (loaded != NULL) return(loaded);

	CCFileClass file(name);
	FileClass * stream = &file;
	void * ptr = NULL;
	if (stream->Is_Available()) {
		int size = stream->Size();
		ptr = new char[size];
		if (ptr != NULL) {
			stream->Read(ptr, size);
		}
	}
	allocated = (ptr != NULL);
	return(ptr);
}

// FUNCTION: 0x004A3970
void * __fastcall FUN_004A3970(Wstring const & name, bool * loaded)
{
	char const * filename = name.text();
	*loaded = false;
	void * found = (void *)FileSystem::LoadFile(filename, false);
	if (found != NULL) return(found);

	CCFileClass file(filename);
	FileClass * stream = &file;
	void * ptr = NULL;
	if (stream->Is_Available()) {
		int size = stream->Size();
		ptr = new char[size];
		if (ptr != NULL) {
			stream->Read(ptr, size);
		}
	}
	*loaded = (ptr != NULL);
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

// FUNCTION: 0x004A3B30
int FUN_004A3B30(void)
{
	return(0);
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

extern DWORD DAT_0089F980;
extern DWORD DAT_0089F984;
extern DWORD DAT_0089F988;
extern DWORD DAT_0089F98C;

struct Flag3D {
	void FUN_004A3C00(void);

	char Value;
};

// FUNCTION: 0x004A3BF0
bool FUN_004A3BF0(void)
{
	return(true);
}

// FUNCTION: 0x004A3C00
void Flag3D::FUN_004A3C00(void)
{
	Value = 0;
}

// FUNCTION: 0x004A3C10
void FUN_004A3C10(void)
{
	DAT_0089F980 = 0;
	DAT_0089F984 = 0;
	DAT_0089F988 = 0;
	DAT_0089F98C = 0;
}
