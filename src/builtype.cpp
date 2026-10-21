// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
#include "builtype.h"
#include "_map.h"
#include "_mixfile.h"
#include "_rules.h"
#include "_theater.h"
#include "airctype.h"
#include "bound.h"
#include "bsurface.h"
#include "building.h"
#include "bullettype.h"
#include "ccfile.h"
#include "cell.h"
#include "data.h"
#include "findmake.h"
#include "foot.h"
#include "globals.h"
#include "house.h"
#include "incdec.h"
#include "isotype.h"
#include "mixfile.h"
#include "msanim.h"
#include "overtype.h"
#include "rules.h"
#include "scenario.h"
#include "shapeset.h"
#include "sun.h"
#include "swizzle.h"
#include "tracker.h"
#include "unittype.h"
#include "warhead.h"
#include "weapon.h"

// FUNCTION: 0x0045B1B0 $E222
// FUNCTION: 0x0045B1C0 $E199
Cell const BuildingTypeClass::OccupyLists[BSIZE_COUNT][30] = {
	/* BSIZE_11 */ { Cell(0,0),REFRESH_EOL },
	/* BSIZE_21 */ { Cell(0,0),Cell(1,0),REFRESH_EOL },
	/* BSIZE_12 */ { Cell(0,0),Cell(0,1),REFRESH_EOL },
	/* BSIZE_22 */ { Cell(0,0),Cell(1,0),Cell(0,1),Cell(1,1),REFRESH_EOL },
	/* BSIZE_23 */ { Cell(0,0),Cell(1,0),Cell(0,1),Cell(1,1),Cell(0,2),Cell(1,2),REFRESH_EOL },
	/* BSIZE_32 */ { Cell(0,0),Cell(1,0),Cell(2,0),Cell(0,1),Cell(1,1),Cell(2,1),REFRESH_EOL },
	/* BSIZE_33 */ { Cell(0,0),Cell(1,0),Cell(2,0),Cell(0,1),Cell(1,1),Cell(2,1),Cell(0,2),Cell(1,2),Cell(2,2),REFRESH_EOL },
	/* BSIZE_35 */ { Cell(0,0),Cell(1,0),Cell(2,0),Cell(0,1),Cell(1,1),Cell(2,1),Cell(0,2),Cell(1,2),Cell(2,2),Cell(0,3),Cell(1,3),Cell(2,3),Cell(0,4),Cell(1,4),Cell(2,4),REFRESH_EOL },
	/* BSIZE_42 */ { Cell(0,0),Cell(1,0),Cell(2,0),Cell(3,0),Cell(0,1),Cell(1,1),Cell(2,1),Cell(3,1),REFRESH_EOL },
	/* BSIZE_33_REF */ { Cell(0,0),Cell(1,0),Cell(2,0),Cell(0,1),Cell(1,1),Cell(0,2),Cell(1,2),Cell(2,2),REFRESH_EOL },
	/* BSIZE_13 */ { Cell(0,0),Cell(0,1),Cell(0,2),REFRESH_EOL },
	/* BSIZE_31 */ { Cell(0,0),Cell(1,0),Cell(2,0),REFRESH_EOL },
	/* BSIZE_43 */ { Cell(0,0),Cell(1,0),Cell(2,0),Cell(3,0),Cell(0,1),Cell(1,1),Cell(2,1),Cell(3,1),Cell(0,2),Cell(1,2),Cell(2,2),Cell(3,2),REFRESH_EOL },
	/* BSIZE_14 */ { Cell(0,0),Cell(0,1),Cell(0,2),Cell(0,3),REFRESH_EOL },
	/* BSIZE_15 */ { Cell(0,0),Cell(0,1),Cell(0,2),Cell(0,3),Cell(0,4),REFRESH_EOL },
	/* BSIZE_26 */ { Cell(0,0),Cell(1,0),Cell(0,1),Cell(1,1),Cell(0,2),Cell(1,2),Cell(0,3),Cell(1,3),Cell(0,4),Cell(1,4),Cell(0,5),Cell(1,5),REFRESH_EOL },
	/* BSIZE_25 */ { Cell(0,0),Cell(1,0),Cell(0,1),Cell(1,1),Cell(0,2),Cell(1,2),Cell(0,3),Cell(1,3),Cell(0,4),Cell(1,4),REFRESH_EOL },
	/* BSIZE_53 */ { Cell(0,0),Cell(1,0),Cell(2,0),Cell(3,0),Cell(4,0),Cell(0,1),Cell(1,1),Cell(2,1),Cell(3,1),Cell(4,1),Cell(0,2),Cell(1,2),Cell(2,2),Cell(3,2),Cell(4,2),REFRESH_EOL },
	/* BSIZE_44 */ { Cell(0,0),Cell(1,0),Cell(2,0),Cell(3,0),Cell(0,1),Cell(1,1),Cell(2,1),Cell(3,1),Cell(0,2),Cell(1,2),Cell(2,2),Cell(3,2),Cell(0,3),Cell(1,3),Cell(2,3),Cell(3,3),REFRESH_EOL },
	/* BSIZE_34 */ { Cell(0,0),Cell(1,0),Cell(2,0),Cell(0,1),Cell(1,1),Cell(2,1),Cell(0,2),Cell(1,2),Cell(2,2),Cell(0,3),Cell(1,3),Cell(2,3),REFRESH_EOL },
	/* BSIZE_64 */ { Cell(0,0),Cell(1,0),Cell(2,0),Cell(3,0),Cell(4,0),Cell(5,0),Cell(0,1),Cell(1,1),Cell(2,1),Cell(3,1),Cell(4,1),Cell(5,1),Cell(0,2),Cell(1,2),Cell(2,2),Cell(3,2),Cell(4,2),Cell(5,2),Cell(0,3),Cell(1,3),Cell(2,3),Cell(3,3),Cell(4,3),Cell(5,3),REFRESH_EOL },
	/* BSIZE_00 */ { REFRESH_EOL }
};

