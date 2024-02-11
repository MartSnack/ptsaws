#include <iostream>
#include <map>
#include <string>
#include <cmath>
#include "battle_effects.h"

#include "battle.h"
// 11.9.23
// What you were working on: Getting last_waza_kind working by having the
// pokeclients keep track of their last used move
// you looked through the game's code and prevMove does not get reset
// if you use an item.  It only occurs when you use a move.
// Switching and using items do not update the previous move
// unless it is reset to NONE somewhere else, I don't think it ever gets reset
// until you make another move even if you've switched pokemon
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

PokeInfo::PokeInfo(std::string _name = "None", int _hp = 0, int _atk = 0, int _def = 0, int _spAtk = 0, int _spDef = 0, int _spd = 0, Type _primaryType = Type::None, Type _secondaryType = Type::None, AbilityId _ability = NO_ABILITY, AbilityId _ability2 = NO_ABILITY ){
    name = _name;
    hp = _hp;
    atk = _atk;
    def = _def;
    spAtk = _spAtk;
    spDef = _spDef;
    spd = _spd;
    primaryType = _primaryType;
    secondaryType = _secondaryType;
    ability = _ability;
    ability2 = _ability2;
}
// id, power, accuracy, secondary?, secondaryAccuracy, MoveEffect, Move Type, DamageType attack, DamageType defend, priority, HighCritRatio?
/*
Move(MoveId _id, std::string _name, int _power = 0, int _accuracy = 0, bool _secondary = false, int _secondaryAccuracy = 0, int _effect = 0, Type _moveType = None, DamageType _defend = DamageType::NONE, DamageType _attack = DamageType::NONE, int _priority = 0, bool _highCritRatio = false, int _range = 0)
*/
Move Empty = Move();
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
// TODO: Implement spark (and paralysis)

Nature hardy = {100,100,100,100,100};
Nature docile = {100,100,100,100,100};
Nature serious = {100,100,100,100,100};
Nature bashful = {100,100,100,100,100};
Nature quirky = {100,100,100,100,100};
Nature lonely = {110,90,100,100,100};
Nature brave = {110,100,100,100,90};
Nature adamant = {110,100,90,100,100};
Nature naughty = {110,100,100,110,100};
Nature bold = {90,110,100,100,100};
Nature relaxed = {100,110,100,100,90};
Nature impish = {100,110,90,100,100};
Nature lax = {100,110,100,90,100};
Nature timid = {90,100,100,100,110};
Nature hasty = {100,90,100,100,110};
Nature jolly = {100,100,90,100,110};
Nature naive = {100,100,100,90,110};
Nature modest = {90,100,110,100,100};
Nature mild = {100,90,110,100,100};
Nature quiet = {100,100,110,100,90};
Nature rash = {100,100,110,90,100};
Nature calm = {90,100,100,110,100};
Nature gentle = {100,90,100,110,100};
Nature sassy = {100,100,100,110,90};
Nature careful = {100,100,90,110,100};

std::map<Mons, PokeInfo> pokemap = {
    {MONFERNO, PokeInfo("Monferno", 64,78,52,78,52,81,Type::Fire,Type::Fighting,BLAZE)},
    {SKUNTANK, PokeInfo("Skuntank", 103,93,67,71,61,84,Type::Poison, Type::Dark, STENCH, AFTERMATH)},
    {ZUBAT, PokeInfo("Zubat", 40,45,35,30,40,55,Type::Poison,Type::Flying,INNER_FOCUS)},
    {STARAVIA, PokeInfo("Staravia", 55,75,50,40,40,80, Type::Flying, Type::Normal, INTIMIDATE)},
    {PACHIRISU, PokeInfo("Pachirisu", 60,45,70,45,90,95, Type::Electric, Type::None, RUN_AWAY)}, // pickup as well, but who cares
};


// PokeInfo pokemap[1] = {
//     PokeInfo(64,78,52,78,52,81,Fire,Fighting)
// };
int getStatValue(int base, int iv, int ev, int level, int nature) {
    int evBase = ev/4;
    int statBase = ((2*base + iv + evBase) * level)/100;
    int result = ((statBase + 5) * nature)/100;

    return result;
}
void Pokemon::setEvs(int hp, int atk, int def, int spAtk, int spDef, int spd) {
    hpEv = hp;
    atkEv = atk;
    defEv = def;
    spAtkEv = spAtk;
    spDefEv = spDef;
    spdEv = spd;
}
void Pokemon::calcStats() {
    int evHp = hpEv/4;
    int hpBase = ((2*info.hp + hpIv + evHp)*level)/100;
    cHp = hpBase + level + 10;
    cAtk = getStatValue(info.atk, atkIv, atkEv, level, nature.val[0]);
    cDef = getStatValue(info.def, defIv, defEv, level, nature.val[1]);
    cSpAtk = getStatValue(info.spAtk, spAtkIv, spAtkEv, level, nature.val[2]);
    cSpDef = getStatValue(info.spDef, spDefIv, spDefEv, level, nature.val[3]);
    cSpd = getStatValue(info.spd, spdIv, spdEv, level, nature.val[4]);
    bVal.bHp = cHp;

}
Pokemon::Pokemon(){
    level = 0;
    bVal.bHp = 0;
}
Pokemon::Pokemon(int _lvl, Nature _nature, int _ivs, Mons _id, Move _moveset[4], int _ability = 1){
    level = _lvl;
    nature = _nature;
    hpIv =_ivs;
    atkIv = _ivs;
    defIv = _ivs;
    spAtkIv = _ivs;
    spDefIv = _ivs;
    spdIv = _ivs;
    moveset[0] = _moveset[0];
    moveset[1] = _moveset[1];
    moveset[2] = _moveset[2];
    moveset[3] = _moveset[3];
    setEvs(0,0,0,0,0,0);
    info = pokemap.at(_id);
    if(_ability == 1) {
        cAbility = info.ability;
    } else {
        cAbility = info.ability2;
    }
    calcStats();
    bVal.testStatus = 0;
    bVal.condition = 0;
    bVal.turnsTaunted = 0;
    bVal.moveEffectsMask = 0;
    bVal.abilityKnownToAi = false;
    bVal.item = ITEM_NONE;
}
void Pokemon::sendOut() {
    bVal.atkStg = 6;
    bVal.defStg = 6;
    bVal.spAtkStg = 6;
    bVal.spDefStg = 6;
    bVal.spdStg = 6;
    bVal.evaStg = 6;
    bVal.accStg = 6;
    bVal.critStg = 0;
    bVal.volConditions = 0;
    bVal.turnsTaunted = 0;
    bVal.moveEffectsMask = 0;
}
void Pokemon::setBHp(int hp) {
    bVal.bHp = hp;
}
void setupClient(PokeClient *pc) {
    pc->previouslySelectedMove = Empty;
}
// Pokemon::~Pokemon(){

