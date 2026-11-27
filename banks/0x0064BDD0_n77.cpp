// 0x0064BDD0 99.8% raw / 99.8% normalised (1455/1455 B), 2 diff lines, n77 twin sweep 27 Nov: body identical to 0x0064BDD0.cpp (splice after 0x0064B660, rename FUN_0064BDD0; 71 others MATCH). Left: numunits stored before Whom's first dword in the >1 loop.
// Robin's Extract_Compressed_Events (RA2 0x0061FA40, NEAR_EQUAL, 595 bytes off) forms: memcpy Whom from src, return(count) on empty, incoming local, units * 5, == 0 tests, [pos + 1] index, (char) slot, 2-D SavedUnitCounts all identical; his >1 loop order 38.2, char * src 98.3, do/while 99.3, ==0 branch order 97.3, memcpy length datasize 43.7.
extern xTargetClass DAT_00AFA468[64][128];

// FUNCTION: GAMEMD 0x0064BDD0
int __fastcall Extract_Compressed_Events(void * buf, int bufsize)
{
	int pos = 0;
	int leftover = bufsize;
	EventClass * event;
	int count = 0;
	int datasize = 0;
	EventClass eventdata;
	unsigned char numunits = 0;
	int sender;
	int index;
	int units;
	int reused;
	xTargetClass * src;
	xTargetClass * from;
	xTargetClass * dest;

	memset(&eventdata, 0, sizeof(EventClass));

	datasize = (offsetof(EventClass, Data) + sizeof(eventdata.Data.FrameInfo)) - sizeof(eventdata.Type);
	event = (EventClass *)(((char *)buf) + pos);
	sender = event->ID;

	while ((unsigned)leftover >= (datasize + sizeof(eventdata.Type))) {
		if (event->Type != EventClass::FRAMESYNC) {
			if (event->Type == EventClass::FRAMEINFO) {
				eventdata.Frame = event->Frame;
				eventdata.ID = event->ID;
				pos += (offsetof(EventClass, Data) - sizeof(eventdata.Type));
				leftover -= (offsetof(EventClass, Data) - sizeof(eventdata.Type));
			} else if (event->Type == EventClass::MEGAMISSION) {
				numunits = *(((unsigned char *)buf) + pos + sizeof(eventdata.Type));
				pos += sizeof(numunits);
				leftover -= sizeof(numunits);
			}

			memset(&eventdata.Data, 0, sizeof(eventdata.Data));
			eventdata.Type = event->Type;
			datasize = EventClass::EventLength[eventdata.Type];

			switch (eventdata.Type) {
			case EventClass::RESPONSE_TIME:
				memcpy(&eventdata.Data.FrameInfo.Delay, ((char *)buf) + pos + sizeof(eventdata.Type), datasize);
				break;

			case EventClass::ADDPLAYER:
				memcpy(&eventdata.Data.Variable.Size, ((char *)buf) + pos + sizeof(eventdata.Type), datasize);
				eventdata.Data.Variable.Pointer = new char[eventdata.Data.Variable.Size];
				memcpy(eventdata.Data.Variable.Pointer, ((char *)buf) + pos + sizeof(eventdata.Type) + datasize,
					   eventdata.Data.Variable.Size);
				pos += eventdata.Data.Variable.Size;
				leftover -= eventdata.Data.Variable.Size;
				break;

			case EventClass::MEGAMISSION:
				memcpy(&eventdata.Data.MegaMission, ((char *)buf) + pos + sizeof(eventdata.Type), datasize);

				if (numunits > 1) {
					pos += (datasize + sizeof(eventdata.Type));
					leftover -= (datasize + sizeof(eventdata.Type));
					datasize = sizeof(eventdata.Data.MegaMission.Whom);
					DebugString("Decoding new MEGAMISSION\n");
					index = eventdata.Data.MegaMission.Slot + sender * 8;
					DAT_00AFA468[index][0] = eventdata.Data.MegaMission.Whom;
					units = 1;
					DAT_00AC50FC[index] = units;
					if (!eventdata.Frame || !eventdata.Type) {
						DebugString("Frame %d:Empty megamission decoded\n", Frame);
					}
					if (numunits) {
						src = (xTargetClass *)(((char *)buf) + pos);
						dest = &DAT_00AFA468[index][units];
						while (numunits) {
							if (!DoList.Add(eventdata)) {
								return(-1);
							}
							eventdata.Data.MegaMission.Whom = *src;
							count++;
							numunits--;
							units++;
							memcpy(dest++, &eventdata.Data.MegaMission.Whom, sizeof(xTargetClass));
							DAT_00AC50FC[index] = units;
							if (numunits == 1) {
								datasize -= sizeof(eventdata.Type);
								DebugString("Got new unit list for player %d with %d units in slot %d\n", sender, units, eventdata.Data.MegaMission.Slot);
								break;
							} else {
								pos += datasize;
								src++;
								leftover -= datasize;
							}
						}
					}
				} else if (numunits == 0) {
					pos += (datasize + sizeof(eventdata.Type));
					index = eventdata.Data.MegaMission.Slot + sender * 8;
					numunits = DAT_00AC50FC[index];
					leftover -= (datasize + sizeof(eventdata.Type));
					datasize = sizeof(eventdata.Data.MegaMission.Whom);
					reused = 1;
					eventdata.Data.MegaMission.Whom = DAT_00AFA468[index][0];
					if (!eventdata.Frame || !eventdata.Type) {
						DebugString("Frame %d:Empty megamission decoded from slot %d. Player %d\n", Frame, eventdata.Data.MegaMission.Slot, sender);
					}
					if (numunits) {
						from = &DAT_00AFA468[index][reused];
						while (numunits) {
							if (!DoList.Add(eventdata)) {
								return(-1);
							}
							count++;
							numunits--;
							reused++;
							memcpy(&eventdata.Data.MegaMission.Whom, from++, sizeof(xTargetClass));
							if (numunits == 1) {
								datasize = -1;
								DebugString("Used old unit list in slot %d - %d bytes saved\n", eventdata.Data.MegaMission.Slot, reused * sizeof(xTargetClass));
								break;
							}
						}
					}
				}
				break;

			default:
				memcpy(&eventdata.Data, ((char *)buf) + pos + sizeof(eventdata.Type), datasize);
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

			if (!leftover) {
				break;
			}
			event = (EventClass *)(((char *)buf) + pos);
			datasize = EventClass::EventLength[event->Type];
			if (event->Type == EventClass::MEGAMISSION) {
				datasize += sizeof(numunits);
			}
		} else {
			pos += (datasize + sizeof(eventdata.Type));
			leftover -= (datasize + sizeof(eventdata.Type));
			event = (EventClass *)(((char *)buf) + pos);
			datasize = (offsetof(EventClass, Data) + sizeof(eventdata.Data.FrameInfo)) - sizeof(eventdata.Type);
		}
	}

	return(count);
}
