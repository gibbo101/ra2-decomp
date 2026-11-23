// best 99.4% (6884/6884 bytes), fragment for src/super.cpp before 0x006CDE80; needs include/cell.h's "private:" before OccupierPtr changed to "public:" so the loops read OccupierPtr and BridgeOccupierPtr directly; left: Y half of the Cell adds in the iron curtain and genetic loops loads cell.Y first where the exe loads DAT.Y first
#include "airctype.h"
#include "combat.h"
#include "foot.h"
#include "ion.h"
#include "ipiggy.h"
#include "map.h"
#include "parasite.h"
#include "revent.h"
#include "session.h"
#include "velocity.h"
#include "voc.h"
#include "cast.h"

namespace LocomotionClass { namespace CLSIDs { extern const CLSID Teleport; } }
void __fastcall FUN_0053AE50(HouseClass * owner, Cell cell);
int __fastcall Create_Paradrop_Planes(HouseClass * house, int air, int number, MissionType mission, AbstractClass * tarcom, AbstractClass * navcom, int passenger, int passengers);
int __fastcall Create_Spy_Planes(HouseClass * house, int air, int number, MissionType mission, AbstractClass * tarcom, AbstractClass * navcom);
int __fastcall FUN_00753250(char const * name);

static inline void Ready_Voice(char const * name)
{
	Map.IsTargettingMode = SUPER_NONE;
	VoxClass::SilenceIndex(FUN_00753250(name));
}
// FUNCTION: GAMEMD 0x006CC390
void SuperClass::Launch(Cell const & cell, bool player)
{
	switch (Class->Type) {
		case SUPER_CHRONO_SPHERE:
			if (Is_Ready()) {
				ChronoMapCoords = cell;
				Coord coord = Map[cell].Center_Coord();
				if (Map[cell].IsUnderBridge) {
					coord.Z += BRIDGE_LEPTON_HEIGHT;
				}
				CreateChronoAnim(coord + Coord(0, 0, 5));
				if (player) {
					Map.IsTargettingMode = SUPER_CHRONO_WARP;
					return;
				}
				if (Animation != NULL) {
					Animation->Invisible = true;
					Animation->StartSoundAudioController.Stop();
				}
			}
			break;

		case SUPER_CHRONO_WARP: {
			RadarEventClass::Create(RADAREVENT_SUPERWEAPON_ACTIVATED, ChronoMapCoords);
			RadarEventClass::Create(RADAREVENT_SUPERWEAPON_ACTIVATED, cell);
			Coord dest = Map[cell].Center_Coord();
			Coord source = Map[ChronoMapCoords].Center_Coord();
			if (Map[cell].IsUnderBridge) {
				dest.Z += BRIDGE_LEPTON_HEIGHT;
			}
			if (Map[ChronoMapCoords].IsUnderBridge) {
				source.Z += BRIDGE_LEPTON_HEIGHT;
			}
			DetachAnimation();
			new AnimClass(Rule->ChronoBlastDest, dest + Coord(0, 0, 5), 0, 1, (ShapeFlags_Type)0x600, 0, false);
			new AnimClass(Rule->ChronoBlast, source + Coord(0, 0, 5), 0, 1, (ShapeFlags_Type)0x600, 0, false);

			for (int index = 0; index < 9; index++) {
				ObjectClass * object = Map[DAT_00B0C038[index] + ChronoMapCoords].IsUnderBridge ? Map[DAT_00B0C038[index] + ChronoMapCoords].BridgeOccupierPtr : Map[DAT_00B0C038[index] + ChronoMapCoords].OccupierPtr;
				while (object != NULL) {
					if (object->Is_Foot() && !object->In_Air()) {
						FootClass * foot = (FootClass *)object;
						if (foot->ParasiteEatingMe != NULL && foot->ParasiteEatingMe->Techno_Type_Class()->Naval && foot->ParasiteEatingMe->ParasiteImUsing != NULL) {
							foot->ParasiteEatingMe->ParasiteImUsing->SuppressionTimer = 500;
							foot->ParasiteEatingMe->ParasiteImUsing->ExitUnit();
						}

						bool infactory = false;
						if (foot->Fetch_RTTI() == RTTI_UNIT) {
							TechnoClass * contact = As_Unit(foot)->FUN_0065AD30(0);
							if (contact != NULL && contact->Fetch_RTTI() == RTTI_BUILDING && ((BuildingClass *)contact)->Class->IsWeaponsFactory && Map[(Coord const &)As_Unit(foot)->PositionCoord].Cell_Building() == contact) {
								infactory = true;
							}
						}

						if (foot->Techno_Type_Class()->Organic && !foot->Techno_Type_Class()->Teleporter && !foot->In_Air()) {
							int damage = foot->Techno_Type_Class()->MaxStrength;
							foot->Take_Damage(damage, 0, Rule->C4Warhead, NULL, true, false, House);
						} else if (!foot->IsIronCurtained() && !foot->IsImmobilized && !foot->IsBeingWarpedOut() && !foot->IsWarpingIn() && !infactory) {
							if (foot->BunkerLinkedItem != NULL && foot->BunkerLinkedItem->Fetch_RTTI() == RTTI_BUILDING) {
								As_Building(foot->BunkerLinkedItem)->ClearBunker();
							}
							IPiggybackPtr piggy(foot->Locomotion);
							ILocomotionPtr teleport(LocomotionClass::CLSIDs::Teleport);
							UnitClass * unit = As_Unit(foot);
							Coord coord = Map[DAT_00B0C038[index] + cell].Center_Coord();
							if (Map[coord].IsUnderBridge) {
								coord.Z += BRIDGE_LEPTON_HEIGHT;
							}
							if (unit != NULL) {
								foot->Locomotion->Mark_All_Occupation_Bits(0);
								foot->Locomotion->Force_Track(-1, coord);
								foot->Clear_Occupy_Bit(coord);
								foot->IsOccupyingCell = true;
							}
							teleport->Link_To_Object(foot);
							piggy = IPiggybackPtr(teleport);
							if (piggy != NULL) {
								piggy->Begin_Piggyback(foot->Locomotion);
								foot->Locomotion = teleport;
							}
							Coord offset = foot->Center_Coord() - Map[ChronoMapCoords].Center_Coord();
							if (unit == NULL) {
								coord = Map[cell].Center_Coord() + offset;
								if (Map[coord].IsUnderBridge) {
									coord.Z += BRIDGE_LEPTON_HEIGHT;
								}
							}
							foot->IsImmobilized = true;
							foot->ChronoDestCoords = coord;
							foot->SendToEachLink(RADIO_OVER_OUT);
							foot->ChronoWarpedByHouse = House;
							foot->Assign_Destination(&Map[DAT_00B0C038[index] + cell], true);
						}
					}
					object = object->Next;
				}
			}
			if (!Session.MultiplayerObserver) {
				VoxClass::Play("EVA_ChronosphereActivated");
			}
			if (player) {
				Map.IsTargettingMode = SUPER_NONE;
				VoxClass::SilenceIndex(FUN_00753250("EVA_ChronosphereReady"));
				return;
			}
			break;
		}

		case SUPER_LIGHTNING_STORM:
			if (Is_Ready()) {
				IonStormClass::Ion_Storm_Begin(Rule->LightningStormDuration, Rule->LightningDeferment, cell, House);
				if (!Session.MultiplayerObserver) {
					VoxClass::Play("EVA_LightningStormCreated");
				}
				if (player) {
					Map.IsTargettingMode = SUPER_NONE;
					VoxClass::SilenceIndex(FUN_00753250("EVA_LightningStormReady"));
					return;
				}
			}
			break;

		case SUPER_PSYCHIC_DOMINATOR:
			if (Is_Ready()) {
				RadarEventClass::Create(RADAREVENT_SUPERWEAPON_ACTIVATED, cell);
				FUN_0053AE50(House, cell);
				if (!Session.MultiplayerObserver) {
					VoxClass::Play("EVA_PsychicDominatorActivated");
				}
				VocClass::PlayIndexAtPos(Rule->PsychicDominatorActivateSound, Map[cell].Center_Coord(), 0);
				if (player) {
					Map.IsTargettingMode = SUPER_NONE;
					VoxClass::SilenceIndex(FUN_00753250("EVA_PsychicDominatorReady"));
					return;
				}
			}
			break;

		case SUPER_IRON_CURTAIN:
			if (Is_Ready()) {
				Coord coord = Map[cell].Center_Coord();
				if (Map[cell].IsUnderBridge) {
					coord.Z += BRIDGE_LEPTON_HEIGHT;
				}
				new AnimClass(Rule->IronCurtainInvokeAnim, coord + Coord(0, 0, 5), 0, 1, (ShapeFlags_Type)0x600, 0, false);
				if (!Session.MultiplayerObserver) {
					VoxClass::Play("EVA_IronCurtainActivated");
				}
				RadarEventClass::Create(RADAREVENT_SUPERWEAPON_ACTIVATED, cell);
				for (int index = 0; index < 9; index++) {
					ObjectClass * object = Map[DAT_00B0C038[index] + cell].IsUnderBridge ? Map[DAT_00B0C038[index] + cell].BridgeOccupierPtr : Map[DAT_00B0C038[index] + cell].OccupierPtr;
					while (object != NULL) {
						if (As_Foot(object) == NULL || !As_Foot(object)->IsImmobilized) {
							object->IronCurtain(Rule->IronCurtainDuration, House, false);
						}
						object = object->Next;
					}
				}
				if (player) {
					Map.IsTargettingMode = SUPER_NONE;
					VoxClass::SilenceIndex(FUN_00753250("EVA_IronCurtainReady"));
					return;
				}
			}
			break;

		case SUPER_FORCE_SHIELD:
			if (Is_Ready()) {
				Coord coord = Map[cell].Center_Coord();
				if (Map[cell].IsUnderBridge) {
					coord.Z += BRIDGE_LEPTON_HEIGHT;
				}
				new AnimClass(Rule->ForceShieldInvokeAnim, coord + Coord(0, 0, 5), 0, 1, (ShapeFlags_Type)0x600, 0, false);
				SpecialSoundDuration = Rule->ForceShieldDuration - Rule->ForceShieldPlayFadeSoundTime;
				*(Coord *)SpecialSoundLocation = coord;
				if (Class->StartSound != VOC_NONE) {
					VocClass::PlayAt(Class->StartSound, coord, NULL);
				}
				House->CreatePowerOutage(Rule->ForceShieldBlackoutDuration);
				Coord center = Map[cell].Center_Coord();
				int radius = Rule->ForceShieldRadius * CELL_LEPTON_W;
				if (center != COORD_NONE && center != CELL_CENTER_COORD) {
					for (int index = Buildings.Count() - 1; index >= 0; index--) {
						BuildingClass * building = Buildings[index];
						if (building->House->Is_Ally(House)) {
							if (Distance(center, building->Center_Coord()) < radius) {
								building->IronCurtain(Rule->ForceShieldDuration, House, true);
							}
						}
					}
				}
				if (player) {
					Map.IsTargettingMode = SUPER_NONE;
					VoxClass::SilenceIndex(FUN_00753250("EVA_ForceShieldReady"));
					return;
				}
			}
			break;

		case SUPER_PARA_DROP:
			if (Is_Ready()) {
				int plane = AircraftTypeClass::From_Name("PDPLANE");
				CellClass * cellptr = &Map[cell];
				if (cellptr != NULL && cellptr != &InvalidCell) {
					if (cellptr->Is_Tile_Water()) {
						Cell nearby = Map.Nearby_Location(cell, SPEED_FOOT);
						if (nearby != CELL_NONE) {
							CellClass * nearptr = &Map[nearby];
							if (nearptr != NULL && nearptr != &InvalidCell && !nearptr->Is_Tile_Water()) {
								cellptr = nearptr;
							}
						}
					}
					if (House->ActLike == 0) {
						if (Rule->AllyParaDropInf.Count() == Rule->AllyParaDropNum.Count()) {
							for (int index = 0; index < Rule->AllyParaDropInf.Count(); index++) {
								int infantry = Rule->AllyParaDropInf[index]->HeapID;
								if (plane != -1 && infantry != -1) {
									Create_Paradrop_Planes(House, plane, 1, MISSION_PARADROP_APPROACH, cellptr, NULL, infantry, Rule->AllyParaDropNum[index]);
								}
							}
						}
						if (player) {
							Ready_Voice("EVA_ReinforcementsReady");
							return;
						}
					} else if (House->ActLike == 2) {
						if (Rule->YuriParaDropInf.Count() == Rule->YuriParaDropNum.Count()) {
							for (int index = 0; index < Rule->YuriParaDropInf.Count(); index++) {
								int infantry = Rule->YuriParaDropInf[index]->HeapID;
								if (plane != -1 && infantry != -1) {
									Create_Paradrop_Planes(House, plane, 1, MISSION_PARADROP_APPROACH, cellptr, NULL, infantry, Rule->YuriParaDropNum[index]);
								}
							}
						}
						if (player) {
							Ready_Voice("EVA_ReinforcementsReady");
							return;
						}
					} else {
						for (int index = 0; index < Rule->SovParaDropInf.Count(); index++) {
							int infantry = Rule->SovParaDropInf[index]->HeapID;
							if (plane != -1 && infantry != -1) {
								Create_Paradrop_Planes(House, plane, 1, MISSION_PARADROP_APPROACH, cellptr, NULL, infantry, Rule->SovParaDropNum[index]);
							}
						}
						if (player) {
							Ready_Voice("EVA_ReinforcementsReady");
							return;
						}
					}
				}
				if (player) {
					Ready_Voice("EVA_ReinforcementsReady");
					return;
				}
			}
			break;

		case SUPER_AMER_PARA_DROP:
			if (Is_Ready()) {
				int plane = AircraftTypeClass::From_Name("PDPLANE");
				CellClass * cellptr = &Map[cell];
				if (cellptr != NULL && cellptr != &InvalidCell) {
					if (cellptr->Is_Tile_Water()) {
						Cell nearby = Map.Nearby_Location(cell, SPEED_FOOT);
						if (nearby != CELL_NONE) {
							CellClass * nearptr = &Map[nearby];
							if (nearptr != NULL && nearptr != &InvalidCell && !nearptr->Is_Tile_Water()) {
								cellptr = nearptr;
							}
						}
					}
					if (Rule->AmerParaDropInf.Count() == Rule->AmerParaDropNum.Count()) {
						for (int index = 0; index < Rule->AmerParaDropInf.Count(); index++) {
							int infantry = Rule->AmerParaDropInf[index]->HeapID;
							if (plane != -1 && infantry != -1) {
								Create_Paradrop_Planes(House, plane, 1, MISSION_PARADROP_APPROACH, cellptr, NULL, infantry, Rule->AmerParaDropNum[index]);
							}
						}
					}
				}
				if (player) {
					Ready_Voice("EVA_ReinforcementsReady");
				}
			}
			break;

		case SUPER_SPY_PLANE:
			if (Is_Ready()) {
				int plane = AircraftTypeClass::From_Name("SPYP");
				CellClass * cellptr = &Map[cell];
				if (cellptr != NULL && cellptr != &InvalidCell && Rule->AllyParaDropInf.Count() == Rule->AllyParaDropNum.Count()) {
					for (int index = 0; index < Rule->AllyParaDropInf.Count(); index++) {
						if (plane != -1) {
							Create_Spy_Planes(House, plane, 1, MISSION_SPYPLANE_APPROACH, cellptr, NULL);
						}
					}
				}
				if (player) {
					Ready_Voice("EVA_SpyPlaneReady");
				}
			}
			break;

		case SUPER_PSYCHIC_REVEAL:
			if (Is_Ready()) {
				CellClass * cellptr = &Map[cell];
				Coord coord = cellptr->Center_Coord();
				Map.RevealArea2(&coord, Rule->PsychicRevealRadius, House, false, false, false, false, false);
				Map.RevealArea2(&coord, Rule->PsychicRevealRadius, House, false, false, false, false, true);
				VocClass::PlayIndexAtPos(Rule->PsychicRevealActivateSound, cellptr->Center_Coord(), 0);
				if (player) {
					Ready_Voice("EVA_PsychicRevealReady");
				}
			}
			break;

		case SUPER_GENETIC_CONVERTER:
			if (Is_Ready()) {
				Coord coord = Map[cell].Center_Coord();
				if (Map[cell].IsUnderBridge) {
					coord.Z += BRIDGE_LEPTON_HEIGHT;
				}
				new AnimClass(Rule->IonBlast, coord + Coord(0, 0, 5), 0, 1, (ShapeFlags_Type)0x600, 0, false);
				if (!Session.MultiplayerObserver) {
					VoxClass::Play("EVA_GeneticMutatorActivated");
				}
				VocClass::PlayIndexAtPos(Rule->GeneticMutatorActivateSound, coord, 0);
				RadarEventClass::Create(RADAREVENT_SUPERWEAPON_ACTIVATED, cell);
				if (Rule->MutateExplosion) {
					Explosion_Damage(coord, 10000, NULL, Rule->MutateExplosionWarhead, false, House);
				} else {
					for (int index = 0; index < 9; index++) {
						ObjectClass * object = Map[DAT_00B0C038[index] + cell].IsUnderBridge ? Map[DAT_00B0C038[index] + cell].BridgeOccupierPtr : Map[DAT_00B0C038[index] + cell].OccupierPtr;
						while (object != NULL) {
							ObjectClass * next = object->Next;
							InfantryClass * infantry = As_Infantry(object);
							if (infantry != NULL) {
								int damage = infantry->Techno_Type_Class()->MaxStrength;
								infantry->Take_Damage(damage, 0, Rule->MutateWarhead, NULL, true, false, House);
							}
							object = next;
						}
					}
				}
				if (player) {
					Ready_Voice("EVA_GeneticMutatorReady");
				}
			}
			break;

		case SUPER_MULTI_MISSILE:
			if (Is_Ready()) {
				if (Is_One_Time()) {
					Cell target = cell;
					Coord coord = target.As_Coord();
					coord.Z = Map.Get_Height_GL(coord);
					if (Map[target].IsUnderBridge) {
						coord.Z += BRIDGE_LEPTON_HEIGHT;
					}
					WeaponTypeClass * weapon = Weapons[WeaponTypeClass::From_Name("NukePayload")];
					CellClass * cellptr = &Map[coord];
					Coord sound = coord;
					Coord launch(coord.X, coord.Y, coord.Z + 20000);
					BulletTypeClass const * btype = weapon->Bullet;
					BulletClass * bullet = Create_Bullet(btype, cellptr, NULL, weapon->Attack, weapon->WarheadPtr, weapon->MaxSpeed, false);
					bullet->Set_Weapon_Type(weapon);
					HouseClass::PsiWarn(House, cellptr, bullet, "PSIWARN");
					if (bullet != NULL) {
						TVelocity3D<double> velocity(Dir256(DIR_N), Dir256(DIR_N), -100.0);
						bullet->Unlimbo(launch, velocity);
						if (!Session.MultiplayerObserver) {
							VoxClass::Play("EVA_NuclearMissileLaunched");
						}
						VocClass::PlayIndexAtPos(Rule->DigSound, sound, 0);
					}
					if (player) {
						Map.IsTargettingMode = SUPER_NONE;
						VoxClass::SilenceIndex(FUN_00753250("EVA_NuclearMissileReady"));
					}
					House->Production_Status_Changed();
				} else {
					for (int index = 0; index < BuildingTypes.Count(); index++) {
						BuildingTypeClass const * btype = BuildingTypes[index];
						if (btype->IsNukeSilo && (btype->SuperWeapon == Class->HeapID || btype->SuperWeapon2 == Class->HeapID)) {
							BuildingClass * launchsite = House->Find_Building(StructType(index));
							if (launchsite != NULL) {
								Cell target = cell;
								Coord coord = target.As_Coord();
								coord.Z = Map.Get_Height_GL(coord);
								Coord sound = coord;
								launchsite->QueueMission(MISSION_MISSILE, false);
								launchsite->Commence();
								House->NukeDest = cell;
								launchsite->LastSuperWeaponIndex = Class->Type;
								VocClass::PlayIndexAtPos(Rule->DigSound, sound, 0);
								if (!Session.MultiplayerObserver) {
									VoxClass::Play("EVA_NuclearMissileLaunched");
								}
							}
							break;
						}
					}
					if (player) {
						Map.IsTargettingMode = SUPER_NONE;
						VoxClass::SilenceIndex(FUN_00753250("EVA_NuclearMissileReady"));
					}
					House->Production_Status_Changed();
				}
			}
			break;
	}
}