// FUNCTION: 0x0045C2F0 $E247
// FUNCTION: 0x0045C300 $E224
Cell const BuildingTypeClass::ExitLists[BSIZE_COUNT][30] = {
	/* BSIZE_11 */ { Cell(0,1),Cell(-1,1),Cell(1,1),Cell(-1,0),Cell(1,0),Cell(0,-1),Cell(-1,-1),Cell(1,-1),REFRESH_EOL },
	/* BSIZE_21 */ { Cell(0,1),Cell(1,1),Cell(-1,1),Cell(2,1),Cell(-1,0),Cell(2,0),Cell(0,-1),Cell(1,-1),Cell(-1,-1),Cell(2,-1),REFRESH_EOL },
	/* BSIZE_12 */ { Cell(0,2),Cell(1,2),Cell(-1,2),Cell(-1,1),Cell(1,1),Cell(-1,0),Cell(1,0),Cell(0,-1),Cell(-1,-1),Cell(1,-1),REFRESH_EOL },
	/* BSIZE_22 */ { Cell(0,2),Cell(1,2),Cell(-1,2),Cell(2,2),Cell(-1,1),Cell(2,1),Cell(-1,0),Cell(2,0),Cell(0,-1),Cell(1,-1),Cell(-1,-1),Cell(2,-1),REFRESH_EOL },
	/* BSIZE_23 */ { Cell(0,3),Cell(1,3),Cell(-1,3),Cell(2,3),Cell(-1,2),Cell(2,2),Cell(-1,1),Cell(2,1),Cell(-1,0),Cell(2,0),Cell(0,-1),Cell(1,-1),Cell(-1,-1),Cell(2,-1),REFRESH_EOL },
	/* BSIZE_32 */ { Cell(0,2),Cell(1,2),Cell(2,2),Cell(-1,2),Cell(3,2),Cell(-1,1),Cell(3,1),Cell(-1,0),Cell(3,0),Cell(0,-1),Cell(1,-1),Cell(2,-1),Cell(-1,-1),Cell(3,-1),REFRESH_EOL },
	/* BSIZE_33 */ { Cell(0,3),Cell(1,3),Cell(2,3),Cell(-1,3),Cell(3,3),Cell(-1,2),Cell(3,2),Cell(-1,1),Cell(3,1),Cell(-1,0),Cell(3,0),Cell(0,-1),Cell(1,-1),Cell(2,-1),Cell(-1,-1),Cell(3,-1),REFRESH_EOL },
	/* BSIZE_35 */ { Cell(-1,-1),Cell(0,-1),Cell(1,-1),Cell(2,-1),Cell(3,-1),Cell(-1,0),Cell(3,0),Cell(-1,1),Cell(3,1),Cell(-1,2),Cell(3,2),Cell(-1,3),Cell(3,3),Cell(-1,4),Cell(3,4),Cell(-1,5),Cell(0,5),Cell(1,5),Cell(2,5),Cell(3,5),REFRESH_EOL },
	/* BSIZE_42 */ { Cell(0,2),Cell(1,2),Cell(2,2),Cell(3,2),Cell(-1,2),Cell(4,2),Cell(-1,1),Cell(4,1),Cell(-1,0),Cell(4,0),Cell(0,-1),Cell(1,-1),Cell(2,-1),Cell(3,-1),Cell(-1,-1),Cell(4,-1),REFRESH_EOL },
	/* BSIZE_33_REF */ { Cell(0,0),REFRESH_EOL },
	/* BSIZE_13 */ { Cell(-1,-1),Cell(0,-1),Cell(1,-1),Cell(-1,0),Cell(1,0),Cell(-1,1),Cell(1,1),Cell(-1,2),Cell(1,2),Cell(-1,3),Cell(0,3),Cell(1,3),REFRESH_EOL },
	/* BSIZE_31 */ { Cell(-1,-1),Cell(0,-1),Cell(1,-1),Cell(2,-1),Cell(3,-1),Cell(-1,0),Cell(3,0),Cell(-1,1),Cell(0,1),Cell(1,1),Cell(2,1),Cell(3,1),REFRESH_EOL },
	/* BSIZE_43 */ { Cell(0,3),Cell(1,3),Cell(2,3),Cell(-1,3),Cell(3,3),Cell(-1,2),Cell(3,2),Cell(-1,1),Cell(3,1),Cell(-1,0),Cell(3,0),Cell(0,-1),Cell(1,-1),Cell(2,-1),Cell(-1,-1),Cell(3,-1),REFRESH_EOL },
	/* BSIZE_14 */ { Cell(-1,-1),Cell(0,-1),Cell(1,-1),Cell(-1,0),Cell(1,0),Cell(-1,1),Cell(1,1),Cell(-1,2),Cell(1,2),Cell(-1,3),Cell(1,3),Cell(-1,4),Cell(0,4),Cell(1,4),REFRESH_EOL },
	/* BSIZE_15 */ { Cell(-1,-1),Cell(0,-1),Cell(1,-1),Cell(-1,0),Cell(1,0),Cell(-1,1),Cell(1,1),Cell(-1,2),Cell(1,2),Cell(-1,3),Cell(1,3),Cell(-1,4),Cell(1,4),Cell(-1,5),Cell(0,5),Cell(1,5),REFRESH_EOL },
	/* BSIZE_26 */ { Cell(-1,-1),Cell(0,-1),Cell(1,-1),Cell(2,-1),Cell(-1,0),Cell(2,0),Cell(-1,1),Cell(2,1),Cell(-1,2),Cell(2,2),Cell(-1,3),Cell(2,3),Cell(-1,5),Cell(2,5),Cell(-1,6),Cell(0,6),Cell(1,6),Cell(2,6),REFRESH_EOL },
	/* BSIZE_25 */ { Cell(-1,-1),Cell(0,-1),Cell(1,-1),Cell(2,-1),Cell(-1,0),Cell(2,0),Cell(-1,1),Cell(2,1),Cell(-1,2),Cell(2,2),Cell(-1,3),Cell(2,3),Cell(-1,5),Cell(0,5),Cell(1,5),Cell(2,5),REFRESH_EOL },
	/* BSIZE_53 */ { Cell(-1,-1),Cell(0,-1),Cell(1,-1),Cell(2,-1),Cell(3,-1),Cell(4,-1),Cell(5,-1),Cell(-1,0),Cell(5,0),Cell(-1,1),Cell(5,1),Cell(-1,2),Cell(5,2),Cell(-1,3),Cell(0,3),Cell(1,3),Cell(2,3),Cell(3,3),Cell(4,3),Cell(5,3),REFRESH_EOL },
	/* BSIZE_44 */ { Cell(-1,-1),Cell(0,-1),Cell(1,-1),Cell(2,-1),Cell(3,-1),Cell(4,-1),Cell(-1,4),Cell(0,4),Cell(1,4),Cell(2,4),Cell(3,4),Cell(4,4),Cell(-1,0),Cell(-1,1),Cell(-1,2),Cell(-1,3),Cell(-1,4),Cell(4,0),Cell(4,1),Cell(4,2),Cell(4,3),Cell(4,4),REFRESH_EOL },
	/* BSIZE_34 */ { Cell(0,4),Cell(1,4),Cell(2,4),Cell(-1,4),Cell(3,4),Cell(-1,2),Cell(3,2),Cell(-1,1),Cell(3,1),Cell(-1,0),Cell(3,0),Cell(0,-1),Cell(1,-1),Cell(2,-1),Cell(-1,-1),Cell(3,-1),REFRESH_EOL },
	/* BSIZE_64 */ { Cell(2,-1),REFRESH_EOL },
	/* BSIZE_00 */ { REFRESH_EOL }
};


