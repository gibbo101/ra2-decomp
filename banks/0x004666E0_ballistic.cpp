// BulletClass::AI best natural 80.0% raw (92.2% normalised, 6403/6422 bytes, frame 0x198) with overlay 0x004666E0_bullet_velocity.h (78.7% raw without it); drops into src/bullet.cpp exactly as 0x004666E0_bullet.cpp does (after Mark 0x004666C0, Coord_Dist moved above it, revent.h after voc.h, _$E171 renumbered _$E172). Every inlining decision now matches the exe: the ballistic twin takes float Ballistic_Move's full parameter list (gravity, elasticity, max_velocity, payback) with the max_velocity clamp (dead at 0.0) written through Speed(), and the end of the step is coord = Coord(position.X, position.Y, position.Z) (the exe converts all three before storing). Left: head copies at 0x004670D6, frame slot order and register choices in the twin and the tail.

#include "revent.h"

void FUN_0053AB70(void);
Matrix3D Get_Slope_Matrix(int ramp);

static inline int Coord_Dist(Coord const & a, Coord const & b)
{
	Coord c = a - b;
	return(c.Length());
}

static inline bool Ballistic_Move(TVelocity3D<double> & position, TVelocity3D<double> & velocity, double gravity, double elasticity, double max_velocity, TechnoClass * payback)
{
	velocity.Z -= gravity;

	if (max_velocity > 0.0) {
		double speed = velocity.Speed();
		if (speed > max_velocity) {
			velocity *= (float)(speed / speed);
		}
	}

	Coord old_coord(position.X, position.Y, position.Z);
	position += velocity;

	Coord new_coord(position.X, position.Y, position.Z);
	int height = Map.Get_Height_GL(new_coord);
	int bridge_height = height + BRIDGE_LEPTON_HEIGHT;

	CellClass * cellptr = &Map[new_coord];

	bool fell_through_bridge = false;
	bool rose_through_bridge = false;
	if (cellptr->IsUnderBridge || Map[old_coord].IsUnderBridge) {
		if (new_coord.Z >= bridge_height) {
			if (old_coord.Z < bridge_height) {
				rose_through_bridge = true;
			}
		} else if (old_coord.Z >= bridge_height) {
			fell_through_bridge = true;
		}
	}

	bool hit_obstacle = false;
	if (!fell_through_bridge && !rose_through_bridge) {
		if (position.Z >= height && position.Z - 150 < height) {
			BuildingClass * building = cellptr->Cell_Building();
			if (building != NULL || cellptr->Has_Wall_Or_Gate()) {
				hit_obstacle = true;
				if (building != NULL) {
					if (building == payback || building->Class->IsLaserFence && building->LaserFenceFrame >= 8) {
						hit_obstacle = false;
					}
					if (building->IsStrange()) {
						hit_obstacle = false;
					}
					if (payback != NULL && payback->House->Is_Ally(building)) {
						hit_obstacle = false;
					}
				}
			}
		}
	}

	bool do_bounce = true;
	if (position.Z >= height) {
		if (!fell_through_bridge && !rose_through_bridge && !hit_obstacle) {
			do_bounce = false;
		}
	}

	if (do_bounce) {
		if (payback == NULL) {
			if (fell_through_bridge) {
				position.Z = bridge_height;
			} else if (rose_through_bridge) {
				bridge_height -= 20;
				position.Z = bridge_height;
			} else {
				bridge_height = height - 100;
				if (bridge_height < position.Z) {
					position.Z = height;
				}
			}

			Matrix3D slope_matrix = Get_Slope_Matrix(TacticalMap->Get_Cell_Ramp(new_coord));
			Matrix3D slope_matrix_inv = Matrix3D::Orthogonal_Inverse(slope_matrix);
			Vector3 vec(velocity.X, -velocity.Y, velocity.Z);
			vec = slope_matrix_inv.Rotate_Vector(vec);
			vec *= elasticity;
			vec.Z = -vec.Z;
			vec = slope_matrix.Rotate_Vector(vec);
			velocity.X = vec.X;
			velocity.Y = -vec.Y;
			velocity.Z = vec.Z;

			return(true);
		} else if (fell_through_bridge) {
			position.Z = bridge_height;
			return(true);
		} else if (rose_through_bridge) {
			bridge_height -= 20;
			position.Z = bridge_height;
			return(true);
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x004666E0
void BulletClass::AI(void)
{
	Coord coord;
	int distance;

	BASECLASS::AI();

	if (!IsActive) return;

	if (SpawnNextAnim) {
		if (NextAnim == NULL) {
			PointerExpiredNotification::NotifyInvalidAnim.Delete(this);
			SpawnNextAnim = false;
			Bullet_Explodes(false);
			Delete_Me();
		}
		return;
	}

	bool collided = false;
	bool forced = false;

	if (Class->IsDropping && !IsFalling) {
		forced = true;
	}

	if (Class->AnimLow || Class->AnimHigh) {
		AnimRate--;
		if (AnimRate == 0) {
			AnimRate = Class->AnimRate;
			AnimFrame++;
			if (AnimFrame > Class->AnimHigh) {
				AnimFrame = Class->AnimLow;
			}
		}
	}

	coord = PositionCoord;

	if (Class->Trailer != NULL) {
		if (Class->ScaledSpawnDelay) {
			if (!(Frame % Class->ScaledSpawnDelay)) {
				new AnimClass(Class->Trailer, coord, 1, 1);
			}
		} else if (!(Frame % Class->SpawnDelay)) {
			new AnimClass(Class->Trailer, coord, 1, 1);
		}
	}

	ImpactType impact = IMPACT_NONE;
	
	if (Class->ROT > 0) {
		int max_speed = MaxSpeed;
		double speed = Velocity.Speed();

		if (Class->CourseLockDuration == 0) {
			if (MaxSpeed >= 40 || speed + 0.5 >= max_speed) {
				IsLaunching = false;
			}
		} else if (CourseLockCounter < Class->CourseLockDuration) {
			if (++CourseLockCounter >= Class->CourseLockDuration) {
				IsLaunching = false;
			}
		}

		int acceleration = Class->Acceleration;

		if (IsLaunching && Class->CourseLockDuration == 0) {
			acceleration = (Frame % 2 == 0) ? 1 : 0;
		}

		if (speed < max_speed) {
			speed += acceleration;
			if (speed >= max_speed) {
				speed = max_speed;
			}
			Velocity.Set_Speed(speed);
		} else if (speed > max_speed) {
			speed -= acceleration / 2;
			if (speed <= 0) {
				speed = 0;
			}
			Velocity.Set_Speed(speed);
		}

		TVelocity3D<double> velocity = Velocity;

		Coord target_coord = TarCom != NULL ? TarCom->As_Coord() : COORD_NONE;
		if (TarCom != NULL && (TarCom->AbstractFlags & 2)) {
			target_coord = ((ObjectClass *)TarCom)->Target_Coord();
		}

		double & rotvar = Rule->MissileROTVar;
		double phase = ((Frame + Fetch_ID()) % 15) * (1.0 / 15);
		int rot = (fastmath::sin(phase * DEG_TO_RAD(360)) * rotvar + (rotvar + 1)) * Class->ROT;

		if (Coord_Dist(Center_Coord(), target_coord) < CELL_LEPTON) {
			rot *= 1.5;
		}

		Coord old_coord = coord;

		int is_aircraft;
		if (TarCom != NULL && TarCom->Fetch_RTTI() == RTTI_AIRCRAFT) {
			is_aircraft = true;
		} else {
			is_aircraft = false;
		}

		distance = Projectile_Motion(coord, velocity, target_coord, DirType((Dir256)(IsLaunching == false ? rot : 0)), (bool &)is_aircraft, Class->IsAirburst, Class->IsVeryHigh, Class->Level);
		CellClass * cellptr = &Map[coord];
		TVelocity3D<double> new_velocity = velocity;
		Velocity = new_velocity;

		double half_speed = Velocity.Speed() * 0.5;
		if (distance <= half_speed || HeightAGL <= 0) {
			forced = true;
			impact = IMPACT_NORMAL;
			if (HeightAGL > 0 && !Class->IsAirburst) {
				if (target_coord != COORD_NONE) {
					coord = target_coord;
				}
			}
		}

		if (target_coord == COORD_NONE && HeightAGL >= Rule->MissileSafetyAltitude) {
			forced = true;
			impact = IMPACT_NORMAL;
		}

		int delta = Coord_Dist(old_coord, target_coord) - Coord_Dist(coord, target_coord);

		if (!IsLaunching) {
			if (ClosureSamples < 60) {
				ClosureSamples++;
				SmoothedClosure += delta;
			} else {
				SmoothedClosure = SmoothedClosure * (59.0 / 60.0) + delta;
				if (SmoothedClosure >= 0 && SmoothedClosure < 60) {
					if (!Class->IsAirburst && !Class->IsVeryHigh) {
						forced = true;
						impact = IMPACT_NORMAL;
					}
				}
			}
		}

		if (impact == IMPACT_NONE && (cellptr->IsUnderBridge || Map[old_coord].IsUnderBridge)) {
			int bridge_height = Map.Get_Height_GL(coord) + BRIDGE_LEPTON_HEIGHT;
			if (coord.Z > bridge_height && old_coord.Z < bridge_height) {
				impact = IMPACT_NORMAL;
				coord.Z = bridge_height;
				forced = true;
			} else if (coord.Z < bridge_height && old_coord.Z > bridge_height) {
				impact = IMPACT_NORMAL;
				coord.Z = bridge_height;
				forced = true;
			}
		}
	} else {
		TVelocity3D<double> position = Velocity;
		TVelocity3D<double> new_velocity;
		new_velocity = position;
		TVelocity3D<double> velocity(position.X, position.Y, position.Z);

		if (position.Speed() < 8) {
			impact = IMPACT_NORMAL;
		}
		position.X = coord.X;
		position.Y = coord.Y;
		position.Z = coord.Z;

		double gravity = Rule->Gravity;
		if (Class->IsFloater) {
			gravity = Get_Floater_Gravity();
		}

		if (Class->Vertical) {
			int speed = Velocity.Speed();
			if (speed < MaxSpeed) {
				Velocity.Set_Speed(Class->Acceleration + speed);
			}

			TVelocity3D<double> vel = Velocity;
			Coord step(vel.X, vel.Y, vel.Z);
			Coord old_coord = coord;
			coord += step;

			if (coord.Z > Class->DetonationAltitude) {
				impact = IMPACT_NORMAL;
				forced = true;
			} else if (HeightAGL < 0) {
				impact = IMPACT_NORMAL;
				forced = true;
			} else {
				int bridge_height = Map.Get_Height_GL(coord) + BRIDGE_LEPTON_HEIGHT;
				if (Map[coord].IsUnderBridge || Map[old_coord].IsUnderBridge) {
					if (coord.Z >= bridge_height) {
						if (old_coord.Z < bridge_height) {
							impact = IMPACT_NORMAL;
							forced = true;
						}
					} else if (old_coord.Z >= bridge_height) {
						impact = IMPACT_NORMAL;
						forced = true;
					}
				}
			}
		} else {
			if (Ballistic_Move(position, velocity, gravity, Class->Elasticity, 0.0, Payback)) {
				impact = IMPACT_NORMAL;
				forced = true;
			}


			coord = Coord(position.X, position.Y, position.Z);
		}

		Cell cell = coord.As_Cell();
		Cell target_cell = TargetCoords.As_Cell();
		if (cell == target_cell && !Class->Vertical && HeightAGL < LEVEL_LEPTON_H * 2) {
			impact = IMPACT_NORMAL;
			forced = true;
			collided = true;
		} else {
			CellClass * cellptr = &Map[cell];
			CellClass * target_cellptr = &Map[target_cell];
			BuildingClass * building = cellptr->Cell_Building();
			BuildingClass * target_building = target_cellptr->Cell_Building();
			if (building != NULL && !Class->Vertical && building == target_building && HeightAGL < LEVEL_LEPTON_H * 2) {
				impact = IMPACT_NORMAL;
				forced = true;
				collided = true;
			} else {
				TechnoClass * techno = Map[coord].Cell_Techno();
				bool has_techno = techno != NULL;
				bool has_payback = Payback != NULL;
				bool is_payback = has_payback && techno == Payback;
				bool is_ally = has_techno && has_payback && Payback->House->Is_Ally(techno);
				bool is_close = has_techno && coord.Distance_To(techno->PositionCoord) < 128;

				if (!is_payback && !is_ally && is_close) {
					impact = IMPACT_NORMAL;
					forced = true;
					if (!Class->IsInaccurate) {
						coord = techno->PositionCoord;
					}
				} else if (!Map.In_Radar(coord)) {
					impact = IMPACT_EDGE;
					forced = true;
					coord = PositionCoord;
				} else {
					new_velocity.X = velocity.X;
					new_velocity.Y = velocity.Y;
					if (!Class->Vertical) {
						TVelocity3D<double> v = new_velocity;
						v.Z = velocity.Z;
						Velocity = v;
					}

					if (Velocity.Speed() < 10 && HeightAGL < 10) {
						impact = IMPACT_NORMAL;
						forced = true;
					}
				}
			}
		}
	}

	switch (impact) {
		case IMPACT_EDGE:
			Mark();
			Delete_Me();
			break;

		default: {
			Mark();
			PositionCoord = coord;

			if (!forced) {
				Coord cur_coord = PositionCoord;
				forced = Is_Forced_To_Explode(cur_coord);
				PositionCoord = cur_coord;
			}

			if (forced) {
				if (HeightAGL < 0) {
					HeightAGL = 0;
				}
			}

			FuseResultType fuse = (Class->ROT > 0 || Class->IsFueled) ? Fuse.Fuse_Checkup(coord) : FUSE_WAIT;
			if (Payback != NULL && Payback->TClass->JumpJet && fuse == FUSE_EXPLODE_FAR) {
				fuse = FUSE_EXPLODE_CLOSE;
			}

			if (!forced && (Class->IsDropping || fuse == FUSE_WAIT)) {
				if (Class->IsDegenerate && Strength > 5) {
					Strength--;
				}
			} else {
				if (TarCom != NULL) {
					if ((fuse == FUSE_EXPLODE_CLOSE || collided) && !Class->IsAirburst && !Class->IsInaccurate) {
						Coord midpoint = coord;
						Coord target_coord = TarCom->As_Coord();
						midpoint.Z = (midpoint.Z + target_coord.Z) / 2;
						int target_distance = midpoint.Distance_To(target_coord);
						if (collided) {
							target_distance /= 3;
						}

						if (fuse == FUSE_EXPLODE_CLOSE || target_distance <= MAX(CELL_LEPTON / 2, Velocity.Speed() * 2)) {
							PositionCoord = TarCom->Center_Coord();
						}
					}
				}

				if (*Warhead == "NUKE") {
					if (HeightAGL < 0) {
						HeightAGL = 0;
					}
					FUN_0053AB70();
					RadarEventClass::Create(RADAREVENT_SUPERWEAPON_ACTIVATED, Get_Cell());
					AnimType anim = AnimTypeClass::From_Name("NUKEBALL");
					if (anim != ANIM_NONE) {
						AnimTypeClass * animtype = AnimTypes[anim];
						if (animtype != NULL) {
							NextAnim = new AnimClass(animtype, PositionCoord, 0, 1, ShapeFlags_Type(SHAPE_WIN_REL|SHAPE_CENTER|0x2000), Get_Explosion_Z(PositionCoord));
							SpawnNextAnim = true;
							PointerExpiredNotification::NotifyInvalidAnim.Add(this);
							break;
						}
					}
				}
				Bullet_Explodes(forced);
				Delete_Me();
			}
			break;
		}
	}

	LastMapCoords = coord.As_Cell();
}
