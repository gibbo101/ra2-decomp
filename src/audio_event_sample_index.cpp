// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#include "always.h"
#include "audio_event_internal.h"
#include "audio_resource_internal.h"
#include "random.h"
namespace Randomizer { extern Random2Class Global; }

// FUNCTION: GAMEMD 0x00404BB0
int __fastcall FUN_00404BB0(AudioEvent * handle)
{
    AudioEventRuntime * event = static_cast<AudioEventRuntime *>(handle);
    VocClass * type = event->EventType;
    int control = type->Control;
    int result;
    if (event->SampleSelectionCount == event->SampleSelectionCursor) {
        int count = 0;
        if ((!(type->Control & 1) && event->SampleSelectionCycle == 1) ||
            ((type->Control & 1) && type->Loop && event->SampleSelectionCycle >= type->Loop)) return -1;
        if (type->Attack && !(event->Flags & 8)) {
            event->SampleSelection[0] = Randomizer::Global(0, type->Attack - 1);
            count = 1;
        }
        if (type->Control & 4) {
            for (int index = type->Attack; index < type->NumSamples - type->Decay; ++index)
                event->SampleSelection[count++] = index;
        } else {
            int index;
            if (type->Control & 2) {
                if (!event->SampleSelectionCycle) {
                    int minimum = type->Attack;
                    int maximum = type->NumSamples - type->Decay - 1;
                    index = Randomizer::Global(minimum, maximum);
                    event->StoredRandomSampleIndex = index;
                } else {
                    index = event->StoredRandomSampleIndex;
                }
            } else {
                index = type->Attack;
            }
            event->SampleSelection[count++] = index;
        }
        event->SampleSelectionCount = count;
        event->SampleSelectionCursor = 0;
        ++event->SampleSelectionCycle;
    }
    if (control & 2) {
        if (!type->Attack || (event->Flags & 8) || event->SampleSelectionCursor > 0) {
            int minimum = type->Attack && !(event->Flags & 8) ? 1 : 0;
            int maximum = event->SampleSelectionCount - 1;
            int selection = Randomizer::Global(minimum, maximum);
            result = event->SampleSelection[selection];
            for (int index = selection; index < event->SampleSelectionCount - 1; ++index)
                event->SampleSelection[index] = event->SampleSelection[index + 1];
            --event->SampleSelectionCount;
            goto done;
        }
    }
    result = event->SampleSelection[event->SampleSelectionCursor];
    ++event->SampleSelectionCursor;
done:
    return result;
}
