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
		CheckObject(wchar_t const * text = 0, bool checked=false) :
			Text(text),
			IsChecked(checked)
		{};

		wchar_t const * Text;
		bool IsChecked;
};

class CheckListClass : public ListClass
{
		typedef ListClass BASECLASS;

	public:

		CheckListClass(int id, int x, int y, int w, int h, TextPrintType flags,
			ShapeSet const * up, ShapeSet const * down);
		~CheckListClass(void);

		virtual int Add_Item(wchar_t const * text) override;
		virtual wchar_t const * Current_Item(void) const override;
		virtual wchar_t const * Get_Item(int index) const override;
		virtual void Remove_Item(wchar_t const * text) override;
		virtual void Remove_Item(int text) override {BASECLASS::Remove_Item(text);}
		virtual void Set_Selected_Index(wchar_t const * text) override;
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
