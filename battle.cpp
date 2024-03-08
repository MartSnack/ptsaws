#include <iostream>
#include <map>
#include <string>
#include <cmath>
#include <vector>
#include "battle.h"
#include "battle_effects.h"
#include "tr_ai.h"
void log(std::string logText) {
    if(DEBUG) {
        std::cout << logText << std::endl;
    }
}
std::string logName(Pokemon p, int dmg = 0) {
    std::string hpString = "";
    if(dmg == 0) {
        hpString = " (" + std::to_string(p.bVal.bHp) + "/" + std::to_string(p.cHp) + ")";
    } else {
        int newVal = p.bVal.bHp - dmg;
        if(newVal < 0) {
            newVal = 0;
        }
        hpString = " (" + std::to_string(p.bVal.bHp) + "->" + std::to_string(newVal) + ")";
    }
    return p.info.name + hpString;
}
void log(int logText) {
    if(DEBUG) {
        std::cout << logText << std::endl;
    }
}
unsigned short advanceSeed(BattleContext *bc, std::string blurb){

    unsigned long seed = (1103515245 * bc -> battleRng.seed + 24691) & 0xFFFFFFFF;
    unsigned short result = seed / 65536;
    bc -> battleRng = {seed, result};
    if(DEBUG & 4) {
        if(blurb == "none") {
            std::cout << seed << std::endl;
        } else {
            std::cout << std::to_string(seed) << " -> " << blurb << std::endl;
        }
    }
    return result;
};
int critStages[6] = {16,8,4,3,2,2};