static struct {
	char const * Name;
	int Value;
} _buildcats[6] = {
	{ "DontCare", 0 },
	{ "Tech", 1 },
	{ "Power", 3 },
	{ "Resource", 2 },
	{ "Infrastructure", 4 },
	{ "Combat", 5 }
};

// FUNCTION: 0x0045DD20
char const * Name_From_BuildCat(int value)
{
	for (int index = 0; index < 6; index++) {
		if (_buildcats[index].Value == value) {
			return(_buildcats[index].Name);
		}
	}
	return(_buildcats[0].Name);
}

// FUNCTION: 0x0045DD50
int BuildCat_From_Name(char const * name)
{
	for (int index = 0; index < 6; index++) {
		if (strcmpi(_buildcats[index].Name, name) == 0) {
			return(_buildcats[index].Value);
		}
	}
	return(0);
}

// FUNCTION: 0x0045E520
BuildingTypeClass::BuildingTypeClass(NoInitClass const & x) :
	BASECLASS(x),
	BuildupData(NULL),
	BuildupFilename(x),
	PowersUpBuilding(x)
{
}

// FUNCTION: 0x0045E580
BuildingTypeClass::~BuildingTypeClass(void)
{
	if (BuildupLoaded && BuildupData != NULL) {
		delete (ShapeSet *)BuildupData;
		BuildupData = NULL;
		BuildupLoaded = false;
	}
	if (BibShapeLoaded && BibShape != NULL) {
		delete (ShapeSet *)BibShape;
		BibShape = NULL;
		BibShapeLoaded = false;
	}
	if (DeployingAnimLoaded && DeployingAnim != NULL) {
		delete (ShapeSet *)DeployingAnim;
		DeployingAnim = NULL;
		DeployingAnimLoaded = false;
	}
	if (UnderDoorAnimLoaded && UnderDoorAnim != NULL) {
		delete (ShapeSet *)UnderDoorAnim;
		UnderDoorAnim = NULL;
		UnderDoorAnimLoaded = false;
	}
	if (RubbleLoaded && Rubble != NULL) {
		delete (ShapeSet *)Rubble;
		Rubble = NULL;
		RubbleLoaded = false;
	}
	if (RoofDeployingAnimLoaded && RoofDeployingAnim != NULL) {
		delete (ShapeSet *)RoofDeployingAnim;
		RoofDeployingAnim = NULL;
		RoofDeployingAnimLoaded = false;
	}
	if (UnderRoofDoorAnimLoaded && UnderRoofDoorAnim != NULL) {
		delete (ShapeSet *)UnderRoofDoorAnim;
		UnderRoofDoorAnim = NULL;
		UnderRoofDoorAnimLoaded = false;
	}
	Detach_This_From_All(this, true);
	BuildingTypes.Delete(this);
}

// FUNCTION: 0x0045E750
void BuildingTypeClass::One_Time(void)
{
}

// FUNCTION: 0x0045E760
StructType BuildingTypeClass::From_Given_Name(char const * name)
{
	if (name != NULL) {
		for (int classid = STRUCT_FIRST; classid < BuildingTypes.Count(); classid++) {
			if (stricmp(BuildingTypes[classid]->GivenName, name) == 0) {
				return((StructType)classid);
			}
		}
	}
	return(STRUCT_NONE);
}

// FUNCTION: 0x0045E7B0
StructType BuildingTypeClass::From_Name(char const * name)
{
	if (name != NULL) {
		for (StructType classid = STRUCT_FIRST; classid < BuildingTypes.Count(); classid++) {
			if (stricmp(BuildingTypes[classid]->Name(), name) == 0) {
				return(classid);
			}
		}
	}
	return(STRUCT_NONE);
}

// FUNCTION: 0x0045E800
bool BuildingTypeClass::Create_And_Place(Cell const & cell, HouseClass * house) const
{
	BuildingClass * ptr;

	ptr = new BuildingClass(this, house);
	if (ptr != NULL) {
		return(ptr->Unlimbo(cell, DIR_N));
	}
	return(false);
}

// FUNCTION: 0x0045E880
ObjectClass * BuildingTypeClass::Create_One_Of(HouseClass * house) const
{
	return(new BuildingClass(this, house));
}

// FUNCTION: 0x0045E8B0
void BuildingTypeClass::Init_Anim(BStateType state, int start, int count, int rate) const
{
	((int &)Anims[state].Start) = start;
	((int &)Anims[state].Count) = count;
	((int &)Anims[state].Rate) = rate;
}

struct FileSystem {
	static void const * LoadFile(char const * name, bool flag);
};

