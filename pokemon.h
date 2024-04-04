

#ifndef POKEMON_H_
#define POKEMON_H_
#include <vector>
#include "effects.h"
#include "moves.h"
#include <string>


#define ITEM_NONE 0
#define ITEM_SITRUS_BERRY 1
#define ITEM_ORAN_BERRY 2
#define ITEM_PECHA_BERRY 3
#define ITEM_CHERI_BERRY 4
#define ITEM_PERSIM_BERRY 5
#define ITEM_BOOST_DARK 6
#define ITEM_SUPER_POTION 7

// int calcDamage(Pokemon attacker, Pokemon defender, Move move, int crit = 1, int randomRoll = 100);


struct Nature {
    int val[5];
};
enum AbilityId {
    NO_ABILITY,
    BLAZE,
    STENCH,
    AFTERMATH,
    IMMUNITY,
    MAGIC_GUARD,
    LEAF_GUARD,
    POISON_HEAL,
    HYDRATION,
    MOLD_BREAKER,
    VOLT_ABSORB,
    WATER_ABSORB,
    MOTOR_DRIVE,
    FLASH_FIRE,
    WONDER_GUARD,
    LEVITATE,
    SOUNDPROOF,
    CLEAR_BODY,
    WHITE_SMOKE,
    NO_GUARD,
    KEEN_EYE,
    INNER_FOCUS,
    INTIMIDATE,
    SIMPLE,
    RUN_AWAY,
    HYPER_CUTTER,
    OWN_TEMPO,
    ADAPTABILITY,
};
// note that we really only care what stat nature is reducing
// since we can just extrapolate that our bonus stat is whatever
// is least helpful at the moment
enum NatureId {
    HARDY,
    LONELY,
    BRAVE,
    ADAMANT,
    NAUGHTY,
    BOLD,
    DOCILE,
    RELAXED,
    IMPISH,
    LAX,
    TIMID,
    HASTY,
    SERIOUS,
    JOLLY,
    NAIVE,
    MODEST,
    MILD,
    QUIET,
    BASHFUL,
    RASH,
    CALM,
    GENTLE,
    SASSY,
    CAREFUL,
    QUIRKY
};
enum Mons {
    MONFERNO,
    SKUNTANK,
    ZUBAT,
    STARAVIA,
    PACHIRISU,
    STARLY,
    MISMAGIUS,
    EEVEE,
};




struct BattleVal {
    int condition;
    int testStatus;
    int volConditions;
    int moveEffectsMask;
    int item;

    int substituteHp;
    bool substituteWasHit;

    int bHp;
    int atkStg;
    int defStg;
    int spAtkStg;
    int spDefStg;
    int spdStg;
    int accStg;
    int evaStg;
    int critStg;

    int turnsTaunted;

    bool abilityKnownToAi;

    Move moveHit;
    Move movePrevByBattler;
};
class PokeInfo{
    public:
        std::string name;
        int hp;
        int atk;
        int def;
        int spDef;
        int spAtk;
        int spd;
        Type primaryType;
        Type secondaryType;
        AbilityId ability;
        AbilityId ability2;
        PokeInfo(std::string _name, int _hp, int _atk, int _def, int _spAtk, int _spDef, int _spd, Type _primaryType, Type _secondaryType, AbilityId _ability, AbilityId _ability2);
};
class Pokemon{
    public:
        int level;
        PokeInfo info;
        int cHp;
        int cAtk;
        int cDef;
        int cSpDef;
        int cSpAtk;
        int cSpd;
        AbilityId cAbility;


        int hpIv;
        int atkIv;
        int defIv;
        int spAtkIv;
        int spDefIv;
        int spdIv;

        int hpEv;
        int atkEv;
        int defEv;
        int spAtkEv;
        int spDefEv;
        int spdEv;

        Nature nature;
        BattleVal bVal;
        Move moveset[4];
        Pokemon();
        Pokemon(int _lvl, Nature _nature, int _ivs, Mons _id, Move _moveset[4], int _ability);
        void calcStats();
        void sendOut();
        void setEvs(int hp, int atk, int def, int spAtk, int spDef, int spd);
        void setBHp(int hp);
};

class Ability{
    public:
        AbilityId aid;
};

// fight setup
BattleContext setupJupiterFight(unsigned long startingSeed);
BattleContext setupVarFight(unsigned long startingSeed);
#endif /* POKEMON_H_ */