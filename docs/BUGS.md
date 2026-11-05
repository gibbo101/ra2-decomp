# Bugs in the original game

The source reproduces the game as shipped, bugs included, so these stay until the
decompilation is finished. Each entry gives the address, what the original does, and its
effect, so it can be fixed in a separate build later.

- **HouseClass::AI_Building (0x004FE3E0).** When the AI adds a power plant ahead of a
  building that needs more power, a house whose ActLike is neither 0 nor 2 fills the
  owned list for its prerequisite check with BuildingClass instances instead of their
  types. An instance never equals a type, so the check fails whenever NodAdvancedPower
  has a prerequisite, and the AI always picks NodRegularPower.
