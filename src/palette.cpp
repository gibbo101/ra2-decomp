// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "palette.h"
#include "hashstr.h"
#include "hashtable.h"
#include "vector.h"
#include "scheme.h"
#include "_theater.h"
#include "rect.h"
#include <cstring>

// FUNCTION: GAMEMD 0x00626020
PaletteClass::PaletteClass(RGBClass const & rgb)
{
	for (int index = 0; index < COLOR_COUNT; index++) {
		Palette[index] = rgb;
	}
}

// FUNCTION: GAMEMD 0x00626070
PaletteClass::PaletteClass(unsigned char * binary_palette)
{
	memcpy(&Palette[0], binary_palette, sizeof(Palette));
}

// FUNCTION: GAMEMD 0x006260A0
bool PaletteClass::operator == (PaletteClass const & palette) const
{
	if (this == &palette) return(true);
	return(memcmp(&Palette[0], &palette.Palette[0], sizeof(Palette)) == 0);
}

// FUNCTION: GAMEMD 0x006260D0
PaletteClass & PaletteClass::operator = (PaletteClass const & palette)
{
	if (this == &palette) return(*this);

	memcpy(&Palette[0], &palette.Palette[0], sizeof(Palette));
	return(*this);
}

// FUNCTION: GAMEMD 0x006260F0
void PaletteClass::Adjust(int ratio)
{
	for (int index = 0; index < COLOR_COUNT; index++) {
		Palette[index].Adjust(ratio, BlackColor);
	}
}

// FUNCTION: GAMEMD 0x00626120
void PaletteClass::Adjust(int ratio, PaletteClass const & palette)
{
	for (int index = 0; index < COLOR_COUNT; index++) {
		Palette[index].Adjust(ratio, palette[index]);
	}
}

// FUNCTION: GAMEMD 0x00626170
void PaletteClass::Partial_Adjust(int ratio, char *lut)
{
	for (int index = 0; index < COLOR_COUNT; index++) {
		if (lut[index]) {
			Palette[index].Adjust(ratio, BlackColor);
		}
	}
}

// FUNCTION: GAMEMD 0x006261B0
void PaletteClass::Partial_Adjust(int ratio, PaletteClass const & palette, char *lut)
{
	for (int index = 0; index < COLOR_COUNT; index++) {
		if (lut[index]) {
			Palette[index].Adjust(ratio, palette[index]);
		}
	}
}

// FUNCTION: GAMEMD 0x00626200
int PaletteClass::Closest_Color(RGBClass const & rgb) const
{
	int closest = 0;
	int value = -1;

	RGBClass const * ptr = &Palette[0];
	for (int index = 0; index < COLOR_COUNT; index++) {
		int difference = rgb.Difference(*ptr++);
		if (value == -1 || difference < value) {
			value = difference;
			closest = index;
		}
	}
	return(closest);
}

struct FileSystem
{
	static void const * LoadFile(char const * name, bool flag);
	static char TEMPERAT_PAL[];
};

extern char * DAT_00836464;

class SchemeNode
{
	public:
		SchemeNode(char const * name, DynamicVectorClass<ColorScheme *> * schemes);
		void Release(void);
		~SchemeNode(void);
		void Replace(DynamicVectorClass<ColorScheme *> * schemes);

		char Name[256];
		DynamicVectorClass<ColorScheme *> * Schemes;
};

// FUNCTION: GAMEMD 0x00626250
SchemeNode::SchemeNode(char const * name, DynamicVectorClass<ColorScheme *> * schemes)
{
	strcpy(Name, name);
	Schemes = schemes;
}

// FUNCTION: GAMEMD 0x00626290
void SchemeNode::Release(void)
{
	if (Schemes) {
		int count = Schemes->Count();
		for (int index = 0; index < count; index++) {
			delete (*Schemes)[index];
		}
		delete Schemes;
		Schemes = 0;
	}
}

// FUNCTION: GAMEMD 0x006262F0
void SchemeNode::Replace(DynamicVectorClass<ColorScheme *> * schemes)
{
	if (Schemes) {
		int count = Schemes->Count();
		for (int index = 0; index < count; index++) {
			delete (*Schemes)[index];
		}
		delete Schemes;
		Schemes = 0;
	}
	Schemes = schemes;
}

