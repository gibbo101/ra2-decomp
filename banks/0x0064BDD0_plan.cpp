// 0x0064BDD0 best 48.4% raw / 64.8% normalised (1448/1455 B), no overlay; splice after 0x0064B660 in src/queue.cpp and rename FUN_0064BDD0 (declaration and call in 0x0064A0C0). The other form, 0x0064BDD0_plan_exit.cpp, has the exe's shape but a lower score.
extern xTargetClass DAT_00AFA468[64][128];

// FUNCTION: GAMEMD 0x0064BDD0
int __fastcall Extract_Compressed_Events(void *buf, int bufsize)
{
	int pos = 0;
	int leftover = bufsize;
	EventClass *event;
	int count = 0;
	int datasize = 0;
	EventClass eventdata;
	unsigned char numunits = 0;
	int index;
	int cached;
	xTargetClass *src;
	xTargetClass *dest;

	memset(&eventdata, 0, sizeof(EventClass));
	datasize = (offsetof(EventClass, Data) + sizeof(eventdata.Data.FrameInfo)) - sizeof(eventdata.Type);
	event = (EventClass *)(((char *)buf) + pos);
	int sender = event->ID;

	while ((unsigned)leftover >= (datasize + sizeof(eventdata.Type))) {

		if (event->Type != EventClass::FRAMESYNC) {
			if (event->Type == EventClass::FRAMEINFO) {
				eventdata.Frame = event->Frame;
				eventdata.ID = event->ID;

				pos += (offsetof(EventClass, Data) - sizeof(eventdata.Type));
				leftover -= (offsetof(EventClass, Data) - sizeof(eventdata.Type));
			}
			else if (event->Type == EventClass::MEGAMISSION) {
				numunits = *(((unsigned char *)buf) + pos + sizeof(eventdata.Type));
				pos += sizeof(numunits);
				leftover -= sizeof(numunits);
			}

			memset(&eventdata.Data, 0, sizeof(eventdata.Data));
			eventdata.Type = event->Type;
			datasize = EventClass::EventLength[eventdata.Type];

			switch (eventdata.Type) {
				case (EventClass::RESPONSE_TIME):
					memcpy(&eventdata.Data.FrameInfo.Delay,
						((char *)buf) + pos + sizeof(eventdata.Type),
						datasize);
					break;

				case (EventClass::ADDPLAYER):
					memcpy(&eventdata.Data.Variable.Size,
						((char *)buf) + pos + sizeof(eventdata.Type),
						datasize);

					eventdata.Data.Variable.Pointer = new char[eventdata.Data.Variable.Size];
					memcpy(eventdata.Data.Variable.Pointer,
						((char *)buf) + pos + sizeof(eventdata.Type) + datasize,
						eventdata.Data.Variable.Size);

					pos += eventdata.Data.Variable.Size;
					leftover -= eventdata.Data.Variable.Size;
					break;

				case (EventClass::MEGAMISSION):
					memcpy(&eventdata.Data.MegaMission,
						((char *)buf) + pos + sizeof(eventdata.Type),
						datasize);

					if (numunits > 1) {
						pos += (datasize + sizeof(eventdata.Type));
						leftover -= (datasize + sizeof(eventdata.Type));
						datasize = sizeof(eventdata.Data.MegaMission.Whom);
						DebugString("Decoding new MEGAMISSION\n");

						index = eventdata.Data.MegaMission.Slot + sender * 8;
						DAT_00AFA468[index][0] = eventdata.Data.MegaMission.Whom;
						cached = 1;
						DAT_00AC50FC[index] = 1;

						if (!eventdata.Frame || !eventdata.Type) {
							DebugString("Frame %d:Empty megamission decoded\n", Frame);
						}

						if (numunits) {
							src = (xTargetClass *)(((char *)buf) + pos);
							dest = &DAT_00AFA468[index][1];
							do {
								if (!DoList.Add(eventdata)) {
									return(-1);
								}
								count++;
								*dest = eventdata.Data.MegaMission.Whom = *src;
								numunits--;
								cached++;
								dest++;
								DAT_00AC50FC[index] = cached;
								if (numunits == 1) {
									datasize = sizeof(eventdata.Data.MegaMission.Whom) - sizeof(eventdata.Type);
									DebugString("Got new unit list for player %d with %d units in slot %d\n", sender, cached, eventdata.Data.MegaMission.Slot);
									break;
								}
								pos += datasize;
								src++;
								leftover -= datasize;
							} while (numunits);
						}
					}
					else if (numunits == 0) {
						index = eventdata.Data.MegaMission.Slot + sender * 8;
						cached = 1;
						pos += (datasize + sizeof(eventdata.Type));
						leftover -= (datasize + sizeof(eventdata.Type));
						datasize = sizeof(eventdata.Data.MegaMission.Whom);
						numunits = DAT_00AC50FC[index];
						eventdata.Data.MegaMission.Whom = DAT_00AFA468[index][0];

						if (!eventdata.Frame || !eventdata.Type) {
							DebugString("Frame %d:Empty megamission decoded from slot %d. Player %d\n", Frame, eventdata.Data.MegaMission.Slot, sender);
						}

						if (numunits) {
							src = &DAT_00AFA468[index][1];
							do {
								if (!DoList.Add(eventdata)) {
									return(-1);
								}
								count++;
								cached++;
								numunits--;
								eventdata.Data.MegaMission.Whom = *src++;
								if (numunits == 1) {
									datasize = -1;
									DebugString("Used old unit list in slot %d - %d bytes saved\n", eventdata.Data.MegaMission.Slot, cached * sizeof(xTargetClass));
									break;
								}
							} while (numunits);
						}
					}
					break;

				default:
					memcpy(&eventdata.Data,
						((char *)buf) + pos + sizeof(eventdata.Type),
						datasize);
					break;
			}

			if (!eventdata.Frame || !eventdata.Type) {
				DebugString("Frame %d: Empty event being added to queue\n", Frame);
			}

			if (!DoList.Add(eventdata)) {
				if (eventdata.Type == EventClass::ADDPLAYER) {
					delete [] eventdata.Data.Variable.Pointer;
				}
				return(-1);
			}

			count++;

			pos += (datasize + sizeof(eventdata.Type));
			leftover -= (datasize + sizeof(eventdata.Type));

			if (leftover) {
				event = (EventClass *)(((char *)buf) + pos);
				datasize = EventClass::EventLength[event->Type];
				if (event->Type == EventClass::MEGAMISSION) {
					datasize += sizeof(numunits);
				}
			}
		}
		else {
			pos += (datasize + sizeof(eventdata.Type));
			leftover -= (datasize + sizeof(eventdata.Type));
			event = (EventClass *)(((char *)buf) + pos);

			datasize = (offsetof(EventClass, Data) + sizeof(eventdata.Data.FrameInfo)) - sizeof(eventdata.Type);
		}
	}

	return(count);
}
