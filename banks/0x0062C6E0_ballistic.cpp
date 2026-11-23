// ParticleClass::Spark_Behavior_AI MATCH (1072/1072) with overlay 0x0062C6E0_ballistic.h (Ballistic_Move gains a TechnoClass * payback parameter; bounce.cpp and Gas_Behavior_AI pass NULL) appended after Gas_Behavior_AI in src/particle.cpp; checkall 19258/19258 exit 0 with it. Held: the payback checks are dead at every float caller (the Is_Ally check alone flips Spark; the early payback return keeps BounceClass::AI's call targets), backed only by bullet AI 0x004666E0's double twin.
// FUNCTION: GAMEMD 0x0062C6E0
void ParticleClass::Spark_Behavior_AI(void)
{
	Coord coord = PositionCoord;
	MovementDirection.Z -= Rule->Gravity;
	Vector3 velocity(MovementDirection.X, MovementDirection.Y, MovementDirection.Z);
	Vector3 position(coord.X, coord.Y, coord.Z);

	if (Ballistic_Move(position, velocity, Rule->Gravity, 1.0, 0.0, NULL)) {
		IsToDie = true;
	}

	Set_Coord(Coord(position.X, position.Y, position.Z));
	ColorAccum += Class->ColorSpeed + Random_Double(0.0, 0.05);
	if (ColorAccum > 1.0) {
		if (ColorIndex < Class->ColorList.Count() - 2) {
			ColorIndex++;
			ColorAccum = 0;
		} else {
			ColorAccum = 1.0;
		}
	}
}
