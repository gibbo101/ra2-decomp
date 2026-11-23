// 0x00793D20 Start_Talking_To 51.3% (2685/2699 bytes, norm 78.8%) on main 8a10b56, no overlay; splice before '// FUNCTION: GAMEMD 0x007947B0'. Natural port of Renegade NAT.cpp FirewallHelperClass::Negotiate_Port's do { } while (true) loop with break exits, 'trying++; Send_Connection_Result(CONNRESULT_TRY1 + trying, 0)' tail (FUN_00793B30(trying + 1, ...)) and loop-scoped locals; inlinebudget: no differing decisions; localprobe 0-16 and declprobe 0-12 flat. Blocker: ebx goes to trying in the outer loop (exe: session_players_index) and gotit is never enregistered (exe: bl from the probe loop through the tail); two diagnostic uses of session_players_index before 'their_port =' reach 69.7%.
// FUNCTION: GAMEMD 0x00793D20
unsigned char __fastcall FUN_00793D20(int partner, int tag, int retries)
{
	CDTimerClass<SystemTimerClass> timer;
	CDTimerClass<SystemTimerClass> resend;
	bool mangling = true;
	int len = 0;
	IPXAddressClass address;
	int index_in = 0;
	bool gotit = false;
	int i;

	FUN_004068E0("Start_Talking_To player %d, retries = %d\n", partner, retries);

	int session_players_index = -1;
	for (i = 0; i < Session.Players.Count(); i++) {
		NodeNameType * player = Session.Players[i];
		NodeNameType * other = Session.Slots[partner];
		char const * name = FUN_007350C0(player->Name);
		if (stricmp(name, FUN_007350C0(other->Name)) == 0) {
			session_players_index = i;
		}
	}
	if (session_players_index == -1) {
		FUN_00793B30(0, NULL);
		FUN_004068E0("session_players_index == %d, returning false from Start_Talking_To.\n", session_players_index);
		return false;
	}

	for (i = 0; i < MAX_PLAYERS; i++) {
		if (Session.unknown_2E28[i][0] == 0) {
			break;
		}
		if (stricmp(Session.unknown_2E28[i], FUN_007350C0(Session.Slots[partner]->Name)) == 0) {
			FUN_004068E0("Behind the same NAT, mangling == false\n");
			mangling = false;
			break;
		}
	}

	int theirs = Session.PlayerFirewall[partner];
	if (mangling) {
		if (theirs == 0 || (theirs & 1)) {
			int mine = (unsigned short)FirewallHelper.Detected;
			if (mine == 0 || (mine & 1)) {
				FUN_004068E0("Both have crap firewalls, or modems, or somesuch. mangling = false\n");
				mangling = false;
			}
		}
	}

	int trying = 0;
	unsigned short their_port = 0;
	unsigned short my_port = 0;

	do {
		FUN_004068E0("Sending keep alive packets\n");
		FUN_00793BB0();

		if (!mangling) {
			unsigned short port = Session.PlayerPorts[session_players_index];
			FUN_004068E0("Easy mangling, Session.OtherPlayersMangledPorts[%d] = %d\n", partner, port);
			Session.unknown_2E18[partner] = port;
		} else {
			int behavior = (unsigned short)FirewallHelper.Detected;
			unsigned short port = 0;
			FUN_004068E0("Raw firewall behavior is %d\n", behavior);
			if (!(behavior & 0x40)) {
				FUN_004068E0("No destination port delta - looking for earlier connections at the same address\n");
				for (int j = 0; j < Session.KeepAliveCount; j++) {
					int slot = Session.KeepAliveSlots[j];
					IPXAddressClass old = Session.Slots[slot]->Address;
					if (old.FUN_0053EF30(Session.Slots[partner]->Address)) {
						port = Session.unknown_2F4C[slot];
						FUN_004068E0("Found old connection port number to use - port number %d\n", port);
					}
				}
			}
			if (port == 0) {
				FUN_004068E0("Work out what my port will be.\n");
				for (i = 0; i < Session.Players.Count(); i++) {
					char const * name = FUN_007350C0(Session.Players[i]->Name);
					if (stricmp(name, g_NickName.get()) == 0) {
						unsigned short def = Session.PlayerPorts[i];
						FUN_004068E0("My default port = %d\n", def);
						port = FirewallHelper.Get_Next_Mangled_Source_Port(def);
						FUN_004068E0("My mangled port = %d\n", port);
						break;
					}
				}
			}
			FUN_00793A70(FUN_007350C0(Session.Slots[partner]->Name), port);
			my_port = port;
		}

		timer = 360;
		if (trying > 0) {
			timer = 240;
		}
		while (timer && ((mangling == true && Session.unknown_2E18[partner] == their_port) || (mangling == false && Session.unknown_2E18[partner] == 0))) {
			WOL_Wait_Callback();
		}

		their_port = Session.unknown_2E18[partner];
		if (their_port != 0) {
			FUN_004068E0("Session.OtherPlayersMangledPorts[%d] != 0\n", partner);
			Session.PlayerPorts[session_players_index] = Session.unknown_2E18[partner];
			Session.Players[session_players_index]->Address.Set_Port(Session.unknown_2E18[partner]);
			Session.Slots[partner]->Address.Set_Port(Session.unknown_2E18[partner]);

			GlobalPacketType packet;
			memset(&packet, 0, 0x2F);
			packet.Command = NET_PORT_PROBE;
			strcpy(packet.Name, g_NickName.get());

			if ((FirewallHelper.Detected & 8) && !(Session.PlayerFirewall[partner] & 8)) {
				FUN_004068E0("Doing the netgear sleep thing\n");
				CDTimerClass<SystemTimerClass> sleep = 240;
				while (sleep) {
					WOL_Wait_Callback();
				}
			}

			FUN_004068E0("Sending NET_PORT_PROBE packet to %s at port %d\n", FUN_007350C0(Session.Slots[partner]->Name), Session.PlayerPorts[session_players_index]);
			timer = 360;
			resend = 0;
			do {
				if (!resend) {
					FUN_004068E0("Sending NET_PORT_PROBE packet to player %S\n", Session.Slots[partner]->Name);
					Ipx.Send_Global_Message_To_Name(&packet, 0x2F, 0, Session.Slots[partner]->Name);
					resend = 30;
				}
				WOL_Wait_Callback();
				if (!gotit) {
					if (index_in >= Ipx.Global_Num_Receive()) {
						index_in = 0;
					}
					GlobalPacketType reply;
					unsigned short product_id;
					if (Ipx.Global_Channel_Op(index_in, (int)&reply, (int)&len, (int)&address, (int)&product_id, 0) && product_id == 0xAA04) {
						if (reply.Command == NET_PING || reply.Command == NET_PORT_PROBE) {
							if (reply.Command == NET_PORT_PROBE) {
							FUN_004068E0("Got NET_PORT_PROBE packet\n");
							if (Session.Players[session_players_index]->Address.FUN_0053EF30(address) && stricmp(reply.Name, FUN_007350C0(Session.Slots[partner]->Name)) == 0) {
								FUN_004068E0("Got NET_PORT_PROBE packet from %s\n", reply.Name);
								FUN_00793B30(11, reply.Name);
								FUN_007931F0(session_players_index);
								FUN_00793CD0(partner);
								gotit = true;
								Session.unknown_2F4C[partner] = my_port;
							}
							}
							Ipx.Global_Channel_Op_3C(index_in);
						}
					}
				}
				if (gotit && Session.ConnectionResults[tag] == 11) {
					FUN_004068E0("Got connection result - my packet must have got through\n");
					break;
				}
				int result = Session.ConnectionResults[tag];
				if (result >= 1 && result < 11) {
					int their_try = result - 1;
					if (their_try > trying) {
						FUN_004068E0("Other player is trying again - their_try = %d\n", their_try);
						trying = their_try - 1;
						break;
					}
				}
				index_in++;
			} while (timer);
		}

		if (gotit && Session.ConnectionResults[tag] == 11) {
			FUN_004068E0("gotit && Session.ConnectionResults[completion_index] == CONNRESULT_CONNECTED\n");
			break;
		}
		if (trying >= retries) {
			FUN_004068E0("trying >= retries\n");
			break;
		}
		trying++;
		FUN_00793B30(trying + 1, FUN_007350C0(Session.Slots[partner]->Name));
		FUN_004068E0("Trying again - trying = %d\n", trying);
	} while (true);
	return gotit;
}
