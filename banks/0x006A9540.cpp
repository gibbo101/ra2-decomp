// best natural form: 76.5% raw, 97.3% normalised (4276/4280 bytes), no overlay; insert after FUN_006A94F0 in src/sidebar.cpp. Not a match.
// Diagnostic only, not source: `int & slotrow = row;` declared in the cameo loop body and read once, in the index sum (`col + (slotrow + TopIndex) * columns`), gives 90.4% raw, 98.4% normalised; 9 to 15 dead `int` locals on top of that give 90.7% / 98.6%, leaving only the observer loop head.
// Left: (1) both index sums: the exe adds TopIndex to a copy of row (`mov r,row; add r,top`), ours loads TopIndex and adds row. The observer head moves with no form tried; the cameo head moves only with the pointer above, and only when it is assigned in the same block and read once.
// (2) the row-count operand order (exe Height - F8 - reserved + Y, ours Height - reserved - F8 + Y). It follows the function's variable numbering in words of 32: right when 9 to 15 more variables are numbered before the fifth case of the country switch and no more than 15 more in total (Strip_Rows' own locals count in the total). A variable is a local that is assigned (1 each, 2 for a Point2D, 2 for a reference bound to a temporary) or a global first referenced in the function; unassigned locals, arrays, statics and inline parameters do not count.
// (3) with the twelve country-flag globals written as members of one object the row count is right with no extra locals, but the hover pixel's blue and green terms swap; Map.Max_Visible() in place of Strip_Rows() swaps them too. 18 or more variables before `row` turn H * row round.
extern RGBClass DAT_00B0FA1C;
extern wchar_t DAT_00B078B0[];
ConvertClass * FUN_0072F4D0(void);
void __fastcall Print_Cameo_Text(wchar_t const * text, Point2D const & point, Rect const & window, int width);
void __fastcall ODFillRectTrans(Rect * rect, Surface * surface, unsigned short color, int alpha);

