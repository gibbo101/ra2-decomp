// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "bfiofile.h"
#include <cstring>

// FUNCTION: 0x00431B20
BufferIOFileClass::BufferIOFileClass(void) :
	IsAllocated(false),
	IsOpen(false),
	IsDiskOpen(false),
	IsCached(false),
	IsChanged(false),
	UseBuffer(false),
	BufferRights(0),
	Buffer(0),
	BufferSize(0),
	BufferPos(0),
	BufferFilePos(0),
	BufferChangeBeg(-1),
	BufferChangeEnd(-1),
	FileSize(0),
	FilePos(0),
	TrueFileStart(0)
{
}

// FUNCTION: 0x00431B80
BufferIOFileClass::~BufferIOFileClass(void)
{
	Free();
}

// FUNCTION: 0x00431BC0
bool BufferIOFileClass::Cache( int size, void * ptr )
{
	if (Buffer) {
		if (size || ptr) {
			return( false );
		} else {
			return( true );
		}
	}
	if ( Is_Available() ) {
		FileSize = Size();
	} else {
		FileSize = 0;
	}
	if (size) {
		if (size < MINIMUM_BUFFER_SIZE) {
			size = MINIMUM_BUFFER_SIZE;
			if (ptr) {
				Error(EINVAL);
			}
		}
		BufferSize = size;
	} else {
		BufferSize = FileSize;
	}
	if ( (size == 0 && ptr) || !BufferSize) {
		return( false );
	}
	if (ptr) {
		Buffer = ptr;
	} else {
		Buffer = new char [BufferSize];
	}
	if (Buffer) {
		IsAllocated			= true;
		IsDiskOpen			= false;
		BufferPos			= 0;
		BufferFilePos		= 0;
		BufferChangeBeg	= -1;
		BufferChangeEnd	= -1;
		FilePos				= 0;
		TrueFileStart		= 0;
		if (FileSize) {
			int readsize;
			int opened = false;
			int prevpos = 0;
			if (FileSize <= BufferSize) {
				readsize = FileSize;
			} else {
				readsize = BufferSize;
			}
			if ( Is_Open() ) {
				prevpos = Seek(0);
				if ( BASECLASS::Is_Open() ) {
					TrueFileStart = BASECLASS::Seek(0);
				} else {
					TrueFileStart = prevpos;
				}
				if (FileSize <= BufferSize) {
					if (prevpos) {
						Seek(0, SEEK_SET);
					}
					BufferPos = prevpos;
				} else {
					BufferFilePos = prevpos;
				}
				FilePos = prevpos;
			} else {
				if ( Open() ) {
					TrueFileStart = BASECLASS::Seek(0);
					opened = true;
				}
			}
			int actual = Read(Buffer, readsize);
			if (actual != readsize) {
				Error(EIO);
			}
			if (opened) {
				Close();
			} else {
				Seek(prevpos, SEEK_SET);
			}
			IsCached = true;
		}
		UseBuffer = true;
		return(true);
	}
	Error(ENOMEM);
	return(false);
}

// FUNCTION: 0x00431D90
void BufferIOFileClass::Free(void)
{
	if (Buffer) {
		if (IsAllocated) {
			delete [] Buffer;
			IsAllocated = false;
		}
		Buffer = 0;
	}
	BufferSize		= 0;
	IsOpen			= false;
	IsCached			= false;
	IsChanged		= false;
	UseBuffer		= false;
}

// FUNCTION: 0x00431DD0
bool BufferIOFileClass::Commit( void )
{
	int size;
	if (UseBuffer) {
		if (IsChanged) {
			size = BufferChangeEnd - BufferChangeBeg;
			if (IsDiskOpen) {
				BASECLASS::Seek( TrueFileStart + BufferFilePos +
										  BufferChangeBeg, SEEK_SET );
				BASECLASS::Write( Buffer, size );
				BASECLASS::Seek( TrueFileStart + FilePos, SEEK_SET );
			} else {
				BASECLASS::Open();
				BASECLASS::Seek( TrueFileStart + BufferFilePos +
										  BufferChangeBeg, SEEK_SET );
				BASECLASS::Write( Buffer, size );
				BASECLASS::Close();
			}
			IsChanged = false;
			return( true );
		} else {
			return( false );
		}
	} else {
		return( false );
	}
}

// FUNCTION: 0x00431E80
char const * BufferIOFileClass::Set_Name(char const * filename)
{
	if ( File_Name() && UseBuffer) {
		if ( strcmp(filename, File_Name() ) == 0) {
			return( File_Name() );
		} else {
			Commit();
			IsCached = false;
		}
	}
	BASECLASS::Set_Name(filename);
	return( File_Name() );
}