// }
// Move::~Move() {
    
// }
BattleContext setupJupiterFight(unsigned long startingSeed) {
    Move moveset1[4] = {Tackle, FlameWheel, Taunt, Tackle};	
    Move moveset2[4] = {NightSlash,PoisonGas, Screech, Smokescreen};
    Move moveset3[4] = {Tackle, Tackle, Tackle, Tackle}; // zubat's moveset doesn't matter, it gets OHKO'd anyway
    Pokemon skuntank = Pokemon(23, modest, 12, SKUNTANK, moveset2);
    Pokemon monferno = Pokemon(33, gentle, 0, MONFERNO, moveset1);
    Pokemon zubat = Pokemon(21,brave,12,ZUBAT,moveset3);
    skuntank.bVal.item = ITEM_SITRUS_BERRY;
    monferno.setEvs(36,8,16,28,4,64);
    monferno.calcStats(); // recalc stats after setting evs
    // monferno.bVal.item = ITEM_ORAN_BERRY;
    // monferno.bVal.item = ITEM_PECHA_BERRY;
    PokeClient p1;
    p1.battler = 0;
    PokeClient p2;
    p2.aiControl = true;
    p2.aiLevel = 7;
    p2.battler = 0;
    p2.team[1] = skuntank;
    p2.team[0] = zubat;
    p1.team[0] = monferno;
    p1.name = "Player";
    p2.name = "AI";
    RngSeed rs = {startingSeed,0};
    p1.pokeSwitch(p1.battler);
    p2.pokeSwitch(p2.battler); 
    BattleContext bc = {Weather::None, rs, p1, p2, false};

    // p1.team[p1.battler].sendOut();
    // p2.team[p2.battler].sendOut();
    bc.turnNumber = 0;
    return bc;
}

BattleContext setupVarFight(unsigned long startingSeed) {
    Move moveset1[4] = {Tackle, FlameWheel, Taunt, Tackle};	
    Move moveset2[4] = {QuickAttack, WingAttack, DoubleTeam, Endeavor};
    Pokemon monferno = Pokemon(33, gentle, 0, MONFERNO, moveset1);
    monferno.setEvs(36,8,16,28,4,64);
    monferno.calcStats(); // recalc stats after setting evs

    Pokemon stara = Pokemon(21, serious, 0, STARAVIA, moveset2);

    PokeClient p1;
    PokeClient p2;
    p2.aiControl = true;
    p2.aiLevel = 1;
    p2.battler = 0;
    p1.battler = 0;
    p1.team[0] = monferno;
    p2.team[0] = stara;
    p1.name = "Player";
    p2.name = "AI";
    p1.pokeSwitch(p1.battler);
    p2.pokeSwitch(p2.battler);
    // p1.team[p1.battler].sendOut();
    // p2.team[p2.battler].sendOut();
    RngSeed rs = {startingSeed, 0};
    BattleContext bc = {Weather::None, rs, p1, p2, false};
    bc.turnNumber = 0;
    return bc;
}
// int main() {
//     BattleContext bc = setupJupiterFight(6565656);
//     bc.attacker.command = COMMAND_MOVE_SLOT_2;
//     bc.defender.command = COMMAND_MOVE_SLOT_1;

//     // bc.attacker = monferno;
//     // bc.defender = monferno2;
//     // bc.battleRng = {5,5};
//     bool shouldContinue = true;
//     int i = 0;
//     while(shouldContinue) {
//         shouldContinue = doTurn(&bc);
//         i++;
//         if(i > 25) {
//             shouldContinue = false;
//         }
//     }
//     int burned = bc.defender.team[bc.defender.battler].bVal.condition & MON_CONDITION_POISON;
//     std::cout << "Battler Defender: " << bc.defender.team[bc.defender.battler].info.name << " Current HP: " << bc.defender.team[bc.defender.battler].bVal.bHp << " Status: " << bc.defender.team[bc.defender.battler].bVal.condition << std::endl;
//     std::cout << "Battler Attacker: " << bc.attacker.team[bc.attacker.battler].info.name << " Current HP: " << bc.attacker.team[bc.attacker.battler].bVal.bHp << " Status: " << bc.attacker.team[bc.attacker.battler].bVal.condition << std::endl;
//     std::cout << "Ending Seed: " << bc.battleRng.seed << std::endl;
//     if(bc.attacker.team[bc.attacker.battler].bVal.condition & MON_CONDITION_BURN) {
//         std::cout << "burned" << std::endl;
//     }
//     // std::cout << bc.defender.team[bc->defender.battler].bVal.bHp << std::endl;
//     return 0;
// };