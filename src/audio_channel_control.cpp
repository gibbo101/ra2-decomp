// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#include "always.h"
#include "audio_channel_internal.h"
// FUNCTION: GAMEMD 0x00402430
int AudioChannel::FUN_00402430(void) { return Flags & 8; }
// FUNCTION: GAMEMD 0x00402440
void __fastcall AudioChannel::FUN_00402440(int type)
{
    Type = type;
    StopChannel(this);
    Flags |= 8;
}
// FUNCTION: GAMEMD 0x004024A0
void AudioChannel::FUN_004024A0(void)
{
    if (Flags & 8) { Flags &= ~8; StandardChannel(this); }
}
// FUNCTION: GAMEMD 0x00402550
void AudioChannel::FUN_00402550(void)
{
    StopChannel(this);
    FUN_004028B0();
    Driver->Close(this);
    ::operator delete(this);
}
// FUNCTION: GAMEMD 0x004025B0
void AudioChannel::FUN_004025B0(void) { StopChannel(this); }
// FUNCTION: GAMEMD 0x00402600
void AudioChannel::FUN_00402600(void)
{
    Driver->Lock(this);
    if (Flags & 1) {
        Flags &= ~1;
        Driver->Pause(this);
        Flags |= 4;
    }
    Driver->Unlock(this);
}
// FUNCTION: GAMEMD 0x00402650
void AudioChannel::FUN_00402650(void)
{
    Driver->Lock(this);
    if (Flags & 4) {
        Driver->Resume(this);
        Flags &= ~4;
        Flags |= 1;
    }
    Driver->Unlock(this);
}
// FUNCTION: GAMEMD 0x004026A0
void AudioChannel::FUN_004026A0(void) { Driver->Lock(this); }
// FUNCTION: GAMEMD 0x004026B0
void __fastcall AudioChannel::FUN_004026B0(void) { Driver->Unlock(this); }
// FUNCTION: GAMEMD 0x004026C0
void AudioChannel::FUN_004026C0(void)
{
    Driver->Lock(this);
    Flags |= 0x10;
    Driver->Unlock(this);
}
// FUNCTION: GAMEMD 0x004026F0
void AudioChannel::FUN_004026F0(void)
{
    Driver->Lock(this);
    Flags &= ~0x10;
    Driver->Unlock(this);
}
// FUNCTION: GAMEMD 0x00402720
int AudioChannel::FUN_00402720(void)
{
    if (Flags & 5) return 0x800F0000;
    if (!Sample) return 0x80070000;
    FUN_00401080(&Attributes);
    FUN_00402220();
    Driver->Lock(this);
    int result = Driver->Play(this);
    if (!result) Flags |= 1;
    Driver->Unlock(this);
    return result;
}