// FUNCTION: 0x0045E8F0
void BuildingTypeClass::Fetch_Z_Data(void)
{
	if (BuildingZShape == NULL) {
		BuildingZShape = FileSystem::LoadFile("BUILDNGZ.SHA", false);
		((SHPStruct const *)BuildingZShape)->GetFrameBounds(0);
		char * data = (char *)((SHPStruct const *)BuildingZShape)->GetPixels(0);
		int width = ((SHPStruct const *)BuildingZShape)->Get_Width();
		int height = ((SHPStruct const *)BuildingZShape)->Get_Height();

		for (int h = 0; h < height; h++) {
			for (int w = 0; w < width; w++) {
				if (data[h * width + w] != 0) {
					data[h * width + w] -= 0x41;
				}
			}
		}
	}
}

// FUNCTION: 0x0045E970
void BuildingTypeClass::Init(TheaterType theater)
{
	Fetch_Z_Data();
	PowerOffShapes = FileSystem::LoadFile("POWEROFF.SHP", false);
	WrenchShapes = FileSystem::LoadFile("WRENCH.SHP", false);

	char fullname[_MAX_FNAME+_MAX_EXT];

	int maxcloak = 0;

	for (StructType sindex = STRUCT_FIRST; sindex < BuildingTypes.Count(); sindex++) {
		BuildingTypeClass * classptr = BuildingTypes[sindex];

		maxcloak = MAX(maxcloak, classptr->CloakRadiusInCells);

		if (classptr->IsTheater) {
			if (!classptr->IsDemandLoad) {
				_makepath(fullname, NULL, NULL, classptr->Graphic_Name(), Theaters[theater].Suffix);
				classptr->ImageData = FileSystem::LoadFile(fullname, false);
			} else {
				if (classptr->ImageData != NULL) {
					delete (ShapeSet *)classptr->ImageData;
					classptr->ImageData = NULL;
					classptr->ImageAllocated = false;
				}
			}

			if (!classptr->IsDemandLoadBuildup) {
				_makepath(fullname, NULL, NULL, classptr->BuildupFilename, Theaters[theater].Suffix);
				classptr->BuildupData = FileSystem::LoadFile(fullname, false);
			} else {
				if (classptr->BuildupData != NULL) {
					delete (ShapeSet *)classptr->BuildupData;
					classptr->BuildupData = NULL;
					classptr->BuildupLoaded = false;
				}
			}

			if (classptr->BuildupData) {
				int timedelay = 1;
				int count = ((SHPStruct const *)classptr->BuildupData)->Get_Count();
				if (count != 0) {
					timedelay = (5 * TICKS_PER_SECOND) / count;
				}
				classptr->Init_Anim(BSTATE_CONSTRUCTION, 0, count, timedelay);
			}
		} else if (classptr->IsNewTheater) {
			if (classptr->IsDemandLoad) {
				if (classptr->ImageData != NULL) {
					delete (ShapeSet *)classptr->ImageData;
					classptr->ImageData = NULL;
					classptr->ImageAllocated = false;
				}
			}
			if (classptr->IsDemandLoadBuildup) {
				if (classptr->BuildupData != NULL) {
					delete (ShapeSet *)classptr->BuildupData;
					classptr->BuildupData = NULL;
					classptr->BuildupLoaded = false;
				}
			}
			classptr->Fetch_Building_Normal_Image(theater);
		}
	}

	if (maxcloak != 0) {
		delete CloakingSurface;
		CloakingSurface = new BSurface(maxcloak + 16, maxcloak + 16, 1);
	}
}

// FUNCTION: 0x0045EBD0
Point3D BuildingTypeClass::Pixel_Dimensions(void) const
{
	SHPStruct const * data = (SHPStruct const *)Get_Image_Data();
	int w;
	int h;
	if (data != NULL) {
		Rect r = data->GetFrameBounds(0);
		w = r.Width;
		h = r.Height;
	} else {
		w = 0;
		h = 0;
	}
	return(Point3D(w, w, h));
}

// FUNCTION: 0x0045EC20
Cell const * BuildingTypeClass::Occupy_List(bool placement) const
{
	static Cell const _templap[] = {REFRESH_EOL};
	if (OccupyList != NULL) {
		return(OccupyList);
	}
	return(_templap);
}

// FUNCTION: 0x0045EC80 $E
// FUNCTION: 0x0045EC90
int BuildingTypeClass::Width(void) const
{
	return(SizeWidth[Size]);
}

// FUNCTION: 0x0045ECA0
int BuildingTypeClass::Height(bool bib) const
{
	return(SizeHeight[Size] + ((bib && IsBibbed) ? 1 : 0));
}

// FUNCTION: 0x0045ECE0
int BuildingTypeClass::Max_Pips(void) const
{
	int maxpips = (Width() * 60) / 8;

	switch (PipScale) {
		case PIPSCALE_TIBERIUM:
			if (IsWeeder) {
				return(Bound((int)(Rule->WeedCapacity), 0, maxpips));
			} else {
				return(Bound((int)(Capacity), 0, maxpips));
			}

		case PIPSCALE_POWER:
			return(maxpips);

		default:
			return(BASECLASS::Max_Pips());
	}
}

// FUNCTION: 0x0045ED50
int BuildingTypeClass::Raw_Cost(void) const
{
	int cost = BASECLASS::Raw_Cost();

	if (this == Rule->PadAircraft[0]->Dock[0] && !Rule->IsSeparate) {
		cost -= (Rule->PadAircraft[0]->Raw_Cost() + Rule->PadAircraft[1]->Raw_Cost())/2;
	}
	if (FreeUnit != NULL) {
		cost -= FreeUnit->Raw_Cost();
		cost = MAX(cost, 0);
	}
	return(cost);
}

// FUNCTION: 0x0045EDD0
int BuildingTypeClass::Cost_Of(HouseClass * house) const
{
	int cost = BASECLASS::Cost_Of(house);

	if (this == Rule->PadAircraft[0]->Dock[0] && !Rule->IsSeparate) {
		cost += (Rule->PadAircraft[0]->Cost_Of(house) + Rule->PadAircraft[1]->Cost_Of(house))/2;
	}
	if (FreeUnit != NULL) {
		cost += FreeUnit->Cost_Of(house);
		cost = MAX(cost, 0);
	}
	return(cost);
}