// FUNCTION: GAMEMD 0x00626370
SchemeNode::~SchemeNode(void)
{
	if (Schemes) {
		int count = Schemes->Count();
		for (int index = 0; index < count; index++) {
			delete (*Schemes)[index];
		}
		delete Schemes;
		Schemes = 0;
		count = 0;
		count++;
		if (count > 5) {
			count = 1;
		}
		count = 0;
		count++;
	}
}

// FUNCTION: GAMEMD 0x00626660
int SchemeHash(char const * str)
{
	int hash = 0;
	while (*str) {
		hash = (*str++ + hash * 26) % 31;
	}
	return(hash);
}

class SchemeLibrary
{
	public:
		SchemeLibrary(void) :
			Mode(-1),
			Names(new DynamicVectorClass<char *>),
			Colors(new DynamicVectorClass<HSVClass>),
			Table(new HashTableClass<HashString, SchemeNode *>(10, 31, (int (__fastcall *)(HashString const &))SchemeHash))
		{
		}

		~SchemeLibrary(void);

		DynamicVectorClass<ColorScheme *> * Lookup(char const * name);
		DynamicVectorClass<ColorScheme *> * Generate(char const * name);
		void Add(char const * name);
		void Replace(char const * name);
		void Set_Mode(int mode);

		int Mode;
		DynamicVectorClass<char *> * Names;
		DynamicVectorClass<HSVClass> * Colors;
		HashTableClass<HashString, SchemeNode *> * Table;
};

extern SchemeLibrary * DAT_00AC48F0;

inline SchemeLibrary * Library(void)
{
	if (DAT_00AC48F0 == 0) {
		DAT_00AC48F0 = new SchemeLibrary;
	}
	return(DAT_00AC48F0);
}

typedef HashTableClass<HashString, SchemeNode *> SchemeTable;
typedef HashObject<HashString, SchemeNode *> SchemeEntry;

inline HashString * Next_Key(SchemeTable * table, HashIterator * it)
{
	if (!it->OutOfBuckets) {
		while (it->BucketIndex < table->NumBuckets) {
			DynamicVectorClass<SchemeEntry> & bucket = table->Buckets[it->BucketIndex];
			while (it->InBucketIndex < bucket.Count()) {
				SchemeEntry * object = &bucket[it->InBucketIndex];
				it->InBucketIndex++;
				if (!!object->Key) {
					return(&object->Key);
				}
			}
			it->BucketIndex++;
			it->InBucketIndex = 0;
		}
		it->OutOfBuckets = true;
	}
	return(0);
}

inline SchemeNode ** Find_Node(SchemeTable * table, HashString const & key)
{
	DynamicVectorClass<SchemeEntry> & bucket = table->Buckets[table->HashFunction(key) % table->NumBuckets];
	for (int index = bucket.Count() - 1; index >= 0; index--) {
		SchemeEntry * object = &bucket[index];
		if (object->Key == key) {
			return(&object->Value);
		}
	}
	return(0);
}

// FUNCTION: GAMEMD 0x00626690
DynamicVectorClass<ColorScheme *> * __fastcall GetPaletteSchemesFromIterator(HashIterator * it)
{
	HashString * key = Next_Key(DAT_00AC48F0->Table, it);
	if (key == 0) {
		return(0);
	}
	SchemeNode ** node = Find_Node(DAT_00AC48F0->Table, *key);
	if (node == 0) {
		return(0);
	}
	return((*node)->Schemes);
}

// FUNCTION: GAMEMD 0x006269A0
void SchemeLibrary_Free(void)
{
	SchemeLibrary * library = Library();
	int count = library->Names->Count();
	for (int index = 0; index < count; index++) {
		delete (*DAT_00AC48F0->Names)[index];
	}
	DAT_00AC48F0->Names->Clear();
	DAT_00AC48F0->Colors->Clear();
}

// FUNCTION: GAMEMD 0x00626AB0
void __fastcall SchemeLibrary_Add(char const * name, HSVClass const & color)
{
	Library();
	char * copy = new char[256];
	strcpy(copy, name);
	DAT_00AC48F0->Names->Add(copy);
	DAT_00AC48F0->Colors->Add(color);
}


// FUNCTION: GAMEMD 0x00626C60
int __fastcall SchemeLibrary_Count(void)
{
	return(Library()->Names->Count());
}

// FUNCTION: GAMEMD 0x00626FF0
void SchemeLibrary_Destroy(void)
{
	if (DAT_00AC48F0 != 0) {
		delete DAT_00AC48F0;
		DAT_00AC48F0 = 0;
	}
}

