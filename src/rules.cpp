// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "rules.h"
#include "_bench.h"
#include "_palette.h"
#include "_rules.h"
#include "_warhead.h"
#include "_weapon.h"
#include "addon.h"
#include "airctype.h"
#include "anim.h"
#include "animtype.h"
#include "bench.h"
#include "builtype.h"
#include "bullettype.h"
#include "ccfile.h"
#include "conquer.h"
#include "convert.h"
#include "dbgprint.h"
#include "findmake.h"
#include "geometry.h"
#include "globals.h"
#include "house.h"
#include "houstype.h"
#include "infatype.h"
#include "levitate.h"
#include "mission.h"
#include "movie.h"
#include "multiplayer.h"
#include "overtype.h"
#include "psystype.h"
#include "rect.h"
#include "ptype.h"
#include "scheme.h"
#include "session.h"
#include "script.h"
#include "side.h"
#include "smudtype.h"
#include "stimer.h"
#include "sun.h"
#include "suprtype.h"
#include "swizzle.h"
#include "taskforc.h"
#include "teamtype.h"
#include "terrtype.h"
#include "tiberium.h"
#include "trim.h"
#include "unittype.h"
#include "vanimtype.h"
#include "vector.h"
#include "warhead.h"
#include "weapon.h"
#include "bench.hh"
#include <cstring>

namespace Powerups {
extern char const * Effects[19];
extern int Weights[19];
extern AnimType Anims[19];
extern double Arguments[19];
extern bool Naval[19];
}
void __fastcall FUN_006CFD20(int button, int slot);
int __fastcall FUN_006CFCC0(char const * name);
void __fastcall FUN_006CFDB0(int count);
extern int DAT_008427CC;
template<> void VectorClass<ColorScheme *>::Clear(void);
void FUN_006269A0(void);
void __fastcall FUN_00626AB0(char const * name, HSVClass & hsv);
void Difficulty_Get(CCINIClass const & ini, DifficultyClass & diff, char const * section);

// SYNTHETIC: GAMEMD 0x00664B10 SYMBOL
// _$E35

// SYNTHETIC: GAMEMD 0x00664B40 SYMBOL
// _$E38

// SYNTHETIC: GAMEMD 0x00664B60 SYMBOL
// _$E41

// SYNTHETIC: GAMEMD 0x00664B80 SYMBOL
// _$E44

// SYNTHETIC: GAMEMD 0x00664BA0 SYMBOL
// _$E47

// SYNTHETIC: GAMEMD 0x00664BC0 SYMBOL
// _$E50

// SYNTHETIC: GAMEMD 0x00664BE0 SYMBOL
// _$E53

// SYNTHETIC: GAMEMD 0x00664C20 SYMBOL
// _$E56

// SYNTHETIC: GAMEMD 0x00664C50 SYMBOL
// _$E59

// SYNTHETIC: GAMEMD 0x00664C80 SYMBOL
// _$E62

// SYNTHETIC: GAMEMD 0x00664CB0 SYMBOL
// _$E65

// SYNTHETIC: GAMEMD 0x00664CC0 SYMBOL
// _$E68

// SYNTHETIC: GAMEMD 0x00664CE0 SYMBOL
// _$E71

// SYNTHETIC: GAMEMD 0x00664D00 SYMBOL
// _$E10

// FUNCTION: GAMEMD 0x00664D20
RulesClass::RulesClass(NoInitClass const & x) :
	RadColor(x),
	LineTrailColorOverride(x),
	ChronoBeamColor(x),
	MagnaBeamColor(x)
{
	x();
}