// FUNCTION: 0x0045EE70
int BuildingTypeClass::Flush_For_Placement(Cell const & cell, HouseClass * house) const
{
	bool again = false;
	if (cell != CELL_NONE) {
		Cell const * list = Occupy_List(true);

		while (*list != REFRESH_EOL) {
			Cell newcell = cell + *list++;

			if (Map.In_Radar(newcell)) {
				CellClass & cptr = Map[newcell];
				if (cptr.Overlay != OVERLAY_NONE && (this != Rule->WallTower || cptr.Overlay != OVERLAY_BRICK_WALL)) {
					return(2);
				}
				ObjectClass * occupier = cptr.Cell_Occupier();
				if (occupier != NULL) {
					if (occupier->RTTI == RTTI_TERRAIN) {
						return(2);
					}
					if (occupier->AbstractFlags & 1) {
						TechnoClass * techno = (TechnoClass *)occupier;
						if (techno->RTTI == RTTI_BUILDING) {
							if (!((BuildingClass *)techno)->Can_Upgrade(this, house)) {
								return(2);
							}
						} else {
							if (techno->House->Is_Ally(house) && techno->Is_Foot()) {
								again = true;
								if (((FootClass *)techno)->NavCom == NULL || &Map[techno->Get_Coord()] == ((FootClass *)techno)->NavCom) {
									Map[newcell].Incoming(COORD_NONE, true, true);
								}
							} else {
								return(2);
							}
						}
					}
				}
			}
		}
	}
	if (again) {
		return(1);
	} else {
		return(0);
	}
}

void FUN_005F9710(char * name);

// FUNCTION: 0x0045F040
void const * BuildingTypeClass::Get_Image_Data(void) const
{
	if (ImageData == NULL && IsDemandLoad) {
		if (TheaterImageFile[0] != '\0') {
			CCFileClass file(TheaterImageFile);
			file.Is_Available(false);
			(void *&)ImageData = Load_Alloc_Data(file);
			if (ImageData != NULL) {
				(bool &)ImageAllocated = true;
			} else {
				char fallback[260];
				strncpy(fallback, TheaterImageFile, 19);
				FUN_005F9710(fallback);
				CCFileClass file2(fallback);
				(void *&)ImageData = Load_Alloc_Data(file2);
				if (ImageData != NULL) {
					(bool &)ImageAllocated = true;
				}
			}
		}
		return(ImageData);
	}
	return(ImageData);
}

// FUNCTION: 0x0045F160
bool BuildingTypeClass::FUN_0045F160(ShapeSet const ** shape, int * frame)
{
	if (LeaveRubble && shape != NULL) {
		if (Rubble == NULL) {
			*shape = (ShapeSet const *)Get_Image_Data();
			if (*shape != NULL && (*shape)->Get_Count() / 2 > 3) {
				*frame = 3;
				return(true);
			}
		} else {
			*shape = Rubble;
			*frame = 0;
			return(true);
		}
	}
	return(false);
}

// FUNCTION: 0x0045F1D0
bool BuildingTypeClass::FUN_0045F1D0(ShapeSet const ** shape, int * frame)
{
	if (LeaveRubble && shape != NULL) {
		ShapeSet const * rubble = Rubble;
		if (rubble == NULL) {
			*shape = (ShapeSet const *)Get_Image_Data();
			if (*shape != NULL && (*shape)->Get_Count() / 2 > 3) {
				*frame = (*shape)->Get_Count() / 2 + 3;
				return(true);
			}
		} else {
			*shape = rubble;
			*frame = rubble->Get_Count() / 2;
		}
	}
	return(false);
}

// FUNCTION: 0x0045FA90
void BuildingTypeClass::Fetch_Building_Voxel_Image(void)
{
	char buffer[128];
	char fullname[256];

	if (IsTurretAnimAVoxel || IsBarrelAnimAVoxel) {
		strcpy(buffer, AnimData[BANIM_TURRET].Anim);
		_makepath(fullname, NULL, NULL, buffer, ".VXL");

		if (strstr(strupr(&fullname[4]), strupr("TUR"))) {
			CCFileClass vxl(fullname);
			if (vxl.Is_Available()) {
				delete AuxVoxel.VoxLib;
				AuxVoxel.VoxLib = new VoxelLibrary(vxl);

				_makepath(fullname, NULL, NULL, buffer, ".HVA");
				CCFileClass hva(fullname);
				delete AuxVoxel.MotLib;
				AuxVoxel.MotLib = new MotionLibrary(hva);

				if (AuxVoxel.MotLib != NULL && !AuxVoxel.MotLib->Load_Failed()) {
					AuxVoxel.MotLib->Scale(AuxVoxel.VoxLib->Get_Layer_Info(0, 0).Scale);
				}
			}

			if (char * nameroot = strstr(strupr(&buffer[4]), strupr("TUR"))) {
				*nameroot++ = 'B';
				*nameroot++ = 'A';
				*nameroot++ = 'R';
				*nameroot++ = 'L';
				*nameroot++ = '\0';
			}
		} else if (IsBarrelAnimAVoxel) {
			strncpy(buffer, VoxelBarrelFile, sizeof(buffer));
		}

		_makepath(fullname, NULL, NULL, buffer, ".VXL");
		CCFileClass turvxl(fullname);
		if (turvxl.Is_Available()) {
			delete AuxVoxel2.VoxLib;
			AuxVoxel2.VoxLib = new VoxelLibrary(turvxl);

			_makepath(fullname, NULL, NULL, buffer, ".HVA");
			CCFileClass turhva(fullname);
			delete AuxVoxel2.MotLib;
			AuxVoxel2.MotLib = new MotionLibrary(turhva);

			if (AuxVoxel2.MotLib != NULL && !AuxVoxel2.MotLib->Load_Failed()) {
				AuxVoxel2.MotLib->Scale(AuxVoxel2.VoxLib->Get_Layer_Info(0, 0).Scale);
			}
		}
	}
}

// FUNCTION: 0x00464A70
Coord const BuildingTypeClass::Coord_Fixup(Coord const & coord) const
{
	Coord crd = coord;
	crd.Z = Map.Get_Height_GL(crd);
	return(crd);
}

// FUNCTION: 0x00464AC0
bool BuildingTypeClass::Legal_Placement(Cell const & pos, HouseClass * house) const
{
	if (IsCanPlaceAnywhere) {
		return(true);
	}
	return(TechnoTypeClass::Legal_Placement(pos, house));
}