// FUNCTION: 0x00431F10
bool BufferIOFileClass::Is_Available(int )
{
	if (UseBuffer) {
		return(true);
	}
	return( BASECLASS::Is_Available() );
}

// FUNCTION: 0x00431F30
bool BufferIOFileClass::Is_Open(void) const
{
	if (IsOpen && UseBuffer) {
		return( true );
	}
	return( BASECLASS::Is_Open() );
}

// FUNCTION: 0x00431F50
int BufferIOFileClass::Open(char const * filename, int rights)
{
	Set_Name(filename);
	return( BufferIOFileClass::Open( rights ) );
}

// FUNCTION: 0x00431F70
int BufferIOFileClass::Open(int rights)
{
	BufferIOFileClass::Close();
	if (UseBuffer) {
		BufferRights = rights;
		if (rights != READ ||
			 (rights == READ && FileSize > BufferSize) ) {
			if (rights == WRITE) {
				BASECLASS::Open( rights );
				BASECLASS::Close();
				rights = READ | WRITE;
				TrueFileStart = 0;
			}
			if (TrueFileStart) {
				UseBuffer = false;
				Open( rights );
				UseBuffer = true;
			} else {
				BASECLASS::Open( rights );
			}
			IsDiskOpen = true;
			if (BufferRights == WRITE) {
				FileSize = 0;
			}
		} else {
			IsDiskOpen = false;
		}
		BufferPos			= 0;
		BufferFilePos		= 0;
		BufferChangeBeg	= -1;
		BufferChangeEnd	= -1;
		FilePos				= 0;
		IsOpen				= true;
	} else {
		return BASECLASS::Open( rights );
	}
	return( true );
}

// FUNCTION: 0x00432050
int BufferIOFileClass::Write(void const * buffer, int size)
{
	int opened = false;
	if ( !Is_Open() ) {
		if (!Open(FileClass::WRITE)) {
			return(0);
		}
		TrueFileStart = BASECLASS::Seek(0);
		opened = true;
	}
	if (UseBuffer) {
		int sizewritten = 0;
		if (BufferRights != READ) {
			while (size) {
				int sizetowrite;
				if (size >= (BufferSize - BufferPos) ) {
					sizetowrite = (BufferSize - BufferPos);
				} else {
					sizetowrite = size;
				}
				if (sizetowrite != BufferSize) {
					if ( !IsCached ) {
						int readsize;
						if (FileSize < BufferSize) {
							readsize = FileSize;
							BufferFilePos = 0;
						} else {
							readsize = BufferSize;
							BufferFilePos = FilePos;
						}
						if (TrueFileStart) {
							UseBuffer = false;
							Seek( FilePos, SEEK_SET );
							Read( Buffer, BufferSize );
							Seek( FilePos, SEEK_SET );
							UseBuffer = true;
						} else {
							BASECLASS::Seek( BufferFilePos, SEEK_SET );
							BASECLASS::Read( Buffer, readsize );
						}
						BufferPos			= 0;
						BufferChangeBeg	= -1;
						BufferChangeEnd	= -1;
						IsCached = true;
					}
				}
				memmove((char *)Buffer + BufferPos, (char *)buffer + sizewritten, sizetowrite);
				IsChanged = true;
				sizewritten += sizetowrite;
				size -= sizetowrite;
				if (BufferChangeBeg == -1) {
					BufferChangeBeg = BufferPos;
					BufferChangeEnd = BufferPos;
				} else {
					if (BufferChangeBeg > BufferPos) {
						BufferChangeBeg = BufferPos;
					}
				}
				BufferPos += sizetowrite;
				if (BufferChangeEnd < BufferPos) {
					BufferChangeEnd = BufferPos;
				}
				FilePos = BufferFilePos + BufferPos;
				if (FileSize < FilePos) {
					FileSize = FilePos;
				}
				if (BufferPos == BufferSize) {
					Commit();
					BufferPos = 0;
					BufferFilePos = FilePos;
					BufferChangeBeg = -1;
					BufferChangeEnd = -1;
					if (size && FileSize > FilePos) {
						if (TrueFileStart) {
							UseBuffer = false;
							Seek( FilePos, SEEK_SET );
							Read( Buffer, BufferSize );
							Seek( FilePos, SEEK_SET );
							UseBuffer = true;
						} else {
							BASECLASS::Seek( FilePos, SEEK_SET );
							BASECLASS::Read( Buffer, BufferSize );
						}
					} else {
						IsCached = false;
					}
				}
			}
		} else {
			Error(EACCES);
		}
		size = sizewritten;
	} else {
		size = BASECLASS::Write(buffer, size);
	}
	if (opened) {
		Close();
	}
	return( size );
}

