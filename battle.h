#ifndef BATTLE_H_
#define BATTLE_H_

#include "pokemon.h"
#define DEBUG 3 // 0 for none, 1 for basic, 3 for detailed, 7 for dump
#define MAX_CLIENTS 4
#define COMMAND_MOVE_SLOT_1 1 << 0
#define COMMAND_MOVE_SLOT_2 1 << 1
#define COMMAND_MOVE_SLOT_3 1 << 2
#define COMMAND_MOVE_SLOT_4 1 << 3
#define COMMAND_MOVE    ( COMMAND_MOVE_SLOT_1 \
                        | COMMAND_MOVE_SLOT_2 \
                        | COMMAND_MOVE_SLOT_3 \
                        | COMMAND_MOVE_SLOT_4)
#define COMMAND_USE_ITEM_SUPER_POTION 1 << 4
#define COMMAND_USE_ITEM_GUARD_SPEC 1 << 5
#define COMMAND_USE_ITEM_HYPER_POTION 1 << 6
#define COMMAND_USE_ITEM ( COMMAND_USE_ITEM_SUPER_POTION \
                         | COMMAND_USE_ITEM_GUARD_SPEC \
                         | COMMAND_USE_ITEM_HYPER_POTION)

#define TRIGGER_INTIMIDATE 1<<0

unsigned short advanceSeed(BattleContext *bc);
bool useMove(Move move, BattleContext *bc);
int calcDamage(BattleContext *bc, Move move, int crit, int randomRoll);
int getTypeMultiplier(BattleContext *bc, Move move, int damage);
bool determineOrder(BattleContext *bc);
bool doTurn(BattleContext *bc);
bool checkStatusDisruption(BattleContext *bc, Move move);
void updateMoveBuffers(BattleContext *bc, Move move);

enum class FieldCondition {
    NONE,
    MIST,
    SAFEGUARD,

};
enum class Weather {
    None,
    Rain,
    Sunny,
    Hail,
    Sandstorm
};
struct RngSeed {
    unsigned long seed;
    unsigned short result;
};
struct SideCondition {
    int remainingTurns;
    FieldCondition condition;
};
struct BattleSide {
    std::vector<SideCondition> sideConditions;
};
struct PokeClient {
    std::string name;
    int sideConditions;
    // specific effect turn counters
    int safeGuardTurns;
    int mistTurns;
    bool aiControl;
    bool isWinner;
    int aiLevel;
    Move previouslySelectedMove;
    int battler;
    Pokemon team[6];
    int command;
    PokeClient(void);
    void pokeSwitch(int nextBattler);
    // triggers
    int triggers;

};
struct BattleContext {

    Weather weather;
    RngSeed battleRng;
    PokeClient attacker;
    PokeClient defender;
    bool moveWasSuccessful; // only has to be used (get past premove), doesn't have to hit
    int cDmg; // variable flat damage
    int cPwr; // variable power 
    int turnNumber;
    int speedTieBreakers[4];

};
struct Fraction {
    int numerator;
    int denominator;
};
static const Fraction HitRateByStage[] = {
    {  33, 100 },
    {  36, 100 },
    {  43, 100 },
    {  50, 100 },
    {  60, 100 },
    {  75, 100 },
    {   1,   1 },
    { 133, 100 },
    { 166, 100 },
    {   2,   1 },
    { 233, 100 },
    { 133,  50 },
    {   3,   1 }
};
static const Fraction StatBonusByStage[] = {
    {2, 8},//0
    {2, 7},//1
    {2, 6},//2
    {2, 5},//3
    {2, 4},//4
    {2, 3},//5
    {2, 2},//6
    {3, 2},//7
    {4, 2},//8
    {5, 2},//9
    {6, 2},//10
    {7, 2},//11
    {8, 2},//12
};
#endif /* BATTLE_H_ */