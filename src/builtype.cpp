// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
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

// FUNCTION: 0x0045B1B0 $E
// FUNCTION: 0x0045B1C0 $E
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

// FUNCTION: 0x0045C2F0 $E
// FUNCTION: 0x0045C300 $E
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

class SHPStruct
{
	public:
		short Zero;
		short Width;
		short Height;
		short Count;
		Rect GetFrameBounds(int frame) const;
		void * GetPixels(int frame) const;
};

// FUNCTION: 0x0045E8F0
void BuildingTypeClass::Fetch_Z_Data(void)
{
	if (BuildingZShape == NULL) {
		BuildingZShape = FileSystem::LoadFile("BUILDNGZ.SHA", false);
		((SHPStruct const *)BuildingZShape)->GetFrameBounds(0);
		char * data = (char *)((SHPStruct const *)BuildingZShape)->GetPixels(0);
		int width = ((SHPStruct const *)BuildingZShape)->Width;
		int height = ((SHPStruct const *)BuildingZShape)->Height;

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
				int count = ((SHPStruct const *)classptr->BuildupData)->Count;
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

// FUNCTION: 0x00464A70
Coord const BuildingTypeClass::Coord_Fixup(Coord const & coord) const
{
	Coord crd = coord;
	crd.Z = Map.Get_Height_GL(crd);
	return(crd);
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

// FUNCTION: 0x00465380
HRESULT STDMETHODCALLTYPE BuildingTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_BuildingTypeClass;
	return(S_OK);
}
