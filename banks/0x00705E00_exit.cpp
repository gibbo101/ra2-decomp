// best 90.0% raw, 93.7% normalised (2112/2100 bytes), no overlay; needs #include "msanim.h" in techno.cpp; splice before 0x00706640. Body unchanged from 0x00705E00_techno.cpp. Left: in the NORMAL and INDISTINCT cases the exe keeps rect in ebp across both draws; ours keeps shapefile. Measured: the choice is local to each case (uses in other cases or in the dirty-rectangle block do not move it); rect takes ebp when it has as many uses in the case as shapefile, in either draw. Aliases, a shadow-frame or shadow-draw helper, same-type casts, const parameters, explicit default arguments, dead references to rect and declaration moves all compile to the same 2112 bytes.
// FUNCTION: GAMEMD 0x00705E00
void TechnoClass::Techno_Draw_Object(ShapeSet const * shapefile, int shapenum, Point2D const & xdrawpoint, Rect const & rect, Dir256 rotation, int scale, int xzadjust, ZGradientType zgrad, bool zwrite, int brightness, unsigned short tint, ShapeSet const * zshapefile, int zshapenum, Point2D zoff, ShapeFlags_Type negflags) const
{
	if (shapefile == NULL) {
		return;
	}

	bool shadow = DrawShapeShadows;
	VisualType visual = Visual_Character();

	ShapeFlags_Type flags = SHAPE_NORMAL;
	if (negflags == SHAPE_NORMAL) {
		bool visible = false;
		bool shadowy = true;
		switch (visual) {
			case VISUAL_DARKEN:
			case VISUAL_SHADOWY:
				visible = true;
				shadowy = false;
				break;

			case VISUAL_HIDDEN:
				return;

			case VISUAL_RIPPLE:
				visible = true;
				if (CloakingDevice.Fetch_Stage()) {
					shadowy = false;
				}
				break;

			case VISUAL_INDISTINCT:
				visible = true;
				break;
		}
		if (visible) {
			if (shadowy) {
				flags = ShapeFlags_Type(flags|SHAPE_TRANSLUCENT25);
			} else {
				flags = ShapeFlags_Type(flags|SHAPE_TRANSLUCENT50);
			}
		}
	}

	if (IsBeingWarpedOut() || IsWarpingIn()) {
		if (this != NULL && RTTI == RTTI_BUILDING && As_Building((TechnoClass *)this)->Class->DoubleThick) {
			flags = ShapeFlags_Type(flags|SHAPE_TRANSLUCENT75);
		} else {
			flags = ShapeFlags_Type(flags|SHAPE_TRANSLUCENT50);
		}
	}

	ConvertClass * converter;
	if (RTTI == RTTI_BUILDING && ((BuildingClass const *)this)->Class->IsTerrainPalette) {
		CellClass * cptr = &Map[Center_Coord().As_Cell()];
		converter = cptr->Drawer;
		if (converter == NULL) {
			cptr->Init_Drawer();
			converter = cptr->Drawer;
			if (converter == NULL) return;
		}
		brightness = cptr->TileBrightness;
	} else if (RTTI == RTTI_UNIT && ((UnitClass const *)this)->TerrainPalette) {
		converter = EightBitDrawer;
	} else {
		converter = (ConvertClass *)GetRemapColour();
	}

	Point2D drawpoint = xdrawpoint;
	int zadjust = xzadjust;

	switch (Fetch_RTTI()) {
		case RTTI_AIRCRAFT:
			drawpoint.Y -= TacticalMap->Z_Lepton_To_Pixel(Get_Height());
			zadjust -= TacticalMap->Z_Lepton_To_Pixel(Get_Height());
			break;

		case RTTI_UNIT:
			if (((UnitClass const *)this)->Class->SmallVisceroid || ((UnitClass const *)this)->Class->LargeVisceroid) {
				shadow = false;
			}
		case RTTI_INFANTRY:
			if (HeightAGL == 0) {
				zadjust += Get_Z_Adjust();
			} else {
				zadjust -= TacticalMap->Z_Lepton_To_Pixel(Get_Height());
				shadow = false;
			}
			break;
	}

	if (Techno_Type_Class()->NoShadow) {
		shadow = false;
	}

	if (!IsClearlyVisibleTo(PlayerPtr)) {
		if (GetDisguise(true)->RTTI == RTTI_OVERLAYTYPE || GetDisguise(true)->RTTI == RTTI_TERRAINTYPE) {
			CellClass * cptr = Get_Cell_Ptr();
			converter = (ConvertClass *)cptr->Drawer;
			if (converter == NULL) {
				cptr->Init_Drawer();
				converter = (ConvertClass *)cptr->Drawer;
			}
			brightness = cptr->TileBrightness;
		} else {
			converter = (ConvertClass *)ColorSchemes[GetDisguiseHouse(true)->Scheme]->Converter;
		}
		if (converter == NULL) {
			converter = (ConvertClass *)ColorSchemes[PlayerPtr->Scheme]->Converter;
		}
	}

	if (zgrad != ZGRAD_NONE) {
		flags = ShapeFlags_Type(flags|SHAPE_ZGRAD);
	}
	if (zwrite) {
		flags = ShapeFlags_Type(flags|SHAPE_ZWRITE);
	}
	flags = ShapeFlags_Type(flags|SHAPE_ALPHA);

	if (RTTI == RTTI_UNIT && ((UnitClass const *)this)->TerrainPalette) {
		flags = ShapeFlags_Type(flags|SHAPE_NOTRANS);
		shadow = false;
	}

	flags = ShapeFlags_Type(flags & ~negflags);

	if (RTTI == RTTI_UNIT && ((UnitClass const *)this)->TerrainPalette) {
		Rect shaperect = ((SHPStruct const *)shapefile)->GetFrameBounds(shapenum);
		Rect drawrect;
		drawrect.X = drawpoint.X + shaperect.X - shapefile->Get_Width() / 2;
		drawrect.Y = drawpoint.Y + shaperect.Y - shapefile->Get_Height() / 2;
		drawrect.Width = shaperect.Width;
		drawrect.Height = shaperect.Height;
		UnitCompositeDirtyRect = Union(UnitCompositeDirtyRect, drawrect);
	}

	if (House->Is_Player_Control() && IsDisguised()) {
		flags = (ShapeFlags_Type)GetDisguiseFlags(flags);
	}

	brightness = Apparent_Brightness(brightness);

	if (IsIronCurtained() || (this != NULL && RTTI == RTTI_BUILDING && Airstrike != NULL && Airstrike->Target == As_Building((TechnoClass *)this))) {
		brightness = GetAirstrikeTintIntensity(GetInvulnerabilityTintIntensity(brightness));
	}

	if (Map[Center_Coord().As_Cell()].Is_Shrouded()) {
		tint = 0;
	}

	switch (visual) {
		case VISUAL_NORMAL:
			Draw_Shape(*LogicalSurface, *converter, shapefile, shapenum, drawpoint, rect, ShapeFlags_Type(flags|SHAPE_CENTER|SHAPE_WIN_REL), NULL, zadjust - 2, zgrad, brightness, tint, zshapefile, zshapenum, zoff);
			if (shadow) {
				zadjust = -2 - TacticalMap->Z_Lepton_To_Pixel(Get_Height());
				if (IsInTransport) {
					drawpoint.Y -= 14;
				}
				Draw_Shape(*LogicalSurface, *converter, shapefile, shapenum + shapefile->Get_Count() / 2, drawpoint, rect, ShapeFlags_Type((flags & ~SHAPE_TRANSLUCENT75)|SHAPE_DARKEN|SHAPE_CENTER|SHAPE_WIN_REL), NULL, zadjust - 2);
			}
			break;

		case VISUAL_INDISTINCT:
			Draw_Shape(*LogicalSurface, *converter, shapefile, shapenum, drawpoint, rect, ShapeFlags_Type(flags|SHAPE_CENTER|SHAPE_WIN_REL), NULL, zadjust - 2, zgrad, brightness, tint, zshapefile, zshapenum, zoff);
			if (shadow) {
				zadjust = -2 - TacticalMap->Z_Lepton_To_Pixel(Get_Height());
				flags = ShapeFlags_Type(flags & ~SHAPE_TRANSLUCENT75);
				if (IsInTransport) {
					drawpoint.Y -= 14;
				}
				Draw_Shape(*LogicalSurface, *converter, shapefile, shapenum + shapefile->Get_Count() / 2, drawpoint, rect, ShapeFlags_Type(flags|SHAPE_DARKEN|SHAPE_CENTER|SHAPE_WIN_REL), NULL, zadjust - 2);
			}
			break;

		case VISUAL_DARKEN:
		case VISUAL_RIPPLE:
			Draw_Shape(*LogicalSurface, *converter, shapefile, shapenum, drawpoint, rect, ShapeFlags_Type(flags|SHAPE_CENTER|SHAPE_WIN_REL), NULL, zadjust - 2, zgrad, brightness, tint, zshapefile, zshapenum, zoff);
			break;

		case VISUAL_SHADOWY:
			Draw_Shape(*LogicalSurface, *converter, shapefile, shapenum, drawpoint, rect, ShapeFlags_Type(flags|SHAPE_CENTER|SHAPE_WIN_REL), NULL, zadjust - 2, zgrad, brightness, tint, zshapefile, zshapenum, zoff);
			break;
	}
}