// FUNCTION: GAMEMD 0x00665650
RulesClass::RulesClass(void) :
	DetailMinFrameRateNormal(15),
	DetailMinFrameRateMovie(20),
	DetailBufferZoneWidth(5),
	AmmoCrateDamage(100),
	LargeVisceroid(NULL),
	SmallVisceroid(NULL),
	AttackingAircraftSightRange(5),
	TunnelSpeed(1.0),
	TiberiumHeal(1.0/60.0),
	SelfHealInfantryFrames(1000),
	SelfHealInfantryAmount(1),
	SelfHealUnitFrames(1000),
	SelfHealUnitAmount(1),
	IsFreeMCV(false),
	IsBerzerkAllowed(false),
	PoseDir(DIR_N),
	DeployDir(0),
	DropPodPuff(NULL),
	WaypointAnimationSpeed(12),
	BarrelExplode(NULL),
	BarrelDebris(),
	BarrelParticle(NULL),
	RadarEventColorSpeed(0.05f),
	RadarEventMinRadius(5),
	RadarEventSpeed(1.0f),
	RadarEventRotationSpeed(0.1f),
	FlashFrameTime(7),
	RadarCombatFlashTime(21),
	MaxWaypointPathLength(15),
	Wake(NULL),
	NukeTakeOff(NULL),
	InfantryExplode(NULL),
	FlamingInfantry(NULL),
	InfantryHeadPop(NULL),
	InfantryNuked(NULL),
	InfantryVirus(NULL),
	InfantryBrute(NULL),
	InfantryMutate(NULL),
	Behind(NULL),
	AITriggerSuccessWeightDelta(1.0),
	AITriggerFailureWeightDelta(-1.0),
	AITriggerTrackRecordCoefficient(1.0),
	VeinholeMonsterStrength(10000),
	MaxVeinholeGrowth(1000),
	VeinholeGrowthRate(100),
	VeinholeShrinkRate(100),
	VeinAttack(NULL),
	VeinDamage(2),
	MaximumQueuedObjects(5),
	AircraftFogReveal(6),
	WoodCrateImg(NULL),
	CrateImg(NULL),
	WaterCrateImg(NULL),
	DropPod(),
	DeadBodies(),
	MetallicDebris(),
	BridgeExplosions(),
	DigSound(VOC_NONE),
	CreateUnitSound(-1),
	CreateInfantrySound(-1),
	CreateAircraftSound(-1),
	BaseUnderAttackSound(-1),
	GUIMainButtonSound(-1),
	GUIBuildSound(-1),
	GUITabSound(-1),
	GUIOpenSound(-1),
	GUICloseSound(-1),
	GUIMoveOutSound(-1),
	GUIMoveInSound(-1),
	GUIComboOpenSound(-1),
	GUIComboCloseSound(-1),
	GUICheckboxSound(-1),
	ScoreAnimSound(-1),
	IFVTransformSound(-1),
	PsychicSensorDetectSound(-1),
	BuildingGarrisonedSound(-1),
	BuildingAbandonedSound(-1),
	BuildingRepairedSound(-1),
	CheerSound(-1),
	PlaceBeaconSound(-1),
	DefaultChronoSound(-1),
	StartPlanningModeSound(-1),
	AddPlanningModeCommandSound(-1),
	ExecutePlanSound(-1),
	EndPlanningModeSound(-1),
	CrateMoneySound(-1),
	CrateRevealSound(-1),
	CrateFireSound(-1),
	CrateArmourSound(-1),
	CrateSpeedSound(-1),
	CrateUnitSound(-1),
	CratePromoteSound(-1),
	ImpactWaterSound(-1),
	ImpactLandSound(-1),
	SinkingSound(-1),
	BombTickingSound(-1),
	BombAttachSound(-1),
	ChronoInSound(-1),
	ChronoOutSound(-1),
	SpySatActivationSound(-1),
	SpySatDeactivationSound(-1),
	UpgradeVeteranSound(-1),
	UpgradeEliteSound(-1),
	VoiceIFVRepair(-1),
	SlavesFreeSound(-1),
	SlaveMinerDeploySound(-1),
	SlaveMinerUndeploySound(-1),
	BunkerWallsUpSound(-1),
	BunkerWallsDownSound(-1),
	RepairBridgeSound(-1),
	PsychicDominatorActivateSound(-1),
	GeneticMutatorActivateSound(-1),
	PsychicRevealActivateSound(-1),
	MasterMindOverloadDeathSound(-1),
	AirstrikeAbortSound(-1),
	AirstrikeAttackVoice(-1),
	MindClearedSound(-1),
	EnterGrinderSound(-1),
	LeaveGrinderSound(-1),
	EnterBioReactorSound(-1),
	LeaveBioReactorSound(-1),
	ActivateSound(-1),
	DeactivateSound(-1),
	SpyPlaneCamera(-1),
	LetsDoTheTimeWarpOutAgain(-1),
	LetsDoTheTimeWarpInAgain(-1),
	DiskLaserChargeUp(-1),
	SpyPlaneCameraFrames(16),
	Dig(NULL),
	IonBlast(NULL),
	IonBeam(NULL),
	DamageFireTypes(),
	WeatherConClouds(),
	WeatherConBolts(),
	WeatherConBoltExplosion(NULL),
	DominatorWarhead(NULL),
	DominatorFirstAnim(NULL),
	DominatorSecondAnim(NULL),
	DominatorFireAtPercentage(50),
	DominatorCaptureRange(2),
	DominatorDamage(50),
	MindControlAttackLineFrames(15),
	DrainMoneyFrameDelay(100),
	DrainMoneyAmount(0),
	DrainAnimationType(NULL),
	ControlledAnimationType(NULL),
	PermaControlledAnimationType(NULL),
	ChronoBlast(NULL),
	ChronoBlastDest(NULL),
	ChronoPlacement(NULL),
	ChronoBeam(NULL),
	WarpIn(NULL),
	WarpOut(NULL),
	WarpAway(NULL),
	ChronoSparkle1(NULL),
	IronCurtainInvokeAnim(NULL),
	ForceShieldInvokeAnim(NULL),
	WeaponNullifyAnim(NULL),
	AtmosphereEntry(NULL),
	PrerequisitePower(),
	PrerequisiteFactory(),
	PrerequisiteBarracks(),
	PrerequisiteRadar(),
	PrerequisiteTech(),
	PrerequisiteProc(),
	PrerequisiteProcAlternate(NULL),
	GateUpSound(VOC_NONE),
	GateDownSound(VOC_NONE),
	JumpjetTurnRate(3),
	JumpjetSpeed(30),
	JumpjetClimb(5.0),
	JumpjetCruiseHeight(400),
	JumpjetAcceleration(0.25),
	JumpjetWobblesPerSecond(0.25),
	JumpjetWobbleDeviation(40),
	RadarEventSuppressionDistances(),
	RadarEventVisibilityDurations(),
	RadarEventDurations(),
	IonCannonDamage(700),
	RailgunDamageRadius(128),
	PrismType(NULL),
	PrismSupportModifier(100),
	PrismSupportMax(8),
	PrismSupportDelay(100),
	PrismSupportDuration(15),
	PrismSupportHeight(300),
	ParadropRadius(1024),
	ZoomInFactor(2.0),
	ConditionRedSparkingProbability(0.02),
	ConditionYellowSparkingProbability(0.01),
	TiberiumExplosionDamage(100),
	TiberiumStrength(10),
	MinLowPowerProductionSpeed(0.5f),
	MaxLowPowerProductionSpeed(0.9f),
	LowPowerPenaltyModifier(1.0f),
	MultipleFactory(1.0f),
	MaximumCheerRate(1),
	TreeFlammability(0.1),
	MissileSpeedVar(0.25),
	MissileROTVar(0.25),
	MissileSafetyAltitude(500),
	DropPodWeapon(NULL),
	DropPodHeight(1500),
	DropPodSpeed(40),
	DropPodAngle(M_PI/2),
	ScrollMultiplier(1.0),
	CrewEscape(0.5),
	ShakeScreen(400),
	HoverHeight(120),
	HoverBob(30.0),
	HoverBoost(1.3),
	HoverAcceleration(0.03),
	HoverBrake(0.03),
	HoverDampen(0.8),
	PlacementDelay(0.05),
	ExplosiveVoxelDebris(),
	TireVoxelDebris(NULL),
	ScrapVoxelDebris(NULL),
	BridgeVoxelMax(3),
	CloakingStages(9),
	RevealTriggerRadius(5),
	ShipSinkingWeight(3.0),
	IceCrackingWeight(2.0),
	IceBreakingWeight(4.0),
	IceCrackSounds(),
	CliffBackImpassability(0),
	VeteranRatio(10.0),
	VeteranCombat(1.0),
	VeteranSpeed(1.0),
	VeteranSight(1.0),
	VeteranArmor(1.0),
	VeteranROF(1.0),
	VeteranCap(1.0),
	CloakSound(VOC_NONE),
	SellSound(VOC_NONE),
	GameClosed(VOC_NONE),
	IncomingMessage(VOC_NONE),
	SystemError(VOC_NONE),
	OptionsChanged(VOC_NONE),
	GameForming(VOC_NONE),
	PlayerLeft(VOC_NONE),
	PlayerJoined(VOC_NONE),
	MessageCharTyped(-1),
	Construction(VOC_NONE),
	CreditTicks(),
	BuildingDieSound(-1),
	BuildingSlam(VOC_NONE),
	RadarOn(VOC_NONE),
	RadarOff(VOC_NONE),
	MovieOn(-1),
	MovieOff(-1),
	ScoldSound(VOC_NONE),
	TeslaCharge(VOC_NONE),
	TeslaZap(VOC_NONE),
	GenericClick(VOC_NONE),
	GenericBeep(VOC_NONE),
	BuildingDamageSound(-1),
	HealCrateSound(VOC_NONE),
	ChuteSound(VOC_NONE),
	StopSound(VOC_NONE),
	GuardSound(VOC_NONE),
	ScatterSound(VOC_NONE),
	DeploySound(VOC_NONE),
	StormSound(-1),
	LightningSounds(),
	ShellButtonSlideSound(-1),
	WallBuildSpeedCoefficient(0.5),
	ChargeToDrainRatio(3.0),
	TrackedUphill(1.0),
	TrackedDownhill(1.0),
	WheeledUphill(1.0),
	WheeledDownhill(1.0),
	SpotlightMovementRadius(2000),
	SpotlightLocationRadius(1000),
	SpotlightSpeed(0.05),
	SpotlightAcceleration(0.005),
	SpotlightAngle(20.0),
	SpotlightRadius(175),
	WindDirection(FACING_NONE),
	CameraRange(2304),
	FlightLevel(500),
	ParachuteMaxFallRate(-3),
	NoParachuteMaxFallRate(-100),
	BuildingDrop(VOC_NONE),
	Scorches(),
	Scorches1(),
	Scorches2(),
	Scorches3(),
	Scorches4(),
	RepairBay(),
	GDIGateOne(NULL),
	GDIGateTwo(NULL),
	NodGateOne(NULL),
	NodGateTwo(NULL),
	WallTower(NULL),
	Shipyard(),
	GDIPowerPlant(NULL),
	NodRegularPower(NULL),
	NodAdvancedPower(NULL),
	ThirdPowerPlant(NULL),
	BuildConst(),
	BuildPower(),
	BuildRefinery(),
	BuildBarracks(),
	BuildTech(),
	BuildWeapons(),
	AlliedBaseDefenses(),
	SovietBaseDefenses(),
	ThirdBaseDefenses(),
	AIForcePredictionFudge(),
	BuildDefense(),
	BuildPDefense(),
	BuildAA(),
	BuildHelipad(),
	BuildRadar(),
	ConcreteWalls(),
	NSGates(),
	EWGates(),
	BuildNavalYard(),
	BuildDummy(),
	NeutralTechBuildings(),
	GDIWallDefense(6.0),
	GDIWallDefenseCoefficient(3.0),
	NodBaseDefenseCoefficient(1.0),
	GDIBaseDefenseCoefficient(1.0),
	ComputerBaseDefenseResponse(3),
	MaximumBaseDefenseValue(60),
	BaseUnit(),
	HarvesterUnit(),
	PadAircraft(),
	OnFire(),
	TreeFire(),
	Smoke1(NULL),
	Smoke_(NULL),
	MoveFlash(NULL),
	BombParachute(NULL),
	Parachute(NULL),
	SplashList(),
	SmallFire(NULL),
	LargeFire(NULL),
	Paratrooper(NULL),
	EliteFlashTimer(100),
	ChronoDelay(60),
	ChronoReinfDelay(60),
	ChronoDistanceFactor(32),
	ChronoTrigger(true),
	ChronoMinimumDelay(0),
	ChronoRangeMinimum(0),
	AmerParaDropInf(),
	AmerParaDropNum(),
	AllyParaDropInf(),
	AllyParaDropNum(),
	SovParaDropInf(),
	SovParaDropNum(),
	YuriParaDropInf(),
	YuriParaDropNum(),
	AnimToInfantry(),
	SecretInfantry(),
	SecretUnits(),
	SecretBuildings(),
	SecretSum(0),
	AlliedDisguise(NULL),
	SovietDisguise(NULL),
	ThirdDisguise(NULL),
	SpyPowerBlackout(0),
	SpyMoneyStealPercent(0.5f),
	AttackCursorOnDisguise(false),
	AIMinorSuperReadyPercent(0.8f),
	AISafeDistance(8),
	HarvesterTooFarDistance(5),
	ChronoHarvTooFarDistance(50),
	AlliedBaseDefenseCounts(),
	SovietBaseDefenseCounts(),
	ThirdBaseDefenseCounts(),
	AIPickWallDefensePercent(),
	AIRestrictReplaceTime(500),
	ThreatPerOccupant(5),
	ApproachTargetResetMultiplier(1),
	CampaignMoneyDeltaEasy(5000),
	CampaignMoneyDeltaHard(-5000),
	GuardAreaTargetingDelay(36),
	NormalTargetingDelay(27),
	AINavalYardAdjacency(20),
	DisabledDisguiseDetectionPercent(),
	AIAutoDeployFrameDelay(),
	MaximumBuildingPlacementFailures(5),
	AICaptureNormal(),
	AICaptureWounded(),
	AICaptureLowPower(),
	AICaptureLowMoney(),
	AISuperDefenseProbability(),
	AISuperDefenseFrames(25),
	AISuperDefenseDistance(10),
	OverloadCount(),
	OverloadDamage(),
	OverloadFrames(),
	PurifierBonus(0.25f),
	OccupyDamageMultiplier(1.0f),
	OccupyROFMultiplier(1.0f),
	OccupyWeaponRange(5),
	BunkerDamageMultiplier(1.0f),
	BunkerROFMultiplier(1.0f),
	BunkerWeaponRangeBonus(2),
	OpenToppedDamageMultiplier(1.0f),
	OpenToppedRangeBonus(2),
	OpenToppedWarpDistance(5),
	FallingDamageMultiplier(1.0f),
	CurrentStrengthDamage(true),
	Technician(NULL),
	Engineer(NULL),
	Pilot(NULL),
	AlliedCrew(NULL),
	SovietCrew(NULL),
	ThirdCrew(NULL),
	FlameDamage(NULL),
	FlameDamage2(NULL),
	NukeWarhead(NULL),
	NukeProjectile(NULL),
	NukeDown(NULL),
	MutateWarhead(NULL),
	MutateExplosionWarhead(NULL),
	EMPulseWarhead(NULL),
	EMPulseProjectile(NULL),
	C4Warhead(NULL),
	CrushWarhead(NULL),
	V3Warhead(NULL),
	DMislWarhead(NULL),
	V3EliteWarhead(NULL),
	DMislEliteWarhead(NULL),
	CMislWarhead(NULL),
	CMislEliteWarhead(NULL),
	IvanWarhead(NULL),
	IvanDamage(100),
	IvanTimedDelay(450),
	CanDetonateTimeBomb(false),
	CanDetonateDeathBomb(false),
	IvanIconFlickerRate(8),
	DeathWeapon(NULL),
	BOMBCURS_SHP(NULL),
	CHRONOSK_SHP(NULL),
	IronCurtainDuration(0),
	PsychicRevealRadius(3),
	IonCannonWarhead(NULL),
	VeinholeTypeClass(NULL),
	DefaultMirageDisguises(),
	InfantryBlinkDisguiseTime(0),
	DefaultLargeGreySmokeSystem(NULL),
	DefaultSmallGreySmokeSystem(NULL),
	DefaultSparkSystem(NULL),
	DefaultLargeRedSmokeSystem(NULL),
	DefaultSmallRedSmokeSystem(NULL),
	DefaultDebrisSmokeSystem(NULL),
	DefaultFireStreamSystem(NULL),
	DefaultTestParticleSystem(NULL),
	DefaultRepairParticleSystem(NULL),
	MyEffectivenessCoefficientDefault(0.0),
	TargetEffectivenessCoefficientDefault(0.0),
	TargetSpecialThreatCoefficientDefault(0.0),
	TargetStrengthCoefficientDefault(0.0),
	TargetDistanceCoefficientDefault(0.0),
	DumbMyEffectivenessCoefficient(0.0),
	DumbTargetEffectivenessCoefficient(0.0),
	DumbTargetSpecialThreatCoefficient(0.0),
	DumbTargetStrengthCoefficient(0.0),
	DumbTargetDistanceCoefficient(0.0),
	EnemyHouseThreatBonus(0.0),
	TurboBoost(1.5),
	AttackInterval(3.0),
	AttackDelay(5.0),
	PowerEmergencyFraction(0.75),
	AirstripRatio(0.12),
	AirstripLimit(5),
	HelipadRatio(0.12),
	HelipadLimit(5),
	TeslaRatio(0.16),
	TeslaLimit(10),
	AARatio(0.14),
	AALimit(10),
	DefenseRatio(0.5),
	DefenseLimit(40),
	WarRatio(0.1),
	WarLimit(2),
	BarracksRatio(0.16),
	BarracksLimit(2),
	RefineryLimit(4),
	RefineryRatio(0.16),
	BaseSizeAdd(3),
	PowerSurplus(50),
	InfantryReserve(2000),
	InfantryBaseMult(2),
	SoloCrateMoney(2000),
	TreeStrength(25),
	UnitCrateType(NULL),
	PatrolTime(0.016),
	TeamDelays(),
	AIHateDelays(),
	DissolveUnfilledTeamDelay(5000),
	AIIonCannonConYardValue(),
	AIIonCannonWarFactoryValue(),
	AIIonCannonPowerValue(),
	AIIonCannonTechCenterValue(),
	AIIonCannonEngineerValue(),
	AIIonCannonThiefValue(),
	AIIonCannonHarvesterValue(),
	AIIonCannonMCVValue(),
	AIIonCannonAPCValue(),
	AIIonCannonBaseDefenseValue(),
	AIIonCannonPlugValue(),
	AIIonCannonHelipadValue(),
	AIIonCannonTempleValue(),
	AIAlternateProductionCreditCutoff(1000),
	MultiplayerAICreditMultipliers(),
	AIVirtualPurifiers(),
	AISlaveMinerNumber(),
	HarvestersPerRefinery(),
	AIExtraRefineries(),
	MinimumAIDefensiveTeams(),
	MaximumAIDefensiveTeams(),
	TotalAITeamCap(),
	AIUseTurbineUpgradeChance(1.0),
	FillEarliestTeamProbability(),
	CloakDelay(0.0),
	GameSpeedBias(1.0),
	NervousBias(1.0),
	ExplosionSpread(0.5),
	SupressRadius(256),
	MaxIQ(5),
	IQSuperWeapons(4),
	IQProduction(5),
	IQGuardArea(4),
	IQRepairSell(3),
	IQCrush(2),
	IQScatter(3),
	IQContentScan(4),
	IQAircraft(4),
	IQHarvester(3),
	IQSellBack(2),
	AIBaseSpacing(1),
	SilverCrate(CRATE_HEAL_BASE),
	WoodCrate(CRATE_MONEY),
	WaterCrate(CRATE_MONEY),
	CrateMinimum(1),
	CrateMaximum(255),
	unknown_int_1478(8192),
	FlareAnim(NULL),
	MinMoney(2500),
	MPMoney(10000),
	MPMaxMoney(10000),
	MoneyIncrement(100),
	MinUnitCount(1),
	MPUnitCount(10),
	MaxUnitCount(20),
	MPBuildLevel(10),
	GameSpeed(6),
	AIDifficultyStruct(1),
	AIPlayers(1),
	IsMPBridgeDestruction(true),
	IsMPShadowGrow(false),
	Shroud(true),
	IsMPBasesOn(true),
	IsMPTiberiumGrow(true),
	IsMPCrates(true),
	IsMPCaptureTheFlag(false),
	HarvesterTruce(false),
	MultiEngineer(false),
	AlliesAllowed(false),
	ShortGame(false),
	FogOfWar(false),
	MCVRedeploys(true),
	SuperWeaponsAllowed(true),
	BuildOffAlly(true),
	AllyChangeAllowed(true),
	DropZoneRadius(1024),
	MessageDelay(0.6),
	SavourDelay(0.03),
	MaxPlayers(8),
	BaseDefenseDelay(0.25),
	SuspendPriority(20),
	SuspendDelay(2.0),
	SurvivorFraction(0.5),
	AlliedSurvivorDivisor(200),
	SovietSurvivorDivisor(100),
	ThirdSurvivorDivisor(300),
	ReloadRate(0.05),
	AutocreateTime(5.0),
	BuildupTime(0.05),
	HarvesterLoadRate(2),
	HarvesterDumpRate(0.016),
	AtomDamage(1000),
	GrowthRate(2.0),
	ShroudRate(4.0),
	FogRate(0.05),
	IceGrowthRate(1.0),
	VeinGrowthRate(1.0),
	IceSolidifyDelay(500),
	AmbientLightChangeRate(0.2),
	AmbientLightChangeStep(0.1),
	CrateTime(10.0),
	TimerWarning(2.0),
	unknown_double_1690(14.0),
	unknown_double_1698(5.0),
	unknown_double_16A0(10.0),
	SpeakDelay(2.0),
	DamageDelay(1.0),
	Gravity(3),
	LeptonsPerSightIncrease(50),
	Incoming(MPH_IMMOBILE),
	MinDamage(1),
	MaxDamage(1000),
	RepairStep(5),
	RepairPercent(0.25),
	IRepairStep(1),
	RepairRate(0.016),
	URepairRate(0.016),
	IRepairRate(0.016),
	ConditionGreen(1.0),
	ConditionYellow(0.5),
	ConditionRed(0.5),
	RandomAnimateTime(0.083),
	CloseEnoughDistance(640),
	StrayDistance(512),
	RelaxedStray(512),
	CrushDistance(384),
	CrateRadius(640),
	HomingScatter(512),
	BallisticScatter(256),
	RefundPercent(0.5),
	BridgeStrength(1000),
	BuildSpeedBias(1.0),
	C4Delay(0.03),
	RepairThreshhold(1000),
	PathDelay(0.016),
	BlockagePathDelay(60),
	MovieTime(0.25),
	TiberiumShortScan(1536),
	TiberiumLongScan(8192),
	SlaveMinerShortScan(1280),
	SlaveMinerSlaveScan(4096),
	SlaveMinerLongScan(20480),
	SlaveMinerScanCorrection(768),
	SlaveMinerKickFrameDelay(INT_MAX),
	LightningDeferment(250),
	LightningDamage(200),
	LightningStormDuration(900),
	LightningHitDelay(90),
	LightningScatterDelay(10),
	LightningCellSpread(10),
	LightningSeparation(3),
	LightningPrintText(true),
	LightningWarhead(NULL),
	ForceShieldRadius(10),
	ForceShieldDuration(400),
	ForceShieldBlackoutDuration(800),
	ForceShieldPlayFadeSoundTime(50),
	MutateExplosion(false),
	CollapseChance(100),
	WeedCapacity(0),
	ExtraUnitLight(0),
	ExtraInfantryLight(0),
	ExtraAircraftLight(0),
	IsComputerParanoid(true),
	IsCurleyShuffle(false),
	IsBlendedFog(true),
	IsCompEasyBonus(true),
	IsFineDifficulty(false),
	IsExplosiveHarvester(false),
	IsHealthBar(true),
	IsAllyReveal(true),
	IsSeparate(false),
	IsTreeTarget(false),
	IsNamed(false),
	IsAutoCrush(false),
	IsSmartDefense(false),
	IsScatter(false),
	IsRevealByHeight(true),
	IsShroudedSubteranneanMovesAllowed(false),
	IsShroudGrow(false),
	NodAIBuildsWalls(true),
	AIBuildsWalls(true),
	UseMinDefenseRule(true),
	EMPulseSparkles(NULL),
	EngineerCaptureLevel(1.0f),
	EngineerCaptureLevel_(0.0f),
	TalkBubbleTime(300),
	RadDurationMultiple(0),
	RadApplicationDelay(0),
	RadLevelMax(0),
	RadLevelDelay(0),
	RadLightDelay(0),
	RadLevelFactor(0.0),
	RadLightFactor(0.0),
	RadTintFactor(0.0),
	RadColor(0, 0, 0),
	RadSiteWarhead(NULL),
	ElevationIncrement(0),
	ElevationIncrementBonus(1.0),
	ElevationBonusCap(0.0),
	AlliedWallTransparency(false),
	WallPenetratorThreshold(0.5),
	LocalRadarColor(255, 255, 255),
	LineTrailColorOverride(0, 0, 0),
	ChronoBeamColor(0, 0, 0),
	MagnaBeamColor(0, 0, 0),
	OreTwinkleChance(50),
	OreTwinkle(NULL),
	LaserTargetColor(0),
	IronCurtainColor(0),
	BerserkColor(0),
	ForceShieldColor(0),
	DirectRockingCoefficient(1.0f),
	FallBackCoefficient(1.0f)
{
	V3Rocket.PauseFrames = 0;
	V3Rocket.TiltFrames = 60;
	V3Rocket.PitchInitial = 0.0f;
	V3Rocket.PitchFinal = 1.0f;
	V3Rocket.TurnRate = 0.05f;
	V3Rocket.RaiseRate = 1;
	V3Rocket.Acceleration = 0.4f;
	V3Rocket.Altitude = 768;
	V3Rocket.Damage = 1000;
	V3Rocket.EliteDamage = 1000;
	V3Rocket.BodyLength = 256;
	V3Rocket.LazyCurve = true;
	V3Rocket.Type = NULL;

	DMisl.PauseFrames = 10;
	DMisl.TiltFrames = 60;
	DMisl.PitchInitial = 0.0f;
	DMisl.PitchFinal = 1.0f;
	DMisl.TurnRate = 0.05f;
	DMisl.RaiseRate = 1;
	DMisl.Acceleration = 0.4f;
	DMisl.Altitude = 768;
	DMisl.Damage = 1000;
	DMisl.EliteDamage = 1000;
	DMisl.BodyLength = 128;
	DMisl.LazyCurve = false;
	DMisl.Type = NULL;

	CMisl.PauseFrames = 10;
	CMisl.TiltFrames = 30;
	CMisl.PitchInitial = 1.0f;
	CMisl.PitchFinal = 1.0f;
	CMisl.TurnRate = 1.0f;
	CMisl.RaiseRate = 1;
	CMisl.Acceleration = 0.6f;
	CMisl.Altitude = 768;
	CMisl.Damage = 500;
	CMisl.EliteDamage = 500;
	CMisl.BodyLength = 128;
	CMisl.LazyCurve = false;
	CMisl.Type = NULL;

	for (int index = 0; index < ARRAY_SIZE(ColorAdd); index++) {
		ColorAdd[index] = RGBClass(0, 0, 0);
	}
}

