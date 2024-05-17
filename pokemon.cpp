#include <iostream>
#include <map>
#include <string>
#include <cmath>
#include "battle_effects.h"
#include "moves.h"
#include "battle.h"
// 11.9.23
// What you were working on: Getting last_waza_kind working by having the
// pokeclients keep track of their last used move
// you looked through the game's code and prevMove does not get reset
// if you use an item.  It only occurs when you use a move.
// Switching and using items do not update the previous move
// unless it is reset to NONE somewhere else, I don't think it ever gets reset
// until you make another move even if you've switched pokemon

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
    {PACHIRISU, PokeInfo("Pachirisu", 60,45,70,45,90,95, Type::Electric, Type::None, RUN_AWAY)}, // pickup as well, but who cares we can't catch one anyway
    {STARLY, PokeInfo("Starly", 40, 55, 30, 30, 30, 60, Type::Normal, Type::Flying, KEEN_EYE)},
    {MISMAGIUS, PokeInfo("Mismagius", 60, 60, 60, 105, 105, 105, Type::Ghost, Type::None, LEVITATE)},
    {EEVEE, PokeInfo("Eevee", 55, 55, 50, 45, 65, 55, Type::Normal, Type::None, RUN_AWAY, ADAPTABILITY)},
    {HIPPOPOTAS, PokeInfo("Hippopotas", 68, 72, 78, 38, 42, 32, Type::Ground, Type::None, SAND_STREAM)},
    {TOGETIC, PokeInfo("Togetic", 55, 40, 85, 80, 105, 40, Type::Normal, Type::Flying, HUSTLE)},
    {VAPOREON, PokeInfo("Vaporeon", 130, 65, 60, 110, 95, 65, Type::Water, Type::None, WATER_ABSORB)}

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
    bVal.substituteHp = 0;
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
    bVal.substituteHp = 0;
    bVal.substituteWasHit = false;

    bVal.movePrevByBattler = Empty;
    bVal.moveHit = Empty;
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

// clients 
PokeClient getMonfernoEternaClient() {
    PokeClient p1;
    Move moveset1[4] = {Tackle, FlameWheel, Taunt, Tackle};	
    Pokemon monferno = Pokemon(33, gentle, 0, MONFERNO, moveset1);
    monferno.setEvs(36,8,16,28,4,64);
    monferno.calcStats(); // recalc stats after setting evs
    // monferno.bVal.condition |= MON_CONDITION_PARALYSIS;
    p1.battler = 0;
    p1.aiControl = false;
    p1.team[0] = monferno;
    p1.name = "Player";
    return p1;
}
PokeClient getPlayerFantinaClient() {
    PokeClient p1;
    Move moveset1[4] = {Tackle, FlameWheel, Taunt, Tackle};	
    Move moveset2[4] = {Tackle, Bite, Substitute, Tackle};
    Pokemon monferno = Pokemon(35, gentle, 0, MONFERNO, moveset1);
    Pokemon eevee = Pokemon(32, timid, 0, EEVEE, moveset2);
    eevee.setEvs(252, 0, 0, 0, 0, 252);
    eevee.calcStats();
    monferno.setEvs(36,8,16,28,8,64);
    monferno.calcStats(); // recalc stats after setting evs
    // monferno.bVal.condition |= MON_CONDITION_PARALYSIS;
    p1.battler = 0;
    p1.aiControl = false;
    p1.team[0] = eevee;
    p1.name = "Player";
    return p1;
}
PokeClient getStaraviaBikeClient() {
    PokeClient p2;
    Move moveset2[4] = {QuickAttack, WingAttack, DoubleTeam, Endeavor};
    Pokemon stara = Pokemon(21, serious, 0, STARAVIA, moveset2);
    p2.aiControl = true;
    p2.aiLevel = 1;
    p2.battler = 0;
    p2.team[0] = stara;
    p2.name = "BikeGirl";
    return p2;
}

PokeClient getPachirisuClient() {
    PokeClient p2;
    Move moveset[4] = {Spark, QuickAttack, Charm, Empty};
    Pokemon pachi = Pokemon(22, bold, 0, PACHIRISU, moveset);
    p2.aiControl = true;
    p2.aiLevel = 7;
    p2.battler = 0;
    p2.team[0] = pachi;
    p2.name = "AI";
    return p2;
}