// FUNCTION: GAMEMD 0x006A9540
void SidebarClass::StripClass::Draw_It(bool complete)
{
	if (AllowedToDraw) {
		if (IsToRedraw || complete) {
			IsToRedraw = false;
			IsToBlitSidebar = true;
			Rect cliprect;
			cliprect.X = 0;
			cliprect.Y = 0;
			cliprect.Width = SidebarSurface->Get_Width();
			cliprect.Height = SidebarSurface->Get_Height();

			bool inproduction = false;
			int index = 0;
			if (PlayerPtr == DAT_00AC1198) {
				index = BuildableCount;
			}
			for (; index < BuildableCount; index++) {
				if (Buildables[index].unknown_10 == 1 || Buildables[index].unknown_10 == 2) {
					inproduction = true;
					break;
				}
			}

			int rows = Strip_Rows() + (IsScrolling ? 1 : 0);
			int columns = 2;
			int row = 0;

			if (PlayerPtr != DAT_00AC1198) {
				for (; row < rows; row++) {
					for (int col = 0; col < columns; col++) {
						bool redraw = complete;
						int stage = 0;
						ShapeSet const * shapefile = NULL;
						FactoryClass * factory = NULL;
						wchar_t const * state = NULL;
						int index = col + (row + TopIndex) * columns;
						bool production = false;
						bool completed = false;
						bool darken = false;
						int x = X + SidebarClass::FUN_006A8080(col) - SidebarRect.X;
						int y = Y + SidebarClass::FUN_006A8090(row);
						bool hover = false;
						SelectClass & button = SelectButton[ID * 60 + row * columns + col];

						if (IsScrolling) {
							y += Slid - DAT_00B0B500;
						}

						wchar_t const * name = NULL;
						TechnoTypeClass const * obj = NULL;

						if (index < BuildableCount) {
							hover = button.FUN_006ABA20();

							if (Buildables[index].BuildableType != RTTI_SPECIAL) {
								obj = Fetch_Techno_Type(Buildables[index].BuildableType, Buildables[index].BuildableID);
								if (obj != NULL) {
									name = obj->UIName;

									bool isbusy = false;
									if (obj->Fetch_RTTI() == RTTI_BUILDINGTYPE && inproduction) {
										isbusy = true;
									} else if (obj->Who_Can_Build_Me(true, true, true, PlayerPtr) == NULL) {
										isbusy = true;
									} else {
										TechnoTypeClass const * type = Fetch_Techno_Type(Buildables[index].BuildableType, Buildables[index].BuildableID);
										production = PlayerPtr->Can_Build(type, false, false) == -1;
										bool limited = PlayerPtr->FUN_0050B370(type);
										if (production || limited) {
											isbusy = true;
										}
									}

									shapefile = (ShapeSet const *)obj->Get_Cameo_Data();
									factory = Buildables[index].Factory;
									if (factory != NULL) {
										production = true;
										completed = factory->Has_Completed();
										if (completed) {
											state = StringTable::LoadString("TXT_READY", NULL, "D:\\ra2mdpost\\Sidebar.CPP", 3411);
										} else {
											redraw = true;
										}
										stage = FUN_006A94F0(index);
										darken = false;
									} else {
										darken = isbusy;
										production = false;
										switch (Buildables[index].unknown_10) {
											case 1:
												redraw = true;
												production = true;
												stage = FUN_006A94F0(index);
												darken = false;
												break;

											case 3:
												production = true;
												completed = true;
												darken = false;
												break;

											case 2:
												production = true;
												completed = false;
												darken = true;
												stage = FUN_006A94F0(index);
												break;

											case 0:
												if (!complete) {
													continue;
												}
												break;
										}
									}
								} else {
									darken = false;
								}
							} else {
								SuperWeaponType spc = SuperWeaponType(Buildables[index].BuildableID);
								name = SuperWeaponTypes[spc]->UIName;
								shapefile = Get_Special_Cameo(spc);

								production = true;
								completed = PlayerPtr->SuperWeapon[spc]->ShouldDrawProgress() == false;
								state = PlayerPtr->SuperWeapon[spc]->NameReadiness();
								stage = PlayerPtr->SuperWeapon[spc]->Anim_Stage();
								darken = false;
								redraw = true;
							}
						} else {
							production = false;
						}

						Rect rect(0, 0, x + 60, y + 48);
						if (!redraw) {
							continue;
						}

						if (shapefile != NULL) {
							Draw_Shape(*SidebarSurface, *CameoDrawer, shapefile, 0, Point2D(x, y), rect, SHAPE_WIN_REL);
						}

						if (hover) {
							Rect box(x, y, 60, 48);
							RGBClass color = DAT_00B0FA1C;
							SidebarSurface->Draw_Rect(rect, box, DSurface::Build_Hicolor_Pixel(color));
						}

						if (darken) {
							Draw_Shape(*SidebarSurface, *SidebarDrawer, (ShapeSet const *)FileSystem::DARKEN_SHP, 0, Point2D(x, y), rect, ShapeFlags_Type(SHAPE_WIN_REL|SHAPE_DARKEN));
						}

						if (Buildables[index].FlashEndFrame > Frame && Frame % 16 > 8) {
							Draw_Shape(*SidebarSurface, *SidebarDrawer, (ShapeSet const *)FileSystem::DARKEN_SHP, 0, Point2D(x, y), rect, ShapeFlags_Type(SHAPE_WIN_REL|SHAPE_TRANSLUCENT50));
						}

						if (name != NULL) {
							Print_Cameo_Text(name, Point2D(x, y + 36), cliprect, 60);
						}

						int color = DSurface::Build_Hicolor_Pixel(DAT_00B0FA1C);
						bool hasqueuecount = false;
						if (obj != NULL) {
							FactoryClass * builder = PlayerPtr->Fetch_Factory(obj->Fetch_RTTI(), obj->Naval, 0);
							if (builder != NULL) {
								int total = builder->Total(obj);
								if (total > 1 || total > 0 && !builder->Is_Currently_Producing(obj)) {
									wchar_t buffer[256];
									swprintf(buffer, L"%d", total);
									Point2D pt(x + 60, y + 1);
									Rect box = Drawing_GetTextDimensions(buffer, pt, 0x242, 2, 1);
									ODFillRectTrans(&box, SidebarSurface, 0, 0xaf);
									Fancy_Text_Print_Wide(L"%d", SidebarSurface, &cliprect, &pt, color, 0, TextPrintType(0x242), total);
									hasqueuecount = true;
								}
							}
						}

						if (production) {
							if (state != NULL) {
								Point2D pt(x + 30, y + 1);
								Rect box = Drawing_GetTextDimensions(state, pt, 0x142, 2, 1);
								ODFillRectTrans(&box, SidebarSurface, 0, 0xaf);
								Fancy_Text_Print_Wide(state, SidebarSurface, &cliprect, &pt, color, 0, TextPrintType(0x142));
							}

							if (!completed) {
								Draw_Shape(*SidebarSurface, *SidebarDrawer, (ShapeSet const *)FileSystem::GCLOCK2_SHP, stage + 1, Point2D(x, y), rect, ShapeFlags_Type(SHAPE_WIN_REL|SHAPE_TRANSLUCENT50));

								if (factory && !factory->Is_Building() || Buildables[index].unknown_10 == 2) {
									wchar_t const * hold = StringTable::LoadString("TXT_HOLD", NULL, "D:\\ra2mdpost\\Sidebar.CPP", 3636);
									if (hasqueuecount) {
										Point2D pt(x + 2, y + 1);
										Rect box = Drawing_GetTextDimensions(hold, pt, 0x42, 2, 1);
										ODFillRectTrans(&box, SidebarSurface, 0, 0xaf);
										Fancy_Text_Print_Wide(hold, SidebarSurface, &cliprect, &pt, color, 0, TextPrintType(0x42));
									} else {
										Point2D pt(x + 30, y + 1);
										Rect box = Drawing_GetTextDimensions(hold, pt, 0x142, 2, 1);
										ODFillRectTrans(&box, SidebarSurface, 0, 0xaf);
										Fancy_Text_Print_Wide(hold, SidebarSurface, &cliprect, &pt, color, 0, TextPrintType(0x142));
									}
								}
							}
						}
					}
				}
			}

			if (PlayerPtr == DAT_00AC1198) {
				int count = rows / 2;
				for (row = 0; row < count; row++) {
					int index = row + TopIndex;
					int x = X - SidebarRect.X;
					int y = Y + SidebarClass::FUN_006A8090(row * 2);
					if (index >= Map.DiplomacyNumHouses) {
						break;
					}
					HouseClass * house = Map.DiplomacyHouses[index];
					if (house == NULL) {
						break;
					}

					if (house->Class != NULL) {
						ShapeSet const * logo = NULL;
						switch (house->Class->Side) {
							case 0:
								logo = (ShapeSet const *)DAT_00B0B490;
								break;
							case 1:
								logo = (ShapeSet const *)DAT_00B0B494;
								break;
							case 2:
								logo = (ShapeSet const *)DAT_00B0B498;
								break;
						}
						if (logo != NULL) {
							Rect rect(0, 0, x + logo->Get_Width(), y + logo->Get_Height());
							Draw_Shape(*SidebarSurface, *DAT_0087F6D0, logo, 0, Point2D(x, y), rect, SHAPE_WIN_REL);
						}
					}

					if (house->Class != NULL) {
						ShapeSet const * flag = NULL;
						int country = house->Class->House;
						switch (country) {
							case -2: flag = (ShapeSet const *)DAT_00B0B49C; break;
							case 0: flag = (ShapeSet const *)DAT_00B0B4A4; break;
							case 1: flag = (ShapeSet const *)DAT_00B0B4A8; break;
							case 2: flag = (ShapeSet const *)DAT_00B0B4AC; break;
							case 3: flag = (ShapeSet const *)DAT_00B0B4B0; break;
							case 4: flag = (ShapeSet const *)DAT_00B0B4B4; break;
							case 5: flag = (ShapeSet const *)DAT_00B0B4B8; break;
							case 6: flag = (ShapeSet const *)DAT_00B0B4BC; break;
							case 7: flag = (ShapeSet const *)DAT_00B0B4C0; break;
							case 8: flag = (ShapeSet const *)DAT_00B0B4C4; break;
							case -3: flag = (ShapeSet const *)DAT_00B0B4A0; break;
							case 9: flag = (ShapeSet const *)DAT_00B0B4C8; break;
						}
						if (flag != NULL) {
							if (country == 9) {
								ConvertClass * drawer = FUN_0072F4D0();
								Rect rect(0, 0, x + flag->Get_Width() + 70, y + flag->Get_Height() + 70);
								Draw_Shape(*SidebarSurface, *drawer, flag, 0, Point2D(x + 70, y + 70), rect, SHAPE_WIN_REL);
							} else {
								Rect rect(0, 0, x + flag->Get_Width() + 70, y + flag->Get_Height() + 70);
								Draw_Shape(*SidebarSurface, *DAT_0087F6D0, flag, 0, Point2D(x + 70, y + 70), rect, SHAPE_WIN_REL);
							}
						}
					}

					wchar_t text[64];
					wchar_t rank[20];
					wchar_t kills[20];
					wchar_t owned[20];
					wchar_t money[20];

					wcscpy(DAT_00B078B0, house->UIName);
					if (Session.Type == GAME_INTERNET) {
						_itow(Map.DiplomacyRung[index], rank, 10);
					}
					_itow(Map.DiplomacyKills[index], kills, 10);
					_itow(Map.DiplomacyOwned[index], owned, 10);
					_itow(Map.DiplomacyCredits[index], money, 10);
					ColorScheme * scheme = Map.DiplomacyColors[index];

					if (IsScrolling) {
						y += Slid - DAT_00B0B500;
					}

					Point2D pt(x + 8, y + 4);
					Fancy_Text_Print_Wide(DAT_00B078B0, SidebarSurface, &cliprect, &pt, scheme, 0, TextPrintType(0x42));

					if (Session.Type == GAME_INTERNET) {
						pt.Y += 17;
						if (Map.DiplomacyRung[index] < 1) {
							wcscpy(text, StringTable::LoadString("TXT_UNRANKED", NULL, "D:\\ra2mdpost\\Sidebar.CPP", 3781));
						} else {
							swprintf(text, StringTable::LoadString("GUI:ObsRank", NULL, "D:\\ra2mdpost\\Sidebar.CPP", 3785), rank);
						}
						Fancy_Text_Print_Wide(text, SidebarSurface, &cliprect, &pt, scheme, 0, TextPrintType(0x42));
					}

					pt.Y += 17;
					swprintf(text, StringTable::LoadString("GUI:ObsKills", NULL, "D:\\ra2mdpost\\Sidebar.CPP", 3807), kills);
					Fancy_Text_Print_Wide(text, SidebarSurface, &cliprect, &pt, scheme, 0, TextPrintType(0x42));

					pt.Y += 17;
					swprintf(text, StringTable::LoadString("GUI:ObsUnits", NULL, "D:\\ra2mdpost\\Sidebar.CPP", 3822), owned);
					Fancy_Text_Print_Wide(text, SidebarSurface, &cliprect, &pt, scheme, 0, TextPrintType(0x42));

					pt.Y += 17;
					swprintf(text, StringTable::LoadString("GUI:ObsCredits", NULL, "D:\\ra2mdpost\\Sidebar.CPP", 3835), money);
					Fancy_Text_Print_Wide(text, SidebarSurface, &cliprect, &pt, scheme, 0, TextPrintType(0x42));
				}
			}

			LastSlid = Slid;
		}
	}
}
