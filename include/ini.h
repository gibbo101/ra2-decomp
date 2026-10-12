// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "crc.h"
#include "index.h"
#include "listnode.h"

#include <comdef.h>
#include <cstdlib>

class FileClass;
class Pipe;
class PKey;
class Straw;
template<class T> class TRect;
typedef TRect<int> Rect;
template<class T> class TPoint2D;
template<class T> class TPoint3D;

class INIClass {
	public:
		INIClass(void) : CurrentSectionName(NULL), CurrentSection(NULL), TailComment(NULL) {}
		virtual ~INIClass(void);

		int Load(FileClass & file, bool keepcomments = false);
		int Load(Straw & file, bool keepcomments = false);
		int Save(FileClass & file) const;
		int Save(Pipe & file) const;

		bool Clear(char const * section = NULL, char const * entry = NULL);
		void Reset(void);

		bool Is_Loaded(void) const {return(!SectionList.Is_Empty());}
		int Size(void) const;
		bool Is_Present(char const * section, char const * entry = NULL) const {if (entry == 0) return(Find_Section(section) != 0);return(Find_Entry(section, entry) != 0);}

		int Section_Count(void) const;
		bool Section_Present(char const * section) const {return(Find_Section(section) != NULL);}

		int Entry_Count(char const * section) const;
		char const * Get_Entry(char const * section, int index) const;

		PKey Get_PKey(bool fast) const;
		bool Get_Bool(char const * section, char const * entry, bool defvalue=false) const;
		double Get_Float(char const * section, char const * entry, double defvalue=0.0) const;
		int Get_Hex(char const * section, char const * entry, int defvalue=0) const;
		int Get_Int(char const * section, char const * entry, int defvalue=0) const;
		int Get_String(char const * section, char const * entry, char const * defvalue, char * buffer, int size) const;
		int Get_TextBlock(char const * section, char * buffer, int len) const;
		int Get_UUBlock(char const * section, void * buffer, int len) const;
		Rect const Get_Rect(char const * section, char const * entry, Rect const & defvalue) const;
		TPoint3D<int> const Get_Point(char const * section, char const * entry, TPoint3D<int> const & defvalue) const;
		TPoint2D<int> const Get_Point(char const * section, char const * entry, TPoint2D<int> const & defvalue) const;
		TPoint3D<float> const Get_Point(char const * section, char const * entry, TPoint3D<float> const & defvalue) const;
		CLSID const Get_CLSID(char const * section, char const * entry, CLSID defvalue) const;
		class TechnoTypeClass * GetTechnoType(char const * section, char const * entry) const;

		bool Put_Bool(char const * section, char const * entry, bool value);
		bool Put_Float(char const * section, char const * entry, double number);
		bool Put_Hex(char const * section, char const * entry, int number);
		bool Put_Int(char const * section, char const * entry, int number, int format=0);
		bool Put_PKey(PKey const & key);
		bool Put_String(char const * section, char const * entry, char const * string);
		bool Put_TextBlock(char const * section, char const * text);
		bool Put_UUBlock(char const * section, void const * block, int len);
		bool Put_Rect(char const * section, char const * entry, Rect const & value);
		bool Put_Point(char const * section, char const * entry, TPoint3D<int> const & value);
		bool Put_Point(char const * section, char const * entry, TPoint3D<float> const & value);
		bool Put_Point(char const * section, char const * entry, TPoint2D<int> const & value);
		bool Put_CLSID(char const * section, char const * entry, CLSID const & value);

		enum {MAX_LINE_LENGTH=512};

		struct INIComment {
			INIComment(void) :
				Comment(NULL),
				Next(NULL)
			{
			}

			char * Comment;

			INIComment * Next;
		};

		struct INIEntry : public Node<INIEntry *> {
			INIEntry(char * entry = NULL, char * value = NULL, INIComment * prefix_comment = NULL, char * line_comment = NULL, int comment_col = 0, int assign_col = 0, int value_col = 0) :
				Entry(entry),
				Value(value),
				PrefixComment(prefix_comment),
				LineComment(line_comment),
				AssignColumn(assign_col),
				ValueColumn(value_col),
				CommentColumn(comment_col)
			{
			}

			~INIEntry(void);
			int Index_ID(void) const {return(CRCEngine()(Entry, strlen(Entry)));};

			char * Entry;
			char * Value;

			INIComment * PrefixComment;

			char * LineComment;

			int AssignColumn;
			int ValueColumn;
			int CommentColumn;

			private:

				INIEntry(INIEntry const & rvalue);
				INIEntry operator = (INIEntry const & rvalue);
		};

		struct INISection : public Node<INISection *> {
			INISection(char * section, INIComment * prefixcomment = NULL) :
				Section(section),
				PrefixComment(prefixcomment)
			{
			}
			~INISection(void);
			INIEntry * Find_Entry(char const * entry) const;
			int Index_ID(void) const {return(CRCEngine()(Section, strlen(Section)));};

			char * Section;
			List<INIEntry *> EntryList;
			IndexClass<int, INIEntry *> EntryIndex;

			INIComment * PrefixComment;

			private:

				INISection(INISection const & rvalue);
				INISection operator = (INISection const & rvalue);
		};

		static bool Is_A_Section(char * buffer);
		INISection * Find_Section(char const * section) const;
		INISection * GetSection(char const * section);
		INIEntry * Find_Entry(char const * section, char const * entry) const;
		static void Strip_Comments(char * buffer);
		static char * Scan_Line_For_Columns(char * buffer, int & assign_pos, int & value_pos, int & comment_pos);

		char * CurrentSectionName;
		INISection * CurrentSection;

		List<INISection *> SectionList;

		IndexClass<int, INISection *> SectionIndex;

	private:

		INIComment * TailComment;

		INIClass(INIClass const & rvalue);
		INIClass operator = (INIClass const & rvalue);

};