// FUNCTION: GAMEMD 0x00627020
void SchemeLibrary_Create(void)
{
	Library();
}

// FUNCTION: GAMEMD 0x00627170
// SchemeLibrary::SchemeLibrary

// FUNCTION: GAMEMD 0x006272A0
SchemeLibrary::~SchemeLibrary(void)
{
	if (Table != 0) {
		SchemeLibrary_Free();
		delete Names;
		delete Colors;

		HashIterator it = {0, 0, false};
		for (HashString * key = Next_Key(Table, &it); key != 0; key = Next_Key(Table, &it)) {
			SchemeNode * node = *Find_Node(Table, *key);
			delete node;
		}
		SchemeTable * table = Table;
		for (int index = 0; index < table->NumBuckets; index++) {
			table->Buckets[index].Clear();
		}
		delete Table;
	}
}

// FUNCTION: GAMEMD 0x006274F0
DynamicVectorClass<ColorScheme *> * SchemeLibrary::Lookup(char const * name)
{
	HashString key(name);
	SchemeNode ** node = Find_Node(Table, key);
	if (node == 0) {
		return(0);
	}
	return((*node)->Schemes);
}

// FUNCTION: GAMEMD 0x00627590
DynamicVectorClass<ColorScheme *> * SchemeLibrary::Generate(char const * name)
{
	if (Mode == -1) {
		return(0);
	}

	char path[256];
	strcpy(path, name);
	strcat(path, Theaters[Mode].Suffix);
	strcat(path, ".");
	strcat(path, DAT_00836464);
	strupr(path);

	PaletteClass pal;
	void const * file = FileSystem::LoadFile(path, false);
	if (file == 0) {
		return(0);
	}
	memcpy(&pal, file, sizeof(pal));

	for (int slot = 0; slot < 256; slot++) {
		RGBClass & entry = pal[slot];
		RGBClass color = entry;
		entry.Set_Red(color.Get_Red() << 2);
		entry.Set_Green(color.Get_Green() << 2);
		entry.Set_Blue(color.Get_Blue() << 2);
	}

	int count = Colors->Count();
	DynamicVectorClass<ColorScheme *> * schemes = new DynamicVectorClass<ColorScheme *>(20);
	for (int index = 0; index < count; index++) {
		char * id = (*Names)[index];
		HSVClass base = (*Colors)[index];
		ColorScheme * scheme = new ColorScheme(id, base, pal, *(PaletteClass *)FileSystem::TEMPERAT_PAL, 1, false);
		scheme->ID = index * 2;
		schemes->Add(scheme);
		scheme = new ColorScheme(id, base, pal, *(PaletteClass *)FileSystem::TEMPERAT_PAL, 53, false);
		scheme->ID = index * 2 + 1;
		schemes->Add(scheme);
	}
	return(schemes);
}

// FUNCTION: GAMEMD 0x006278C0
void SchemeLibrary::Add(char const * name)
{
	DynamicVectorClass<ColorScheme *> * schemes = Generate(name);
	SchemeNode * node = new SchemeNode(name, schemes);
	HashString key(name);
	SchemeTable * table = Table;
	DynamicVectorClass<SchemeEntry> & bucket = table->Buckets[table->HashFunction(key) % table->NumBuckets];
	for (int index = bucket.Count() - 1; index >= 0; index--) {
		if (bucket[index].Key == key) {
			return;
		}
	}
	bucket.Add(SchemeEntry(key, node));
}

// FUNCTION: GAMEMD 0x00627A10
void SchemeLibrary::Replace(char const * name)
{
	HashString key(name);
	SchemeNode * node = *Find_Node(Table, key);
	node->Replace(Generate(name));
}

// FUNCTION: GAMEMD 0x00627B20
void SchemeLibrary::Set_Mode(int mode)
{
	Mode = mode;
	HashIterator it = {0, 0, false};
	HashString * key = Next_Key(Table, &it);
	while (key != 0) {
		HashString name(key->Str);
		SchemeNode * node = *Find_Node(Table, name);
		node->Replace(Generate(key->Str));
		key = Next_Key(Table, &it);
	}
}

// TEMPLATE: GAMEMD 0x00627D20
// ?Clear@?$DynamicVectorClass@PAVColorScheme@@@@UAEXXZ
template class DynamicVectorClass<char *>;

// TEMPLATE: GAMEMD 0x00627D50
// ?ID@?$DynamicVectorClass@PAVColorScheme@@@@UAEHPBQAVColorScheme@@@Z
// TEMPLATE: GAMEMD 0x00627D70
// ?GetItem@?$VectorClass@PAVColorScheme@@@@UBEPAVColorScheme@@H@Z