// FUNCTION: 0x00464AF0
Point3D BuildingTypeClass::Lepton_Dimensions(void) const
{
	return(Point3D(SizeWidth[Size] << 8, SizeHeight[Size] << 8, ZHeight * LEVEL_LEPTON_H));
}

// FUNCTION: 0x00464B30
void BuildingTypeClass::Compute_CRC(CRCEngine & crc) const
{
	TechnoTypeClass::Compute_CRC(crc);
	crc(GateCloseDelay);
	crc(LightVisibility);
	crc(LightIntensity);
	crc(LightRedTint);
	crc(LightGreenTint);
	crc(LightBlueTint);
	crc((char const *)BuildupFilename);
	crc((char const *)PowersUpBuilding);
	crc(Adjacent);
	crc(ToBuild);
	crc(Power);
	crc(Drain);
	crc(ExtraPower);
	crc(ExtraDrain);
	crc(Size);
	crc(ZHeight);
	crc(OccupyHeight);
	crc(MidPoint);
	crc(DoorStages);
	crc(&Anims, 72);
	crc(&AnimData, 1428);
	crc(Upgrades);
	crc(SpecialZOverlayZAdjust);
	crc(IsHoverPad);
	crc(IsPlug);
	crc(IsTemple);
	crc(HasSpotlight);
	crc(IsBase);
	crc(EligibleForAllyBuilding);
	crc(NeedsEngineer);
	crc(IsBibbed);
	crc(IsWall);
	crc(IsCaptureable);
	crc(IsPowered);
	crc(IsUnsellable);
	crc(IsRadar);
	crc(SpySat);
	crc(IsHasChargeAnim);
	crc(IsAnimDelayedFire);
	crc(DelayedFireDelay);
	crc(IsSiloDamage);
	crc(IsCanUnitRepair);
	crc(IsCanUnitReload);
	crc(Cloning);
	crc(Grinding);
	crc(UnitAbsorb);
	crc(InfantryAbsorb);
	crc(SecretLab);
	crc(IsFlat);
	crc(IsDockUnload);
	crc(IsRecoilless);
	crc(IsBridgeRepairHut);
	crc(IsGate);
	crc(IsSAM);
	crc(IsConstructionYard);
	crc(IsNukeSilo);
	crc(IsRefinery);
	crc(IsWeaponsFactory);
	crc(IsLaserFencePost);
	crc(IsLaserFence);
	crc(IsFirestormWall);
	crc(IsHelipad);
	crc(OrePurifier);
	crc(IsExtraDamageStage);
	crc(IsHospital);
	crc(IsArmory);
	crc(GateStages);
	crc(PowersUpToLevel);
	crc(IsDamagedDoor);
	crc(IsInvisibleInGame);
	crc(IsTerrainPalette);
	crc(IsTurretAnimAVoxel);
	crc(IsBarrelAnimAVoxel);
	crc((char const *)VoxelBarrelFile);
	crc(VoxelBarrelScale);
	crc(&VoxelBarrelOffsetToPitchPivotPoint, 12);
	crc(&VoxelBarrelOffsetToRotatePivotPoint, 12);
	crc(&VoxelBarrelOffsetToBuildingPivotPoint, 12);
	crc(&VoxelBarrelOffsetToBarrelEnd, 12);
	crc(CrateBeneath);
	crc(LeaveRubble);
	crc(CrateBeneathIsMoney);
	crc(NumberOfDocks);
	crc(StartPitch);
	crc(ConcentricRadialIndicator);
}

// FUNCTION: 0x00465010
HRESULT STDMETHODCALLTYPE BuildingTypeClass::Load(IStream * stream)
{
	if (IsFreeBuildup && BuildupData != NULL) {
		delete (ShapeSet *)BuildupData;
		BuildupData = NULL;
		BuildupLoaded = false;
	}
	if (BibShapeLoaded && BibShape != NULL) {
		delete (ShapeSet *)BibShape;
		BibShape = NULL;
		BibShapeLoaded = false;
	}
	if (DeployingAnimLoaded && DeployingAnim != NULL) {
		delete (ShapeSet *)DeployingAnim;
		DeployingAnim = NULL;
		DeployingAnimLoaded = false;
	}
	if (UnderDoorAnimLoaded && UnderDoorAnim != NULL) {
		delete (ShapeSet *)UnderDoorAnim;
		UnderDoorAnim = NULL;
		UnderDoorAnimLoaded = false;
	}
	if (RubbleLoaded && Rubble != NULL) {
		delete (ShapeSet *)Rubble;
		Rubble = NULL;
		RubbleLoaded = false;
	}
	if (RoofDeployingAnimLoaded && RoofDeployingAnim != NULL) {
		delete (ShapeSet *)RoofDeployingAnim;
		RoofDeployingAnim = NULL;
		RoofDeployingAnimLoaded = false;
	}
	if (UnderRoofDoorAnimLoaded && UnderRoofDoorAnim != NULL) {
		delete (ShapeSet *)UnderRoofDoorAnim;
		UnderRoofDoorAnim = NULL;
		UnderRoofDoorAnimLoaded = false;
	}

	HRESULT result = TechnoTypeClass::Load(stream);
	if (FAILED(result)) {
		return(result);
	}

	{
		new (this) BuildingTypeClass(NoInitClass());

		int count;
		result = stream->Read(&count, sizeof(count), NULL);
		if (FAILED(result)) {
			return(result);
		}

		new (&DockingOffsets) VectorClass<Point3D>();
		DockingOffsets.Resize(count, NULL);
		for (int index = 0; index < count; index++) {
			Point3D offset(0, 0, 0);
			result = stream->Read(&offset, sizeof(offset), NULL);
			if (FAILED(result)) {
				return(result);
			}
			DockingOffsets[index] = offset;
		}
	}

	Fetch_Building_Voxel_Image();
	Fetch_Normal_Image();

	ToTile = NULL;
	OccupyList = OccupyLists[Size];
	ExitList = ExitLists[Size];

	Swizzle_Pointer(&ToOverlay);
	Swizzle_Pointer(&FreeUnit);
	Swizzle_Pointer(&SecretInfantry);
	Swizzle_Pointer(&SecretUnit);
	Swizzle_Pointer(&SecretBuilding);

	Fetch_Building_Voxel_Image();

	BuildupData = NULL;
	BuildupLoaded = false;
	BibShape = NULL;
	BibShapeLoaded = false;
	DeployingAnim = NULL;
	DeployingAnimLoaded = false;
	UnderDoorAnim = NULL;
	UnderDoorAnimLoaded = false;
	Rubble = NULL;
	RubbleLoaded = false;
	RoofDeployingAnim = NULL;
	RoofDeployingAnimLoaded = false;
	UnderRoofDoorAnim = NULL;
	UnderRoofDoorAnimLoaded = false;

	Fetch_Building_Normal_Image(Scen->Theater);
	return(S_OK);
}

