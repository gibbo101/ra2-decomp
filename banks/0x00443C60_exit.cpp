// best 99.8% raw, 99.8% normalised (6759/6759 bytes), no overlay; needs #include "airctype.h" after temporal.h (it renumbers _$E298 to _$E300); splice before 0x004456D0. The file-local Center_Cell helper is gone: targetcell.h's Target_Cell (already included by building.cpp) is the same inline. Left: the add at 0x00444E42. Diagnostic only: it matches when the refinery cell's X sum reads AdjacentCell[5].X directly and its Y sum reads it through a reference; no natural form found.
// FUNCTION: GAMEMD 0x00443C60
int BuildingClass::Exit_Object(TechnoClass * base, Cell preferred)
{
	if (!base) return(0);

	base->IsInPlayfield = true;

	AircraftClass * air = NULL;
	if (base->Fetch_RTTI() == RTTI_AIRCRAFT) {
		air = (AircraftClass *)base;
	}

	Cell cell;

	switch (base->Fetch_RTTI()) {

		case RTTI_AIRCRAFT:

			House->Update_Production_Mode(RTTI_AIRCRAFT);
			House->BuildAircraft = AIRCRAFT_NONE;

			if (!HasFreeLink(base) && (!House->IonSensitivesShouldBeOffline() || air->Class->IsAirportBound)) {

				if (air->Class->IsAirportBound) {
					return(0);
				}

				Cell edge = Cell(Map.LocalRect.X, -Map.LocalRect.X) + Cell(1, Map.PlayRect.Width) + Cell(Map.LocalRect.Y, Map.LocalRect.Y);

				Cell diff;
				diff = Center_Coord().As_Cell() - edge;

				if (diff.X - diff.Y > Map.LocalRect.Width) {
					edge = edge - Cell(1, 0) + Cell(Map.LocalRect.Width, -Map.LocalRect.Width);
					cell = edge;
					int random = Random_Pick(0, Map.LocalRect.Height);
					cell += Cell(random, random);
				} else {
					edge = Cell(edge.X - 1, edge.Y);
					cell = edge;
					int random = Random_Pick(0, Map.LocalRect.Height);
					cell += Cell(random, random);
				}


				ScenarioInit++;
				if (air->Unlimbo(Coord(cell, 0), DIR_N)) {
					if (ArchiveTarget != NULL) {
						air->Assign_Destination(ArchiveTarget);
					} else {
						Cell nearby = air->Nearby_Location(this);
						if (nearby != CELL_NONE) {
							air->Assign_Destination(&Map[nearby]);
						} else {
							air->Assign_Destination(NULL);
						}
					}
					air->Set_Mission(MISSION_MOVE);
					ScenarioInit--;
					return(2);
				}
				ScenarioInit--;

			} else {

				air->Set_Height(0);
				ScenarioInit++;

				if (House->IonSensitivesShouldBeOffline()) {
					Cell nearby = air->Nearby_Location(this);
					if (air->Unlimbo(Coord(nearby, 0), air->Pose_Dir())) {
						ScenarioInit--;
						return(2);
					}
				} else {
					if (air->Unlimbo(Docking_Coord(air), air->Pose_Dir())) {
						Transmit_Message(RADIO_HELLO, air);
						Transmit_Message(RADIO_TETHER, air);
						air->Set_Coord(Docking_Coord(air));
						air->DockNowHeadingTo = this;

						if (ArchiveTarget != NULL && !air->Class->IsAirportBound) {
							air->Assign_Destination(ArchiveTarget);
							air->QueueMission(MISSION_MOVE, false);
						}
						ScenarioInit--;
						return(2);
					}
				}
				ScenarioInit--;
			}
			break;

		case RTTI_INFANTRY:
		case RTTI_UNIT:

			if (!Class->IsHospital && !Class->IsArmory && !Class->IsWeaponsFactory && !HasFreeLink()) {
				return(1);
			}

			if (!Class->IsHospital && !Class->IsArmory) {
				House->Update_Production_Mode(base->Fetch_RTTI());

				if (base->Fetch_RTTI() == RTTI_UNIT) {
					House->BuildUnit = UNIT_NONE;
				}
				if (base->Fetch_RTTI() == RTTI_INFANTRY) {
					House->BuildInfantry = INFANTRY_NONE;
				}
			}

			if (!Class->IsRefinery && !Class->IsWeeder) {

				if (Class->IsWeaponsFactory) {

					if (Class->Naval) {

						if (!HasAnyLink()) {
							QueueMission(MISSION_UNLOAD, false);
						}

						Cell cell = Target_Cell(this);

						if (ArchiveTarget != NULL) {
							DirType dir;
							FacingType facing = Dir_Facing(dir.Direction(Get_Cell(), Target_Cell(ArchiveTarget)));
							while (Map[cell].Cell_Building() == this) {
								cell = Adjacent_Cell(cell, facing);
							}
						}

						if (ArchiveTarget == NULL || Map[cell].Land_Type() != LAND_WATER || Map[cell].Cell_Techno() != NULL || !Map.In_Local_Radar(cell)) {
							cell = Map.Nearby_Location(Target_Cell(this), base->Techno_Type_Class()->Speed);
						}

						if (!base->Unlimbo(Map[cell].Center_Coord(), DIR_E)) {
							return(0);
						}

						if (ArchiveTarget != NULL) {
							base->Assign_Destination(ArchiveTarget);
							base->QueueMission(MISSION_MOVE, false);
						}
						base->Mark(MARK_UP);
						base->Set_Coord(Map[cell].Cell_Coord());
						base->Mark(MARK_DOWN);
						return(2);
					}

					base->Assign_Archive_Target(ArchiveTarget);

					if (Get_Mission() == MISSION_UNLOAD) {
						int count = House->Buildings.Count();
						for (int index = 0; index < count; index++) {
							BuildingClass * bldg = House->Buildings[index];
							if (bldg->Class == Class && bldg != this && bldg->Get_Mission() == MISSION_GUARD && !bldg->Factory) {
								FactoryClass * temp = Factory;
								bldg->Factory = Factory;
								Factory = NULL;
								int retval = bldg->Exit_Object(base, CELL_NONE);
								bldg->Factory = NULL;
								Factory = temp;
								return(retval);
							}
						}
						return(1);
					}

					ScenarioInit++;
					if (base->Unlimbo(Exit_Coord(0), DIR_E)) {
						base->Mark(MARK_UP);
						base->Set_Coord(Exit_Coord(0));
						base->Mark(MARK_DOWN);
						Transmit_Message(RADIO_HELLO, base);
						Transmit_Message(RADIO_TETHER, base);
						QueueMission(MISSION_UNLOAD, false);
						ScenarioInit--;
						return(2);
					}

				} else if (Class->ToBuild == RTTI_INFANTRYTYPE || Class->IsHospital || Class->IsArmory || Class->Cloning) {

					base->Assign_Archive_Target(ArchiveTarget);

					cell = Find_Exit_Cell(base, CELL_NONE);
					if (cell == CELL_NONE) {
						return(0);
					}

					if (Class->ToBuild == RTTI_INFANTRYTYPE && !Class->Cloning) {
						TechnoTypeClass const * ttype = base->Techno_Type_Class();
						for (int i = 0; i < House->CloningVats.Count(); i++) {
							BuildingClass * vat = House->CloningVats[i];
							TechnoClass * clone = (TechnoClass *)ttype->Create_One_Of(vat->House);
							vat->Exit_Object(clone, CELL_NONE);
						}
					}

					Coord exitcellcoord(cell);
					Dir256 dir = ::Direction(Center_Coord(), exitcellcoord).As_Dir256();

					Cell to = cell;
					Cell here = Get_Cell();
					int x = here.X;
					if (to.X >= x + Class->Width()) {
						to.X -= 1;
					} else if (to.X < here.X) {
						to.X += 1;
					}
					int y = here.Y;
					if (to.Y >= y + Class->Height()) {
						to.Y -= 1;
					} else if (to.Y < here.Y) {
						to.Y += 1;
					}

					Coord exitcoord = to.As_Coord();
					if (Class->IsGDIBarracks) {
						if (cell.X == x + 1 && cell.Y == y + 2) {
							exitcoord = exitcoord + Class->ExitCoordinate;
						}
					}
					if (Class->IsNODBarracks && cell.X == x + 2 && cell.Y == y + 2) {
						exitcoord = exitcoord + Class->ExitCoordinate;
					}
					if (Class->IsYuriBarracks && cell.X == x + 2 && cell.Y == y + 1) {
						exitcoord = exitcoord + Class->ExitCoordinate;
					}

					ScenarioInit++;
					if (base->Unlimbo(exitcoord, dir)) {

						if (((FootClass *)base)->NavCom != NULL && !base->Techno_Type_Class()->JumpJet && !base->Techno_Type_Class()->Teleporter) {
							if (((FootClass *)base)->NavCom != NULL) {
								base->Assign_Archive_Target(((FootClass *)base)->NavCom);
							}
							base->QueueMission(MISSION_MOVE, false);
							base->Assign_Destination(&Map[cell]);
						}

						if (!House->FUN_0050B730() || Class->IsHospital) {
							base->QueueMission(MISSION_GUARD_AREA, false);

							Cell where = House->Where_To_Go((FootClass *)base);
							if (where != CELL_NONE && Class->ToBuild != RTTI_NONE) {
								base->Assign_Archive_Target(&Map[where]);
								((FootClass *)base)->Queue_Navigation_List(&Map[where]);
							} else {
								base->Assign_Archive_Target(NULL);
							}
						}

						if (Transmit_Message(RADIO_HELLO, base) == RADIO_ROGER) {
							Transmit_Message(RADIO_UNLOAD, base);
						}
						ScenarioInit--;
						return(2);
					}
				} else {

					Coord exitcoord;
					cell = Find_Exit_Cell(base, preferred);
					if (cell == CELL_NONE) {
						return(0);
					}

					Dir256 dir = ::Direction(Center_Coord(), Coord(cell)).As_Dir256();

					Cell to = cell;
					Cell here = Get_Cell();
					int x = here.X;
					if (to.X >= x + Class->Width()) {
						to.X -= 1;
					} else if (to.X < here.X) {
						to.X += 1;
					}
					int y = here.Y;
					if (to.Y >= y + Class->Height()) {
						to.Y -= 1;
					} else if (to.Y < here.Y) {
						to.Y += 1;
					}

					exitcoord = to.As_Coord();
					if (Class->IsGDIBarracks) {
						if (cell.X == x + 1 && cell.Y == y + 2) {
							exitcoord = exitcoord + Class->ExitCoordinate;
						}
					}
					if (Class->IsNODBarracks && cell.X == x + 2 && cell.Y == y + 2) {
						exitcoord = exitcoord + Class->ExitCoordinate;
					}
					if (Class->IsYuriBarracks && cell.X == x + 2 && cell.Y == y + 1) {
						exitcoord = exitcoord + Class->ExitCoordinate;
					}

					ScenarioInit++;
					if (base->Unlimbo(exitcoord, dir)) {

						base->QueueMission(MISSION_MOVE, false);

						base->Assign_Destination(&Map[cell]);

						if (!House->FUN_0050B730()) {
							base->QueueMission(MISSION_GUARD_AREA, false);

							Cell where = House->Where_To_Go((FootClass *)base);
							if (where != CELL_NONE && Class->ToBuild != RTTI_NONE) {
								base->Assign_Archive_Target(&Map[where]);
							} else {
								base->Assign_Archive_Target(NULL);
							}
						}

						ScenarioInit--;
						return(2);
					}

				}

			} else {

				if (base->Fetch_RTTI() == RTTI_UNIT) {

					cell = Adjacent_Cell(Center_Coord().As_Cell(), FACING_SW);

					ScenarioInit++;
					if (base->Unlimbo(Coord(Adjacent_Cell(cell, FACING_S), 0), DIR_SW)) {
						base->PrimaryFacing.Set(DirType(DIR_S));
						base->QueueMission(MISSION_HARVEST, false);
					}

				} else {
					base->Scatter(COORD_NONE, true, false);
					return(0);
				}
			}
			ScenarioInit--;
			break;

		case RTTI_BUILDING:

			if (!House->FUN_0050B730()) {

				House->Update_Production_Mode(RTTI_BUILDING);
				House->BuildStructure = STRUCT_NONE;

				BuildingClass * building = (BuildingClass *)base;
				BaseNodeClass * node = House->Base.Next_Buildable(building->Class->HeapID);
				Coord placementcoord(0, 0, 0);
				BuildingTypeClass * buildingtype = building->Class;
				Cell upgrade;

				if (node != NULL && node->CellID != Cell(0, 0)) {

					if (buildingtype->PowersUpBuilding[0] != '\0' || House->FUN_0050B760(buildingtype, node->CellID)) {
						placementcoord = Coord(node->CellID);
					} else {
						placementcoord = Coord(House->Where_To_Place_Building(buildingtype, HouseClass::Base_Cell_Weight_By_Distance, -1));
						if (placementcoord == COORD_NONE) {
							return(0);
						}
						node->CellID = placementcoord.As_Cell();
					}

				} else {

					if (buildingtype->PowersUpBuilding[0] == '\0') {
						placementcoord = Coord(House->Where_To_Place_Building(buildingtype, HouseClass::Base_Cell_Weight_By_Distance, -1));
					} else {
						upgrade = House->Where_To_Place_Upgrade(buildingtype);
						if (upgrade != CELL_NONE) {
							placementcoord = Coord(upgrade);
						}
					}

					if (node != NULL && placementcoord != COORD_NONE) {
						node->CellID = placementcoord.As_Cell();
					}
				}

				if (placementcoord != COORD_NONE) {

					switch (((BuildingTypeClass *)base->Class_Of())->Flush_For_Placement(placementcoord.As_Cell(), House)) {

						default:
							return(0);

						case 1:
							{
								int failures = House->Base.FailedToPlaceNode(node);
								if (Session.Type != GAME_NORMAL && failures > Rule->MaximumBuildingPlacementFailures && node != NULL) {
									House->Base.Nodes.Delete_Index(House->Base.Nodes.ID(node));
								}
							}
							return(1);

						case 0:
							if (base->Unlimbo(placementcoord)) {
								if (base->SlaveManager != NULL) {
									base->SlaveManager->Deploy2();
								}

								if (node != NULL) {
									if (building->Class->HeapID == House->BuildStructure) {
										House->BuildStructure = STRUCT_NONE;
									}
								}

								if (base->CurrentMission == MISSION_NONE && base->MissionQueue == MISSION_CONSTRUCTION) {
									base->Commence();
								}

								if (base->Fetch_RTTI() == RTTI_BUILDING) {
									if (building->Class->IsFirestormWall) {
										Map.Place_Firestorm_Wall(placementcoord.As_Cell(), House, building->Class);
									} else if (building->Class->ToOverlay != NULL && building->Class->ToOverlay->IsWall) {
										Map.Place_Wall(placementcoord.As_Cell(), House, building->Class);
									}
								}

								if (building->Class == Rule->WallTower) {
									int index = House->Base.Nodes.ID(node) + 1;
									int count = House->Base.Nodes.Count();
									for (; index < count; index++) {
										if (House->Base.Nodes[index].Type >= STRUCT_FIRST && BuildingTypes[House->Base.Nodes[index].Type]->IsBaseDefense) {
											House->Base.Nodes[index].CellID = base->Get_Coord().As_Cell();
											break;
										}
									}
								}
								return(2);
							}
							// fall through

						case 2:
							if (node == NULL) {
								return(0);
							} else {
								int index = House->Base.Nodes.ID(node);

								BuildingTypeClass * nodetype = BuildingTypes[node->Type];
								if (!nodetype->IsWall && !nodetype->IsGate) {
									Cell placecell = placementcoord.As_Cell();
									for (int i = 0; i < House->Base.Nodes.Count(); i++) {
										if (House->Base.Nodes[i].CellID == placecell) {
											House->Base.Nodes[i].CellID = Cell(0, 0);
										}
									}
									return(0);
								}
								House->Base.Nodes.Delete_Index(index);
							}
							break;
					}

				} else {

					if (buildingtype->PowersUpBuilding[0] != '\0') {
						if (House->Base.Next_Buildable(STRUCT_NONE) == node) {
							House->Base.Nodes.Delete_Index(House->Base.Next_Buildable_Index(STRUCT_NONE));
						}
					}
				}
			}
			break;

		default:
			break;
	}

	return(0);
}
