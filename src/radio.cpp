// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "radio.h"

#include "_rtti.h"
#include "house.h"
#include "mono.h"
#include "swizzle.h"
#include "techno.h"
#include <new.h>

static inline int Index_Of(VectorClass<TechnoClass *> const & links, TechnoClass * link)
{
	for (int i = 0; i < links.Length(); i++) {
		if (links[i] == link) return i;
	}
	return -1;
}

// FUNCTION: 0x0065A750
RadioClass::RadioClass(void) :
	BASECLASS(),
	RadioLinks(1)
{
	RadioLinks[0] = NULL;
	for (int i = 0; i < 3; i++) {
		Old[i] = RADIO_STATIC;
	}
}

// FUNCTION: 0x0065A7E0
RadioClass::RadioClass(NoInitClass const & x) :
	BASECLASS(x),
	RadioLinks(x)
{
}

// FUNCTION: 0x0065A820
RadioMessageType RadioClass::Receive_Message(RadioClass * from, RadioMessageType message, int & param)
{
	if (message != Old[0]) {
		Old[2] = Old[1];
		Old[1] = Old[0];
		Old[0] = message;
	}

	if (message == RADIO_OVER_OUT) {
		for (int i = 0; i < RadioLinks.Length(); i++) {
			if ((TechnoClass *)from == RadioLinks[i]) {
				BASECLASS::Receive_Message(from, RADIO_OVER_OUT, param);
				RadioLinks[i] = NULL;
				return RADIO_ROGER;
			}
		}
	} else if (message == RADIO_HELLO && Strength) {
		if (!((TechnoClass *)from)->House->Is_Ally(this)) return RADIO_NEGATIVE;
		if (this != NULL && (AbstractFlags & 1) && !((TechnoClass *)this)->House->Is_Ally(from)) return RADIO_NEGATIVE;
		if (from != NULL) {
			if (Index_Of(RadioLinks, (TechnoClass *)from) != -1) return RADIO_ROGER;
		}
		for (int i = 0; i < RadioLinks.Length(); i++) {
			if (RadioLinks[i] == NULL) {
				RadioLinks[i] = (TechnoClass *)from;
				return RADIO_ROGER;
			}
		}
		return RADIO_NEGATIVE;
	}
	return BASECLASS::Receive_Message(from, message, param);
}

// FUNCTION: 0x0065A970
RadioMessageType RadioClass::Transmit_Message(RadioMessageType message, int & param, RadioClass * to)
{
	if (to == NULL) {
		to = (RadioClass *)RadioLinks[0];
		if (to == NULL) return RADIO_STATIC;
	}

	if (message == RADIO_OVER_OUT) {
		for (int i = 0; i < RadioLinks.Length(); i++) {
			if (RadioLinks[i] == (TechnoClass *)to) {
				RadioLinks[i] = NULL;
			}
		}
	} else if (message == RADIO_HELLO) {
		int slot = -1;
		int i;
		for (i = 0; i < RadioLinks.Length(); i++) {
			if (slot == -1 && RadioLinks[i] == NULL) slot = i;
			if (RadioLinks[i] == (TechnoClass *)to) return RADIO_ROGER;
		}
		if (slot == -1) {
			Transmit_Message(RADIO_OVER_OUT, RadioLinks[0]);
			slot = 0;
		}
		if (to->Receive_Message(Dynamic_Cast<TechnoClass *>(this), RADIO_HELLO, param) == RADIO_ROGER) {
			RadioLinks[slot] = (TechnoClass *)to;
			return RADIO_ROGER;
		}
		return RADIO_NEGATIVE;
	}
	return to->Receive_Message(Dynamic_Cast<TechnoClass *>(this), message, param);
}

// FUNCTION: 0x0065AA80
bool RadioClass::Limbo(void)
{
	if (!IsInLimbo) {
		SendToEachLink(RADIO_OVER_OUT);
	}
	return BASECLASS::Limbo();
}

// FUNCTION: 0x0065AAA0
RadioMessageType RadioClass::Transmit_Message(RadioMessageType message, RadioClass * to)
{
	return Transmit_Message(message, LParam, to);
}

// FUNCTION: 0x0065AAC0
void RadioClass::Detach(AbstractClass const * target, bool all)
{
	BASECLASS::Detach(target, all);
	for (int i = 0; i < RadioLinks.Length(); i++) {
		if (RadioLinks[i] == (TechnoClass const *)target && all) {
			RadioLinks[i] = NULL;
		}
	}
}

// FUNCTION: 0x0065AB10
void RadioClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(RadioLinks.Length());
	for (int i = 0; i < RadioLinks.Length(); i++) {
		if (RadioLinks[i] != NULL) {
			crc(RadioLinks[i]->Fetch_ID());
			crc((int)RadioLinks[i]->Fetch_RTTI());
		}
	}
}

// FUNCTION: 0x0065AB80
HRESULT STDMETHODCALLTYPE RadioClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		int count;
		new (&RadioLinks) VectorClass<TechnoClass *>(0);
		result = stream->Read(&count, sizeof(count), NULL);
		if (SUCCEEDED(result)) {
			RadioLinks.Resize(count);
			int i;
			for (i = 0; i < count; i++) {
				TechnoClass * link;
				result = stream->Read(&link, sizeof(link), NULL);
				if (FAILED(result)) return result;
				RadioLinks[i] = link;
			}
			for (i = 0; i < count; i++) {
				Swizzle_Pointer(&RadioLinks[i]);
			}
			result = S_OK;
		}
	}
	return result;
}

// FUNCTION: 0x0065AC40
HRESULT STDMETHODCALLTYPE RadioClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		int count = RadioLinks.Length();
		result = stream->Write(&count, sizeof(count), NULL);
		if (SUCCEEDED(result)) {
			for (int i = 0; i < count; i++) {
				result = stream->Write(&RadioLinks[i], sizeof(TechnoClass *), NULL);
				if (FAILED(result)) return result;
			}
			result = S_OK;
		}
	}
	return result;
}

// FUNCTION: 0x0065ACB0
RadioMessageType RadioClass::Transmit_Message(RadioMessageType message)
{
	if (RadioLinks[0] != NULL) {
		return Transmit_Message(message, LParam, (RadioClass *)RadioLinks[0]);
	}
	return RADIO_STATIC;
}

// FUNCTION: 0x0065ACE0
void RadioClass::SendToEachLink(RadioMessageType command)
{
	for (int i = 0; i < RadioLinks.Length(); i++) {
		if (RadioLinks[i] != NULL) {
			Transmit_Message(command, LParam, (RadioClass *)RadioLinks[i]);
		}
	}
}

// FUNCTION: 0x0065AD30
TechnoClass * RadioClass::FUN_0065AD30(int index)
{
	return RadioLinks[index];
}
