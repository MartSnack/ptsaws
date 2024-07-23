#include "moves.h"
#include "battle_effects.h"
#include <string>

Move::Move() {
    id = MoveId::NO_MOVE;
}
Move::Move(MoveId _id, std::string _name, int _flags, int _power = 0, int _accuracy = 0, bool _secondary = false, int _secondaryAccuracy = 0, int _effect = 0, Type _moveType = Type::None, DamageType _defend = DamageType::NONE, DamageType _attack = DamageType::NONE, int _priority = 0, bool _highCritRatio = false, int _range = RANGE_TARGET) {
    id = _id;
    name = _name;
    power = _power;
    accuracy = _accuracy;
    secondary = _secondary;
    secondaryAccuracy = _secondaryAccuracy;
    effect = _effect;
    moveType = _moveType;
    attack = _attack;
    defend = _defend;
    priority = _priority;
    highCritRatio = _highCritRatio;
    disabled = false;
    range = _range;
    flags = _flags;
}


Move Empty = Move();
Move Struggle = Move(MoveId::STRUGGLE, "Struggle", FLAG_CONTACT_PROTECT, 40, 100, false, 0, 0, Type::None, DamageType::PHYSICAL, DamageType::PHYSICAL);
Move Tackle = Move(MoveId::TACKLE, "Tackle", FLAG_CONTACT_PROTECT, 35,100,false,0, 0, Type::Normal, DamageType::PHYSICAL, DamageType::PHYSICAL);
Move FlameWheel = Move(MoveId::FLAME_WHEEL, "Flame Wheel", FLAG_CONTACT_PROTECT, 60, 100, true, 10, BATTLE_EFFECT_BURN_HIT, Type::Fire, DamageType::PHYSICAL, DamageType::PHYSICAL);
Move Taunt = Move(MoveId::TAUNT, "Taunt", FLAG_CAN_PROTECT, 0, 100, false, 0, BATTLE_EFFECT_TAUNT, Type::Dark, DamageType::PHYSICAL, DamageType::PHYSICAL);
Move NightSlash = Move(MoveId::NIGHT_SLASH, "Night Slash", FLAG_CONTACT_PROTECT, 70, 100, false, 0, BATTLE_EFFECT_HIGH_CRITICAL, Type::Dark, DamageType::PHYSICAL, DamageType::PHYSICAL, 0, true);
Move PoisonGas = Move(MoveId::POISON_GAS, "Poison Gas", 0, 55, false, 0, BATTLE_EFFECT_STATUS_POISON, Type::Poison);
Move Screech = Move(MoveId::SCREECH, "Screech", 0, 85, false, 0, BATTLE_EFFECT_DEF_DOWN_2, Type::Normal);
Move Smokescreen = Move(MoveId::SMOKESCREEN, "SmokeScreen", 0, 100, false, 0, BATTLE_EFFECT_ACC_DOWN, Type::Normal);
Move QuickAttack = Move(MoveId::QUICK_ATTACK, "Quick Attack",FLAG_CONTACT_PROTECT, 40, 100, false, 0, BATTLE_EFFECT_PRIORITY_1, Type::Normal, DamageType::PHYSICAL, DamageType::PHYSICAL, 1);
Move WingAttack = Move(MoveId::WING_ATTACK, "Wing Attack",FLAG_CONTACT_PROTECT, 60,100,false,0, 0, Type::Flying, DamageType::PHYSICAL, DamageType::PHYSICAL);
Move DoubleTeam = Move(MoveId::DOUBLE_TEAM, "Double Team",0, 0, 0, false, 0, BATTLE_EFFECT_EVA_UP, Type::Normal, DamageType::NONE, DamageType::NONE, 0, false, RANGE_SELF);
Move Endeavor = Move(MoveId::ENDEAVOR, "Endeavor",FLAG_CONTACT_PROTECT, 1, 100, false, 0, BATTLE_EFFECT_SET_HP_EQUAL_TO_USER, Type::Normal, DamageType::PHYSICAL, DamageType::PHYSICAL);
Move Charm = Move(MoveId::CHARM, "Charm",FLAG_CAN_PROTECT, 0, 100, false, 0, BATTLE_EFFECT_ATK_DOWN_2, Type::Normal);
Move Spark = Move(MoveId::SPARK, "Spark",FLAG_CONTACT_PROTECT, 65, 100, true, 30, BATTLE_EFFECT_PARALYZE_HIT, Type::Electric, DamageType::PHYSICAL, DamageType::PHYSICAL);
Move ConfuseRay = Move(MoveId::CONFUSE_RAY, "Confuse Ray",FLAG_CAN_PROTECT, 0, 100, false, 0, BATTLE_EFFECT_STATUS_CONFUSE, Type::Ghost);
Move Psybeam = Move(MoveId::PSYBEAM, "Psybeam",FLAG_CAN_PROTECT, 65, 100, true, 10, BATTLE_EFFECT_CONFUSE_HIT, Type::Psychic, DamageType::SPECIAL, DamageType::SPECIAL);
Move MagicalLeaf = Move(MoveId::MAGICAL_LEAF, "Magical Leaf",FLAG_CAN_PROTECT, 60, 0, false, 0, BATTLE_EFFECT_BYPASS_ACCURACY, Type::Grass, DamageType::SPECIAL, DamageType::SPECIAL);
Move ShadowBall = Move(MoveId::SHADOW_BALL, "Shadow Ball",FLAG_CAN_PROTECT, 80, 100, true, 20, BATTLE_EFFECT_LOWER_SP_DEF_HIT, Type::Ghost, DamageType::SPECIAL, DamageType::SPECIAL);
Move Bite = Move(MoveId::BITE, "Bite",FLAG_CONTACT_PROTECT, 60, 100, true, 30, BATTLE_EFFECT_FLINCH_HIT, Type::Dark, DamageType::PHYSICAL, DamageType::PHYSICAL);
Move Substitute = Move(MoveId::SUBSTITUTE, "Substitute",0, 0, 0, false, 0, BATTLE_EFFECT_SET_SUBSTITUTE, Type::Normal, DamageType::NONE, DamageType::NONE, 0, false, RANGE_SELF);
Move SandAttack = Move(MoveId::SAND_ATTACK, "Sand Attack",FLAG_CAN_PROTECT, 0, 100, false, 0, BATTLE_EFFECT_ACC_DOWN, Type::Ground);
Move TakeDown = Move(MoveId::TAKE_DOWN, "Take Down",FLAG_CONTACT_PROTECT, 90, 85, false, 0, BATTLE_EFFECT_RECOIL_QUARTER, Type::Normal, DamageType::PHYSICAL, DamageType::PHYSICAL);
Move Yawn = Move(MoveId::YAWN, "Yawn",FLAG_CAN_PROTECT, 0,0, false, 0, BATTLE_EFFECT_STATUS_SLEEP_NEXT_TURN, Type::Normal, DamageType::NONE, DamageType::NONE);
Move Toxic = Move(MoveId::TOXIC, "Toxic",FLAG_CAN_PROTECT, 0, 85, false, 0, BATTLE_EFFECT_STATUS_BADLY_POISON, Type::Poison);
Move Supersonic = Move(MoveId::SUPERSONIC, "Supersonic",FLAG_CAN_PROTECT, 0, 55, false, 0, BATTLE_EFFECT_STATUS_CONFUSE, Type::Normal);
Move AirCutter = Move(MoveId::AIR_CUTTER, "Air Cutter",FLAG_CONTACT_PROTECT, 55, 95, false, 0, BATTLE_EFFECT_HIGH_CRITICAL, Type::Flying, DamageType::SPECIAL, DamageType::SPECIAL, 0, true);
Move Surf = Move(MoveId::SURF, "Surf",FLAG_CAN_PROTECT, 95,100,false,0, 0, Type::Water, DamageType::SPECIAL, DamageType::SPECIAL);
Move Revenge = Move(MoveId::REVENGE, "Revenge",FLAG_CONTACT_PROTECT, 3, 100, false, 0, BATTLE_EFFECT_DOUBLE_POWER_IF_HIT, Type::Fighting, DamageType::PHYSICAL, DamageType::PHYSICAL, -4);
Move PoisonJab = Move(MoveId::POISON_JAB, "Poison Jab",FLAG_CONTACT_PROTECT, 80, 100, true, 30, BATTLE_EFFECT_POISON_HIT, Type::Poison, DamageType::PHYSICAL, DamageType::PHYSICAL);
Move FaintAttack = Move(MoveId::FAINT_ATTACK, "Faint Attack",FLAG_CONTACT_PROTECT, 60, 0, false, 0, BATTLE_EFFECT_BYPASS_ACCURACY, Type::Dark, DamageType::PHYSICAL, DamageType::PHYSICAL);
Move MudBomb = Move(MoveId::MUD_BOMB, "Mud Bomb",FLAG_CAN_PROTECT, 65, 85, true, 30, BATTLE_EFFECT_LOWER_ACCURACY_HIT, Type::Ground, DamageType::SPECIAL, DamageType::SPECIAL);
Move Earthquake = Move(MoveId::EARTHQUAKE, "Earthquake", FLAG_CAN_PROTECT, 100, 100, false, 0, 0, Type::Ground, DamageType::PHYSICAL, DamageType::PHYSICAL);
Move ChargeBeam = Move(MoveId::CHARGE_BEAM, "Charge Beam",FLAG_CAN_PROTECT, 50, 90, true, 70, BATTLE_EFFECT_RAISE_SP_ATK_HIT, Type::Electric, DamageType::SPECIAL, DamageType::SPECIAL);
Move ThunderWave = Move(MoveId::THUNDER_WAVE, "Thunder Wave",FLAG_CAN_PROTECT, 0, 100, false, 0, BATTLE_EFFECT_STATUS_PARALYZE, Type::Electric);
Move IronTail = Move(MoveId::IRON_TAIL, "Iron Tail",FLAG_CONTACT_PROTECT, 100, 75, true, 30, BATTLE_EFFECT_LOWER_DEFENSE_HIT, Type::Steel, DamageType::PHYSICAL, DamageType::PHYSICAL);
Move SignalBeam = Move(MoveId::SIGNAL_BEAM, "Signal Beam", FLAG_CAN_PROTECT, 75, 100, true, 10, BATTLE_EFFECT_CONFUSE_HIT, Type::Bug, DamageType::SPECIAL, DamageType::SPECIAL);
Move FocusBlast = Move(MoveId::FOCUS_BLAST, "Focus Blast", FLAG_CAN_PROTECT, 120, 70, true, 10, BATTLE_EFFECT_LOWER_SP_DEF_HIT, Type::Fighting, DamageType::SPECIAL, DamageType::SPECIAL, 0);
Move ThunderFang = Move(MoveId::THUNDER_FANG, "Thunder Fang", FLAG_CONTACT_PROTECT, 65, 95, false, 0, BATTLE_EFFECT_FLINCH_PARALYZE_HIT, Type::Electric, DamageType::PHYSICAL, DamageType::PHYSICAL, 0);
Move FireFang = Move(MoveId::FIRE_FANG, "Fire Fang", FLAG_CONTACT_PROTECT, 65, 95, false, 0, BATTLE_EFFECT_FLINCH_BURN_HIT, Type::Fire, DamageType::PHYSICAL, DamageType::PHYSICAL, 0);
Move IceFang = Move(MoveId::ICE_FANG, "Ice Fang", FLAG_CONTACT_PROTECT, 65, 95, false, 0, BATTLE_EFFECT_FLINCH_FREEZE_HIT, Type::Ice, DamageType::PHYSICAL, DamageType::PHYSICAL, 0);
Move Crunch = Move(MoveId::CRUNCH, "Crunch", FLAG_CONTACT_PROTECT, 80, 100, true, 20, BATTLE_EFFECT_LOWER_DEFENSE_HIT, Type::Dark, DamageType::PHYSICAL, DamageType::PHYSICAL, 0);
Move ThunderPunch = Move(MoveId::THUNDERPUNCH, "ThunderPunch", FLAG_CONTACT_PROTECT, 75, 100, true, 10, BATTLE_EFFECT_PARALYZE_HIT, Type::Electric, DamageType::PHYSICAL, DamageType::PHYSICAL, 0);
Move FirePunch = Move(MoveId::FIRE_PUNCH, "Fire Punch", FLAG_CONTACT_PROTECT, 75, 100, true, 10, BATTLE_EFFECT_BURN_HIT, Type::Fire, DamageType::PHYSICAL, DamageType::PHYSICAL, 0);
Move GigaImpact = Move(MoveId::GIGA_IMPACT, "Giga Impact", FLAG_CONTACT_PROTECT, 150, 90, false, 0, BATTLE_EFFECT_RECHARGE_AFTER, Type::Normal, DamageType::PHYSICAL, DamageType::PHYSICAL, 0);
Move Protect = Move(MoveId::PROTECT, "Protect", 0, 0, 0, false, 0, BATTLE_EFFECT_PROTECT, Type::Normal, DamageType::NONE, DamageType::NONE, 3, false, RANGE_SELF);
Move SuckerPunch = Move(MoveId::SUCKER_PUNCH, "Sucker Punch", FLAG_CONTACT_PROTECT, 80, 100, false, 0, BATTLE_EFFECT_HIT_FIRST_IF_TARGET_ATTACKING, Type::Dark, DamageType::PHYSICAL, DamageType::PHYSICAL, 1);
Move ToxicSpikes = Move(MoveId::TOXIC_SPIKES, "Toxic Spikes", 0, 0, 0, false, 0, BATTLE_EFFECT_TOXIC_SPIKES, Type::Poison, DamageType::NONE, DamageType::NONE, 0);