// FUNCTION: GAMEMD 0x00667A30
RulesClass::~RulesClass(void)
{
}

// FUNCTION: GAMEMD 0x006686C0
void RulesClass::Initialize(CCINIClass & ini)
{
	while (ColorSchemes.Count()) {
		delete ColorSchemes[0];
	}
	ColorSchemes.Clear();
	FUN_006269A0();
	while (TeamTypes.Count()) {
		delete TeamTypes[0];
	}
	while (TaskForces.Count()) {
		delete TaskForces[0];
	}
	while (ScriptTypes.Count()) {
		delete ScriptTypes[0];
	}
	while (OverlayTypes.Count()) {
		delete OverlayTypes[0];
	}
	while (SmudgeTypes.Count()) {
		delete SmudgeTypes[0];
	}
	while (TerrainTypes.Count()) {
		delete TerrainTypes[0];
	}
	while (InfantryTypes.Count()) {
		delete InfantryTypes[0];
	}
	while (UnitTypes.Count()) {
		delete UnitTypes[0];
	}
	while (AircraftTypes.Count()) {
		delete AircraftTypes[0];
	}
	while (BulletTypes.Count()) {
		delete BulletTypes[0];
	}
	while (BuildingTypes.Count()) {
		delete BuildingTypes[0];
	}
	while (SuperWeaponTypes.Count()) {
		delete SuperWeaponTypes[0];
	}
	while (AnimTypes.Count()) {
		delete AnimTypes[0];
	}
	while (Weapons.Count()) {
		delete Weapons[0];
	}
	while (::Warheads.Count()) {
		delete ::Warheads[0];
	}
	while (VoxelAnimTypes.Count()) {
		delete VoxelAnimTypes[0];
	}
	while (ParticleTypes.Count()) {
		delete ParticleTypes[0];
	}
	while (ParticleSystemTypes.Count()) {
		delete ParticleSystemTypes[0];
	}
	while (::Houses.Count()) {
		delete ::Houses[0];
	}
	while (HouseTypes.Count()) {
		delete HouseTypes[0];
	}

	Heap_Maximums(ini);
	Addition(ini);

	for (int index = 0; index < 16; index++) {
		ColorAdd[index] = RGBClass(0, 0, 0);
	}

	CCFileClass langfile("LANGRULE.INI");
	if (langfile.Is_Available() == true) {
		CCINIClass langini;
		if (langini.Load(langfile, true) > 1) {
			return;
		}
		Addition(langini);
	}

	Read_ColorAdd(ini);

	if (Session.Type != GAME_NORMAL && Session.MPGameMode != NULL) {
		Addition(*Session.MPGameMode->FUN_005D67B0());
	}
}

