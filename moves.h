

#ifndef MOVES_H_
#define MOVES_H_
#include <vector>
#include <string>


#define RANGE_TARGET 0
#define RANGE_SELF 1

enum Type {
    Normal,
    Fighting,
    Flying,
    Poison,
    Ground,
    Rock,
    Bug,
    Ghost,
    Steel,
    Fire,
    Water,
    Grass,
    Electric,
    Psychic,
    Ice,
    Dragon,
    Dark,
    None
};


enum class DamageType:int {
    NONE,
    PHYSICAL,
    SPECIAL
};

enum class MoveId {
    NO_MOVE,
    STRUGGLE,
    FLAME_WHEEL,
    NIGHT_SLASH,
    TAUNT,
    SMOKESCREEN,
    SCREECH,
    POISON_GAS,
    TACKLE,
    QUICK_ATTACK,
    WING_ATTACK,
    DOUBLE_TEAM,
    ENDEAVOR,
    SPARK,
    CHARM,
    CONFUSE_RAY,
    PSYBEAM,
    SHADOW_BALL,
    MAGICAL_LEAF,
    BITE,
    SUBSTITUTE,
};
struct Move {
        MoveId id;
        std::string name;
        int power;
        int accuracy;
        bool secondary;
        int secondaryAccuracy;
        int effect;
        Type moveType;
        DamageType defend; // 0 physical, 1 special
        DamageType attack; // 0 physical, 1 special
        int priority;
        bool highCritRatio;
        bool disabled;
        int range;
        Move();
        Move(MoveId _id, std::string name, int _power, int _accuracy , bool _secondary, int _secondaryAccuracy, int _effect, Type _moveType, DamageType defend, DamageType attack, int priority, bool _highCritRatio, int range);
};
extern Move Empty;
extern Move Struggle;
extern Move Tackle;
extern Move FlameWheel;
extern Move Taunt;
extern Move NightSlash;
extern Move PoisonGas;
extern Move Screech;
extern Move Smokescreen;
extern Move QuickAttack;
extern Move WingAttack;
extern Move DoubleTeam;
extern Move Endeavor;
extern Move Charm;
extern Move Spark;
extern Move ConfuseRay;
extern Move Psybeam;
extern Move ShadowBall;
extern Move MagicalLeaf;
extern Move Substitute;
extern Move Bite;
#endif /* MOVES_H_ */