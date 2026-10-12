// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "list.h"

class ShapeSet;

class CheckObject
{
	public:
		CheckObject(char const * text = 0, bool checked=false) :
			Text(text),
			IsChecked(checked)
		{};

		char const * Text;
		bool IsChecked;
};

class CheckListClass : public ListClass
{
		typedef ListClass BASECLASS;

	public:

		CheckListClass(int id, int x, int y, int w, int h, TextPrintType flags,
			ShapeSet const * up, ShapeSet const * down);
		~CheckListClass(void);

		virtual int Add_Item(int text) override {return(BASECLASS::Add_Item(text));}
		virtual int Add_Item(char const * text) override;
		virtual char const * Current_Item(void) const override;
		virtual char const * Get_Item(int index) const override;
		virtual void Remove_Item(char const * text) override;
		virtual void Remove_Item(int text) override {BASECLASS::Remove_Item(text);}
		virtual void Set_Selected_Index(char const * text) override;
		virtual void Set_Selected_Index(int index) override {BASECLASS::Set_Selected_Index(index);};

		void Check_Item(int index, bool checked);
		bool Is_Checked(int index) const;

		void Set_Read_Only(bool rdonly) {IsReadOnly = rdonly;}

		enum CheckListClassEnum {
			CHECK_CHAR = '\3',
			UNCHECK_CHAR = ' '
		};

	protected:
		virtual int Action(unsigned flags, KeyNumType &key, int modifier) override;
		virtual void Draw_Entry(int index, int x, int y, int width, int selected) override;

	private:
		bool IsReadOnly;
};