// TEMPLATE: GAMEMD 0x00627D80
// ?Clear@?$DynamicVectorClass@VHSVClass@@@@UAEXXZ

// TEMPLATE: GAMEMD 0x00627DB0
// ?ID@?$DynamicVectorClass@VHSVClass@@@@UAEHPBVHSVClass@@@Z
// TEMPLATE: GAMEMD 0x00627DE0
// ?GetItem@?$VectorClass@VHSVClass@@@@UBE?AVHSVClass@@H@Z

// TEMPLATE: GAMEMD 0x00627E10
// ??8?$VectorClass@PAVColorScheme@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x00627E50
// ?Resize@?$VectorClass@PAVColorScheme@@@@UAE_NHPBQAVColorScheme@@@Z
// TEMPLATE: GAMEMD 0x00627F00
// ?Clear@?$VectorClass@PAVColorScheme@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x00627F30
// ?ID@?$VectorClass@PAVColorScheme@@@@UAEHABQAVColorScheme@@@Z
// TEMPLATE: GAMEMD 0x00627F70
// ?ID@?$VectorClass@PAVColorScheme@@@@UAEHPBQAVColorScheme@@@Z
// TEMPLATE: GAMEMD 0x00627F90
// ?Resize@?$DynamicVectorClass@PAVColorScheme@@@@UAE_NHPBQAVColorScheme@@@Z
// TEMPLATE: GAMEMD 0x00628040
// ?ID@?$DynamicVectorClass@PAVColorScheme@@@@UAEHABQAVColorScheme@@@Z
template class DynamicVectorClass<ColorScheme *>;


// TEMPLATE: GAMEMD 0x00628070
// ??0?$HashTableClass@VHashString@@PAVSchemeNode@@@@QAE@HHP6IHABVHashString@@@Z@Z

// TEMPLATE: GAMEMD 0x00628110
// ?Find@?$HashTableClass@VHashString@@PAVSchemeNode@@@@QAEPAPAVSchemeNode@@ABVHashString@@@Z
template<>
SchemeNode ** HashTableClass<HashString, SchemeNode *>::Find(HashString const & key)
{
	return(Find_Node(this, key));
}

// TEMPLATE: GAMEMD 0x00628170
// ?Next@?$HashTableClass@VHashString@@PAVSchemeNode@@@@QAEPAVHashString@@PAUHashIterator@@@Z
template<>
HashString * HashTableClass<HashString, SchemeNode *>::Next(HashIterator * it)
{
	return(Next_Key(this, it));
}

// TEMPLATE: GAMEMD 0x006281F0
// ??8?$VectorClass@VHSVClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x00628240
// ??0?$DynamicVectorClass@VHSVClass@@@@QAE@IPBVHSVClass@@@Z
// TEMPLATE: GAMEMD 0x006282E0
// ?Resize@?$VectorClass@VHSVClass@@@@UAE_NHPBVHSVClass@@@Z
// TEMPLATE: GAMEMD 0x006283E0
// ?Clear@?$VectorClass@VHSVClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x00628410
// ?ID@?$VectorClass@VHSVClass@@@@UAEHABVHSVClass@@@Z
// TEMPLATE: GAMEMD 0x00628470
// ?ID@?$VectorClass@VHSVClass@@@@UAEHPBVHSVClass@@@Z
// TEMPLATE: GAMEMD 0x006284A0
// ?Resize@?$DynamicVectorClass@VHSVClass@@@@UAE_NHPBVHSVClass@@@Z
// TEMPLATE: GAMEMD 0x006285A0
// ?ID@?$DynamicVectorClass@VHSVClass@@@@UAEHABVHSVClass@@@Z
// SYNTHETIC: GAMEMD 0x006285F0
// ??_G?$VectorClass@PAVColorScheme@@@@UAEPAXI@Z
template class DynamicVectorClass<HSVClass>;

// SYNTHETIC: GAMEMD 0x00628640
// ??_G?$DynamicVectorClass@PAVColorScheme@@@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x00628690
// ??_E?$DynamicVectorClass@U?$HashObject@VHashString@@PAVSchemeNode@@@@@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x00628740
// ??_F?$DynamicVectorClass@U?$HashObject@VHashString@@PAVSchemeNode@@@@@@QAEXXZ

