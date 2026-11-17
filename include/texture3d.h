// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

#include <d3d.h>

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