// FUNCTION: 0x004322A0
int BufferIOFileClass::Read(void * buffer, int size)
{
	int opened = false;
	if ( !Is_Open() ) {
		if ( Open() ) {
			TrueFileStart = BASECLASS::Seek(0);
			opened = true;
		}
	}
	if (UseBuffer) {
		int sizeread = 0;
		if (BufferRights != WRITE) {
			while (size) {
				int sizetoread;
				if (size >= (BufferSize - BufferPos) ) {
					sizetoread = (BufferSize - BufferPos);
				} else {
					sizetoread = size;
				}
				if ( !IsCached ) {
					int readsize;
					if (FileSize < BufferSize) {
						readsize = FileSize;
						BufferFilePos = 0;
					} else {
						readsize = BufferSize;
						BufferFilePos = FilePos;
					}
					if (TrueFileStart) {
						UseBuffer = false;
						Seek( FilePos, SEEK_SET );
						Read( Buffer, BufferSize );
						Seek( FilePos, SEEK_SET );
						UseBuffer = true;
					} else {
						BASECLASS::Seek( BufferFilePos, SEEK_SET );
						BASECLASS::Read( Buffer, readsize );
					}
					BufferPos			= 0;
					BufferChangeBeg	= -1;
					BufferChangeEnd	= -1;
					IsCached = true;
				}
				memmove((char *)buffer + sizeread, (char *)Buffer + BufferPos, sizetoread);
				sizeread += sizetoread;
				size -= sizetoread;
				BufferPos += sizetoread;
				FilePos = BufferFilePos + BufferPos;
				if (BufferPos == BufferSize) {
					Commit();
					BufferPos = 0;
					BufferFilePos = FilePos;
					BufferChangeBeg = -1;
					BufferChangeEnd = -1;
					if (size && FileSize > FilePos) {
						if (TrueFileStart) {
							UseBuffer = false;
							Seek( FilePos, SEEK_SET );
							Read( Buffer, BufferSize );
							Seek( FilePos, SEEK_SET );
							UseBuffer = true;
						} else {
							BASECLASS::Seek( FilePos, SEEK_SET );
							BASECLASS::Read( Buffer, BufferSize );
						}
					} else {
						IsCached = false;
					}
				}
			}
		} else {
			Error(EACCES);
		}
		size = sizeread;
	} else {
		size = BASECLASS::Read(buffer, size);
	}
	if (opened) {
		Close();
	}
	return( size );
}

// FUNCTION: 0x004324B0
int BufferIOFileClass::Seek(int pos, int dir)
{
	if (UseBuffer) {
		bool adjusted = false;
		switch (dir) {
			case SEEK_END:
				FilePos = FileSize;
				break;
			case SEEK_SET:
				FilePos = 0;
				break;
			case SEEK_CUR:
			default:
				break;
		}
		if (TrueFileStart) {
			if (pos >= TrueFileStart) {
				pos -= TrueFileStart;
				adjusted = true;
			}
		}
		FilePos += pos;
		if (FilePos < 0) {
			FilePos = 0;
		}
		if (FilePos > FileSize ) {
			FilePos = FileSize;
		}
		if (FileSize <= BufferSize) {
			BufferPos = FilePos;
		} else {
			if (FilePos >= BufferFilePos &&
				 FilePos < (BufferFilePos + BufferSize) ) {
				BufferPos = FilePos - BufferFilePos;
			} else {
				Commit();
				if (TrueFileStart) {
					UseBuffer = false;
					Seek(FilePos, SEEK_SET);
					UseBuffer = true;
				} else {
					BASECLASS::Seek(FilePos, SEEK_SET);
				}
				IsCached = false;
			}
		}
		if (TrueFileStart && adjusted) {
			return( FilePos + TrueFileStart );
		}
		return( FilePos );
	}
	return( BASECLASS::Seek(pos, dir) );
}


// FUNCTION: 0x004325A0
int BufferIOFileClass::Size(void)
{
	if (IsOpen && UseBuffer) {
		return( FileSize );
	}

	return( BASECLASS::Size() );
}

// FUNCTION: 0x004325C0
void BufferIOFileClass::Close(void)
{
	if (UseBuffer) {
		Commit();

		if (IsDiskOpen) {

			if (TrueFileStart) {
				UseBuffer = false;
				Close();
				UseBuffer = true;
			} else {
				BASECLASS::Close();
			}

			IsDiskOpen = false;
		}

		IsOpen = false;
	} else {
		BASECLASS::Close();
	}
}

// FUNCTION: 0x00432610 BufferIOFileClass::~BufferIOFileClass

struct Dat4 { int a, b, c, d; };
extern Dat4 DAT_0089C498;

// FUNCTION: 0x00432670
void FUN_00432670(void)
{
	DAT_0089C498.a = 0;
	DAT_0089C498.b = 0;
	DAT_0089C498.c = 0;
	DAT_0089C498.d = 0;
}