PokeClient getTripleStarlyClient() {
    PokeClient p2;
    Move moveset[4] = {Endeavor, DoubleTeam, WingAttack, QuickAttack};
    Move moveset2[4] = {QuickAttack, WingAttack, DoubleTeam, Endeavor};
    Pokemon star1 = Pokemon(17, quirky, 0, STARLY, moveset2);
    Pokemon star2 = Pokemon(18, quirky, 0, STARLY, moveset2);
    Pokemon star3 = Pokemon(19, quirky, 0, STARLY, moveset2);
    p2.aiControl = true;
    p2.aiLevel = 1;
    p2.battler = 0;
    p2.aiJumpNum = 4;
    p2.team[0] = star1;
    p2.team[1] = star2;
    p2.team[2] = star3;
    p2.name = "AI";
    return p2;
}


PokeClient getFantinaClient() {
    PokeClient p2;
    Move moveset[4] = {ShadowBall, Psybeam, MagicalLeaf, ConfuseRay};
    Pokemon mismag = Pokemon(26, impish, 12, MISMAGIUS, moveset);
    mismag.bVal.item = ITEM_SITRUS_BERRY;
    p2.aiControl = true;
    p2.aiLevel = 7;
    p2.battler = 0;
    p2.team[0] = mismag;
    p2.useItems[0] = ITEM_SUPER_POTION;
    p2.numUseItems = 1;
    p2.name = "AI";
    return p2;
}
PokeClient getHippopotasClient() {
    PokeClient p2;
    Move moveset[4] = {SandAttack, Bite, Yawn, TakeDown};
    Pokemon hippo = Pokemon(24, lonely, 31, HIPPOPOTAS, moveset);
    hippo.hpIv = 0;
    hippo.spdIv = 0;
    hippo.calcStats(); // recalc stats
    p2.aiControl = true;
    p2.aiLevel = 0;
    p2.battler = 0;
    p2.team[0] = hippo;
    p2.name = "Wild Hippo";
    return p2;
}
PokeClient getPlayerHippoClient() {
    PokeClient p1;
    Move moveset1[4] = {Charm, Charm, Charm, Charm};	
    Move moveset2[4] = {Substitute, Toxic, Substitute, Substitute};
    Pokemon vapor = Pokemon(36, lonely, 0, VAPOREON, moveset2);
    Pokemon togetic = Pokemon(24, lonely, 0, TOGETIC, moveset1);
    togetic.setEvs(6,6,15,7,2,17);
    togetic.calcStats(); // recalc stats after setting evs
    vapor.setEvs(252, 0, 0, 0, 0, 252);
    vapor.calcStats();
    // monferno.bVal.condition |= MON_CONDITION_PARALYSIS;
    p1.battler = 0;
    p1.aiControl = false;
    p1.team[0] = togetic;
    p1.team[1] = vapor;
    p1.name = "Player";
    return p1;
}
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
    BattleContext bc = {rs, p1, p2};
    bc.weather = 0;
    bc.weatherTurns = 0;
    bc.moveWasSuccessful = false;

    // p1.team[p1.battler].sendOut();
    // p2.team[p2.battler].sendOut();
    bc.turnNumber = 0;
    return bc;
}
BattleContext setupVarFight(unsigned long startingSeed) {
    PokeClient p1 = getPlayerHippoClient();
    PokeClient p2 = getHippopotasClient();
    p1.pokeSwitch(p1.battler);
    p2.pokeSwitch(p2.battler);
    // p1.team[p1.battler].bVal.evaStg = 12;
    // p1.team[p1.battler].bVal.spDefStg = 7;
    // p1.team[p1.battler].bVal.substituteHp = 24;
    // p1.team[p1.battler].bVal.item = ITEM_BOOST_DARK;
    // p1.team[p1.battler].sendOut();
    // p2.team[p2.battler].sendOut();
    RngSeed rs = {startingSeed, 0};
    BattleContext bc = {rs, p1, p2};
    bc.weather = 0;
    bc.weatherTurns = 0;
    bc.moveWasSuccessful = false;
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