// FUNCTION: 0x00465300
HRESULT STDMETHODCALLTYPE BuildingTypeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = TechnoTypeClass::Save(stream, cleardirty);
	if (FAILED(result)) {
		return(result);
	}

	int count = DockingOffsets.Length();
	result = stream->Write(&count, sizeof(count), NULL);
	if (FAILED(result)) {
		return(result);
	}

	for (int index = 0; index < count; index++) {
		result = stream->Write(&DockingOffsets[index], sizeof(Point3D), NULL);
		if (FAILED(result)) {
			return(result);
		}
	}
	return(S_OK);
}

// FUNCTION: 0x00465380
HRESULT STDMETHODCALLTYPE BuildingTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_BuildingTypeClass;
	return(S_OK);
}

// FUNCTION: 0x004653C0
BuildingTypeClass * BuildingTypeClass::Find_Or_Make(char const * name)
{
	return(TFind_Or_Make<BuildingTypeClass>(name, BuildingTypes));
}

// FUNCTION: 0x00465450
void BuildingTypeClass::Post_Load_Game(void)
{
	int maxcloak = 0;

	for (int sindex = 0; sindex < BuildingTypes.Count(); sindex++) {
		BuildingTypeClass * classptr = BuildingTypes[sindex];
		maxcloak = MAX(maxcloak, classptr->CloakRadiusInCells);
	}

	if (maxcloak != 0) {
		delete CloakingSurface;
		CloakingSurface = new BSurface(maxcloak + 16, maxcloak + 16, 1);
	}

	char buffer[60];
	for (int i = 0; i < BuildingTypes.Count(); i++) {
		BuildingTypeClass * building = BuildingTypes[i];
		buffer[0] = '\0';
		RuleINI->Get_String(building->Name(), "ToTile", NULL, buffer, sizeof(buffer));
		IsometricTileType ittype = IsometricTileTypeClass::From_Name(buffer);
		if (ittype >= ISOTILE_FIRST && ittype < IsometricTileTypes.Count()) {
			building->ToTile = IsometricTileTypes[ittype];
		}
	}
}

// FUNCTION: 0x00465550
Cell const * BuildingTypeClass::Exit_List(void) const
{
	BSizeType size = Size;
	if (size == BSIZE_33_REF) {
		size = BSIZE_33;
	}
	return(ExitLists[size]);
}

// FUNCTION: 0x00465910
bool BuildingTypeClass::CanUseWaypoint(void) const
{
	return(false);
}

// FUNCTION: 0x00465920
bool BuildingTypeClass::CanAttackMove(void) const
{
	return(false);
}

// FUNCTION: 0x00465930
int BuildingTypeClass::Get_Max_Drain(DynamicVectorClass<BuildingTypeClass *> const & list)
{
	int maxdrain = 0;
	for (int i = 0; i < list.Count(); i++) {
		if (list[i]->Drain > maxdrain) {
			maxdrain = list[i]->Drain;
		}
	}
	return(maxdrain);
}

void FUN_005F96B0(char * name, TheaterType theater);

// FUNCTION: 0x00465960
void const * BuildingTypeClass::Get_Buildup_Data(void) const
{
	char fullname[MAX_PATH];

	if (BuildupData == NULL && IsDemandLoadBuildup) {
		if (strlen(BuildupFilename) != 0) {
			_makepath(fullname, NULL, NULL, BuildupFilename, ".SHP");
			FUN_005F96B0(fullname, Scen->Theater);
			CCFileClass file(fullname);
			ShapeSet * data = (ShapeSet *)Load_Alloc_Data(file);
			if (data == NULL) {
				FUN_005F9710(fullname);
				CCFileClass file2(fullname);
				data = (ShapeSet *)Load_Alloc_Data(file2);
			}
			((void const *&)BuildupData) = data;
			if (BuildupData != NULL) {
				((bool &)BuildupLoaded) = true;
				int timedelay = 1;
				int count = ((ShapeSet const *)BuildupData)->Get_Count()/2;
				if (IsGate) {
					count = GateStages + 1;
				}
				if (count > 0) {
					timedelay = (Rule->BuildupTime * TICKS_PER_MINUTE) / count;
				}
				Init_Anim(BSTATE_CONSTRUCTION, 0, count, timedelay);
			}
		}
		return(BuildupData);
	}
	return(BuildupData);
}

// FUNCTION: 0x00465AF0
void BuildingTypeClass::Free_Buildup_Data(void)
{
	if (IsFreeBuildup && BuildupData != NULL) {
		delete (ShapeSet *)BuildupData;
		BuildupData = NULL;
		BuildupLoaded = false;
	}
}

// FUNCTION: 0x00465B30
void BuildingTypeClass::Free_Bib_Shape(void)
{
	if (BibShapeLoaded && BibShape != NULL) {
		delete (ShapeSet *)BibShape;
		BibShape = NULL;
		BibShapeLoaded = false;
	}
}

// FUNCTION: 0x00465B70
void BuildingTypeClass::Free_Deploying_Anim(void)
{
	if (DeployingAnimLoaded && DeployingAnim != NULL) {
		delete (ShapeSet *)DeployingAnim;
		DeployingAnim = NULL;
		DeployingAnimLoaded = false;
	}
}

// FUNCTION: 0x00465BB0
void BuildingTypeClass::Free_Under_Door_Anim(void)
{
	if (UnderDoorAnimLoaded && UnderDoorAnim != NULL) {
		delete (ShapeSet *)UnderDoorAnim;
		UnderDoorAnim = NULL;
		UnderDoorAnimLoaded = false;
	}
}