std::map<Type, std::map<Type, float>> typeMap = {
    {Normal,     {{Normal, 1.0}, {Fighting, 1.0}, {Flying, 1.0}, {Poison, 1.0}, {Ground, 1.0}, {Rock, 0.5}, {Bug, 1.0}, {Ghost, 0.0}, {Steel, 0.5}, {Fire, 1.0}, {Water, 1.0}, {Grass, 1.0}, {Electric, 1.0}, {Psychic, 1.0}, {Ice, 1.0}, {Dragon, 1.0}, {Dark, 1.0}, {None, 1.0}}},
    {Fighting,   {{Normal, 2.0}, {Fighting, 1.0}, {Flying, 0.5}, {Poison, 0.5}, {Ground, 1.0}, {Rock, 2.0}, {Bug, 0.5}, {Ghost, 0.0}, {Steel, 2.0}, {Fire, 1.0}, {Water, 1.0}, {Grass, 1.0}, {Electric, 1.0}, {Psychic, 0.5}, {Ice, 2.0}, {Dragon, 1.0}, {Dark, 2.0}, {None, 1.0}}},
    {Flying,     {{Normal, 1.0}, {Fighting, 2.0}, {Flying, 1.0}, {Poison, 1.0}, {Ground, 1.0}, {Rock, 0.5}, {Bug, 2.0}, {Ghost, 1.0}, {Steel, 0.5}, {Fire, 1.0}, {Water, 1.0}, {Grass, 2.0}, {Electric, 0.5}, {Psychic, 1.0}, {Ice, 1.0}, {Dragon, 1.0}, {Dark, 1.0}, {None, 1.0}}},
    {Poison,     {{Normal, 1.0}, {Fighting, 1.0}, {Flying, 1.0}, {Poison, 0.5}, {Ground, 0.5}, {Rock, 0.5}, {Bug, 1.0}, {Ghost, 0.5}, {Steel, 0.0}, {Fire, 1.0}, {Water, 1.0}, {Grass, 2.0}, {Electric, 1.0}, {Psychic, 1.0}, {Ice, 1.0}, {Dragon, 1.0}, {Dark, 1.0}, {None, 1.0}}},
    {Ground,     {{Normal, 1.0}, {Fighting, 1.0}, {Flying, 0.0}, {Poison, 2.0}, {Ground, 1.0}, {Rock, 2.0}, {Bug, 0.5}, {Ghost, 1.0}, {Steel, 2.0}, {Fire, 2.0}, {Water, 1.0}, {Grass, 0.5}, {Electric, 2.0}, {Psychic, 1.0}, {Ice, 1.0}, {Dragon, 1.0}, {Dark, 1.0}, {None, 1.0}}},
    {Rock,       {{Normal, 1.0}, {Fighting, 0.5}, {Flying, 2.0}, {Poison, 1.0}, {Ground, 0.5}, {Rock, 1.0}, {Bug, 2.0}, {Ghost, 1.0}, {Steel, 0.5}, {Fire, 2.0}, {Water, 1.0}, {Grass, 1.0}, {Electric, 1.0}, {Psychic, 1.0}, {Ice, 2.0}, {Dragon, 1.0}, {Dark, 1.0}, {None, 1.0}}},
    {Bug,        {{Normal, 1.0}, {Fighting, 0.5}, {Flying, 0.5}, {Poison, 0.5}, {Ground, 1.0}, {Rock, 1.0}, {Bug, 1.0}, {Ghost, 0.5}, {Steel, 0.5}, {Fire, 0.5}, {Water, 1.0}, {Grass, 2.0}, {Electric, 1.0}, {Psychic, 2.0}, {Ice, 1.0}, {Dragon, 1.0}, {Dark, 2.0}, {None, 1.0}}},
    {Ghost,      {{Normal, 0.0}, {Fighting, 1.0}, {Flying, 1.0}, {Poison, 1.0}, {Ground, 1.0}, {Rock, 1.0}, {Bug, 1.0}, {Ghost, 2.0}, {Steel, 0.5}, {Fire, 1.0}, {Water, 1.0}, {Grass, 1.0}, {Electric, 1.0}, {Psychic, 2.0}, {Ice, 1.0}, {Dragon, 1.0}, {Dark, 0.5}, {None, 1.0}}},
    {Steel,      {{Normal, 1.0}, {Fighting, 1.0}, {Flying, 1.0}, {Poison, 1.0}, {Ground, 1.0}, {Rock, 2.0}, {Bug, 1.0}, {Ghost, 1.0}, {Steel, 0.5}, {Fire, 0.5}, {Water, 0.5}, {Grass, 1.0}, {Electric, 0.5}, {Psychic, 1.0}, {Ice, 2.0}, {Dragon, 1.0}, {Dark, 1.0}, {None, 1.0}}},
    {Fire,       {{Normal, 1.0}, {Fighting, 1.0}, {Flying, 1.0}, {Poison, 1.0}, {Ground, 1.0}, {Rock, 0.5}, {Bug, 2.0}, {Ghost, 1.0}, {Steel, 2.0}, {Fire, 0.5}, {Water, 0.5}, {Grass, 2.0}, {Electric, 1.0}, {Psychic, 1.0}, {Ice, 2.0}, {Dragon, 0.5}, {Dark, 1.0}, {None, 1.0}}},
    {Water,      {{Normal, 1.0}, {Fighting, 1.0}, {Flying, 1.0}, {Poison, 1.0}, {Ground, 2.0}, {Rock, 2.0}, {Bug, 1.0}, {Ghost, 1.0}, {Steel, 1.0}, {Fire, 2.0}, {Water, 0.5}, {Grass, 0.5}, {Electric, 1.0}, {Psychic, 1.0}, {Ice, 1.0}, {Dragon, 0.5}, {Dark, 1.0}, {None, 1.0}}},
    {Grass,      {{Normal, 1.0}, {Fighting, 1.0}, {Flying, 0.5}, {Poison, 0.5}, {Ground, 2.0}, {Rock, 2.0}, {Bug, 0.5}, {Ghost, 1.0}, {Steel, 0.5}, {Fire, 0.5}, {Water, 2.0}, {Grass, 0.5}, {Electric, 1.0}, {Psychic, 1.0}, {Ice, 1.0}, {Dragon, 0.5}, {Dark, 1.0}, {None, 1.0}}},
    {Electric,   {{Normal, 1.0}, {Fighting, 1.0}, {Flying, 2.0}, {Poison, 1.0}, {Ground, 0.0}, {Rock, 1.0}, {Bug, 1.0}, {Ghost, 1.0}, {Steel, 1.0}, {Fire, 1.0}, {Water, 2.0}, {Grass, 0.5}, {Electric, 0.5}, {Psychic, 1.0}, {Ice, 1.0}, {Dragon, 0.5}, {Dark, 1.0}, {None, 1.0}}},
    {Psychic,    {{Normal, 1.0}, {Fighting, 2.0}, {Flying, 1.0}, {Poison, 2.0}, {Ground, 1.0}, {Rock, 1.0}, {Bug, 1.0}, {Ghost, 1.0}, {Steel, 0.5}, {Fire, 1.0}, {Water, 1.0}, {Grass, 1.0}, {Electric, 1.0}, {Psychic, 0.5}, {Ice, 1.0}, {Dragon, 1.0}, {Dark, 0.0}, {None, 1.0}}},
    {Ice,        {{Normal, 1.0}, {Fighting, 1.0}, {Flying, 2.0}, {Poison, 1.0}, {Ground, 2.0}, {Rock, 1.0}, {Bug, 1.0}, {Ghost, 1.0}, {Steel, 0.5}, {Fire, 0.5}, {Water, 0.5}, {Grass, 2.0}, {Electric, 1.0}, {Psychic, 1.0}, {Ice, 0.5}, {Dragon, 2.0}, {Dark, 1.0}, {None, 1.0}}},
    {Dragon,     {{Normal, 1.0}, {Fighting, 1.0}, {Flying, 1.0}, {Poison, 1.0}, {Ground, 1.0}, {Rock, 1.0}, {Bug, 1.0}, {Ghost, 1.0}, {Steel, 0.5}, {Fire, 1.0}, {Water, 1.0}, {Grass, 1.0}, {Electric, 1.0}, {Psychic, 1.0}, {Ice, 1.0}, {Dragon, 2.0}, {Dark, 1.0}, {None, 1.0}}},
    {Dark,       {{Normal, 1.0}, {Fighting, 0.5}, {Flying, 1.0}, {Poison, 1.0}, {Ground, 1.0}, {Rock, 1.0}, {Bug, 1.0}, {Ghost, 2.0}, {Steel, 0.5}, {Fire, 1.0}, {Water, 1.0}, {Grass, 1.0}, {Electric, 1.0}, {Psychic, 2.0}, {Ice, 1.0}, {Dragon, 1.0}, {Dark, 0.5}, {None, 1.0}}},
    {None,       {{Normal, 1.0}, {Fighting, 1.0}, {Flying, 1.0}, {Poison, 1.0}, {Ground, 1.0}, {Rock, 1.0}, {Bug, 1.0}, {Ghost, 1.0}, {Steel, 1.0}, {Fire, 1.0}, {Water, 1.0}, {Grass, 1.0}, {Electric, 1.0}, {Psychic, 1.0}, {Ice, 1.0}, {Dragon, 1.0}, {Dark, 1.0}, {None, 1.0}}},

};