// FUNCTION: GAMEMD 0x00668FB0
bool RulesClass::Special_Weapons(CCINIClass & ini)
{
	static char const * SPECIALWEAPONS = "SpecialWeapons";
	if (ini.Is_Present(SPECIALWEAPONS)) {
		NukeWarhead = TGet_Class(ini, SPECIALWEAPONS, "NukeWarhead", NukeWarhead);
		NukeProjectile = TGet_Class(ini, SPECIALWEAPONS, "NukeProjectile", NukeProjectile);
		NukeDown = TGet_Class(ini, SPECIALWEAPONS, "NukeDown", NukeDown);
		MutateWarhead = TGet_Class(ini, SPECIALWEAPONS, "MutateWarhead", MutateWarhead);
		MutateExplosionWarhead = TGet_Class(ini, SPECIALWEAPONS, "MutateExplosionWarhead", MutateExplosionWarhead);
		EMPulseWarhead = TGet_Class(ini, SPECIALWEAPONS, "EMPulseWarhead", EMPulseWarhead);
		EMPulseProjectile = TGet_Class(ini, SPECIALWEAPONS, "EMPulseProjectile", EMPulseProjectile);
		for (int i = 0; i < ::Warheads.Count(); i++) {
			::Warheads[i]->Read_INI(ini);
		}
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0066B900
bool RulesClass::Crate_Rules(CCINIClass const & ini)
{
	static char const * CRATERULES = "CrateRules";
	if (ini.Is_Present(CRATERULES)) {
		IsFreeMCV = ini.Get_Bool(CRATERULES, "FreeMCV", IsFreeMCV);
		WoodCrateImg = TGet_Class(ini, CRATERULES, "WoodCrateImg", WoodCrateImg);
		CrateImg = TGet_Class(ini, CRATERULES, "CrateImg", CrateImg);
		WaterCrateImg = TGet_Class(ini, CRATERULES, "WaterCrateImg", WaterCrateImg);
		HealCrateSound = ini.Get_VocType(CRATERULES, "HealCrateSound", HealCrateSound);
		CrateMinimum = ini.Get_Int(CRATERULES, "CrateMinimum", CrateMinimum);
		CrateMaximum = ini.Get_Int(CRATERULES, "CrateMaximum", CrateMaximum);
		CrateRadius = ini.Get_Lepton(CRATERULES, "CrateRadius", CrateRadius);
		CrateTime = ini.Get_Float(CRATERULES, "CrateRegen", CrateTime);
		UnitCrateType = TGet_Class(ini, CRATERULES, "UnitCrateType", UnitCrateType);
		SoloCrateMoney = ini.Get_Int(CRATERULES, "SoloCrateMoney", SoloCrateMoney);
		SilverCrate = ini.Get_CrateType(CRATERULES, "SilverCrate", SilverCrate);
		WoodCrate = ini.Get_CrateType(CRATERULES, "WoodCrate", WoodCrate);
		WaterCrate = ini.Get_CrateType(CRATERULES, "WaterCrate", WaterCrate);
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0066CF70
bool RulesClass::Radiation(CCINIClass const & ini)
{
	static char const * RADIATION = "Radiation";
	if (ini.Is_Present(RADIATION)) {
		RadDurationMultiple = ini.Get_Int(RADIATION, "RadDurationMultiple", RadDurationMultiple);
		RadApplicationDelay = ini.Get_Int(RADIATION, "RadApplicationDelay", RadApplicationDelay);
		RadLevelMax = ini.Get_Int(RADIATION, "RadLevelMax", RadLevelMax);
		RadLevelDelay = ini.Get_Int(RADIATION, "RadLevelDelay", RadLevelDelay);
		RadLightDelay = ini.Get_Int(RADIATION, "RadLightDelay", RadLightDelay);
		RadLevelFactor = ini.Get_Float(RADIATION, "RadLevelFactor", RadLevelFactor);
		RadLightFactor = ini.Get_Float(RADIATION, "RadLightFactor", RadLightFactor);
		RadTintFactor = ini.Get_Float(RADIATION, "RadTintFactor", RadTintFactor);
		RadColor = ini.Get_RGBClass(RADIATION, "RadColor", RadColor);
		RadSiteWarhead = TGet_Class(ini, RADIATION, "RadSiteWarhead", RadSiteWarhead);
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0066D150
bool RulesClass::Elevation_Model(CCINIClass const & ini)
{
	static char const * ELEVATIONMODEL = "ElevationModel";
	if (ini.Is_Present(ELEVATIONMODEL)) {
		ElevationIncrement = ini.Get_Int(ELEVATIONMODEL, "ElevationIncrement", ElevationIncrement);
		ElevationIncrementBonus = ini.Get_Float(ELEVATIONMODEL, "ElevationIncrementBonus", ElevationIncrementBonus);
		ElevationBonusCap = ini.Get_Float(ELEVATIONMODEL, "ElevationBonusCap", ElevationBonusCap);
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0066D1F0
bool RulesClass::Wall_Model(CCINIClass const & ini)
{
	static char const * WALLMODEL = "WallModel";
	if (ini.Is_Present(WALLMODEL)) {
		AlliedWallTransparency = ini.Get_Bool(WALLMODEL, "AlliedWallTransparency", AlliedWallTransparency);
		WallPenetratorThreshold = ini.Get_Float(WALLMODEL, "WallPenetratorThreshold", WallPenetratorThreshold);
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0066D270
void Difficulty_Get(CCINIClass const & ini, DifficultyClass & diff, char const * section)
{
	if (ini.Is_Present(section)) {
		diff.FirepowerBias = ini.Get_Float(section, "FirePower", 1);
		diff.GroundspeedBias = ini.Get_Float(section, "Groundspeed", 1);
		diff.AirspeedBias = ini.Get_Float(section, "Airspeed", 1);
		diff.ArmorBias = ini.Get_Float(section, "Armor", 1);
		diff.ROFBias = ini.Get_Float(section, "ROF", 1);
		diff.CostBias = ini.Get_Float(section, "Cost", 1);
		diff.RepairDelay = ini.Get_Float(section, "RepairDelay", .02);
		diff.BuildDelay = ini.Get_Float(section, "BuildDelay", .03);
		diff.IsBuildSlowdown = ini.Get_Bool(section, "BuildSlowdown", false);
		diff.BuildSpeedBias = ini.Get_Float(section, "BuildTime", 1);
		diff.IsWallDestroyer = ini.Get_Bool(section, "DestroyWalls", true);
		diff.IsContentScan = ini.Get_Bool(section, "ContentScan", false);
	}
}

// FUNCTION: GAMEMD 0x0066D3A0
bool RulesClass::Color_Schemes(CCINIClass const & ini)
{
	static char const * COLORS = "Colors";
	if (ini.Is_Present(COLORS)) {
		int count = ini.Entry_Count(COLORS);
		for (int i = 0; i < count; i++) {
			char const * name = ini.Get_Entry(COLORS, i);
			HSVClass hsv = ini.Get_HSVClass(COLORS, name, HSVClass(0, 0, 0));
			FUN_00626AB0(name, hsv);
			ColorScheme::Find_Or_Make(name, hsv, SchemePalette, GamePalette, 1);
			ColorScheme::Find_Or_Make(name, hsv, SchemePalette, GamePalette, 0x35);
		}
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0066D480
bool RulesClass::Read_ColorAdd(CCINIClass const & ini)
{
	static char const * COLORADD = "ColorAdd";
	if (ini.Is_Present(COLORADD)) {
		int count = ini.Entry_Count(COLORADD);
		for (int index = 0; index < count; index++) {
			char const * key = ini.Get_Entry(COLORADD, index);
			RGBClass color = ini.Get_RGBClass(COLORADD, key, RGBClass(0, 0, 0));
			ColorAdd[index] = color;
		}
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00671EA0
bool RulesClass::MPlayer(CCINIClass const & ini)
{
	static char const * MPLAYER = "MultiplayerDialogSettings";
	if (ini.Is_Present(MPLAYER)) {
		MinMoney = ini.Get_Int(MPLAYER, "MinMoney", MinMoney);
		MPMoney = ini.Get_Int(MPLAYER, "Money", MPMoney);
		MPMaxMoney = ini.Get_Int(MPLAYER, "MaxMoney", MPMaxMoney);
		MoneyIncrement = ini.Get_Int(MPLAYER, "MoneyIncrement", MoneyIncrement);
		MinUnitCount = ini.Get_Int(MPLAYER, "MinUnitCount", MinUnitCount);
		MPUnitCount = ini.Get_Int(MPLAYER, "UnitCount", MPUnitCount);
		MaxUnitCount = ini.Get_Int(MPLAYER, "MaxUnitCount", MaxUnitCount);
		MPBuildLevel = ini.Get_Int(MPLAYER, "TechLevel", MPBuildLevel);
		GameSpeed = ini.Get_Int(MPLAYER, "GameSpeed", GameSpeed);
		AIDifficultyStruct = ini.Get_Int(MPLAYER, "AIDifficulty", AIDifficultyStruct);
		AIPlayers = ini.Get_Int(MPLAYER, "AIPlayers", AIPlayers);
		IsMPBridgeDestruction = ini.Get_Bool(MPLAYER, "BridgeDestruction", IsMPBridgeDestruction);
		IsMPShadowGrow = ini.Get_Bool(MPLAYER, "ShadowGrow", IsMPShadowGrow);
		Shroud = ini.Get_Bool(MPLAYER, "Shroud", Shroud);
		IsMPBasesOn = ini.Get_Bool(MPLAYER, "Bases", IsMPBasesOn);
		IsMPTiberiumGrow = ini.Get_Bool(MPLAYER, "TiberiumGrows", IsMPTiberiumGrow);
		IsMPCrates = ini.Get_Bool(MPLAYER, "Crates", IsMPCrates);
		IsMPCaptureTheFlag = ini.Get_Bool(MPLAYER, "CaptureTheFlag", IsMPCaptureTheFlag);
		HarvesterTruce = ini.Get_Bool(MPLAYER, "HarvesterTruce", HarvesterTruce);
		MultiEngineer = ini.Get_Bool(MPLAYER, "MultiEngineer", MultiEngineer);
		AlliesAllowed = ini.Get_Bool(MPLAYER, "AlliesAllowed", AlliesAllowed);
		AllyChangeAllowed = ini.Get_Bool(MPLAYER, "AllyChangeAllowed", AllyChangeAllowed);
		ShortGame = ini.Get_Bool(MPLAYER, "ShortGame", ShortGame);
		SuperWeaponsAllowed = ini.Get_Bool(MPLAYER, "SuperWeaponsAllowed", SuperWeaponsAllowed);
		BuildOffAlly = ini.Get_Bool(MPLAYER, "BuildOffAlly", BuildOffAlly);
		FogOfWar = ini.Get_Bool(MPLAYER, "FogOfWar", FogOfWar);
		MCVRedeploys = ini.Get_Bool(MPLAYER, "MCVRedeploys", MCVRedeploys);
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00672230
bool RulesClass::Heap_Maximums(CCINIClass const & ini)
{
	static char const * MAXIMUMS = "Maximums";
	if (ini.Is_Present(MAXIMUMS)) {
		MaxPlayers = ini.Get_Int(MAXIMUMS, "Players", MaxPlayers);
	}
	return(true);
}

// FUNCTION: GAMEMD 0x00672280
bool RulesClass::Do_InfantryTypes(CCINIClass const & ini)
{
	static char const * INFANTRYTYPES = "InfantryTypes";
	char buffer[32];
	int count = ini.Entry_Count(INFANTRYTYPES);
	for (int i = 0; i < count; i++) {
		if (ini.Get_String(INFANTRYTYPES, ini.Get_Entry(INFANTRYTYPES, i), "", buffer, sizeof(buffer))) {
			InfantryTypeClass::Find_Or_Make(buffer);
		}
	}
	return(count > 0);
}

// FUNCTION: GAMEMD 0x006722F0
bool RulesClass::Do_HouseTypes(CCINIClass const & ini)
{
	static char const * HOUSES = "Countries";
	char buffer[32];
	int count = ini.Entry_Count(HOUSES);
	for (int i = 0; i < count; i++) {
		if (ini.Get_String(HOUSES, ini.Get_Entry(HOUSES, i), "", buffer, sizeof(buffer))) {
			HouseTypeClass::Find_Or_Make(buffer);
		}
	}
	return(count > 0);
}

// FUNCTION: GAMEMD 0x00672360
bool RulesClass::Do_VehicleTypes(CCINIClass const & ini)
{
	static char const * VEHICLETYPES = "VehicleTypes";
	char buffer[32];
	int count = ini.Entry_Count(VEHICLETYPES);
	for (int i = 0; i < count; i++) {
		if (ini.Get_String(VEHICLETYPES, ini.Get_Entry(VEHICLETYPES, i), "", buffer, sizeof(buffer))) {
			UnitTypeClass::Find_Or_Make(buffer);
		}
	}
	return(count > 0);
}

// FUNCTION: GAMEMD 0x006723D0
bool RulesClass::Do_AircraftTypes(CCINIClass const & ini)
{
	static char const * AIRCRAFTTYPES = "AircraftTypes";
	char buffer[32];
	int count = ini.Entry_Count(AIRCRAFTTYPES);
	for (int i = 0; i < count; i++) {
		if (ini.Get_String(AIRCRAFTTYPES, ini.Get_Entry(AIRCRAFTTYPES, i), "", buffer, sizeof(buffer))) {
			AircraftTypeClass::Find_Or_Make(buffer);
		}
	}
	return(count > 0);
}

// FUNCTION: GAMEMD 0x00672440
bool RulesClass::Do_Sides(CCINIClass const & ini)
{
	static char const * SIDES = "Sides";
	int count = ini.Entry_Count(SIDES);
	DebugString("Processing sides.\n");
	for (int i = 0; i < count; i++) {
		char const * name = ini.Get_Entry(SIDES, i);
		int side = SideClass::From_Name(name);
		SideClass * sidep;
		if (side == SIDE_NONE) {
			sidep = new SideClass(name);
		} else {
			sidep = ::Sides[side];
		}
		DebugString("Side %d: %s \n", i, (char const *)sidep->IniName);
		sidep->Houses = ini.Get_House_List(SIDES, name, sidep->Houses);
		for (int house = 0; house < sidep->Houses.Count(); house++) {
			side = sidep->Houses[house];
			HouseTypes[side]->Side = (SideType)::Sides.ID(sidep);
			DebugString("  %s\n", (char const *)HouseTypes[side]->IniName);
		}
	}
	return(count > 0);
}

// FUNCTION: GAMEMD 0x006725F0
bool RulesClass::Do_SuperWeaponTypes(CCINIClass const & ini)
{
	static char const * SUPERWEAPONTYPES = "SuperWeaponTypes";
	char buffer[32];
	int count = ini.Entry_Count(SUPERWEAPONTYPES);
	for (int i = 0; i < count; i++) {
		if (ini.Get_String(SUPERWEAPONTYPES, ini.Get_Entry(SUPERWEAPONTYPES, i), "", buffer, sizeof(buffer))) {
			SuperWeaponTypeClass::Find_Or_Make(buffer);
		}
	}
	return(count > 0);
}

// FUNCTION: GAMEMD 0x00672660
bool RulesClass::Do_BuildingTypes(CCINIClass const & ini)
{
	static char const * BUILDINGTYPES = "BuildingTypes";
	char buffer[32];
	int count = ini.Entry_Count(BUILDINGTYPES);
	for (int i = 0; i < count; i++) {
		if (ini.Get_String(BUILDINGTYPES, ini.Get_Entry(BUILDINGTYPES, i), "", buffer, sizeof(buffer))) {
			BuildingTypeClass::Find_Or_Make(buffer);
		}
	}
	return(count > 0);
}

// FUNCTION: GAMEMD 0x006726D0
bool RulesClass::Do_TerrainTypes(CCINIClass const & ini)
{
	static char const * TERRAINTYPES = "TerrainTypes";
	char buffer[32];
	int count = ini.Entry_Count(TERRAINTYPES);
	for (int i = 0; i < count; i++) {
		if (ini.Get_String(TERRAINTYPES, ini.Get_Entry(TERRAINTYPES, i), "", buffer, sizeof(buffer))) {
			TerrainTypeClass::Find_Or_Make(buffer);
		}
	}
	return(count > 0);
}

// FUNCTION: GAMEMD 0x00672740
bool RulesClass::Do_TeamTypes(CCINIClass & ini)
{
	static char const * TEAMS = "Teams";
	int count = ini.Entry_Count(TEAMS);
	for (int i = 0; i < count; i++) {
		TeamTypeClass * team = TGet_Class<TeamTypeClass>(ini, TEAMS, ini.Get_Entry(TEAMS, i), NULL);
		team->Read_INI(ini);
	}
	return(count > 0);
}

// FUNCTION: GAMEMD 0x006727D0
bool RulesClass::Do_SmudgeTypes(CCINIClass const & ini)
{
	static char const * SMUDGETYPES = "SmudgeTypes";
	char buffer[32];
	int count = ini.Entry_Count(SMUDGETYPES);
	for (int i = 0; i < count; i++) {
		if (ini.Get_String(SMUDGETYPES, ini.Get_Entry(SMUDGETYPES, i), "", buffer, sizeof(buffer))) {
			SmudgeTypeClass::Find_Or_Make(buffer);
		}
	}
	return(count > 0);
}

// FUNCTION: GAMEMD 0x00672840
bool RulesClass::Do_OverlayTypes(CCINIClass const & ini)
{
	static char const * OVERLAYTYPES = "OverlayTypes";
	char buffer[32];
	int count = ini.Entry_Count(OVERLAYTYPES);
	for (int i = 0; i < count; i++) {
		if (ini.Get_String(OVERLAYTYPES, ini.Get_Entry(OVERLAYTYPES, i), "", buffer, sizeof(buffer))) {
			OverlayTypeClass::Find_Or_Make(buffer);
		}
	}
	return(count > 0);
}

// FUNCTION: GAMEMD 0x006728B0
bool RulesClass::Do_AnimTypes(CCINIClass const & ini)
{
	char const * const ANIMATIONS = "Animations";
	char buffer[32];
	int count = ini.Entry_Count(ANIMATIONS);
	for (int i = 0; i < count; i++) {
		if (ini.Get_String(ANIMATIONS, ini.Get_Entry(ANIMATIONS, i), "", buffer, sizeof(buffer))) {
			AnimTypeClass::Find_Or_Make(buffer);
		}
	}
	return(count > 0);
}

// FUNCTION: GAMEMD 0x00672920
bool RulesClass::Do_VoxelAnimTypes(CCINIClass const & ini)
{
	char const * const VOXELANIMS = "VoxelAnims";
	char buffer[32];
	int count = ini.Entry_Count(VOXELANIMS);
	for (int i = 0; i < count; i++) {
		if (ini.Get_String(VOXELANIMS, ini.Get_Entry(VOXELANIMS, i), "", buffer, sizeof(buffer))) {
			VoxelAnimTypeClass::Find_Or_Make(buffer);
		}
	}
	return(count > 0);
}

// FUNCTION: GAMEMD 0x00672990
bool RulesClass::Do_WarheadTypes(CCINIClass const & ini)
{
	char buffer[32];
	int count = ini.Entry_Count("Warheads");
	for (int i = 0; i < count; i++) {
		if (ini.Get_String("Warheads", ini.Get_Entry("Warheads", i), "", buffer, sizeof(buffer))) {
			WarheadTypeClass::Find_Or_Make(buffer);
		}
	}
	return(count > 0);
}

// FUNCTION: GAMEMD 0x00672A00
bool RulesClass::Do_ParticleTypes(CCINIClass const & ini)
{
	char const * const PARTICLES = "Particles";
	char buffer[32];
	int count = ini.Entry_Count(PARTICLES);
	for (int i = 0; i < count; i++) {
		if (ini.Get_String(PARTICLES, ini.Get_Entry(PARTICLES, i), "", buffer, sizeof(buffer))) {
			ParticleTypeClass::Find_Or_Make(buffer);
		}
	}
	return(count > 0);
}

// FUNCTION: GAMEMD 0x00672A70
bool RulesClass::Do_ParticleSystemTypes(CCINIClass const & ini)
{
	char const * const PARTICLESYSTEMS = "ParticleSystems";
	char buffer[32];
	int count = ini.Entry_Count(PARTICLESYSTEMS);
	for (int i = 0; i < count; i++) {
		if (ini.Get_String(PARTICLESYSTEMS, ini.Get_Entry(PARTICLESYSTEMS, i), "", buffer, sizeof(buffer))) {
			ParticleSystemTypeClass::Find_Or_Make(buffer);
		}
	}
	return(count > 0);
}

// FUNCTION: GAMEMD 0x00673E80
bool RulesClass::Powerups(CCINIClass const & ini)
{
	static char const * POWERUPS = "Powerups";
	if (ini.Is_Present(POWERUPS)) {
		for (int crate = 0; crate < 19; crate++) {
			char buffer[128];
			if (ini.Get_String(POWERUPS, ::Powerups::Effects[crate], "0,NONE", buffer, sizeof(buffer))) {
				char * token = strtok(buffer, ",");
				if (token) {
					strtrim(token);
					::Powerups::Weights[crate] = atoi(token);
				}
				token = strtok(NULL, ",");
				if (token) {
					strtrim(token);
					::Powerups::Anims[crate] = Anim_From_Name(token);
				}
				token = strtok(NULL, ",");
				if (token != NULL) {
					strtrim(token);
					if (stricmp(token, "yes") == 0) {
						::Powerups::Naval[crate] = true;
					} else if (stricmp(token, "no") == 0) {
						::Powerups::Naval[crate] = false;
					}
				}
				token = strtok(NULL, ",");
				if (token != NULL) {
					if (strchr(token, '%') != NULL) {
						::Powerups::Arguments[crate] = atof(token) * 0.01;
					} else {
						strtrim(token);
						::Powerups::Arguments[crate] = atof(token);
					}
				}
			}
		}
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00674000
bool RulesClass::Land_Types(CCINIClass const & ini)
{
	static char const * _lands[LAND_COUNT] = {
		"Clear", "Road", "Water", "Rock", "Wall", "Tiberium",
		"Beach", "Rough", "Ice", "Railroad", "Tunnel", "Weeds",
	};
	for (int land = LAND_FIRST; land < LAND_COUNT; land++) {
		GroundType * gptr = &Ground[land];
		if (ini.Is_Present(_lands[land])) {
			gptr->Cost[SPEED_HOVER] = MIN(ini.Get_Float(_lands[land], "Hover", 1.0), 1.0);
			gptr->Cost[SPEED_FOOT] = MIN(ini.Get_Float(_lands[land], "Foot", 1.0), 1.0);
			gptr->Cost[SPEED_TRACK] = MIN(ini.Get_Float(_lands[land], "Track", 1.0), 1.0);
			gptr->Cost[SPEED_WHEEL] = MIN(ini.Get_Float(_lands[land], "Wheel", 1.0), 1.0);
			gptr->Cost[SPEED_WINGED] = 1.0f;
			gptr->Cost[SPEED_FLOAT] = MIN(ini.Get_Float(_lands[land], "Float", 1.0), 1.0);
			gptr->Cost[SPEED_AMPHIBIOUS] = MIN(ini.Get_Float(_lands[land], "Amphibious", 1.0), 1.0);
			gptr->Cost[SPEED_CREEP] = MIN(ini.Get_Float(_lands[land], "FloatBeach", 1.0), 1.0);
			gptr->Build = ini.Get_Bool(_lands[land], "Buildable", false);
		}
	}
	return(true);
}

// FUNCTION: GAMEMD 0x00674240
bool RulesClass::IQ(CCINIClass const & ini)
{
	static char const * IQCONTROL = "IQ";
	if (ini.Is_Present(IQCONTROL)) {
		MaxIQ = ini.Get_Int(IQCONTROL, "MaxIQLevels", MaxIQ);
		IQSuperWeapons = ini.Get_Int(IQCONTROL, "SuperWeapons", IQSuperWeapons);
		IQProduction = ini.Get_Int(IQCONTROL, "Production", IQProduction);
		IQGuardArea = ini.Get_Int(IQCONTROL, "GuardArea", IQGuardArea);
		IQRepairSell = ini.Get_Int(IQCONTROL, "RepairSell", IQRepairSell);
		IQCrush = ini.Get_Int(IQCONTROL, "AutoCrush", IQCrush);
		IQScatter = ini.Get_Int(IQCONTROL, "Scatter", IQScatter);
		IQContentScan = ini.Get_Int(IQCONTROL, "ContentScan", IQContentScan);
		IQAircraft = ini.Get_Int(IQCONTROL, "Aircraft", IQAircraft);
		IQHarvester = ini.Get_Int(IQCONTROL, "Harvester", IQHarvester);
		IQSellBack = ini.Get_Int(IQCONTROL, "SellBack", IQSellBack);
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x006743D0
bool RulesClass::Jumpjet_Controls(CCINIClass const & ini)
{
	static char const * SECTION = "JumpjetControls";
	if (ini.Is_Present(SECTION)) {
		JumpjetTurnRate = ini.Get_Int(SECTION, "TurnRate", JumpjetTurnRate);
		JumpjetSpeed = ini.Get_Int(SECTION, "Speed", JumpjetSpeed);
		JumpjetClimb = ini.Get_Float(SECTION, "Climb", JumpjetClimb);
		JumpjetCruiseHeight = ini.Get_Int(SECTION, "CruiseHeight", JumpjetCruiseHeight);
		JumpjetAcceleration = ini.Get_Float(SECTION, "Acceleration", JumpjetAcceleration);
		JumpjetWobblesPerSecond = ini.Get_Float(SECTION, "WobblesPerSecond", JumpjetWobblesPerSecond);
		JumpjetWobbleDeviation = ini.Get_Int(SECTION, "WobbleDeviation", JumpjetWobbleDeviation);
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00674500
bool RulesClass::Difficulty(CCINIClass const & ini)
{
	Difficulty_Get(ini, Diff[DIFF_EASY], "Easy");
	Difficulty_Get(ini, Diff[DIFF_NORMAL], "Normal");
	Difficulty_Get(ini, Diff[DIFF_HARD], "Difficult");
	return(true);
}

// FUNCTION: GAMEMD 0x00674550
bool RulesClass::Do_Movies(CCINIClass const & ini)
{
	static char const * const MOVIES = "Movies";
	char buffer[32];
	if (ini.Is_Present(MOVIES)) {
		int count = ini.Entry_Count(MOVIES);
		for (int i = 0; i < count; i++) {
			if (ini.Get_String(MOVIES, ini.Get_Entry(MOVIES, i), "<none>", buffer, sizeof(buffer))) {
				if (VQ_From_Name(buffer) == -1) {
					::Movies.Add(strdup(buffer));
				}
			}
		}
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00674650
bool RulesClass::Read_AdvancedCommandBar(CCINIClass const & ini, bool multiplayer)
{
	static char const * BAR = "AdvancedCommandBar";
	static char const * MPBAR = "MultiplayerAdvancedCommandBar";
	static char const * BUTTONLIST = "ButtonList";
	static char const * NONE = "";
	char const * section = multiplayer ? MPBAR : BAR;
	char buffer[512];
	if (ini.Is_Present(section)) {
		for (int i = 0; i < 25; i++) {
			FUN_006CFD20(i, DAT_008427CC);
		}
		if (ini.Get_String(section, BUTTONLIST, NONE, buffer, sizeof(buffer))) {
			int count = 0;
			int slot = 0;
			for (char * token = strtok(buffer, ","); token; token = strtok(NULL, ",")) {
				int button = FUN_006CFCC0(token);
				if (button != DAT_008427CC) {
					FUN_006CFD20(button, slot);
				}
				count++;
				slot++;
			}
			FUN_006CFDB0(count);
		}
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00674730
void RulesClass::Save(IStream * stream)
{
	stream->Write(this, sizeof(*this), NULL);
	AIIonCannonConYardValue.Save(stream);
	AIIonCannonWarFactoryValue.Save(stream);
	AIIonCannonPowerValue.Save(stream);
	AIIonCannonTechCenterValue.Save(stream);
	AIIonCannonEngineerValue.Save(stream);
	AIIonCannonThiefValue.Save(stream);
	AIIonCannonHarvesterValue.Save(stream);
	AIIonCannonMCVValue.Save(stream);
	AIIonCannonAPCValue.Save(stream);
	AIIonCannonBaseDefenseValue.Save(stream);
	AIIonCannonPlugValue.Save(stream);
	AIIonCannonHelipadValue.Save(stream);
	AIIonCannonTempleValue.Save(stream);
	FillEarliestTeamProbability.Save(stream);
	TeamDelays.Save(stream);
	AIForcePredictionFudge.Save(stream);
	AlliedBaseDefenseCounts.Save(stream);
	SovietBaseDefenseCounts.Save(stream);
	ThirdBaseDefenseCounts.Save(stream);
	OverloadCount.Save(stream);
	OverloadDamage.Save(stream);
	OverloadFrames.Save(stream);
	DisabledDisguiseDetectionPercent.Save(stream);
	AISuperDefenseProbability.Save(stream);
	AIAutoDeployFrameDelay.Save(stream);
	AIPickWallDefensePercent.Save(stream);
	MinimumAIDefensiveTeams.Save(stream);
	MaximumAIDefensiveTeams.Save(stream);
	AICaptureNormal.Save(stream);
	AICaptureWounded.Save(stream);
	AICaptureLowPower.Save(stream);
	AICaptureLowMoney.Save(stream);
	MultiplayerAICreditMultipliers.Save(stream);
	AIVirtualPurifiers.Save(stream);
	AISlaveMinerNumber.Save(stream);
	HarvestersPerRefinery.Save(stream);
	AIExtraRefineries.Save(stream);
	TotalAITeamCap.Save(stream);
	AIHateDelays.Save(stream);
	RadarEventSuppressionDistances.Save(stream);
	RadarEventVisibilityDurations.Save(stream);
	RadarEventDurations.Save(stream);
	BarrelDebris.Save(stream);
	ExplosiveVoxelDebris.Save(stream);
	PrerequisitePower.Save(stream);
	PrerequisiteFactory.Save(stream);
	PrerequisiteBarracks.Save(stream);
	PrerequisiteRadar.Save(stream);
	PrerequisiteTech.Save(stream);
	PrerequisiteProc.Save(stream);
	CreditTicks.Save(stream);
	LightningSounds.Save(stream);
	Scorches.Save(stream);
	Scorches1.Save(stream);
	Scorches2.Save(stream);
	Scorches3.Save(stream);
	Scorches4.Save(stream);
	RepairBay.Save(stream);
	Shipyard.Save(stream);
	BuildConst.Save(stream);
	BuildPower.Save(stream);
	BuildRefinery.Save(stream);
	BuildBarracks.Save(stream);
	BuildTech.Save(stream);
	BuildWeapons.Save(stream);
	AlliedBaseDefenses.Save(stream);
	SovietBaseDefenses.Save(stream);
	ThirdBaseDefenses.Save(stream);
	BuildDefense.Save(stream);
	BuildPDefense.Save(stream);
	BuildAA.Save(stream);
	BuildHelipad.Save(stream);
	BuildRadar.Save(stream);
	ConcreteWalls.Save(stream);
	NSGates.Save(stream);
	EWGates.Save(stream);
	BuildNavalYard.Save(stream);
	BuildDummy.Save(stream);
	NeutralTechBuildings.Save(stream);
	PadAircraft.Save(stream);
	OnFire.Save(stream);
	TreeFire.Save(stream);
	SplashList.Save(stream);
	BaseUnit.Save(stream);
	DefaultMirageDisguises.Save(stream);
	HarvesterUnit.Save(stream);
	SecretInfantry.Save(stream);
	SecretUnits.Save(stream);
	SecretBuildings.Save(stream);
	DeadBodies.Save(stream);
	DropPod.Save(stream);
	MetallicDebris.Save(stream);
	BridgeExplosions.Save(stream);
	IceCrackSounds.Save(stream);
	AmerParaDropInf.Save(stream);
	AmerParaDropNum.Save(stream);
	AllyParaDropInf.Save(stream);
	AllyParaDropNum.Save(stream);
	SovParaDropInf.Save(stream);
	SovParaDropNum.Save(stream);
	YuriParaDropInf.Save(stream);
	YuriParaDropNum.Save(stream);
	AnimToInfantry.Save(stream);
	DamageFireTypes.Save(stream);
	WeatherConClouds.Save(stream);
	WeatherConBolts.Save(stream);
	for (int index = 0; index < 16; index++) {
		ColorAdd[index].Save(stream);
	}
}

// FUNCTION: GAMEMD 0x00678850
void RulesClass::Detach(AbstractClass const * target, bool all)
{
	if (target == LargeVisceroid) LargeVisceroid = NULL;
	if (target == SmallVisceroid) SmallVisceroid = NULL;
	if (target == VeinAttack) VeinAttack = NULL;
	if (target == Wake) Wake = NULL;
	if (target == OreTwinkle) OreTwinkle = NULL;
	if (target == FlamingInfantry) FlamingInfantry = NULL;
	if (target == NukeTakeOff) NukeTakeOff = NULL;
	if (target == InfantryHeadPop) InfantryHeadPop = NULL;
	if (target == InfantryNuked) InfantryNuked = NULL;
	if (target == InfantryVirus) InfantryVirus = NULL;
	if (target == InfantryBrute) InfantryBrute = NULL;
	if (target == InfantryMutate) InfantryMutate = NULL;
	if (target == Behind) Behind = NULL;
	if (target == DropPodWeapon) DropPodWeapon = NULL;
	if (target == FlameDamage) FlameDamage = NULL;
	if (target == FlameDamage2) FlameDamage2 = NULL;
	if (target == NukeWarhead) NukeWarhead = NULL;
	if (target == MutateWarhead) MutateWarhead = NULL;
	if (target == MutateExplosionWarhead) MutateExplosionWarhead = NULL;
	if (target == EMPulseWarhead) EMPulseWarhead = NULL;
	if (target == C4Warhead) C4Warhead = NULL;
	if (target == CrushWarhead) C4Warhead = NULL;
	if (target == V3Warhead) V3Warhead = NULL;
	if (target == DMislWarhead) DMislWarhead = NULL;
	if (target == V3EliteWarhead) V3EliteWarhead = NULL;
	if (target == DMislEliteWarhead) DMislEliteWarhead = NULL;
	if (target == CMislWarhead) CMislWarhead = NULL;
	if (target == CMislEliteWarhead) CMislEliteWarhead = NULL;
	if (target == V3Rocket.Type) V3Rocket.Type = NULL;
	if (target == DMisl.Type) DMisl.Type = NULL;
	if (target == CMisl.Type) CMisl.Type = NULL;
	if (target == IvanWarhead) IvanWarhead = NULL;
	if (target == DeathWeapon) DeathWeapon = NULL;
	if (target == IonCannonWarhead) IonCannonWarhead = NULL;
	if (target == EMPulseSparkles) EMPulseSparkles = NULL;
	if (target == DefaultLargeGreySmokeSystem) DefaultLargeGreySmokeSystem = NULL;
	if (target == DefaultSmallGreySmokeSystem) DefaultSmallGreySmokeSystem = NULL;
	if (target == DefaultSparkSystem) DefaultSparkSystem = NULL;
	if (target == DefaultLargeRedSmokeSystem) DefaultLargeRedSmokeSystem = NULL;
	if (target == DefaultSmallRedSmokeSystem) DefaultSmallRedSmokeSystem = NULL;
	if (target == DefaultDebrisSmokeSystem) DefaultDebrisSmokeSystem = NULL;
	if (target == DefaultFireStreamSystem) DefaultFireStreamSystem = NULL;
	if (target == DefaultTestParticleSystem) DefaultTestParticleSystem = NULL;
	if (target == DefaultRepairParticleSystem) DefaultRepairParticleSystem = NULL;
	if (target == VeinholeTypeClass) VeinholeTypeClass = NULL;
	if (target == WeatherConBoltExplosion) WeatherConBoltExplosion = NULL;
	if (target == DominatorFirstAnim) DominatorFirstAnim = NULL;
	if (target == DominatorSecondAnim) DominatorSecondAnim = NULL;
	if (target == DrainAnimationType) DrainAnimationType = NULL;
	if (target == ControlledAnimationType) ControlledAnimationType = NULL;
	if (target == PermaControlledAnimationType) PermaControlledAnimationType = NULL;
	if (target == Smoke1) Smoke1 = NULL;
	if (target == Smoke_) Smoke_ = NULL;
	if (target == MoveFlash) MoveFlash = NULL;
	if (target == BombParachute) BombParachute = NULL;
	if (target == Parachute) Parachute = NULL;
	if (target == SmallFire) SmallFire = NULL;
	if (target == LargeFire) LargeFire = NULL;
	if (target == FlareAnim) FlareAnim = NULL;
	if (target == UnitCrateType) UnitCrateType = NULL;
	if (target == Paratrooper) Paratrooper = NULL;
	if (target == AlliedDisguise) AlliedDisguise = NULL;
	if (target == SovietDisguise) SovietDisguise = NULL;
	if (target == ThirdDisguise) ThirdDisguise = NULL;
	if (target == Technician) Technician = NULL;
	if (target == Engineer) Engineer = NULL;
	if (target == Pilot) Pilot = NULL;
	if (target == AlliedCrew) AlliedCrew = NULL;
	if (target == SovietCrew) SovietCrew = NULL;
	if (target == ThirdCrew) ThirdCrew = NULL;
	if (target == GDIGateOne) GDIGateOne = NULL;
	if (target == GDIGateTwo) GDIGateTwo = NULL;
	if (target == NodGateOne) NodGateOne = NULL;
	if (target == NodGateTwo) NodGateTwo = NULL;
	if (target == WallTower) WallTower = NULL;
	if (target == GDIPowerPlant) GDIPowerPlant = NULL;
	if (target == NodRegularPower) NodRegularPower = NULL;
	if (target == NodAdvancedPower) NodAdvancedPower = NULL;
	if (target == ThirdPowerPlant) ThirdPowerPlant = NULL;
	if (target == PrerequisiteProcAlternate) ThirdPowerPlant = NULL;
	if (target == NukeProjectile) NukeProjectile = NULL;
	if (target == NukeDown) NukeDown = NULL;
	if (target == EMPulseProjectile) EMPulseProjectile = NULL;
	if (target == TireVoxelDebris) TireVoxelDebris = NULL;
	if (target == ScrapVoxelDebris) ScrapVoxelDebris = NULL;
	if (target == AtmosphereEntry) AtmosphereEntry = NULL;
	if (target == InfantryExplode) InfantryExplode = NULL;
	if (target == IonBlast) IonBlast = NULL;
	if (target == IonBeam) IonBeam = NULL;
	if (target == ChronoBlast) ChronoBlast = NULL;
	if (target == ChronoBlastDest) ChronoBlastDest = NULL;
	if (target == ChronoPlacement) ChronoPlacement = NULL;
	if (target == ChronoBeam) ChronoBeam = NULL;
	if (target == WarpIn) WarpIn = NULL;
	if (target == WarpOut) WarpOut = NULL;
	if (target == WarpAway) WarpAway = NULL;
	if (target == ChronoSparkle1) ChronoSparkle1 = NULL;
	if (target == IronCurtainInvokeAnim) IronCurtainInvokeAnim = NULL;
	if (target == ForceShieldInvokeAnim) ForceShieldInvokeAnim = NULL;
	if (target == WeaponNullifyAnim) WeaponNullifyAnim = NULL;
	if (target == Dig) Dig = NULL;
	if (target == BarrelExplode) BarrelExplode = NULL;
	if (target == BarrelParticle) BarrelParticle = NULL;
	if (target == DropPodPuff) DropPodPuff = NULL;
	if (target == LightningWarhead) LightningWarhead = NULL;
	if (target == DominatorWarhead) DominatorWarhead = NULL;
	if (target == RadSiteWarhead) RadSiteWarhead = NULL;
	BarrelDebris.Delete((VoxelAnimTypeClass const *)target);
	OnFire.Delete((AnimTypeClass const *)target);
	TreeFire.Delete((AnimTypeClass const *)target);
	SplashList.Delete((AnimTypeClass const *)target);
	Scorches.Delete((SmudgeTypeClass const *)target);
	Scorches1.Delete((SmudgeTypeClass const *)target);
	Scorches2.Delete((SmudgeTypeClass const *)target);
	Scorches3.Delete((SmudgeTypeClass const *)target);
	Scorches4.Delete((SmudgeTypeClass const *)target);
	BaseUnit.Delete((UnitTypeClass const *)target);
	DefaultMirageDisguises.Delete((TerrainTypeClass const *)target);
	HarvesterUnit.Delete((UnitTypeClass const *)target);
	SecretInfantry.Delete((InfantryTypeClass const *)target);
	SecretUnits.Delete((UnitTypeClass const *)target);
	SecretBuildings.Delete((BuildingTypeClass const *)target);
	RepairBay.Delete((BuildingTypeClass const *)target);
	Shipyard.Delete((BuildingTypeClass const *)target);
	BuildConst.Delete((BuildingTypeClass const *)target);
	BuildPower.Delete((BuildingTypeClass const *)target);
	BuildRefinery.Delete((BuildingTypeClass const *)target);
	BuildBarracks.Delete((BuildingTypeClass const *)target);
	BuildTech.Delete((BuildingTypeClass const *)target);
	BuildWeapons.Delete((BuildingTypeClass const *)target);
	AlliedBaseDefenses.Delete((BuildingTypeClass const *)target);
	SovietBaseDefenses.Delete((BuildingTypeClass const *)target);
	ThirdBaseDefenses.Delete((BuildingTypeClass const *)target);
	BuildDefense.Delete((BuildingTypeClass const *)target);
	BuildPDefense.Delete((BuildingTypeClass const *)target);
	BuildAA.Delete((BuildingTypeClass const *)target);
	BuildHelipad.Delete((BuildingTypeClass const *)target);
	BuildRadar.Delete((BuildingTypeClass const *)target);
	ConcreteWalls.Delete((BuildingTypeClass const *)target);
	NSGates.Delete((BuildingTypeClass const *)target);
	EWGates.Delete((BuildingTypeClass const *)target);
	BuildNavalYard.Delete((BuildingTypeClass const *)target);
	BuildDummy.Delete((BuildingTypeClass const *)target);
	NeutralTechBuildings.Delete((BuildingTypeClass const *)target);
	PadAircraft.Delete((AircraftTypeClass const *)target);
	DeadBodies.Delete((AnimTypeClass const *)target);
	DropPod.Delete((AnimTypeClass const *)target);
	MetallicDebris.Delete((AnimTypeClass const *)target);
	BridgeExplosions.Delete((AnimTypeClass const *)target);
	WeatherConClouds.Delete((AnimTypeClass const *)target);
	WeatherConBolts.Delete((AnimTypeClass const *)target);
	AmerParaDropInf.Delete((InfantryTypeClass const *)target);
	AllyParaDropInf.Delete((InfantryTypeClass const *)target);
	SovParaDropInf.Delete((InfantryTypeClass const *)target);
	YuriParaDropInf.Delete((InfantryTypeClass const *)target);
	AnimToInfantry.Delete((InfantryTypeClass const *)target);
	DamageFireTypes.Delete((AnimTypeClass const *)target);
}

extern unsigned int DAT_00B053B4;
extern bool DAT_00B053B8;
int FUN_005D5ED0(void);

// FUNCTION: GAMEMD 0x00679A10
bool RulesClass::Objects(CCINIClass & ini)
{
	for (int house = HOUSE_FIRST; house < HouseTypes.Count(); house++) {
		HouseTypes[house]->Read_INI(ini);
	}
	for (int index = 0; index < SuperWeaponTypes.Count(); index++) {
		SuperWeaponTypes[index]->Read_INI(ini);
	}
	for (int anim = 0; anim < AnimTypes.Count(); anim++) {
		AnimTypes[anim]->Read_INI(ArtINI);
	}
	int bindex;
	for (bindex = 0; bindex < BuildingTypes.Count(); bindex++) {
		BuildingTypes[bindex]->Read_INI(ini);
	}
	for (int aindex = 0; aindex < AircraftTypes.Count(); aindex++) {
		AircraftTypes[aindex]->Read_INI(ini);
	}
	for (int uindex = 0; uindex < UnitTypes.Count(); uindex++) {
		UnitTypes[uindex]->Read_INI(ini);
	}
	for (int iindex = 0; iindex < InfantryTypes.Count(); iindex++) {
		InfantryTypes[iindex]->Read_INI(ini);
	}
	int windex;
	for (windex = 0; windex < Weapons.Count(); windex++) {
		Weapons[windex]->Read_INI(ini);
	}
	for (int proj = 0; proj < BulletTypes.Count(); proj++) {
		BulletTypes[proj]->Read_INI(ini);
	}
	for (int whead = 0; whead < ::Warheads.Count(); whead++) {
		::Warheads[whead]->Read_INI(ini);
	}
	for (windex = 0; windex < Weapons.Count(); windex++) {
		Weapons[windex]->Init_Max_Speed();
	}
	for (bindex = 0; bindex < BuildingTypes.Count(); bindex++) {
		BuildingTypes[bindex]->Calculate_Base_Defense_Values();
	}
	for (int tindex = 0; tindex < TerrainTypes.Count(); tindex++) {
		TerrainTypes[tindex]->Read_INI(ini);
	}
	for (int sindex = 0; sindex < SmudgeTypes.Count(); sindex++) {
		SmudgeTypes[sindex]->Read_INI(ini);
	}
	for (int oindex = 0; oindex < OverlayTypes.Count(); oindex++) {
		OverlayTypes[oindex]->Read_INI(ini);
	}
	for (int pindex = 0; pindex < ParticleTypes.Count(); pindex++) {
		ParticleTypes[pindex]->Read_INI(ini);
	}
	for (int psindex = 0; psindex < ParticleSystemTypes.Count(); psindex++) {
		ParticleSystemTypes[psindex]->Read_INI(ini);
	}
	for (int vindex = 0; vindex < VoxelAnimTypes.Count(); vindex++) {
		VoxelAnimTypes[vindex]->Read_INI(ini);
	}
	for (int mission = MISSION_FIRST; mission < MISSION_COUNT; mission++) {
		MissionControlClass * miss = &MissionControl[mission];
		miss->Mission = (MissionType)mission;
		miss->Read_INI(ini);
	}
	return(true);
}

// FUNCTION: GAMEMD 0x00679CC0
int FUN_00679CC0(void)
{
	if (!DAT_00B053B8) {
		CCINIClass ini;
		CCFileClass file("CoopCampMD.ini");
		if (ini.ReadCCFile(file, false, false)) {
			DAT_00B053B4 = ini.Get_Unique_ID();
		}
		DAT_00B053B8 = true;
	}
	return DAT_00B053B4;
}

// FUNCTION: GAMEMD 0x00679D90
int RulesClass::Get_Rule_Unique_ID(void)
{
	int id = RuleINI->Get_Unique_ID();
	int count = FUN_005D5ED0();
	for (int index = 1; index <= count; index++) {
		MultiplayerGameMode * mode = MultiplayerGameMode::Get(index);
		if (mode != NULL) {
			id += mode->FUN_005D67B0()->Get_Unique_ID();
		}
	}
	return id + FUN_00679CC0();
}

// FUNCTION: GAMEMD 0x00679EC0
int RulesClass::Get_Art_Unique_ID(void)
{
	return ArtINI.Get_Unique_ID();
}

// FUNCTION: GAMEMD 0x00679ED0
int RulesClass::Get_AI_Unique_ID(void)
{
	return AIINI.Get_Unique_ID();
}

// FUNCTION: GAMEMD 0x00679EE0
void RulesClass::Load_Art_INI(void)
{
	ArtINI.Clear();
	CCFileClass art("ARTMD.INI");
	ArtINI.ReadCCFile(art, false, false);
}

// TEMPLATE: GAMEMD 0x00679F70
// ??1?$VectorClass@H@@UAE@XZ
template class VectorClass<int>;

class BuildingTypeClass;
class UnitTypeClass;
class AircraftTypeClass;
class InfantryTypeClass;

// TEMPLATE: GAMEMD 0x00679FA0
// ??1?$VectorClass@PBVBuildingTypeClass@@@@UAE@XZ
// TEMPLATE: GAMEMD 0x0067B180
// ??4?$VectorClass@PBVBuildingTypeClass@@@@QAEAAV0@ABV0@@Z
template class VectorClass<BuildingTypeClass const *>;

// TEMPLATE: GAMEMD 0x00679FD0
// ??1?$VectorClass@PBVUnitTypeClass@@@@UAE@XZ
template class VectorClass<UnitTypeClass const *>;

// TEMPLATE: GAMEMD 0x0067A000
// ??1?$VectorClass@PBVAircraftTypeClass@@@@UAE@XZ
template class VectorClass<AircraftTypeClass const *>;

// TEMPLATE: GAMEMD 0x0067A030
// ??1?$VectorClass@PBVInfantryTypeClass@@@@UAE@XZ
template class VectorClass<InfantryTypeClass const *>;

class TerrainTypeClass;
class VoxelAnimTypeClass;

// TEMPLATE: GAMEMD 0x0067A060
// ??1?$VectorClass@PBVTerrainTypeClass@@@@UAE@XZ
template class VectorClass<TerrainTypeClass const *>;

// TEMPLATE: GAMEMD 0x0067A090
// ??4?$TypeList@H@@QAEAAV0@ABV0@@Z
template class TypeList<int>;

// TEMPLATE: GAMEMD 0x0067A110
// ??4?$TypeList@PBVBuildingTypeClass@@@@QAEAAV0@ABV0@@Z
template class TypeList<BuildingTypeClass const *>;

// TEMPLATE: GAMEMD 0x0067A190
// ??4?$TypeList@PBVUnitTypeClass@@@@QAEAAV0@ABV0@@Z
template class TypeList<UnitTypeClass const *>;

// TEMPLATE: GAMEMD 0x0067A210
// ??4?$TypeList@PBVAircraftTypeClass@@@@QAEAAV0@ABV0@@Z
template class TypeList<AircraftTypeClass const *>;

// TEMPLATE: GAMEMD 0x0067A290
// ??4?$TypeList@PBVInfantryTypeClass@@@@QAEAAV0@ABV0@@Z
template class TypeList<InfantryTypeClass const *>;

// TEMPLATE: GAMEMD 0x0067A310
// ??4?$TypeList@PBVTerrainTypeClass@@@@QAEAAV0@ABV0@@Z
template class TypeList<TerrainTypeClass const *>;

// TEMPLATE: GAMEMD 0x0067A390
// ??0?$TypeList@PBVVoxelAnimTypeClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x0067A3C0
// ?Save@?$TypeList@PBVVoxelAnimTypeClass@@@@QBEXPAUIStream@@@Z
// TEMPLATE: GAMEMD 0x0067A410
// ?Clear@?$DynamicVectorClass@PBVVoxelAnimTypeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0067A440
// ?ID@?$DynamicVectorClass@PBVVoxelAnimTypeClass@@@@UAEHPBQBVVoxelAnimTypeClass@@@Z
// TEMPLATE: GAMEMD 0x0067A460
// ?GetItem@?$VectorClass@PBVVoxelAnimTypeClass@@@@UBEPBVVoxelAnimTypeClass@@H@Z
template class TypeList<VoxelAnimTypeClass const *>;

class AnimTypeClass;
class SmudgeTypeClass;

// TEMPLATE: GAMEMD 0x0067A470
// ??0?$TypeList@PBVAnimTypeClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x0067A4A0
// ?Save@?$TypeList@PBVAnimTypeClass@@@@QBEXPAUIStream@@@Z
template class TypeList<AnimTypeClass const *>;

// TEMPLATE: GAMEMD 0x0067A4F0
// ??0?$TypeList@H@@QAE@XZ
// TEMPLATE: GAMEMD 0x0067A520
// ?Save@?$TypeList@H@@QBEXPAUIStream@@@Z
// TEMPLATE: GAMEMD 0x0067A570
// ??4?$DynamicVectorClass@H@@QAEAAV0@ABV0@@Z
template class DynamicVectorClass<int>;

// TEMPLATE: GAMEMD 0x0067A600
// ??0?$TypeList@PBVSmudgeTypeClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x0067A630
// ?Save@?$TypeList@PBVSmudgeTypeClass@@@@QBEXPAUIStream@@@Z
// TEMPLATE: GAMEMD 0x0067A680
// ?Clear@?$DynamicVectorClass@PBVSmudgeTypeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0067A6B0
// ?ID@?$DynamicVectorClass@PBVSmudgeTypeClass@@@@UAEHPBQBVSmudgeTypeClass@@@Z
// TEMPLATE: GAMEMD 0x0067A6D0
// ?GetItem@?$VectorClass@PBVSmudgeTypeClass@@@@UBEPBVSmudgeTypeClass@@H@Z
template class TypeList<SmudgeTypeClass const *>;

// TEMPLATE: GAMEMD 0x0067A6E0
// ?Save@?$TypeList@PBVBuildingTypeClass@@@@QBEXPAUIStream@@@Z
template class TypeList<BuildingTypeClass const *>;

// TEMPLATE: GAMEMD 0x0067A730
// ?Count@?$DynamicVectorClass@PBVBuildingTypeClass@@@@QBEHXZ
template class DynamicVectorClass<BuildingTypeClass const *>;

// TEMPLATE: GAMEMD 0x0067A740
// ??0?$TypeList@PBVUnitTypeClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x0067A770
// ?Save@?$TypeList@PBVUnitTypeClass@@@@QBEXPAUIStream@@@Z
template class TypeList<UnitTypeClass const *>;

// TEMPLATE: GAMEMD 0x0067A7C0
// ?Count@?$DynamicVectorClass@PBVUnitTypeClass@@@@QBEHXZ
template class DynamicVectorClass<UnitTypeClass const *>;

// TEMPLATE: GAMEMD 0x0067A7D0
// ?Save@?$TypeList@PBVAircraftTypeClass@@@@QBEXPAUIStream@@@Z
template class TypeList<AircraftTypeClass const *>;

// TEMPLATE: GAMEMD 0x0067A820
// ??0?$TypeList@PBVInfantryTypeClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x0067A850
// ?Save@?$TypeList@PBVInfantryTypeClass@@@@QBEXPAUIStream@@@Z
template class TypeList<InfantryTypeClass const *>;

// TEMPLATE: GAMEMD 0x0067A8A0
// ?Count@?$DynamicVectorClass@PBVInfantryTypeClass@@@@QBEHXZ
template class DynamicVectorClass<InfantryTypeClass const *>;

// TEMPLATE: GAMEMD 0x0067A8B0
// ??0?$TypeList@PBVTerrainTypeClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x0067A8E0
// ?Save@?$TypeList@PBVTerrainTypeClass@@@@QBEXPAUIStream@@@Z
// TEMPLATE: GAMEMD 0x0067A930
// ?Clear@?$DynamicVectorClass@PBVTerrainTypeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0067A960
// ?ID@?$DynamicVectorClass@PBVTerrainTypeClass@@@@UAEHPBQBVTerrainTypeClass@@@Z
// TEMPLATE: GAMEMD 0x0067A980
// ?GetItem@?$VectorClass@PBVTerrainTypeClass@@@@UBEPBVTerrainTypeClass@@H@Z
template class TypeList<TerrainTypeClass const *>;

// SYNTHETIC: GAMEMD 0x0067A990
// ??_G?$TypeList@PBVVoxelAnimTypeClass@@@@UAEPAXI@Z
// TEMPLATE: GAMEMD 0x0067C310
// ??0?$VectorClass@PBVVoxelAnimTypeClass@@@@QAE@HPBQBVVoxelAnimTypeClass@@@Z
template class VectorClass<VoxelAnimTypeClass const *>;

// SYNTHETIC: GAMEMD 0x0067A9E0
// ??_G?$TypeList@PBVSmudgeTypeClass@@@@UAEPAXI@Z
// TEMPLATE: GAMEMD 0x0067C3A0
// ??0?$VectorClass@PBVSmudgeTypeClass@@@@QAE@HPBQBVSmudgeTypeClass@@@Z
template class VectorClass<SmudgeTypeClass const *>;

// SYNTHETIC: GAMEMD 0x0067AA30
// ??_G?$TypeList@PBVTerrainTypeClass@@@@UAEPAXI@Z
template class TypeList<TerrainTypeClass const *>;

// TEMPLATE: GAMEMD 0x0067AA80
// ??4?$VectorClass@PBVVoxelAnimTypeClass@@@@QAEAAV0@ABV0@@Z
// TEMPLATE: GAMEMD 0x0067AAF0
// ??8?$VectorClass@PBVVoxelAnimTypeClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x0067AB30
// ?Resize@?$VectorClass@PBVVoxelAnimTypeClass@@@@UAE_NHPBQBVVoxelAnimTypeClass@@@Z
// TEMPLATE: GAMEMD 0x0067ABE0
// ?Clear@?$VectorClass@PBVVoxelAnimTypeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0067AC10
// ?ID@?$VectorClass@PBVVoxelAnimTypeClass@@@@UAEHABQBVVoxelAnimTypeClass@@@Z
// TEMPLATE: GAMEMD 0x0067AC50
// ?ID@?$VectorClass@PBVVoxelAnimTypeClass@@@@UAEHPBQBVVoxelAnimTypeClass@@@Z
// TEMPLATE: GAMEMD 0x0067AC70
// ??0?$DynamicVectorClass@PBVVoxelAnimTypeClass@@@@QAE@IPBQBVVoxelAnimTypeClass@@@Z
// TEMPLATE: GAMEMD 0x0067ACD0
// ?Resize@?$DynamicVectorClass@PBVVoxelAnimTypeClass@@@@UAE_NHPBQBVVoxelAnimTypeClass@@@Z
// TEMPLATE: GAMEMD 0x0067AD80
// ?Add@?$DynamicVectorClass@PBVVoxelAnimTypeClass@@@@QAE_NABQBVVoxelAnimTypeClass@@@Z
// TEMPLATE: GAMEMD 0x0067C1F0
// ??0?$TypeList@PBVVoxelAnimTypeClass@@@@QAE@ABV0@@Z
// TEMPLATE: GAMEMD 0x0067C360
// ?Delete_Index@?$DynamicVectorClass@PBVVoxelAnimTypeClass@@@@QAE_NH@Z
template class DynamicVectorClass<VoxelAnimTypeClass const *>;

// TEMPLATE: GAMEMD 0x0067ADD0
// ?ID@?$DynamicVectorClass@PBVVoxelAnimTypeClass@@@@UAEHABQBVVoxelAnimTypeClass@@@Z
// TEMPLATE: GAMEMD 0x0067AE00
// ??4?$VectorClass@PBVSmudgeTypeClass@@@@QAEAAV0@ABV0@@Z
// TEMPLATE: GAMEMD 0x0067AE70
// ??8?$VectorClass@PBVSmudgeTypeClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x0067AEB0
// ?Resize@?$VectorClass@PBVSmudgeTypeClass@@@@UAE_NHPBQBVSmudgeTypeClass@@@Z
// TEMPLATE: GAMEMD 0x0067AF60
// ?Clear@?$VectorClass@PBVSmudgeTypeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0067AF90
// ?ID@?$VectorClass@PBVSmudgeTypeClass@@@@UAEHABQBVSmudgeTypeClass@@@Z
// TEMPLATE: GAMEMD 0x0067AFD0
// ?ID@?$VectorClass@PBVSmudgeTypeClass@@@@UAEHPBQBVSmudgeTypeClass@@@Z
// TEMPLATE: GAMEMD 0x0067AFF0
// ??0?$DynamicVectorClass@PBVSmudgeTypeClass@@@@QAE@IPBQBVSmudgeTypeClass@@@Z
// TEMPLATE: GAMEMD 0x0067B050
// ?Resize@?$DynamicVectorClass@PBVSmudgeTypeClass@@@@UAE_NHPBQBVSmudgeTypeClass@@@Z
// TEMPLATE: GAMEMD 0x0067B100
// ?Add@?$DynamicVectorClass@PBVSmudgeTypeClass@@@@QAE_NABQBVSmudgeTypeClass@@@Z
// TEMPLATE: GAMEMD 0x0067B150
// ?ID@?$DynamicVectorClass@PBVSmudgeTypeClass@@@@UAEHABQBVSmudgeTypeClass@@@Z
// TEMPLATE: GAMEMD 0x0067C280
// ??0?$TypeList@PBVSmudgeTypeClass@@@@QAE@ABV0@@Z
// TEMPLATE: GAMEMD 0x0067C3F0
// ?Delete_Index@?$DynamicVectorClass@PBVSmudgeTypeClass@@@@QAE_NH@Z
template class DynamicVectorClass<SmudgeTypeClass const *>;

// TEMPLATE: GAMEMD 0x0067B1F0
// ??8?$VectorClass@PBVTerrainTypeClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x0067B230
// ?Resize@?$VectorClass@PBVTerrainTypeClass@@@@UAE_NHPBQBVTerrainTypeClass@@@Z
// TEMPLATE: GAMEMD 0x0067B2E0
// ?Clear@?$VectorClass@PBVTerrainTypeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0067B310
// ?ID@?$VectorClass@PBVTerrainTypeClass@@@@UAEHABQBVTerrainTypeClass@@@Z
// TEMPLATE: GAMEMD 0x0067B350
// ?ID@?$VectorClass@PBVTerrainTypeClass@@@@UAEHPBQBVTerrainTypeClass@@@Z
// TEMPLATE: GAMEMD 0x0067B370
// ??0?$DynamicVectorClass@PBVTerrainTypeClass@@@@QAE@IPBQBVTerrainTypeClass@@@Z
// TEMPLATE: GAMEMD 0x0067B3D0
// ?Resize@?$DynamicVectorClass@PBVTerrainTypeClass@@@@UAE_NHPBQBVTerrainTypeClass@@@Z
// TEMPLATE: GAMEMD 0x0067B480
// ?Add@?$DynamicVectorClass@PBVTerrainTypeClass@@@@QAE_NABQBVTerrainTypeClass@@@Z
// TEMPLATE: GAMEMD 0x0067B4D0
// ?ID@?$DynamicVectorClass@PBVTerrainTypeClass@@@@UAEHABQBVTerrainTypeClass@@@Z
template class DynamicVectorClass<TerrainTypeClass const *>;

// FUNCTION: GAMEMD 0x0067B500
// TGet_Class
template WarheadTypeClass * TGet_Class<WarheadTypeClass>(CCINIClass const &, char const *, char const *, WarheadTypeClass *);

// FUNCTION: GAMEMD 0x0067BAC0
// TGet_Class@@YIPAVInfantryTypeClass
template InfantryTypeClass * TGet_Class<InfantryTypeClass>(CCINIClass const &, char const *, char const *, InfantryTypeClass *);

// FUNCTION: GAMEMD 0x0067BCE0
// TGet_Class@@YIPAVBuildingTypeClass
template BuildingTypeClass * TGet_Class<BuildingTypeClass>(CCINIClass const &, char const *, char const *, BuildingTypeClass *);

// FUNCTION: GAMEMD 0x0067BD30
// TGet_Class@@YIPAVAircraftTypeClass
template AircraftTypeClass * TGet_Class<AircraftTypeClass>(CCINIClass const &, char const *, char const *, AircraftTypeClass *);

// FUNCTION: GAMEMD 0x0067BD80
// TGet_Class@@YIPAVTerrainTypeClass
template TerrainTypeClass * TGet_Class<TerrainTypeClass>(CCINIClass const &, char const *, char const *, TerrainTypeClass *);

// TEMPLATE: GAMEMD 0x0067BFA0
// ??4?$VectorClass@PBVTerrainTypeClass@@@@QAEAAV0@ABV0@@Z

// SYNTHETIC: GAMEMD 0x0067C010
// ??_G?$VectorClass@PBVVoxelAnimTypeClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x0067C060
// ??_G?$DynamicVectorClass@PBVVoxelAnimTypeClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x0067C0B0
// ??_G?$VectorClass@PBVSmudgeTypeClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x0067C100
// ??_G?$DynamicVectorClass@PBVSmudgeTypeClass@@@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0067C150
// ??_G?$VectorClass@PBVTerrainTypeClass@@@@UAEPAXI@Z
// TEMPLATE: GAMEMD 0x0067C430
// ??0?$VectorClass@PBVTerrainTypeClass@@@@QAE@HPBQBVTerrainTypeClass@@@Z
template class VectorClass<TerrainTypeClass const *>;

// SYNTHETIC: GAMEMD 0x0067C1A0
// ??_G?$DynamicVectorClass@PBVTerrainTypeClass@@@@UAEPAXI@Z
template class DynamicVectorClass<TerrainTypeClass const *>;

DynamicVectorClass<VoxelAnimTypeClass const *> * Copy_Voxel_Vector(DynamicVectorClass<VoxelAnimTypeClass const *> const & src)
{
	return new DynamicVectorClass<VoxelAnimTypeClass const *>(src);
}

DynamicVectorClass<SmudgeTypeClass const *> * Copy_Smudge_Vector(DynamicVectorClass<SmudgeTypeClass const *> const & src)
{
	return new DynamicVectorClass<SmudgeTypeClass const *>(src);
}