// FUNCTION: 0x00465BF0
void BuildingTypeClass::Free_Rubble(void)
{
	if (RubbleLoaded && Rubble != NULL) {
		delete (ShapeSet *)Rubble;
		Rubble = NULL;
		RubbleLoaded = false;
	}
}

// FUNCTION: 0x00465C30
void BuildingTypeClass::Free_Roof_Deploying_Anim(void)
{
	if (RoofDeployingAnimLoaded && RoofDeployingAnim != NULL) {
		delete (ShapeSet *)RoofDeployingAnim;
		RoofDeployingAnim = NULL;
		RoofDeployingAnimLoaded = false;
	}
}

// FUNCTION: 0x00465C70
void BuildingTypeClass::Free_Under_Roof_Door_Anim(void)
{
	if (UnderRoofDoorAnimLoaded && UnderRoofDoorAnim != NULL) {
		delete (ShapeSet *)UnderRoofDoorAnim;
		UnderRoofDoorAnim = NULL;
		UnderRoofDoorAnimLoaded = false;
	}
}

// FUNCTION: 0x00465CB0
void BuildingTypeClass::Calculate_Base_Defense_Values(void)
{
}

// FUNCTION: 0x00465CC0
void BuildingTypeClass::Post_Read_Tile_Fixup(void)
{
	char buffer[60];

	for (int i = 0; i < BuildingTypes.Count(); i++) {
		BuildingTypeClass * building = BuildingTypes[i];
		buffer[0] = '\0';
		RuleINI->Get_String(building->Name(), "ToTile", NULL, buffer, sizeof(buffer));
		IsometricTileType ittype = IsometricTileTypeClass::From_Name(buffer);
		if (ittype >= ISOTILE_FIRST && ittype < IsometricTileTypes.Count()) {
			building->ToTile = IsometricTileTypes[ittype];
		}
	}
}

// FUNCTION: 0x00465D30
bool BuildingTypeClass::FUN_00465D30(void) const
{
	return(false);
}

// FUNCTION: 0x00465D40
bool BuildingTypeClass::IsVehicle(void) const
{
	return(UndeploysInto != NULL && SizeWidth[Size] == 1 && SizeHeight[Size] == 1);
}

// FUNCTION: 0x00465D70
Dir256 BuildingTypeClass::Deploy_Facing(void) const
{
	return(Dir256(DeployFacing));
}

// FUNCTION: 0x00465D80
BuildingTypeClass * BuildingTypeClass::FUN_00465D80(void)
{
	return(this);
}

// FUNCTION: 0x00465D90
RTTIType BuildingTypeClass::Fetch_RTTI(void) const
{
	return(RTTI_BUILDINGTYPE);
}

// FUNCTION: 0x00465DA0
int BuildingTypeClass::Fetch_Object_Size(void) const
{
	return(sizeof(*this));
}

// FUNCTION: 0x00465DB0
int BuildingTypeClass::Fetch_Heap_ID(void) const
{
	return(HeapID);
}

// FUNCTION: 0x00465DC0 BuildingTypeClass::~BuildingTypeClass

// FUNCTION: 0x00465DE0 ?$VectorClass@V?$TPoint3D@H@@::GetItem
// FUNCTION: 0x00465E10 ?$VectorClass@V?$TPoint3D@H@@::operator==
// FUNCTION: 0x00465E70 ?$VectorClass@V?$TPoint3D@H@@::Resize
// FUNCTION: 0x00465F50 ?$VectorClass@V?$TPoint3D@H@@::Clear
// FUNCTION: 0x00465F80 ?$VectorClass@V?$TPoint3D@H@@::ID
// FUNCTION: 0x00465FD0 ?$VectorClass@V?$TPoint3D@H@@::ID
template class VectorClass<Point3D>;

// FUNCTION: 0x00466000 TFind_Or_Make
template BuildingTypeClass * TFind_Or_Make<BuildingTypeClass>(char const *, DynamicVectorClass<BuildingTypeClass *> const &);

// FUNCTION: 0x00466080 ?$VectorClass@V?$TPoint3D@H@@::~?$VectorClass@V?$TPoint3D@H@@

#include <math.h>

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007E4660;
extern const double DAT_007E4668;

// FUNCTION: 0x004660D0 $E252
double DAT_0089DDF8 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x00466100 $E255
double DAT_0089DE68 = DAT_0089DDF8 / DAT_007E4660;

// FUNCTION: 0x00466120 $E258
double DAT_0089DE50 = DAT_007E1718 / DAT_0089DE68;

// FUNCTION: 0x00466140 $E261
double DAT_0089DE48 = DAT_007E1720 * DAT_007E4668;

// FUNCTION: 0x00466160 $E264
double DAT_0089DE00 = DAT_007E1728 * DAT_007E4668;

// FUNCTION: 0x00466180 $E267
double DAT_0089DDD8 = DAT_007E1730 * DAT_007E4668;

// FUNCTION: 0x004661A0 $E270
int DAT_0089DE70 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_0089DDD8 - DAT_0089DE00) * DAT_0089DDF8));

// FUNCTION: 0x004661E0 $E273
double DAT_0089DE08 = FUN_004CADE0(DAT_0089DE70 * DAT_007E1740);

// FUNCTION: 0x00466210 $E276
double DAT_0089DE40 = FUN_004CADE0((DAT_0089DE70 * 2) / DAT_0089DDF8);

// FUNCTION: 0x00466240 $E279
int DAT_0089DE64 = (int)(DAT_0089DE70 * 4 + DAT_007E1738);

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

struct IntTriple2 {
	int A, B, C;
	IntTriple2() { C = 0; A = 128; B = 128; }
};

struct IntQuad {
	int A, B, C, D;
	IntQuad() { A = 0; B = 0; C = 0; D = 0; }
};

// FUNCTION: 0x00466270 $E282
ShortPair DAT_0089DDF0;

// FUNCTION: 0x00466280 $E285
IntTriple DAT_0089DE30;

// FUNCTION: 0x004662A0 $E288
IntTriple2 DAT_0089DE58;

// FUNCTION: 0x004662C0 $E291
IntQuad DAT_0089DDE0;
