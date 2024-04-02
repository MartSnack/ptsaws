#include "moves.h"
#include "battle_effects.h"
#include <string>

Move::Move() {
    id = MoveId::NO_MOVE;
}
Move::Move(MoveId _id, std::string _name, int _power = 0, int _accuracy = 0, bool _secondary = false, int _secondaryAccuracy = 0, int _effect = 0, Type _moveType = Type::None, DamageType _defend = DamageType::NONE, DamageType _attack = DamageType::NONE, int _priority = 0, bool _highCritRatio = false, int _range = RANGE_TARGET) {
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
}


Move Empty = Move();
Move Struggle = Move(MoveId::STRUGGLE, "Struggle", 40, 100, false, 0, 0, Type::None, DamageType::PHYSICAL, DamageType::PHYSICAL);
Move Tackle = Move(MoveId::TACKLE, "Tackle", 35,100,false,0, 0, Type::Normal, DamageType::PHYSICAL, DamageType::PHYSICAL);
Move FlameWheel = Move(MoveId::FLAME_WHEEL, "Flame Wheel", 60, 100, true, 10, BATTLE_EFFECT_BURN_HIT, Type::Fire, DamageType::PHYSICAL, DamageType::PHYSICAL);
Move Taunt = Move(MoveId::TAUNT, "Taunt", 0, 100, false, 0, BATTLE_EFFECT_TAUNT, Type::Dark, DamageType::PHYSICAL, DamageType::PHYSICAL);
Move NightSlash = Move(MoveId::NIGHT_SLASH, "Night Slash", 70, 100, false, 0, BATTLE_EFFECT_HIGH_CRITICAL, Type::Dark, DamageType::PHYSICAL, DamageType::PHYSICAL, 0, true);
Move PoisonGas = Move(MoveId::POISON_GAS, "Poison Gas", 0, 55, false, 0, BATTLE_EFFECT_STATUS_POISON, Type::Poison);
Move Screech = Move(MoveId::SCREECH, "Screech", 0, 85, false, 0, BATTLE_EFFECT_DEF_DOWN_2, Type::Normal);
Move Smokescreen = Move(MoveId::SMOKESCREEN, "SmokeScreen", 0, 100, false, 0, BATTLE_EFFECT_ACC_DOWN, Type::Normal);
Move QuickAttack = Move(MoveId::QUICK_ATTACK, "Quick Attack", 40, 100, false, 0, BATTLE_EFFECT_PRIORITY_1, Type::Normal, DamageType::PHYSICAL, DamageType::PHYSICAL, 1);
Move WingAttack = Move(MoveId::WING_ATTACK, "Wing Attack", 60,100,false,0, 0, Type::Flying, DamageType::PHYSICAL, DamageType::PHYSICAL);
Move DoubleTeam = Move(MoveId::DOUBLE_TEAM, "Double Team", 0, 0, false, 0, BATTLE_EFFECT_EVA_UP, Type::Normal, DamageType::NONE, DamageType::NONE, 0, false, RANGE_SELF);
Move Endeavor = Move(MoveId::ENDEAVOR, "Endeavor", 1, 100, false, 0, BATTLE_EFFECT_SET_HP_EQUAL_TO_USER, Type::Normal, DamageType::PHYSICAL, DamageType::PHYSICAL);
Move Charm = Move(MoveId::CHARM, "Charm", 0, 100, false, 0, BATTLE_EFFECT_ATK_DOWN_2, Type::Normal);
Move Spark = Move(MoveId::SPARK, "Spark", 65, 100, true, 30, BATTLE_EFFECT_PARALYZE_HIT, Type::Electric, DamageType::PHYSICAL, DamageType::PHYSICAL);
Move ConfuseRay = Move(MoveId::CONFUSE_RAY, "Confuse Ray", 0, 100, false, 0, BATTLE_EFFECT_STATUS_CONFUSE, Type::Ghost);
Move Psybeam = Move(MoveId::PSYBEAM, "Psybeam", 65, 100, true, 10, BATTLE_EFFECT_CONFUSE_HIT, Type::Psychic, DamageType::SPECIAL, DamageType::SPECIAL);
Move MagicalLeaf = Move(MoveId::MAGICAL_LEAF, "Magical Leaf", 60, 0, false, 0, BATTLE_EFFECT_BYPASS_ACCURACY, Type::Grass, DamageType::SPECIAL, DamageType::SPECIAL);
Move ShadowBall = Move(MoveId::SHADOW_BALL, "Shadow Ball", 80, 100, true, 20, BATTLE_EFFECT_LOWER_SP_DEF_HIT, Type::Ghost, DamageType::SPECIAL, DamageType::SPECIAL);
Move Bite = Move(MoveId::BITE, "Bite", 60, 100, true, 30, BATTLE_EFFECT_FLINCH_HIT, Type::Dark, DamageType::PHYSICAL, DamageType::PHYSICAL);
Move Substitute = Move(MoveId::SUBSTITUTE, "Substitute", 0, 0, false, 0, BATTLE_EFFECT_SET_SUBSTITUTE, Type::Normal, DamageType::NONE, DamageType::NONE, 0, false, RANGE_SELF);