// SYNTHETIC: GAMEMD 0x00628760
// ??_G?$VectorClass@VHSVClass@@@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x006287B0
// ??_G?$DynamicVectorClass@VHSVClass@@@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x00628800
// ?Clear@?$DynamicVectorClass@U?$HashObject@VHashString@@PAVSchemeNode@@@@@@UAEXXZ

// TEMPLATE: GAMEMD 0x00628830
// ?ID@?$DynamicVectorClass@U?$HashObject@VHashString@@PAVSchemeNode@@@@@@UAEHPBU?$HashObject@VHashString@@PAVSchemeNode@@@@@Z

// TEMPLATE: GAMEMD 0x00628860
// ?GetItem@?$VectorClass@U?$HashObject@VHashString@@PAVSchemeNode@@@@@@UBE?AU?$HashObject@VHashString@@PAVSchemeNode@@@@H@Z

// TEMPLATE: GAMEMD 0x006288A0
// ??8?$VectorClass@U?$HashObject@VHashString@@PAVSchemeNode@@@@@@UBE_NABV0@@Z

// TEMPLATE: GAMEMD 0x006288F0
// ?Resize@?$VectorClass@U?$HashObject@VHashString@@PAVSchemeNode@@@@@@UAE_NHPBU?$HashObject@VHashString@@PAVSchemeNode@@@@@Z

// TEMPLATE: GAMEMD 0x00628A40
// ?Clear@?$VectorClass@U?$HashObject@VHashString@@PAVSchemeNode@@@@@@UAEXXZ

// TEMPLATE: GAMEMD 0x00628A70
// ?ID@?$VectorClass@U?$HashObject@VHashString@@PAVSchemeNode@@@@@@UAEHABU?$HashObject@VHashString@@PAVSchemeNode@@@@@Z

// TEMPLATE: GAMEMD 0x00628AD0
// ?ID@?$VectorClass@U?$HashObject@VHashString@@PAVSchemeNode@@@@@@UAEHPBU?$HashObject@VHashString@@PAVSchemeNode@@@@@Z

// TEMPLATE: GAMEMD 0x00628B00
// ??0?$DynamicVectorClass@U?$HashObject@VHashString@@PAVSchemeNode@@@@@@QAE@IPBU?$HashObject@VHashString@@PAVSchemeNode@@@@@Z

// TEMPLATE: GAMEMD 0x00628BB0
// ?Resize@?$DynamicVectorClass@U?$HashObject@VHashString@@PAVSchemeNode@@@@@@UAE_NHPBU?$HashObject@VHashString@@PAVSchemeNode@@@@@Z

// TEMPLATE: GAMEMD 0x00628D10
// ?Add@?$DynamicVectorClass@U?$HashObject@VHashString@@PAVSchemeNode@@@@@@QAE_NABU?$HashObject@VHashString@@PAVSchemeNode@@@@@Z

// TEMPLATE: GAMEMD 0x00628D80
// ?ID@?$DynamicVectorClass@U?$HashObject@VHashString@@PAVSchemeNode@@@@@@UAEHABU?$HashObject@VHashString@@PAVSchemeNode@@@@@Z

// TEMPLATE: GAMEMD 0x00628DD0
// ??0?$HashObject@VHashString@@PAVSchemeNode@@@@QAE@ABVHashString@@ABQAVSchemeNode@@@Z

// TEMPLATE: GAMEMD 0x00628DF0
// ??0?$VectorClass@VHSVClass@@@@QAE@HPBVHSVClass@@@Z

template struct HashObject<HashString, SchemeNode *>;
template class DynamicVectorClass<HashObject<HashString, SchemeNode *> >;

// SYNTHETIC: GAMEMD 0x00628E80
// ??_G?$VectorClass@U?$HashObject@VHashString@@PAVSchemeNode@@@@@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x00628ED0
// ??0?$VectorClass@U?$HashObject@VHashString@@PAVSchemeNode@@@@@@QAE@HPBU?$HashObject@VHashString@@PAVSchemeNode@@@@@Z
template class VectorClass<HashObject<HashString, SchemeNode *> >;

// SYNTHETIC: GAMEMD 0x00628F80 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00628FB0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00628FD0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00628FF0 SYMBOL
// _$E11

// SYNTHETIC: GAMEMD 0x00629010 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x00629030 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x00629050 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x00629090 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x006290C0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x006290F0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x00629120 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00629130 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x00629150 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x00629170 SYMBOL
// _$E42
