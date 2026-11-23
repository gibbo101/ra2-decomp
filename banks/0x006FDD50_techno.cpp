// best 56.8% raw, 86.7% normalised (7112/7167 bytes), no overlay; insert before 0x006FD800 in src/techno.cpp. The cast.h helpers (As_Building, As_Foot) in the tail restore the exe's out-of-line Horizontal_Speed call. Left: frame 0x9c vs exe 0xa4, spray Position read through a pointer, !valid_arc block order, slot and register choices from the bullet block on.
#include "disklaser.h"

namespace Unsorted { extern bool ArmageddonMode; }

static inline ObjectClass * Object_If_Object(AbstractClass * object)
{
	if (object == NULL) {
		return(NULL);
	}
	return((object->AbstractFlags & 2) ? (ObjectClass *)object : NULL);
}

static inline TechnoClass * Techno_If_Techno(AbstractClass * object)
{
	if (object == NULL) {
		return(NULL);
	}
	return((object->AbstractFlags & 1) ? (TechnoClass *)object : NULL);
}

// FUNCTION: GAMEMD 0x006FDD50
BulletClass * TechnoClass::Fire_At(AbstractClass * target, int which)
{
	WeaponTypeClass * weapon = Get_Class_Weapon_Data(which)->Weapon;
	if (weapon == NULL) {
		return(NULL);
	}

	ObjectClass * object = Object_If_Object(target);
	BulletTypeClass const * btype = weapon->Bullet;

	if (Unsorted::ArmageddonMode || target == NULL) {
		return(NULL);
	}
	if (object != NULL && object->IsInLimbo) {
		return(NULL);
	}

	if (weapon->Suicide) {
		int damage = Techno_Type_Class()->MaxStrength;
		Take_Damage(damage, 0, Rule->C4Warhead, NULL, true, false, NULL);
		return(NULL);
	}

	if ((weapon->UseFireParticles && ParticleSystems[0] != NULL)
		|| (weapon->IsRailgun && ParticleSystems[4] != NULL)
		|| (weapon->UseSparkParticles && ParticleSystems[1] != NULL)
		|| (weapon->IsSonic && Wave != NULL)) {

		return(NULL);
	}

	if (weapon->Spawner) {
		SpawnManager->SetTarget(target);
		if ((!IsOwnedByPlayer && !IsDiscoveredByPlayer) || ((Map.Is_Shrouded(Center_Coord()) || Map.IsLocationFogged(Center_Coord())) && (RTTI != RTTI_AIRCRAFT || !IsOwnedByPlayer))) {
			if (object != NULL) {
				HouseClass * owner = object->Owner_HouseClass();
				if (owner != NULL && owner->Is_Player_Control() && weapon->RevealOnFire) {
					Map.RevealArea1(&Center_Coord(), 3, owner, false, false, false, true, false);
				}
				Map.RevealArea3(Center_Coord(), 0, 4, false);
			}
		}
		return(NULL);
	}

	if (weapon->DrainWeapon) {
		if ((target->AbstractFlags & 1) && ((TechnoClass *)target)->Techno_Type_Class()->Drainable) {
			FUN_0070FD70((BuildingClass *)target);
			Assign_Target(NULL);
		}
		return(NULL);
	}

	static Coord _spray_offsets[8] = {
		Coord(256, 0, 0),
		Coord(180, 180, 0),
		Coord(0, 256, 0),
		Coord(-180, 180, 0),
		Coord(-256, 0, 0),
		Coord(-180, -180, 0),
		Coord(0, -256, 0),
		Coord(180, -180, 0)
	};

	Coord target_coord;
	if (Techno_Type_Class()->SprayAttack) {
		int step = 8 / weapon->Burst;
		if (BurstIndex == 0) {
			SprayOffsetIndex = Scen->RandomNumber(0, 7);
		} else {
			SprayOffsetIndex = (SprayOffsetIndex + step) % 8;
		}
		target_coord = Position + _spray_offsets[SprayOffsetIndex];
	} else if (weapon->AreaFire) {
		target_coord = Map[Cell(Center_Coord())].Center_Coord();
		target = &Map[target_coord];
	} else if (object != NULL) {
		target_coord = object->Target_Coord();
	} else {
		target_coord = target->As_Coord();
	}

	if (Techno_Type_Class()->SprayAttack) {
		target = &Map[target_coord];
	}

	Coord fire_coord = Fire_Coord(which, Coord(0, 0, 0));

	DirType dir;
	if (btype->ROT == 0 && !btype->IsDropping) {
		dir = DirType(fastmath::atan2((double)fire_coord.Y - target_coord.Y, (double)target_coord.X - fire_coord.X));
	} else {
		dir = Fire_Direction();
		if (btype->IsDropping) {
			fire_coord = Center_Coord();
		}
	}

	int firepower = weapon->Attack;
	if (weapon->IsSonic || weapon->UseFireParticles) {
		firepower = 0;
	}
	if (firepower > 0) {
		firepower = (int)(House->FirepowerBias * FirepowerBias * weapon->Attack);
		if (Has_Ability(ABILITY_FIREPOWER)) {
			firepower = (int)(firepower * Rule->VeteranCombat);
		}
	}
	if (CanOccupyFire()) {
		firepower = (int)(firepower * Rule->OccupyDamageMultiplier);
	}
	if (BunkerLinkedItem != NULL && RTTI != RTTI_BUILDING) {
		firepower = (int)(firepower * Rule->BunkerDamageMultiplier);
	}
	if (InOpenToppedTransport) {
		firepower = (int)(firepower * Rule->OpenToppedDamageMultiplier);
	}

	if (weapon->DiskLaser) {
		DiskLaserClass * disk = new DiskLaserClass;
		if (disk != NULL) {
			BurstIndex++;
			int delay = Rearm_Delay(which);
			ChargeTurretDelay = delay;
			Arm = delay;
			BurstIndex %= weapon->Burst;
			disk->Fire(this, (TechnoClass *)target, weapon, firepower);
			return(NULL);
		}
	}

	int dx = target_coord.X - fire_coord.X;
	int dy = target_coord.Y - fire_coord.Y;
	int max_speed = weapon->Get_Speed(fastmath::sqrt(dx * dx + dy * dy));
	BulletClass * bullet = Create_Bullet(weapon->Bullet, target, this, firepower, weapon->WarheadPtr, max_speed, weapon->IsBright);

	TVelocity3D<double> velocity;
	if (bullet != NULL) {
		bullet->Set_Weapon_Type(weapon);
		bullet->Limbo();

		if (Is_Foot() && ((FootClass const *)this)->Locomotion->Is_Moving() && !Techno_Type_Class()->JumpJet) {
			bullet->IsInaccurate = true;
		}

		if (!bullet->IsInaccurate && !btype->IsInaccurate) {
			TechnoClass * techno = Techno_If_Techno(TarCom);
			WeaponTypeClass * bweapon = bullet->Get_Weapon_Type();
			if (techno != NULL && bweapon != NULL) {
				techno->EstimatedHealth -= Estimate_Damage(techno, bweapon);
			}
		}

		Coord displacement = Predict_Target_Coord() - fire_coord;

		if (btype->IsInaccurate && btype->IsArcing) {
			if (btype->FlakScatter && !btype->IsInvisible) {
				TPoint3D<float> vec(displacement.X, displacement.Y, displacement.Z);
				float distance = vec.Length();
				int spread = Scen->RandomNumber(0, Rule->BallisticScatter) * (int)distance / Weapon_Range(which);
				DirType sdir(Random_Double(0.0, M_PI * 2));
				displacement = Move_Coord(displacement, sdir, spread);
			} else {
				int spread = Scen->RandomNumber(Rule->BallisticScatter / 2, Rule->BallisticScatter);
				DirType sdir(Random_Double(0.0, M_PI * 2));
				displacement = Move_Coord(displacement, sdir, spread);
			}
		}

		if (btype->ROT == 0 && !btype->IsDropping) {
			dir = DirType(fastmath::atan2((double)-displacement.Y, (double)displacement.X));
		} else {
			dir = Fire_Direction();
			if (btype->IsDropping) {
				fire_coord = Center_Coord();
			}
		}

		if (max_speed > displacement.Length() / 2) {
			max_speed = displacement.Length() / 2;
		}

		int segments = Techno_Type_Class()->RadialFireSegments;
		if (weapon->Bullet->ROT > 0 || weapon->Bullet->Vertical) {
			if (segments == 0) {
				max_speed = 1;
			}
			bullet->MaxSpeed = weapon->MaxSpeed;
		}

		velocity.Set(0.0, 0.0, 0.0);
		velocity.Set_Speed((double)max_speed);

		if (segments > 0) {
			double angle = M_PI / segments * unknown_43C - M_PI_2;
			DirType yaw(PrimaryFacing.Current().As_Radian() + angle);
			velocity.Set_Yaw(yaw);
			unknown_43C++;
			if (unknown_43C >= segments) {
				unknown_43C = 0;
			}
		} else {
			velocity.Set_Yaw(dir);
		}

		DirType pitch(0x3FFF);
		bool valid_arc = true;
		if (bullet->Class->IsArcing) {
			double gravity = Rule->Gravity;
			if (bullet->Class->IsFloater) {
				gravity = Get_Floater_Gravity();
			}
			int planar = Point2D(displacement).Length();
			valid_arc = Calculate_Projectile_Pitch(Should_Use_High_Arc(which), max_speed, planar, displacement.Z, gravity, pitch);
		} else if (bullet->Class->IsVoxel) {
			if (target->Center_Coord().Z < Center_Coord().Z) {
				pitch = DirType(DIR_S);
			} else {
				pitch = DirType(DIR_E);
			}
		} else {
			int abs_z = abs(displacement.Z);
			if (abs_z > 200) {
				double y = 20.0;
				if (TarCom != NULL && TarCom->RTTI == RTTI_BUILDING) {
					Coord turret_coord = Turret_Coord(which);
					displacement.Z = 200 * ((BuildingClass *)TarCom)->Class->ZHeight - turret_coord.Z;
					abs_z = abs(displacement.Z);
					if ((double)abs_z < y) {
						y = 0.0;
					}
				}

				double planar = Point2D(displacement).Length();
				if (planar < 0.05) {
					planar = 0.05;
				}
				double angle = fastmath::atan2f((double)abs_z - y, planar);
				if (displacement.Z < 0) {
					angle = -angle;
				}
				pitch = DirType(angle);
			}
		}

		velocity.Set_Pitch(pitch);

		if (!valid_arc) {
			bullet->Release();
			bullet = NULL;
		} else {
			if (!bullet->Unlimbo(fire_coord, velocity)) {
				bullet->Release();
				bullet = NULL;
			} else {
				if (CanOccupyFire() && As_Building(this) != NULL) {
					BuildingClass * building = As_Building(this);
					building->FiringOccupantIndex++;
					building->FiringOccupantIndex %= building->GetOccupantCount();
				}

				if (bullet->Class->IsInvisible && object != NULL && object->IsOnBridge) {
					bullet->IsOnBridge = true;
				}

				if (Is_Turret_Equipped() && Techno_Type_Class()->TurretRecoil) {
					TurretRecoil.Fire();
					BarrelRecoil.Fire();
				}

				if (weapon->UseFireParticles && ParticleSystems[0] == NULL) {
					ParticleSystems[0] = new ParticleSystemClass(weapon->AttachedParticleSystem, fire_coord, target, this, COORD_NONE, NULL);
				}
				if (weapon->UseSparkParticles && ParticleSystems[1] == NULL) {
					ParticleSystems[1] = new ParticleSystemClass(weapon->AttachedParticleSystem, fire_coord, target, this, COORD_NONE, NULL);
				}
				if (weapon->IsRailgun && ParticleSystems[4] == NULL) {
					Coord end = Railgun_Beam_Damage(fire_coord, target, weapon);
					ParticleSystems[4] = new ParticleSystemClass(weapon->AttachedParticleSystem, fire_coord, NULL, this, end, NULL);
				}

				BurstIndex++;
				int delay = Rearm_Delay(which);
				if (Berzerk) {
					delay /= 2;
				}
				ChargeTurretDelay = delay;
				Arm = delay;
				BurstIndex %= weapon->Burst;

				AnimTypeClass const * a = NULL;
				if (weapon->Anim.Count() == 8) {
					a = weapon->Anim[(Fire_Direction().As_Dir8() + 1) % 8];
				} else if (weapon->Anim.Count() > 0) {
					a = weapon->Anim[0];
				}
				if (CanOccupyFire()) {
					a = weapon->OccupantAnim;
				}
				if (a == NULL && InOpenToppedTransport && weapon->OpenToppedAnim != NULL) {
					a = weapon->OpenToppedAnim;
				}

				if (weapon->Sound.Count() > 0 && !Techno_Type_Class()->IsGattling) {
					VocClass::PlayAt(weapon->Sound.Pick(SoundRandomSeed), fire_coord);
				}

				if (a != NULL) {
					AnimClass * anim = new AnimClass(a, fire_coord);
					if (RTTI == RTTI_BUILDING) {
						int zadjust = (fire_coord.Y - Render_Coord().Y) / -4;
						anim->ZAdjust = MIN(zadjust, 0);
						if (GetOccupantCount() > 0) {
							anim->ZAdjust = -200;
						}
					}
					if (anim != NULL && RTTI != RTTI_BUILDING) {
						anim->Attach_To(this);
					}
				}

				if (weapon->IsSonic) {
					Wave = new WaveClass(target_coord, fire_coord, this, WAVE_SONIC, (TechnoClass *)target);
				}

				if (Techno_Type_Class()->IsTargetLaser && House->Is_Player_Control()) {
					TargetingLaserTimer = 15;
				}

				if (weapon->IsLaser) {
					if (RTTI == RTTI_BUILDING) {
						WeaponTypeClass * turret_weapon = GetTurretWeapon()->Weapon;
						LaserDrawClass * laser = CreateLaser((ObjectClass *)target, which, turret_weapon, COORD_NONE);
						if (laser != NULL && ((BuildingClass *)this)->Class == Rule->PrismType) {
							laser->Thickness = 3;
							if (((BuildingClass *)this)->SupportingPrisms > 0) {
								laser->IsSupported = true;
								laser->Thickness = 5;
							}
						}
					} else {
						LaserDrawClass * laser = CreateLaser((ObjectClass *)target, which, weapon, COORD_NONE);
						if (laser != NULL && weapon->IsHouseColor) {
							laser->Thickness = 2;
						}
					}
				} else if (weapon->IsElectricBolt) {
					FireEBolt((ObjectClass *)target);
				} else if (weapon->IsRadBeam) {
					if (weapon->WarheadPtr != NULL && weapon->WarheadPtr->Temporal) {
						CreateRadBeam((ObjectClass *)target, 1);
					} else {
						CreateRadBeam((ObjectClass *)target, 0);
					}
				} else if (weapon->IsRadEruption) {
					CreateRadBeams(weapon->WarheadPtr->SpreadFactor);
				} else if (weapon->IsMagBeam) {
					if (Wave == NULL || ((target == NULL || target->RTTI != RTTI_BUILDING) && Wave == NULL)) {
						Wave = new WaveClass(target_coord, fire_coord, this, (WaveType)3, (TechnoClass *)target);
					}
				}

				Reduce_Ammunition();
				Mark(MARK_CHANGE);

				if ((!IsOwnedByPlayer && !IsDiscoveredByPlayer) || ((Map.Is_Shrouded(Center_Coord()) || Map.IsLocationFogged(Center_Coord())) && (RTTI != RTTI_AIRCRAFT || !IsOwnedByPlayer))) {
					if (target != NULL && (target->AbstractFlags & 2)) {
						HouseClass * owner = ((ObjectClass *)target)->Owner_HouseClass();
						if (owner != NULL && owner->Is_Player_Control() && weapon->RevealOnFire) {
							Map.RevealArea1(&Center_Coord(), 3, owner, false, false, false, true, false);
							Map.RevealArea3(Center_Coord(), 0, 4, false);
						}
					}
				}
				LastFireBulletFrame = Frame;
			}
		}
	}

	if (weapon->LimboLaunch) {
		if (Techno_Type_Class()->ReselectIfLimboed && IsSelected && Owner_HouseClass()->Is_Player_Control() && target->RTTI == RTTI_INFANTRY) {
			ShouldBeReselectOnUnlimbo = true;
		}
		if (Techno_Type_Class()->RejoinTeamIfLimboed && As_Foot(this) != NULL && As_Foot(this)->Team != NULL && target->RTTI == RTTI_INFANTRY) {
			OldTeam = As_Foot(this)->Team;
		}
		Limbo();
		if (weapon->WarheadPtr->Parasite) {
			if (As_Foot(target) != NULL) {
				As_Foot(target)->LastBeParasitedStartFrame = Frame + 20;
			}
			bullet->Limbo();
			bullet->Construct(weapon->Bullet, target, this, firepower, weapon->WarheadPtr, weapon->MaxSpeed, weapon->IsBright);
			bullet->Unlimbo(fire_coord, velocity);
		}
	}

	if (Techno_Type_Class()->DistributedFire) {
		if (CurrentMission != MISSION_ATTACK || RTTI == RTTI_BUILDING) {
			AttackedTargets.Add(target);
			Assign_Target(NULL);
		}
	}

	if (weapon->FireOnce) {
		if (As_Foot(this) != NULL && As_Foot(this)->Team != NULL) {
			As_Foot(this)->Team->Assign_Mission_Target(NULL);
			As_Foot(this)->Team->IsNextMission = true;
		}
		Assign_Target(NULL);
	}

	return(bullet);
}