PokeClient::PokeClient(void): previouslySelectedMove() {
    sideConditions = 0;
    safeGuardTurns = 0;
    mistTurns = 0;
    isWinner = false;
    triggers = 0;
    aiJumpNum = 2;
}

void PokeClient::pokeSwitch(int nextBattler) {
    battler = nextBattler;
    Pokemon& p = team[battler];
    p.sendOut(); // refresh
    if(p.cAbility == AbilityId::INTIMIDATE) {
        triggers |= TRIGGER_INTIMIDATE;
    }
}
int modifyStatValue(int base, int stage){
    int interm = base * StatBonusByStage[stage].numerator;
    return interm / StatBonusByStage[stage].denominator;
}

int calcSpeed(PokeClient *pc) {
    int speed = modifyStatValue(pc->team[pc->battler].cSpd, pc->team[pc->battler].bVal.spdStg);
    if(pc->team[pc->battler].bVal.condition & MON_CONDITION_PARALYSIS) {
        if(4 & DEBUG) {
            std::cout << "Reducing speed due to paralysis" << std::endl;
        }
        speed = speed/4; // paralysis reduction occurs after staged increases
    }
    return speed;
}
int getPriorityScore(PokeClient *pc) {
    int command1 = pc->command;
    int priorityScore = 0;
    if(command1 & COMMAND_MOVE) {
        // moves have lowest priority
        priorityScore += 6;
        int moveNum = log2(command1);
        priorityScore += pc->team[pc->battler].moveset[moveNum].priority;
    } else if(command1 & COMMAND_USE_ITEM) {
        priorityScore += 13; // higher than any move 
    }
    return priorityScore;
}
//BattleSystem_CompareBattlerSpeed
bool determineOrder(BattleContext *bc) {
    bool attackerGoesFirst = true;
    int priorityScore1 = getPriorityScore(&bc->attacker);
    int priorityScore2 = getPriorityScore(&bc->defender);
    int speed1 = calcSpeed(&bc->attacker);
    int speed2 = calcSpeed(&bc->defender);
    if(priorityScore1 == priorityScore2) {
        // same priority bracket, resort to speed calc
        if(speed1 > speed2) {
            attackerGoesFirst = true;
        } else {
            attackerGoesFirst = false; //TODO - Handle speed ties
        }
    } else if(priorityScore1  > priorityScore2) {
        attackerGoesFirst = true;
    } else {
        attackerGoesFirst = false;
    }
    return attackerGoesFirst;
}
int getCritStage(Move move, BattleContext *bc){
    int stage = bc->attacker.team[bc->attacker.battler].bVal.critStg;
    if(move.highCritRatio) {
        stage++;
    }
    if(stage > 4) {
        stage = 4;
    }
    // std::cout << "\tcrit stage: " << stage << std::endl;
    return critStages[stage];
}
int getTypeMultiplier(BattleContext *bc, Move move, int damage, int *moveStatus) {
    if(!moveStatus) {
        moveStatus = 0;
    }
    int stabBonus = (bc->attacker.team[bc->attacker.battler].info.primaryType == move.moveType || bc->attacker.team[bc->attacker.battler].info.secondaryType == move.moveType) ? 150 : 100;

    int type1Bonus = typeMap[move.moveType][bc->defender.team[bc->defender.battler].info.primaryType] * 10;
    int type2Bonus = typeMap[move.moveType][bc->defender.team[bc->defender.battler].info.secondaryType] * 10;
    int mul = type1Bonus * type2Bonus;
    if(mul >= 200) {
        *moveStatus = MOVE_STATUS_SUPER_EFFECTIVE;
    } else if(mul < 100 && mul > 0) {
        *moveStatus = MOVE_STATUS_NOT_EFFECTIVE;
    }
    damage = damage * stabBonus / 100;
    damage = (damage * mul) / 100;

    return damage;
}
int calcDamage(BattleContext *bc, Move move, int crit = 1, int randomRoll = 100) {
    int lvlBase = ((2 * bc->attacker.team[bc->attacker.battler].level) / 5);
    lvlBase = lvlBase + 2;

    int cAtkStat = 0;
    int cDefStat = 0;
    if( move.attack ==  DamageType::PHYSICAL) {
        cAtkStat = bc->attacker.team[bc->attacker.battler].cAtk;
        if(bc->attacker.team[bc->attacker.battler].bVal.atkStg < 6 && crit>1){
            // we crit, so ignore lowered atk stage
            cAtkStat = modifyStatValue(cAtkStat, 6);
        } else {
            cAtkStat = modifyStatValue(cAtkStat, bc->attacker.team[bc->attacker.battler].bVal.atkStg);
        }
    } else {
        cAtkStat = bc->attacker.team[bc->attacker.battler].cSpAtk;
        if(bc->attacker.team[bc->attacker.battler].bVal.spAtkStg < 6 && crit>1){
            // we crit, so ignore lowered atk stage
            cAtkStat = modifyStatValue(cAtkStat, 6);
        } else {
            cAtkStat = modifyStatValue(cAtkStat, bc->attacker.team[bc->attacker.battler].bVal.spAtkStg);
        }
    }
    if( move.defend ==  DamageType::PHYSICAL) {
        cDefStat = bc->defender.team[bc->defender.battler].cDef;
        if(bc->defender.team[bc->defender.battler].bVal.defStg > 6 && crit > 1) {
            // ignore higher defense on crit
            cDefStat = modifyStatValue(cDefStat, 6);
        } else {
            cDefStat = modifyStatValue(cDefStat, bc->defender.team[bc->defender.battler].bVal.defStg);
        }
    } else {
        cDefStat = bc->defender.team[bc->defender.battler].cSpDef;
        if(bc->defender.team[bc->defender.battler].bVal.spDefStg > 6 && crit > 1) {
            // ignore higher defense on crit
            cDefStat = modifyStatValue(cDefStat, 6);
        } else {
            cDefStat = modifyStatValue(cDefStat, bc->defender.team[bc->defender.battler].bVal.spDefStg);
        }
    }
    int movePower = move.power;
    if(movePower == 1) {
        bc->cDmg = 0;
        bc->cPwr = 0;
        // move has variable (or flat) power
        // endeavor, gyro ball, crush grip, dragon rage, etc.
        if(applyEffect(move, bc)){
            // success
            if(bc->cDmg > 0) {
                // current damage was set, move deals flat damage (no crits)
                return bc->cDmg;
            } else {
                // if cDmg wasn't set, power must be. Set move power to the variable power result 
                movePower = bc->cPwr; 
            }
        } else {
            return 0; // move failed, return 0 for damage 
        }
    }
    if(move.moveType == Type::Fire && bc->attacker.team[bc->attacker.battler].cAbility == AbilityId::BLAZE && bc->attacker.team[bc->attacker.battler].bVal.bHp <= bc->attacker.team[bc->attacker.battler].cHp/3){
        movePower = movePower * 150 / 100;
    }
    int powerBase = (lvlBase * movePower * (cAtkStat));
    int defDivide = powerBase / cDefStat;
    int powerResult = (defDivide/50);
    if((bc->attacker.team[bc->attacker.battler].bVal.condition & MON_CONDITION_BURN) && move.attack == DamageType::PHYSICAL) {
        powerResult = powerResult/2;
    }
    powerResult = powerResult + 2;
    powerResult = powerResult * crit; // should be 2 if we critted :A
    powerResult = powerResult * (100 - randomRoll) / 100;
    int moveStatus = 0;
    powerResult = getTypeMultiplier(bc, move, powerResult, &moveStatus);
    return powerResult;
};
bool rollAccuracy(Move move, BattleContext *bc) {
    int accuracy = move.accuracy;
    if(accuracy == 0){
        return true; // bypass accuracy check
    }
    int accStages = bc->attacker.team[bc->attacker.battler].bVal.accStg - 6;
    int evaStages = 6 - bc->defender.team[bc->defender.battler].bVal.evaStg;
    int sum = 6 + evaStages + accStages;
    if (sum < 0) {
        sum = 0;
    } else if (sum > 12) {
        sum = 12;
    }
    accuracy *= HitRateByStage[sum].numerator;
    accuracy /= HitRateByStage[sum].denominator;
    // add evasion and accuracy checks
    log("\trolling accuracy");
    return (advanceSeed(bc) % 100) < accuracy;
}
void heal(Pokemon *p, int amount) {
    p->bVal.bHp = p->bVal.bHp + amount;
    if(p->bVal.bHp > p->cHp){
        p->bVal.bHp = p->cHp;
    }
}
void dealDamage(Pokemon *p,  int damage) {
    p->bVal.bHp = p->bVal.bHp - damage;

    if(p->bVal.bHp < 0) {
        p->bVal.bHp = 0;
    }
    if((p->bVal.bHp <= (p->cHp / 2)) && p->bVal.bHp > 0) {
        // trigger effects that occur below half health
        if(p->bVal.item == ITEM_ORAN_BERRY) {
            int oldHp = p->bVal.bHp;
            heal(p, 10);
            p->bVal.item = ITEM_NONE; // consumed
            if(DEBUG) {
                std::cout << "\t\tOran berry activates: " << p->info.name << " heals " <<  oldHp << "->" << p->bVal.bHp << std::endl;
            }
        } else if(p->bVal.item == ITEM_SITRUS_BERRY) {
            int oldHp = p->bVal.bHp;
            heal(p, p->cHp/4);
            p->bVal.item = ITEM_NONE; // consumed
            if(DEBUG) {
                std::cout << "\t\tSitrus berry activates: " << p->info.name << " heals " <<  oldHp << "->" << p->bVal.bHp << std::endl;
            }
        }
    }
}
bool preMove(Move move, BattleContext *bc) {
    bool statusDisrupted = checkStatusDisruption(bc, move); // paralysis, sleep, flinch
    if(statusDisrupted) {
        return false;
    }

    if(bc->attacker.team[bc->attacker.battler].bVal.bHp == 0) {
        return false; // im dead
    }
    if(bc->defender.team[bc->defender.battler].bVal.bHp == 0 && move.range == RANGE_TARGET) {
        return false; // no valid target
    }
    bc->moveWasSuccessful = true;
    return true;
}
bool postMove(Move move, BattleContext *bc){
    return true;
}
// return true if move was successful, false if not
bool useMove(Move move, BattleContext *bc) {
    if(DEBUG) {
        std::cout << "Battler " << logName(bc->attacker.team[bc->attacker.battler]) << " uses " << move.name << std::endl;
    }
    bool success = false;
    if(preMove(move, bc)){
        int crit = 15;
        int dmgRoll = 15;
        int dmg = 0;
        if(move.power < 2){
            // non-attacking move
            // or move with variable flat damage 
        } else {
            log("\trolling crit");
            crit = advanceSeed(bc) % getCritStage(move, bc);
            crit = crit == 0;
            crit = crit + 1;
            log("\trolling damage");
            dmgRoll = advanceSeed(bc) % 16;
        }
        bool accRoll = rollAccuracy(move, bc);
        if(accRoll) {
            // move hits
            if(move.power == 0) {
                // non-attacking move handler
            } else {
                dmg = calcDamage(bc, move, crit, dmgRoll);
                if(dmg > 0){
                    success = true; // dmg move successfully executed
                    if(DEBUG) {
                        if(crit > 1) {
                            std::cout << "\tCrit" << std::endl;
                        }
                        std::cout << "\tDamage: " << dmg << std::endl;
                    }

                    dealDamage(&bc->defender.team[bc->defender.battler], dmg);
                }


            }
            bool shouldApplyEffect = false;
            if(move.secondary) {
                //when checking if opponenet is alive, we may need
                // to verify if the moves secondary targets us or them
                // e.g. ancientpower buff vs flamewheel burn
                if(bc->defender.team[bc->defender.battler].bVal.bHp > 0) {
                    log("\tChecking secondary effect chance");
                    shouldApplyEffect = (advanceSeed(bc) % 100) < move.secondaryAccuracy;
                }

            } else {
                if (move.effect != BATTLE_EFFECT_HIT){
                    shouldApplyEffect = true; // if the effect is the prime component of the move, we do it so long as we hit
                }
            }
            if(shouldApplyEffect) {
                if(DEBUG) {
                    std::cout << "\tTry Applying Effect" << std::endl;
                }
                // regardless of whether the effect was a secondary or a primary, we apply it
                bool didApply = applyEffect(move, bc);
                if(!move.secondary && didApply){
                    success = true; // status move successfully executed
                }
            }

        } else {
            log("\tMove Missed");
        }
    } 
    if(!success) {
        log("\tMove Failed");
    }
    // afterMove (technically should occur after any status disruption as well but we have a different control flow than the original code)
    updateMoveBuffers(bc, move);
    return success;
}
bool useItem(BattleContext *bc, int item) {
    if(item & COMMAND_USE_ITEM_GUARD_SPEC) {
        log(bc->attacker.name + " uses Guard Spec");
        applyMist(bc);
        return true;
    } else if(item & COMMAND_USE_ITEM_HYPER_POTION) {
        log(bc->attacker.name + " uses Hyper Potion");
        if(bc->attacker.team[bc->attacker.battler].bVal.bHp < bc->attacker.team[bc->attacker.battler].cHp) {
            heal(&bc->attacker.team[bc->attacker.battler], 200);
            return true;
        } else {
            return false;
        }
    } else if(item & COMMAND_USE_ITEM_PRLZ_HEAL) {
        log(bc->attacker.name + " uses Prlz Heal");
        if(bc->attacker.team[bc->attacker.battler].bVal.condition & MON_CONDITION_PARALYSIS) {
            bc->attacker.team[bc->attacker.battler].bVal.condition = MON_CONDITION_NONE; // clear paralysis
        }
    }
    return false;
}
void setupBattle(BattleContext *bc) {
    bc->turnNumber = 0;
    bc->weather = Weather::None;
    bc->moveWasSuccessful = false;
}
bool checkStatusDisruption(BattleContext *bc, Move move) {
    PokeClient attacker = bc->attacker;
    Pokemon attackingPokemon = attacker.team[attacker.battler];
    if(bc->attacker.team[bc->attacker.battler].bVal.turnsTaunted > 0) {
        // we got taunted :(((
        if(move.power == 0) {
            return true;
        }
    }
    if(attackingPokemon.bVal.condition & MON_CONDITION_PARALYSIS) {
        if(attackingPokemon.cAbility != AbilityId::MAGIC_GUARD) {
            if(advanceSeed(bc) % 4 == 0) {
                if(DEBUG) {
                    std::cout << "\tFully Paralyzed!" << std::endl;
                }
                return true; // disrupted
            }
        }
    }

    return false;
}
void updateMoveBuffers(BattleContext *bc, Move move) {
    if(bc->moveWasSuccessful) {
        bc->attacker.previouslySelectedMove = move;
        bc->attacker.successfulMove = true;
    }
    updateFlagsWhenHit(bc, move);
}
void updateFlagsWhenHit(BattleContext *bc, Move move) {
    if(bc->moveWasSuccessful) {
        bc->defender.team[bc->defender.battler].bVal.moveHit = move;
    } else {
        bc->defender.team[bc->defender.battler].bVal.moveHit = Empty;
    }
}
void resetContext(BattleContext *bc) {
    bc->moveWasSuccessful = false;
    bc->attacker.successfulMove = false;
}
void executeCommand(BattleContext *bc) {
    resetContext(bc);
    if(bc->attacker.command & COMMAND_MOVE) {

        int moveNum = log2(bc->attacker.command);
        bool success = useMove(bc->attacker.team[bc->attacker.battler].moveset[moveNum], bc);
        if(success) {
            log("\tMove Succeeded > RNG advances 2");
            advanceSeed(bc);
            advanceSeed(bc);
        }

    } else if(bc->attacker.command & COMMAND_USE_ITEM) {
        useItem(bc, bc->attacker.command);
    }

}
bool isFightOver(BattleContext *bc) {
    bool attackerWon = true;
    bool defenderWon = true;
    int i;
    for(i=0;i<6;i++){
        if(bc->attacker.team[i].bVal.bHp > 0) {
            defenderWon = false;
        }
    }
    for(i=0;i<6;i++){
        if(bc->defender.team[i].bVal.bHp > 0) {
            attackerWon = false;
        }
    }
    if(attackerWon || defenderWon) {
        if(attackerWon){
            bc->attacker.isWinner = true;
            log(bc->attacker.name + " wins");
        } else {
            bc->defender.isWinner = true;
            log(bc->defender.name + " wins");
        }
        return true;
    }
    return false;
}
void triggerEndTurnConditions(PokeClient *p) {
    if((p->team[p->battler].bVal.condition & MON_CONDITION_POISON) && p->team[p->battler].bVal.bHp) {
        int psnDmg = p->team[p->battler].cHp/8;
        if(DEBUG) {
            std::cout << "Poison deals " << psnDmg << " damage to " << logName(p->team[p->battler], psnDmg) << std::endl;
        }
        dealDamage(&p->team[p->battler], psnDmg);

    } else if((p->team[p->battler].bVal.condition & MON_CONDITION_BURN) && p->team[p->battler].bVal.bHp) {
        int brnDmg = p->team[p->battler].cHp/8;
        if(DEBUG) {
            std::cout << "Burn deals " << brnDmg << " damage to " << logName(p->team[p->battler], brnDmg) << std::endl;
        }
        dealDamage(&p->team[p->battler], brnDmg);

    }
    // taunt check
    if(p->team[p->battler].bVal.bHp && p->team[p->battler].bVal.turnsTaunted > 0) {
        if(--p->team[p->battler].bVal.turnsTaunted > 0) {
            for(int i=0;i<4;i++){
                if(p->team[p->battler].moveset[i].power == 0) {
                    p->team[p->battler].moveset[i].disabled = true;
                }
            }
        } else {
            for(int i=0;i<4;i++){
                if(p->team[p->battler].moveset[i].power == 0) {
                    p->team[p->battler].moveset[i].disabled = false;
                }
            }
        }
    }
    // mist check
    if(p->team[p->battler].bVal.bHp && p->sideConditions & SIDE_CONDITION_MIST) {
        if(--p->mistTurns == 0) {
            p->sideConditions &= ~SIDE_CONDITION_MIST;
        }
    }
}

