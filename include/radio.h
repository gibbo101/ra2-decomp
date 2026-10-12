// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "globals.h"
#include "mission.h"
#include "vector.h"

class ObjectClass;
class TechnoClass;

class RadioClass : public MissionClass
{
		typedef MissionClass BASECLASS;

	private:

		RadioMessageType Old[3];

		VectorClass<TechnoClass *> RadioLinks;

#ifdef _DEBUG

		static char const * Messages[RADIO_COUNT];
#endif

	public:

		RadioClass(void);
		RadioClass(NoInitClass const & x);
		virtual ~RadioClass(void) override {};

		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		bool In_Radio_Contact(void) const {return(RadioLinks[0] != 0);};
		bool HasAnyLink(void) const;
		void SetLinkCount(int count);
		void Radio_Off(void) {RadioLinks[0] = 0;};
		TechnoClass * Contact_With_Whom(void) const {return(RadioLinks[0]);};
		TechnoClass * FUN_0065AD30(int index);
		TechnoClass * FUN_0065AD40(int index) const;

		virtual void Detach(AbstractClass const * target, bool all = true) override;
		virtual void Compute_CRC(CRCEngine &) const override;
		virtual RadioMessageType Receive_Message(RadioClass * from, RadioMessageType message, int & param) override;
		virtual RadioMessageType Transmit_Message(RadioMessageType message, int & param, RadioClass * to=NULL);
		virtual RadioMessageType Transmit_Message(RadioMessageType message, RadioClass * to);
		virtual RadioMessageType Transmit_Message(RadioMessageType message);
		virtual void SendToEachLink(RadioMessageType command);
#ifdef _DEBUG
		virtual void Debug_Dump(MonoClass *mono) const override;
#endif
		virtual bool Limbo(void) override;
};