// switch in a new mon if we need it
// returns true if we swapped
// return false if we didn't.
bool switchIn(BattleContext *bc, bool attacker) {
    PokeClient *pc;
    if(attacker) {
        pc = &bc->attacker;
    } else {
        pc = &bc->defender;
    }
    int numValidMons = 0;
    if(pc->aiControl) {
        for(int i = 0; i<6; i++) {
            if(pc->team[i].bVal.bHp > 0) {
                numValidMons++;
            }
        }
        // if(numValidMons > 1) {
        //     // advanceSeed(bc);
        //     advanceSeed(bc); // TODO: Figure out why the KO switch routine advances the rng 2 frames when it has multiple valid choices to send out 
        //     // research: Against bike trainers with 3 starly's this was first observed. First Starly KO leads to 2 extra frame jumps prior to AI deciding its move
        //     // answer: the ai is evaluating if it should switch, and several of those checks use an rng value.  Notably, it will use 2 at least if its current battler has a super effective move 
        // }
    }
    for(int i = 0; i<6; i++) {
        if(pc->team[i].bVal.bHp > 0) {
            pc->pokeSwitch(i);
            log(pc->name + " sends out " + pc->team[i].info.name);
            return true;
        }
    }
    return false;
}
// doAttacker -> whether to trigger the attacker's effect or defenders
// so that we don't have to do everything twice.
bool preTurnEffects(BattleContext *bc, bool doAttacker) {
    PokeClient *applyClient;
    PokeClient *receiveClient;
    if(doAttacker) {
        applyClient = &bc->attacker;
        receiveClient = &bc->defender;
    } else {
        applyClient = &bc->defender;
        receiveClient = &bc->attacker;
    }

    if(applyClient->triggers & TRIGGER_INTIMIDATE) {
        // doAttacker false, receive client is attacker
        // so targetSelf true, because self == attacker 
        modifyStat(!doAttacker, Stat::ATTACK, -1, bc);
        // announce ability (intimidate)
        applyClient->team[applyClient->battler].bVal.abilityKnownToAi = true;
        if(DEBUG) {
            std::cout << "Intimidate lowers " << logName(receiveClient->team[receiveClient->battler]) << " attack stat by 1" << std::endl;
        }
    }
    applyClient->triggers = 0; // reset all triggers.  TODO: might need to not reset all of them
    return true;
}
bool endOfTurn(BattleContext *bc) {
    bool shouldContinue = true;
    if(isFightOver(bc)){
        return false;
    } else {
        triggerEndTurnConditions(&bc->defender);
        if(bc->defender.team[bc->defender.battler].bVal.bHp <= 0) {
            shouldContinue = switchIn(bc, false);
            bc->defender.isSwitching = true;
        }
        if(shouldContinue) {
            triggerEndTurnConditions(&bc->attacker);
            if(bc->attacker.team[bc->attacker.battler].bVal.bHp <= 0) {
                shouldContinue = switchIn(bc, true);
                bc->attacker.isSwitching = true;
            }
            if(shouldContinue) {
                log("before AI starts >> RNG advances 4");
                if(bc->defender.aiControl & bc->defender.isSwitching & bc->defender.successfulMove) {
                    log("Skipping first AI rng usage due to unknown reason -> ai defender is switching and took its turn");
                } else {
                    advanceSeed(bc);
                }
                advanceSeed(bc);
                advanceSeed(bc);
                advanceSeed(bc); // ai advances the seed 4 for some reason we don't know yet
                processAI(bc);
                return true;
            } else {
                log(bc->defender.name + " Wins");
                bc->defender.isWinner = true;
                return false;
            }
        } else {
            log(bc->attacker.name + " Wins");
            bc->attacker.isWinner = true;
            return false;
        }
    }
}
// returns true if the battle should continue
bool doTurn(BattleContext *bc) {
    if(!determineOrder(bc)) {
        // need to switch current order
        PokeClient pc = bc->attacker;
        bc->attacker = bc->defender;
        bc->defender = pc;
    }
    preTurnEffects(bc, true); // attacker effects
    preTurnEffects(bc, false); // defender effects

    // turn overall start advances 4
    log("Turn Starts > RNG Advances 4"); // speed rand values that we don't really need
    advanceSeed(bc);
    advanceSeed(bc);
    advanceSeed(bc);
    advanceSeed(bc);
    executeCommand(bc);
    PokeClient tempC = bc->attacker;
    bc->attacker = bc->defender;
    bc->defender = tempC;
    if(bc->attacker.team[bc->attacker.battler].bVal.bHp > 0) {
        // only do command if we're still alive
        log("Next battler turn > Rng advances 2");
        advanceSeed(bc);
        advanceSeed(bc);
        executeCommand(bc);
    } else {
        log(bc->attacker.team[bc->attacker.battler].info.name + " Fainted");
    }
    return endOfTurn(bc);
}

static const unsigned long Unk_ov6_02249522[4] = {
    ((1 << 8) | (3 & 0xff)),
    ((11 << 8) | (2 & 0xff)),
    ((12 << 8) | (4 & 0xff)),
    ((29 << 8) | (2 & 0xff))
};

int test() {
    std::cout << 2 << std::endl;
    return 1;
}
struct Tester {
    int val;
};
