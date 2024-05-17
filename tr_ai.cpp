#include <iostream>
#include <map>
#include <string>
#include <cmath>

#include "battle.h"
#include "battle_effects.h"
#include "tr_ai.h"

void ailog(std::string text) {
    if(DEBUG & 2) {
        std::cout << "AI LOG:: " << text << std::endl;
    }
}
void ailog(int text) {
    if(DEBUG & 2) {
        std::cout << "AI LOG:: " << text << std::endl;
    }
}

bool AI_IfRndUnder(BattleContext *bc, int value) {
    ailog("Checking rand under: [" + std::to_string(value) + "]");
    return ((advanceSeed(bc) % 256) < value);
}
bool AI_IfRndOver(BattleContext *bc, int value) {
    ailog("Checking rand over: [" + std::to_string(value) + "]");
    return ((advanceSeed(bc) % 256) > value);
}
// value is percentage as a whole number (70% = 70), target true when checking player's battler
// defaults to CHECK_ATTACK (check self)
bool AI_IfHpUnder(BattleContext *bc, AiContext *ac, int value, bool target = false) {
    PokeClient pc;
    if(target) {
        pc = ac->target;
    } else {
        pc = ac->self;
    }

    return ((pc.team[pc.battler].bVal.bHp * 100) / pc.team[pc.battler].cHp) < value;
}
// value is percentage as a whole number (70% = 70), target true when checking player's battler
// defaults to CHECK_ATTACK (check self)
bool AI_IfHpOver(BattleContext *bc, AiContext *ac, int value, bool target = false) {
    PokeClient pc;
    if(target) {
        pc = ac->target;
    } else {
        pc = ac->self;
    }

    return ((pc.team[pc.battler].bVal.bHp * 100) / pc.team[pc.battler].cHp) > value;
}

// comparator -> 0 for under, 1 for equal, 2 for over
// stg -> 0/atk,1/def,2/
// value -> what we compare to
// defaults to CHECK_ATTACK
bool AI_IfPara(BattleContext *bc, AiContext *ac, int comparator, Stat stg, int value, bool target = false){
    PokeClient pc;
    if(target) {
        pc = ac->target;
    } else {
        pc = ac->self;
    }
    BattleVal aiBval = pc.team[pc.battler].bVal;
    int currentStage = 0;
    switch(stg){
        case(Stat::ATTACK):
            currentStage = aiBval.atkStg;
            break;
        case(Stat::DEFENSE):
            currentStage = aiBval.defStg;
            break;
        case(Stat::SPECIAL_ATTACK):
            currentStage = aiBval.spAtkStg;
            break;
        case(Stat::SPECIAL_DEFENSE):
            currentStage = aiBval.spDefStg;
            break;
        case(Stat::SPEED):
            currentStage = aiBval.spdStg;
            break;
        case(Stat::ACCURACY):
            currentStage = aiBval.accStg;
            break;
        case(Stat::EVASION):
            currentStage = aiBval.evaStg;
            break;
        default:
            ailog("Error getting stat stage");
            return false;
    }
    if(comparator == PARA_UNDER) {
        return currentStage < value;
    }else if(comparator == PARA_EQUAL) {
        return currentStage == value;
    } else if(comparator == PARA_OVER) {
        return currentStage > value;
    }
    ailog("error comparing stat stages");
    return false;
}

// defaults to CHECK_ATTACK
// check condition (slp, psn, par, brn)
bool AI_IfCond(BattleContext *bc, AiContext *ac, int condition, bool target = false) {
    PokeClient pc;
    if(target) {
        pc = ac->target;
    } else {
        pc = ac->self;
    }
    return pc.team[pc.battler].bVal.condition & condition;
}
// defaults to CHECK_ATTACK
// check move effect mask like aqua ring or leech seed
bool AI_IfMoveEffect(BattleContext *bc, AiContext *ac, int moveEffect, bool target = false) {
    PokeClient pc;
    if(target) {
        pc = ac->target;
    } else {
        pc = ac->self;
    }
    return pc.team[pc.battler].bVal.moveEffectsMask & moveEffect;
}
// defaults to CHECK_ATTACK
// check volatile conditions like confusion 
bool AI_IfVolCondition(BattleContext *bc, AiContext *ac, int volCondition, bool target = false) {
    PokeClient pc;
    if(target) {
        pc = ac->target;
    } else {
        pc = ac->self;
    }
    return pc.team[pc.battler].bVal.volConditions & volCondition;
}

// defaults to IF_FIRST_DEFENCE
bool AI_IfFirst(BattleContext *bc, AiContext *ac, bool target = true) {
    if(determineOrder(bc)) {
        // attacker goes first
        return !target;
    } else {
        // defender goes first
        return target;
    }

}
int AI_DamageCalc(BattleContext *bc, AiContext *ac, Move move, int loss = 0) {

    return calcDamage(bc, move, 1, loss);
}
int AI_CompPowerCalc(BattleContext *bc, AiContext *ac, int *damage) {
    int i = 0;
    int loss = 0;
    int ok,no;
    int maxDamage = 0;
    for(i=0;i<4;i++){
        if(ac->dmgCalcLossFlag && ac->lossCalcOn) {
            damage[i] = ac->dmgCalcLoss[i];
        } else if(ac->dmgCalcNoLossFlag && !ac->lossCalcOn) {
            damage[i] = ac->dmgCalcNoLoss[i];
        } else {
            no=0;
            while(NoCompPowerSeqNo[no]!=0xffff){
                if(ac->self.team[ac->self.battler].moveset[i].effect==NoCompPowerSeqNo[no]){
                    break;
                }
                no++;
            }
            ok=0;
            while(OkCompPowerSeqNo[ok]!=0xffff){
                if(ac->self.team[ac->self.battler].moveset[i].effect==OkCompPowerSeqNo[ok]){
                    break;
                }
                ok++;
            }
            if((OkCompPowerSeqNo[ok]!=0xffff)|| 
            ((ac->self.team[ac->self.battler].moveset[i].id != MoveId::NO_MOVE)&& 
            (NoCompPowerSeqNo[no]==0xffff)&&
            (ac->self.team[ac->self.battler].moveset[i].power > 1))) {
                if(ac->lossCalcOn) {
                    loss = ac->damageLoss[i];
                } else {
                    loss = 0;
                }
                int dmgVal = AI_DamageCalc(bc, ac, ac->self.team[ac->self.battler].moveset[i], loss);
                damage[i] = dmgVal;
                if(ac->lossCalcOn) {
                    ac->dmgCalcLoss[i] = dmgVal;
                } else {
                    ac->dmgCalcNoLoss[i] = dmgVal;
                }
            } else {
                damage[i] = 0;
            }
        }
    }
    if(ac->lossCalcOn) {
        ac->dmgCalcLossFlag = true;
    } else {
        ac->dmgCalcNoLossFlag = true;
    }
    for(i=0;i<4;i++) {
        if(maxDamage < damage[i]) {
            maxDamage = damage[i];
        }
    }
    return maxDamage;
}
// sets ac.currentTypeAdvantage to effectiveness value
// makes an attack of base power 40 with current move's typing
// returns a damage value, one of QUAD_EFFECTIVE, SUPER_EFFECTIVE, NORMAL_EFFECTIVE, NOT_EFFECTIVE, QUAD_NOT_EFFECTIVE, IMMUNE_EFFECTIVE
static void AI_CheckTypeAdvantage(BattleContext *bc, AiContext *ac) {
    int damage = NORMAL_EFFECTIVE;
    int moveStatus = 0;
    damage = getTypeMultiplier(bc, ac->self.team[ac->self.battler].moveset[ac->currentIndex], damage, &moveStatus);
    if(damage == STAB_EFFECTIVE * 2) {
        damage = SUPER_EFFECTIVE;
    } else if(damage == STAB_EFFECTIVE * 4) {
        damage = QUAD_EFFECTIVE;
    } else if(damage == STAB_EFFECTIVE / 2) {
        damage = NOT_EFFECTIVE;
    } else if(damage == STAB_EFFECTIVE / 4) {
        damage = QUAD_NOT_EFFECTIVE;
    }
    // TODO needs flag for certain no-hit scenarios 
    ac->currentTypeAdvantage = damage;
}
static void AI_CompPower(BattleContext *bc, AiContext *ac) {
    int damage[4];
    int i = 0;
    int no,ok;
    no=0;
	while(NoCompPowerSeqNo[no]!=0xffff){
		if(ac->self.team[ac->self.battler].moveset[ac->currentIndex].effect==NoCompPowerSeqNo[no]){
			break;
		}
		no++;
	}
	ok=0;
	while(OkCompPowerSeqNo[ok]!=0xffff){
		if(ac->self.team[ac->self.battler].moveset[ac->currentIndex].effect==OkCompPowerSeqNo[ok]){
			break;
		}
		ok++;
	}
    // TODO: Make sure to handle OkCompPowerSeq moves like gyro ball
    if((OkCompPowerSeqNo[ok]!=0xffff)||
    ((ac->self.team[ac->self.battler].moveset[ac->currentIndex].power > 1) &&(NoCompPowerSeqNo[no]==0xffff))) {
        // deals damage
        AI_CompPowerCalc(bc, ac, &damage[0]);
        for(i = 0; i<4; i++) {
            if(damage[i] > damage[ac->currentIndex]) {
                break;
            }
        }
        // no moves were higher power than current 
        if(i==4) {
            ac->currentPowerFlag = COMP_POWER_MAX;
        } else {
            ac->currentPowerFlag = COMP_POWER_NOT_MAX;
        }
    } else {
        ac->currentPowerFlag = COMP_POWER_NONE;
    }
}

static bool AI_DoesMoveKo(BattleContext *bc, AiContext *ac) {
    int loss;
    int	no,ok;
    int damage = 0;
    if(ac->lossCalcOn) {
        if(ac->dmgCalcLossFlag) {
            if(ac->target.team[ac->target.battler].bVal.bHp <= ac->dmgCalcLoss[ac->currentIndex]) {
                return true;
            } else {
                return false;
            }
        }
        loss = ac->damageLoss[ac->currentIndex];
    } else {
        if(ac->dmgCalcNoLossFlag) {
            if(ac->target.team[ac->target.battler].bVal.bHp <= ac->dmgCalcNoLoss[ac->currentIndex]) {
                return true;
            } else {
                return false;
            }
        }
        loss = 0;
    }
    
	no=0;
	while(NoCompPowerSeqNo[no]!=0xffff){
		if(ac->self.team[ac->self.battler].moveset[ac->currentIndex].effect==NoCompPowerSeqNo[no]){
			break;
		}
		no++;
	}
	ok=0;
	while(OkCompPowerSeqNo[ok]!=0xffff){
		if(ac->self.team[ac->self.battler].moveset[ac->currentIndex].effect==OkCompPowerSeqNo[ok]){
			break;
		}
		ok++;
	}
    if((OkCompPowerSeqNo[ok]!=0xffff)||
	  ((ac->self.team[ac->self.battler].moveset[ac->currentIndex].power>1)&&(NoCompPowerSeqNo[no]==0xffff))){
        damage = AI_DamageCalc(bc, ac, ac->self.team[ac->self.battler].moveset[ac->currentIndex], loss);
        if(ac->target.team[ac->target.battler].bVal.bHp <= damage) {
            return true;
        }
    }
    return false;
}

// only checks opponent's (player/defence's) last move
bool AI_CheckPreviousMoveType(BattleContext *bc, AiContext *ac, DamageType dmgTyp, bool equality = true) {
    if(bc->defender.previouslySelectedMove.defend == dmgTyp) {
        return equality; // if equal, return true if equality is true
    } else {
        return !equality; // if not equal, return true if equality is false
    }
}
bool AI_ShouldSwitch(BattleContext *bc, AiContext *ac){
    int validOptions = 0;
    for(int i=0;i<6;i++){
        if(ac->self.team[i].bVal.bHp > 0) {
            validOptions++;        
        }
    }
    // valid options not counting current battler
    if(validOptions > 1) {
        // we have a valid switch option 
        // perish song TODO
        // wonder guard TODO
        // only ineffective moves TODO
        if(AI_HasAbsorbAbilityInParty(bc, ac)) {
            return false;
        }
        if(AI_HasSuperEffectiveMove(bc, ac, false)) {
            return false;
        }
        // heavily boosted TODO
        // party member super effective TODO

    }
    return false;
}
bool AI_HasAbsorbAbilityInParty(BattleContext *bc, AiContext *ac) {
    // could potentially use 2 random values, but only if the AI
    // has an absorbing ability in their party and we hit them with
    // a move they could possibly absorb
    if(AI_HasSuperEffectiveMove(bc, ac, true) && advanceSeed(bc, "from absorb ability") % 3 != 0) {
        return false;
    }
    return false; // TODO: complete this function if ability absorbing ever becomes a real issue
}
bool AI_HasSuperEffectiveMove(BattleContext *bc, AiContext *ac, bool flag) {
    Move move;
    int moveStatus = 0;
    for(int i = 0; i < 4; i++) {
        move = ac->self.team[ac->self.battler].moveset[i];
        if(move.id != MoveId::NO_MOVE) {
            moveStatus = 0;
            getTypeMultiplier(bc, move, 40, &moveStatus);

            if(moveStatus & MOVE_STATUS_SUPER_EFFECTIVE) {
                if(flag) {
                    return true;
                } else if(advanceSeed(bc, "from ai super effective check") % 10 != 0){
                    return true;
                }
            }
        }
    }
    return false;
}
bool AI_ShouldUseItem(BattleContext *bc, AiContext *ac) {
    int aliveMons = 0;
    int itemCount = 0;
    int i;
    for(i = 0; i < 6; i++) {
        if(ac->self.team[i].bVal.bHp > 0) { 
            aliveMons++;
        }
    }
    int item;
    bool result = false;
    int command = 0;
    for(i = 0; i < 4; i++) {
        if(i == 0 || aliveMons <= ac->self.numUseItems - i + 1) {
            item = ac->self.useItems[i];
            if(item == ITEM_NONE) {
                continue;
            }
            if(item == ITEM_SUPER_POTION) {
                if(ac->self.team[ac->self.battler].bVal.bHp &&
                ac->self.team[ac->self.battler].bVal.bHp < (ac->self.team[ac->self.battler].cHp / 4) ||
                (ac->self.team[ac->self.battler].cHp - ac->self.team[ac->self.battler].bVal.bHp) > 50 ){
                    result = true;
                    command = COMMAND_USE_ITEM_SUPER_POTION;
                }
            }

            if(result == true) {
                bc->attacker.useItems[i] = 0;
                ac->self.command = command;
            }
        }
    }
    return result;
}
int AI_SelectCommand(BattleContext *bc, AiContext *ac) {
    if(AI_ShouldSwitch(bc, ac)) {
        return 1;
    }
    if(AI_ShouldUseItem(bc, ac)) {
        return 2;
    }
    return 3;
}
// returns the selected command code to do 
void processAI(BattleContext *bc) {
    AiContext ac = {};
    int point[4];
	int poswork[4];
	int poscnt;
	int pos=0;
    int i = 0;
    bool isAttacker = false;
    if(bc->attacker.aiControl) {
        ac.self = bc->attacker;
        ac.target = bc->defender;
    } else {
        ac.self = bc->defender;
        ac.target = bc->attacker;
    }
    bc->attacker = ac.self;
    bc->defender = ac.target; // temp reassign battle context for damage calc

    ac.dmgCalcLossFlag = false;
    ac.dmgCalcNoLossFlag = false;
    for(i = 0; i < 4; i++) {
        ac.dmgCalcLoss[i] = 0;
        ac.dmgCalcNoLoss[i] = 0;
        if(ac.self.team[ac.self.battler].moveset[i].id == MoveId::NO_MOVE) {
            ac.score[i] = 0;
        } else {
            if(ac.self.team[ac.self.battler].moveset[i].disabled) {
                ac.score[i] = 0;
            } else {
                ac.score[i] = 100; // start with score of 100 for moves that can be chosen
            }
        }

    }
    int choice = AI_SelectCommand(bc, &ac);
    if(choice == 1) {
        // switch
        // TODO do switching logic here
    } else if(choice == 2) {
        bc->attacker.command = ac.self.command;
    } else {
        if(ac.self.aiLevel > 0) {
            ailog("dmgloss rolls >> Rng advances 4");
            for(i = 0; i < 4; i++) {

                ac.damageLoss[i] = advanceSeed(bc)%16;
                ailog(ac.damageLoss[i]);
            }
        }
        ac.currentIndex = 0;
        ac.currentScore = 0;
        // the order in which AI does its moves is all four moves for basic AI, all four for Strong, all four for expert, ETC
        // the commented out code was made thinking it was done one move at a time for all the AI types
        for(i = 0;i<4;i++) {
            if(ac.self.team[ac.self.battler].moveset[i].id != MoveId::NO_MOVE) {
                ac.currentIndex = i;
                ac.currentScore = ac.score[i];
                if(ac.self.aiLevel & AI_BASIC) {
                    AI_BasicSeq(bc, &ac);
                }
                // if(ac.self.aiLevel & AI_STRONG) {
                //     AI_StrongSeq(bc, &ac);
                // }
                // if(ac.self.aiLevel & AI_EXPERT) {
                //     AI_ExpertSeq(bc, &ac);
                // }
                ac.score[i] = ac.currentScore;
            }
        }
        // temp hack for ai?
        for(i = 0;i<4;i++) {
            if(ac.self.team[ac.self.battler].moveset[i].id != MoveId::NO_MOVE) {
                ac.currentIndex = i;
                ac.currentScore = ac.score[i];
                // if(ac.self.aiLevel & AI_BASIC) {
                //     AI_BasicSeq(bc, &ac);
                // }
                if(ac.self.aiLevel & AI_STRONG) {
                    AI_StrongSeq(bc, &ac);
                }
                // if(ac.self.aiLevel & AI_EXPERT) {
                //     AI_ExpertSeq(bc, &ac);
                // }
                ac.score[i] = ac.currentScore;
            }
        }
        // temp hack for AI?
        for(i = 0;i<4;i++) {
            if(ac.self.team[ac.self.battler].moveset[i].id != MoveId::NO_MOVE) {
                ac.currentIndex = i;
                ac.currentScore = ac.score[i];
                // if(ac.self.aiLevel & AI_BASIC) {
                //     AI_BasicSeq(bc, &ac);
                // }
                // if(ac.self.aiLevel & AI_STRONG) {
                //     AI_StrongSeq(bc, &ac);
                // }
                if(ac.self.aiLevel & AI_EXPERT) {
                    AI_ExpertSeq(bc, &ac);
                }
                ac.score[i] = ac.currentScore;
            }
        }
        ailog(ac.self.team[ac.self.battler].moveset[0].name + ": " + std::to_string(ac.score[0]));
        ailog(ac.self.team[ac.self.battler].moveset[1].name + ": " + std::to_string(ac.score[1]));
        ailog(ac.self.team[ac.self.battler].moveset[2].name + ": " + std::to_string(ac.score[2]));
        ailog(ac.self.team[ac.self.battler].moveset[3].name + ": " + std::to_string(ac.score[3]));
        ailog("pick a move > rng advances 1");
        poscnt=1;
        point[0]=ac.score[0];
        poswork[0]=0;
        for(i=1;i<4;i++){
            if(ac.self.team[ac.self.battler].moveset[i].id != MoveId::NO_MOVE){
                if(point[0]==ac.score[i]){
                    point[poscnt]=ac.score[i];
                    poswork[poscnt++]=i;
                }
                if(point[0]<ac.score[i]){
                    poscnt=1;
                    point[0]=ac.score[i];
                    poswork[0]=i;
                }
            }
        }
        int result = poswork[advanceSeed(bc)%poscnt];
        // return the move we should use
        if(result == 0) {
            pos = COMMAND_MOVE_SLOT_1;
        } else if(result == 1) {
            pos = COMMAND_MOVE_SLOT_2;
        } else if(result == 2) {
            pos = COMMAND_MOVE_SLOT_3;
        } else {
            pos = COMMAND_MOVE_SLOT_4;
        }
        ailog(pos);
        bc->attacker.command = pos;
    }

}
static void AI_INCDEC(AiContext *ac, int val) {
    ac->currentScore = ac->currentScore + val;
    if(ac->currentScore < 0) {
        ac->currentScore = 0;
    }
}
bool AI_DEC1(AiContext *ac) {
    AI_INCDEC(ac, -1);
    return true;
}
bool AI_DEC5(AiContext *ac) {
    AI_INCDEC(ac, -5);
    return true;
}
bool AI_DEC10(AiContext *ac) {
    AI_INCDEC(ac, -10);
    return true;
}
bool AI_DEC12(AiContext *ac) {
    AI_INCDEC(ac, -12);
    return true;
}
int AI_CheckWeather(BattleContext *bc) {
    return bc->weather;
}

// Check ability, defaults to returning player's (defence) ability
AbilityId AI_CheckAbility(BattleContext *bc, AiContext *ac, bool target = true) {
    // TODO: update to work correctly, don't forget to notify AI of ability usage
    // needs to pick randomly between abilities if the 
    // target has two and we don't already know which ability
    // the target has
    PokeClient pc;
    if(target) {
        pc = ac->target;
    } else {
        pc = ac->self;
    }
    if(pc.aiControl || pc.team[pc.battler].bVal.abilityKnownToAi) {
        return pc.team[pc.battler].cAbility; // we know the ability, so return it
    } else {
        // we don't know the ability
        AbilityId abi1 = pc.team[pc.battler].info.ability;
        AbilityId abi2 = pc.team[pc.battler].info.ability2;
        if(abi2 == NO_ABILITY) {
            return abi1;
        } else {
            // has two potential abilities
            ailog("using rng for ability check > advance 1");
            if(advanceSeed(bc)&1) {
                return abi1;
            } else {
                return abi2;
            }
        }
    }
    return pc.team[pc.battler].cAbility;
}
bool AI_BasicDamageEnd(BattleContext *bc, AiContext *ac) {
    if(AI_CheckAbility(bc, ac) != SOUNDPROOF) {
        return AI_CheckEffect(bc, ac);
    }
    if(AI_CheckAbility(bc, ac, false) == MOLD_BREAKER) {
        return AI_CheckEffect(bc, ac);
    }
    switch(ac->self.team[ac->self.battler].moveset[ac->currentIndex].id){
        // discourage sound based moves
        case MoveId::SCREECH:
            return AI_DEC10(ac);
    }
    return AI_CheckEffect(bc, ac);
}
bool AI_BasicDamage(BattleContext *bc, AiContext *ac) {
    // this section is basically useless
    // all the code that did something was commented out
    // i guess they didn't want basic ai encouraging strong moves
    ac->lossCalcOn = false;
    AI_CompPower(bc, ac);
    if(ac->currentPowerFlag == COMP_POWER_NONE) {
        return AI_BasicDamageEnd(bc, ac);
    }
    return AI_BasicDamageEnd(bc, ac);
}

bool AI_BasicDamageStart(BattleContext *bc, AiContext *ac) {
    AI_CheckTypeAdvantage(bc, ac);
    if(ac->currentTypeAdvantage == IMMUNE_EFFECTIVE) {
        return AI_DEC10(ac); // if enemy is immune to our move, don't use it
    }
    if(AI_CheckAbility(bc, ac, false) == MOLD_BREAKER) {
        return AI_BasicDamage(bc, ac);
    };

    AbilityId enemyAbility = AI_CheckAbility(bc, ac);
    if(enemyAbility == VOLT_ABSORB || enemyAbility == MOTOR_DRIVE) {
        if(ac->self.team[ac->self.battler].moveset[ac->currentIndex].moveType == Type::Electric) {
            return AI_DEC12(ac);
        }
    } else if(enemyAbility == WATER_ABSORB) {
        if(ac->self.team[ac->self.battler].moveset[ac->currentIndex].moveType == Type::Water) {
            return AI_DEC12(ac);
        }
    } else if(enemyAbility == FLASH_FIRE) {
        if(ac->self.team[ac->self.battler].moveset[ac->currentIndex].moveType == Type::Fire) {
            return AI_DEC12(ac);
        }
    } else if(enemyAbility == WONDER_GUARD) {
        if(ac->currentTypeAdvantage == SUPER_EFFECTIVE || ac->currentTypeAdvantage == QUAD_EFFECTIVE) {
            return AI_BasicDamage(bc, ac);
        } else {
            return AI_DEC12(ac);
        }
    } else if(enemyAbility == LEVITATE) {
        if(ac->self.team[ac->self.battler].moveset[ac->currentIndex].moveType == Type::Ground) {
            return AI_DEC12(ac);
        }
    }

    return AI_BasicDamage(bc, ac);
}

bool AI_BasicSeq(BattleContext *bc, AiContext *ac) {
    ac->lossCalcOn = false;
    // TODO: missing horn drill and fissure calcs
    AI_CompPower(bc, ac);
    if(ac->currentPowerFlag == COMP_POWER_NONE) {
        return AI_BasicDamageEnd(bc, ac);
    } else {
        return AI_BasicDamageStart(bc, ac);
    }
    return AI_BasicDamageStart(bc, ac);
}



bool AI_CheckEffect(BattleContext *bc, AiContext *ac) {
    switch(ac->self.team[ac->self.battler].moveset[ac->currentIndex].effect) {
        case(BATTLE_EFFECT_STATUS_POISON): // 66
            return AI_BasicPoison(bc, ac); 
        case(BATTLE_EFFECT_ATK_DOWN): // 18
        case(BATTLE_EFFECT_ATK_DOWN_2): // 58
            return AI_BasicAttackDown(bc, ac);
        case(BATTLE_EFFECT_DEF_DOWN_2): // 59
            return AI_BasicDefenseDown(bc, ac); 
        case(BATTLE_EFFECT_ACC_DOWN): //23
            return AI_BasicAccuracyDown(bc, ac);
        case(BATTLE_EFFECT_EVA_UP): // 16
            return AI_BasicEvaUp(bc, ac);
        case(BATTLE_EFFECT_STATUS_CONFUSE): // 49
            return AI_BasicConfuse(bc, ac);
        case(BATTLE_EFFECT_SET_HP_EQUAL_TO_USER):
            return AI_BasicRiskyDamage(bc, ac);
        default:
            return true;
    }
}

bool AI_BasicConfuse(BattleContext *bc, AiContext *ac) {
    if(AI_IfVolCondition(bc, ac, VOLATILE_CONDITION_CONFUSION, true)) {
        return AI_DEC5(ac);
    }
    if(AI_CheckAbility(bc, ac) == OWN_TEMPO){
        return AI_DEC10(ac);
    }
    if(ac->target.sideConditions & SIDE_CONDITION_SAFEGUARD) {
        return AI_DEC10(ac);
    }
    return true;
    
}
bool AI_BasicRiskyDamage(BattleContext *bc, AiContext *ac) {
    AI_CheckTypeAdvantage(bc, ac);
    if(ac->currentTypeAdvantage == IMMUNE_EFFECTIVE) {
        return AI_DEC10(ac);
    }
    AbilityId abi = AI_CheckAbility(bc, ac); // get enemy ability
    if(abi==WONDER_GUARD) {
        AbilityId abi2 = AI_CheckAbility(bc, ac, false); // get our ability
        if(abi2 == MOLD_BREAKER) { // pokemon programmers in shambles over this ability
            return true; 
        }
        if(ac->currentTypeAdvantage == SUPER_EFFECTIVE) {
            return true;
        }
        if(ac->currentTypeAdvantage == QUAD_EFFECTIVE) {
            return true;
        }
        return AI_DEC10(ac);
    } else {
        return true;
    }
}
bool AI_BasicEvaUp(BattleContext *bc, AiContext *ac) {
    AbilityId abi = AI_CheckAbility(bc, ac);
    if(abi == NO_GUARD) {
        return AI_DEC10(ac);
    }
    AbilityId abi2 = AI_CheckAbility(bc, ac, false);
    if(abi == NO_GUARD) {
        return AI_DEC10(ac);
    }
    if(abi != SIMPLE){
        if(AI_IfPara(bc, ac, PARA_EQUAL, Stat::EVASION, 12)) {
            return AI_DEC10(ac);
        }
        return true;
    }
    if(AI_IfPara(bc, ac, PARA_OVER, Stat::EVASION, 8)){
        return AI_DEC10(ac);
    }
    return true;
}
bool AI_BasicPoison(BattleContext *bc, AiContext *ac) {
    if(ac->target.team[ac->target.battler].info.primaryType == Type::Steel || ac->target.team[ac->target.battler].info.primaryType == Type::Poison) {
        return AI_DEC10(ac);
    }
    if(ac->target.team[ac->target.battler].info.secondaryType == Type::Steel || ac->target.team[ac->target.battler].info.secondaryType == Type::Poison) {
        return  AI_DEC10(ac);
    }
    AbilityId abi = AI_CheckAbility(bc, ac);
    if(abi == IMMUNITY || abi == MAGIC_GUARD || abi == POISON_HEAL) {
        return AI_DEC10(ac);
    }
    if(abi == LEAF_GUARD) {
        if(AI_CheckWeather(bc) & FIELD_CONDITION_SUNNY) {
            return AI_DEC10(ac);
        }
    }
    return AI_BasicPoison_NoLeafGuard(bc, ac);
}

bool AI_BasicPoison_NoLeafGuard(BattleContext *bc, AiContext *ac) {
    AbilityId abi = AI_CheckAbility(bc, ac);
    if(abi == HYDRATION) {
        if(AI_CheckWeather(bc) & FIELD_CONDITION_RAINING) {
            return AI_DEC10(ac);
        }
    }
    return AI_BasicPoison_NoHydration(bc, ac);
}
bool AI_BasicPoison_NoHydration(BattleContext *bc, AiContext *ac) {
    if(ac->target.team[ac->target.battler].bVal.condition & MON_CONDITION_ANY) {
        return AI_DEC10(ac);
    }
    if(ac->target.sideConditions & SIDE_CONDITION_SAFEGUARD) {
        return AI_DEC10(ac);
    }
    return true; // aiend
}

bool AI_BasicDefenseDown(BattleContext *bc, AiContext *ac) {
    if(ac->target.team[ac->target.battler].bVal.defStg == 0) {
        return AI_DEC10(ac);
    }
    return AI_BasicClearBody(bc, ac);
}
bool AI_BasicAccuracyDown(BattleContext *bc, AiContext *ac) {
    if(ac->target.team[ac->target.battler].bVal.accStg == 0) {
        return AI_DEC10(ac);
    }
    AbilityId abiSelf = AI_CheckAbility(bc, ac, false);
    if(abiSelf == NO_GUARD) {
        return AI_DEC10(ac);
    }
    AbilityId abiTarget = AI_CheckAbility(bc, ac);
    if(abiTarget == NO_GUARD || abiTarget == KEEN_EYE) {
        return AI_DEC10(ac);
    }
    return AI_BasicClearBody(bc, ac);
}

bool AI_BasicAttackDown(BattleContext *bc, AiContext *ac) {
    if(AI_IfPara(bc, ac, 1, Stat::ATTACK, 0, true)) {
        // check defence atk stat
        return AI_DEC10(ac);
    }
    AbilityId aid = AI_CheckAbility(bc, ac);
    if(aid == AbilityId::HYPER_CUTTER) {
        return AI_DEC10(ac);
    }
    return AI_BasicClearBody(bc, ac);
}

bool AI_BasicClearBody(BattleContext *bc, AiContext *ac) {
    AbilityId aid = AI_CheckAbility(bc, ac);
    if(aid == CLEAR_BODY || aid == WHITE_SMOKE) {
        return AI_DEC10(ac);
    }
    return true; // aiend
}

// ===========
// Strong AI
// ===========
bool AI_StrongKO_Priority(BattleContext *bc, AiContext *ac){
    AI_INCDEC(ac, 2);
    return AI_StrongKO2(bc, ac);
}
bool AI_StrongKO_Minor(BattleContext *bc, AiContext *ac) {
    if(AI_IfRndUnder(bc, 170)) {
        return true;
    }
    return AI_StrongKO2(bc, ac);
}
bool AI_StrongKO2(BattleContext *bc, AiContext *ac) {
    AI_INCDEC(ac, 4);
    return true;
}
bool AI_StrongKO(BattleContext *bc, AiContext *ac) {
    int cMoveEffect = ac->self.team[ac->self.battler].moveset[ac->currentIndex].effect;
    if(cMoveEffect == BATTLE_EFFECT_HALVE_DEFENSE) {
        return true; // do not encourage explosion/self destruct
    } else if(cMoveEffect == BATTLE_EFFECT_HIT_LAST_WHIFF_IF_HIT ||
    cMoveEffect == BATTLE_EFFECT_HIT_FIRST_IF_TARGET_ATTACKING ||
    cMoveEffect == BATTLE_EFFECT_HIT_IN_3_TURNS) {
        return AI_StrongKO_Minor(bc, ac);
    } else if(cMoveEffect == BATTLE_EFFECT_PRIORITY_1) {
        return AI_StrongKO_Priority(bc, ac);
    }
    return AI_StrongKO2(bc, ac);
}
bool AI_StrongSuperEffective(BattleContext *bc, AiContext *ac) {
    AI_CheckTypeAdvantage(bc, ac);
    if(ac->currentTypeAdvantage == QUAD_EFFECTIVE) {
        if(AI_IfRndUnder(bc, 80)){
            return true;
        } else {
            AI_INCDEC(ac, 2);
            return true;
        }
    }
    return true;
}
bool AI_StrongMoveCheck_1(BattleContext *bc, AiContext *ac) {
    if(AI_IfRndUnder(bc, 51)) {
        return AI_StrongSuperEffective(bc, ac);
    }
    AI_INCDEC(ac, -2);
    return AI_StrongSuperEffective(bc, ac);
}
bool AI_StrongMoveCheck(BattleContext *bc, AiContext *ac) {
    int cMoveEffect = ac->self.team[ac->self.battler].moveset[ac->currentIndex].effect;
    if(cMoveEffect == BATTLE_EFFECT_HALVE_DEFENSE ||
    cMoveEffect == BATTLE_EFFECT_HIT_LAST_WHIFF_IF_HIT ||
    cMoveEffect == BATTLE_EFFECT_HIT_FIRST_IF_TARGET_ATTACKING) {
        return AI_StrongMoveCheck_1(bc, ac);
    }
    return AI_StrongSuperEffective(bc, ac);
}
bool AI_StrongSeq(BattleContext *bc, AiContext *ac) {
    ac->lossCalcOn = false;
    if(AI_DoesMoveKo(bc, ac)) {
        return AI_StrongKO(bc, ac);
    }
    AI_CompPower(bc, ac);
    if(ac->currentPowerFlag == COMP_POWER_NOT_MAX) {
        return AI_DEC1(ac);
    }
    return AI_StrongMoveCheck(bc, ac);
}

// ===========
// ExpertAi
// ===========

bool AI_ExpertSeq(BattleContext *bc, AiContext *ac) {
    switch(ac->self.team[ac->self.battler].moveset[ac->currentIndex].effect) {
        case(BATTLE_EFFECT_BYPASS_ACCURACY):
            return AI_ExpertBypassAccuracy(bc, ac); // 17
        case(BATTLE_EFFECT_STATUS_POISON): // 66
            return AI_ExpertPoison(bc, ac);
        case(BATTLE_EFFECT_STATUS_CONFUSE): // 49
            return AI_ExpertConfuse(bc, ac);
        case(BATTLE_EFFECT_ATK_DOWN_2): // 58
            return AI_ExpertAttackDown(bc, ac);
        case(BATTLE_EFFECT_DEF_DOWN_2): // 59
            return AI_ExpertDefenceDown(bc, ac);
        case(BATTLE_EFFECT_ACC_DOWN): // 23
            return AI_ExpertAccuracyDown(bc, ac);
        case(BATTLE_EFFECT_HIGH_CRITICAL):
        case(BATTLE_EFFECT_HIGH_CRITICAL_BURN_HIT):
        case(BATTLE_EFFECT_HIGH_CRITICAL_POISON_HIT):
            return AI_ExpertHighCritical(bc, ac);
        case(BATTLE_EFFECT_SET_HP_EQUAL_TO_USER):
            return AI_ExpertEndeavor(bc, ac);

        default:
            return true;
    }
}

bool AI_ExpertBypassAccuracy(BattleContext *bc, AiContext *ac) {
    // check if target has high evasion
    if(AI_IfPara(bc, ac, 2, Stat::EVASION, 10, true)) {
        return AI_ExpertBypassAccuracy_1(bc, ac);
    }
    // check if attacker has low accuracy
    if(AI_IfPara(bc, ac, 0, Stat::ACCURACY, 2)) {
        return AI_ExpertBypassAccuracy_1(bc, ac);
    }
    if(AI_IfPara(bc, ac, 2, Stat::EVASION, 8, true)) {
        return AI_ExpertBypassAccuracy_2(bc, ac);
    }
    if(AI_IfPara(bc, ac, 0, Stat::ACCURACY, 4)) {
        return AI_ExpertBypassAccuracy_2(bc, ac);
    }
    return true; // aiend
}
bool AI_ExpertBypassAccuracy_1(BattleContext *bc, AiContext *ac) {
    AI_INCDEC(ac, 1);
    return AI_ExpertBypassAccuracy_2(bc, ac);
}
bool AI_ExpertBypassAccuracy_2(BattleContext *bc, AiContext *ac) {
    if(AI_IfRndUnder(bc, 100)) {
        return true; // aiend
    }
    AI_INCDEC(ac, 1);
    return true;
}
bool AI_ExpertConfuse(BattleContext *bc, AiContext *ac){
    if(AI_IfHpOver(bc, ac, 70, true)) {
        return AI_ExpertConfuse_end(bc, ac);
    }
    if(AI_IfRndUnder(bc, 128)) {
        return AI_ExpertConfuse_1(bc, ac);
    }
    AI_INCDEC(ac, -1);
    return AI_ExpertConfuse_1(bc, ac);
}
bool AI_ExpertConfuse_1(BattleContext *bc, AiContext *ac) {
    if(AI_IfHpOver(bc, ac, 50, true)) {
        return AI_ExpertConfuse_end(bc, ac);
    }
    AI_INCDEC(ac, -1);
    if(AI_IfHpOver(bc, ac, 30, true)) {
        return AI_ExpertConfuse_end(bc, ac);
    }
    AI_INCDEC(ac, -1);
    return AI_ExpertConfuse_end(bc, ac);
}
bool AI_ExpertConfuse_end(BattleContext *bc, AiContext *ac) {
    return true;
}


bool AI_ExpertAttackDown(BattleContext *bc, AiContext *ac) {
    if(AI_IfPara(bc, ac, 1, Stat::ATTACK, 6, true)) {
        return AI_ExpertAttackDown_2(bc, ac);
    }
    AI_INCDEC(ac, -1);
    if(AI_IfHpOver(bc, ac, 90)) {
        return AI_ExpertAttackDown_1(bc, ac);
    }
    AI_INCDEC(ac, -1);
    return AI_ExpertAttackDown_1(bc, ac);
}
bool AI_ExpertAttackDown_1(BattleContext *bc, AiContext *ac) {
    if(AI_IfPara(bc, ac, 2, Stat::ATTACK, 3, true)) {
        return AI_ExpertAttackDown_2(bc, ac);
    }
    if(AI_IfRndUnder(bc, 50)) {
        return AI_ExpertAttackDown_2(bc, ac);
    }
    AI_INCDEC(ac, -2);
    return AI_ExpertAttackDown_2(bc, ac);
}
bool AI_ExpertAttackDown_2(BattleContext *bc, AiContext *ac) {
    if(AI_IfHpOver(bc, ac, 70, true)) {
        return AI_ExpertAttackDown_3(bc, ac);
    }
    AI_INCDEC(ac, -2);
    return AI_ExpertAttackDown_3(bc, ac);
}
bool AI_ExpertAttackDown_3(BattleContext *bc, AiContext *ac) {
    // need to implement lastWazaKind
    if(AI_CheckPreviousMoveType(bc, ac, DamageType::SPECIAL, false)) {
        return true;
    }
    if(AI_IfRndUnder(bc, 128)) {
        return true;
    }
    AI_INCDEC(ac, -2);
    return true;
}
bool AI_ExpertEndeavor(BattleContext *bc, AiContext *ac) {
    if(AI_IfHpUnder(bc, ac, 70, true)) {
        return AI_ExpertEndeavor_2(bc, ac);
    }
    if(AI_IfFirst(bc, ac)) {
        return AI_ExpertEndeavor_1(bc, ac);
    }
    if(AI_IfHpOver(bc, ac, 40)) {
        return AI_ExpertEndeavor_2(bc, ac);
    }
    AI_INCDEC(ac, 1);
    return true; // aiend
}

bool AI_ExpertEndeavor_1(BattleContext *bc, AiContext *ac) {
    if(AI_IfHpOver(bc, ac, 50)) {
        return AI_ExpertEndeavor_2(bc, ac);
    }
    AI_INCDEC(ac, 1);
    return true; // aiend
}
bool AI_ExpertEndeavor_2(BattleContext *bc, AiContext *ac) {
    AI_INCDEC(ac, -1);
    return true; // aiend
}

bool AI_ExpertHighCritical_1(BattleContext *bc, AiContext *ac) {
    ailog("using rand for high critical_1 128");
    if(AI_IfRndUnder(bc, 128)){
        return true;
    }
    AI_INCDEC(ac, 1);
    return true;
}
bool AI_ExpertHighCritical(BattleContext *bc, AiContext *ac) {
    AI_CheckTypeAdvantage(bc, ac);
    if(ac->currentTypeAdvantage == IMMUNE_EFFECTIVE ||
    ac->currentTypeAdvantage == QUAD_NOT_EFFECTIVE ||
    ac->currentTypeAdvantage == NOT_EFFECTIVE) {
        return true;
    }
    if(ac->currentTypeAdvantage == SUPER_EFFECTIVE ||
    ac->currentTypeAdvantage == QUAD_EFFECTIVE) {
        return AI_ExpertHighCritical_1(bc, ac);
    }
    ailog("using rand for high critical 128");
    if(AI_IfRndUnder(bc, 128)) {
        return true;
    }
    return AI_ExpertHighCritical_1(bc, ac);
}
bool AI_ExpertAccuracyDown(BattleContext *bc, AiContext *ac) {
    if(AI_IfHpUnder(bc, ac, 70)) {
        return AI_ExpertAccuracyDown_2(bc, ac);
    }
    if(AI_IfHpOver(bc, ac, 70, true)) {
        return AI_ExpertAccuracyDown_3(bc, ac);
    }
    return AI_ExpertAccuracyDown_2(bc, ac);
}
bool AI_ExpertAccuracyDown_2(BattleContext *bc, AiContext *ac) {
    if(AI_IfRndUnder(bc, 100)) {
        return AI_ExpertAccuracyDown_3(bc, ac);
    }
    AI_INCDEC(ac, -1);
    return AI_ExpertAccuracyDown_3(bc, ac);
}
bool AI_ExpertAccuracyDown_3(BattleContext *bc, AiContext *ac) {
    if(ac->self.team[ac->self.battler].bVal.accStg > 4) {
        return AI_ExpertAccuracyDown_4(bc, ac);
    }
    if(AI_IfRndUnder(bc, 80)) {
        return AI_ExpertAccuracyDown_4(bc, ac);
    }
    AI_INCDEC(ac, -2);
    return AI_ExpertAccuracyDown_4(bc, ac);
}
bool AI_ExpertAccuracyDown_4(BattleContext *bc, AiContext *ac) {
    // if not toxic
    if((ac->target.team[ac->target.battler].bVal.condition & MON_CONDITION_TOXIC) == 0) {
        return AI_ExpertAccuracyDown_5(bc, ac);
    }
    if(AI_IfRndUnder(bc, 70)) {
        return AI_ExpertAccuracyDown_5(bc, ac);
    }
    AI_INCDEC(ac, 2);
    return AI_ExpertAccuracyDown_5(bc, ac);
}
bool AI_ExpertAccuracyDown_5(BattleContext *bc, AiContext *ac) {
    // if not leech seed
    if((ac->target.team[ac->target.battler].bVal.moveEffectsMask & MOVE_EFFECT_LEECH_SEED) == 0) {
        return AI_ExpertAccuracyDown_6(bc, ac);
    }
    if(AI_IfRndUnder(bc, 70)) {
        return AI_ExpertAccuracyDown_6(bc, ac);
    }
    AI_INCDEC(ac, 2);
    return AI_ExpertAccuracyDown_6(bc, ac);
}
bool AI_ExpertAccuracyDown_6(BattleContext *bc, AiContext *ac) {
    // if not ingrained
    if((ac->self.team[ac->self.battler].bVal.moveEffectsMask & MOVE_EFFECT_INGRAIN) == 0) {
        return AI_ExpertAccuracyDown_6_1(bc, ac);
    }
    ailog("using rand for acc down_6 128");
    if(AI_IfRndUnder(bc, 128)) {
        return AI_ExpertAccuracyDown_7(bc, ac);
    }
    AI_INCDEC(ac, 1);
    return AI_ExpertAccuracyDown_7(bc, ac);
}
bool AI_ExpertAccuracyDown_6_1(BattleContext *bc, AiContext *ac) {
    if((ac->self.team[ac->self.battler].bVal.moveEffectsMask & MOVE_EFFECT_AQUA_RING) == 0) {
        return AI_ExpertAccuracyDown_7(bc, ac);
    }
    ailog("using rand for acc down 6_1 128");
    if(AI_IfRndUnder(bc, 128)) {
        return AI_ExpertAccuracyDown_7(bc, ac);
    }
    AI_INCDEC(ac, 1);
    return AI_ExpertAccuracyDown_7(bc, ac);
}
bool AI_ExpertAccuracyDown_7(BattleContext *bc, AiContext *ac) {
    if((ac->self.team[ac->self.battler].bVal.volConditions & VOLATILE_CONDITION_CURSE) == 0) {
        return AI_ExpertAccuracyDown_8(bc, ac);
    }
    if(AI_IfRndUnder(bc, 70)){
        return AI_ExpertAccuracyDown_8(bc, ac);
    }
    AI_INCDEC(ac, 2);
    return AI_ExpertAccuracyDown_8(bc, ac);

}
bool AI_ExpertAccuracyDown_8(BattleContext *bc, AiContext *ac) {
    if(AI_IfHpOver(bc, ac, 70)){
        return AI_ExpertAccuracyDown_End(bc, ac);
    }
    if(ac->target.team[ac->target.battler].bVal.accStg == 6) {
        return AI_ExpertAccuracyDown_End(bc, ac);
    }
    if(AI_IfHpUnder(bc, ac, 40)) {
        return AI_ExpertAccuracyDown_9(bc, ac);
    }
    if(AI_IfHpUnder(bc, ac, 40, true)) {
        return AI_ExpertAccuracyDown_9(bc, ac);
    }
    if(AI_IfRndUnder(bc, 70)) {
        return AI_ExpertAccuracyDown_End(bc, ac);
    }
    return AI_ExpertAccuracyDown_9(bc, ac);
}
bool AI_ExpertAccuracyDown_9(BattleContext *bc, AiContext *ac) {
    AI_INCDEC(ac, -2);
    return AI_ExpertAccuracyDown_End(bc, ac);
}
bool AI_ExpertAccuracyDown_End(BattleContext *bc, AiContext *ac) {
    return true;
}

bool AI_ExpertPoison(BattleContext *bc, AiContext *ac){
    if(AI_IfHpUnder(bc, ac, 50)) {
        AI_INCDEC(ac, -1);
        return true;
    }
    if(AI_IfHpOver(bc, ac, 50, true)) {
        return true;
    }
    AI_INCDEC(ac, -1);
    return true;
}

bool AI_ExpertDefenceDown(BattleContext *bc, AiContext *ac){
    if(AI_IfHpUnder(bc, ac, 70)) {
        return AI_ExpertDefenceDown_1(bc, ac);
    }
    if(ac->target.team[ac->target.battler].bVal.defStg > 3) {
        return AI_ExpertDefenceDown_2(bc, ac);
    }
    return AI_ExpertDefenceDown_1(bc, ac);
}
bool AI_ExpertDefenceDown_1(BattleContext *bc, AiContext *ac){
    if(AI_IfRndUnder(bc, 50)) {
        return AI_ExpertDefenceDown_2(bc, ac);
    }
    AI_INCDEC(ac, -2);
    return AI_ExpertDefenceDown_2(bc, ac);
}
bool AI_ExpertDefenceDown_2(BattleContext *bc, AiContext *ac){
    if(AI_IfHpOver(bc, ac, 70, true)) {
        return AI_ExpertAccuracyDown_End(bc, ac);
    }
    AI_INCDEC(ac, -2);
    return AI_ExpertDefenceDown_End(bc, ac);
}
bool AI_ExpertDefenceDown_End(BattleContext *bc, AiContext *ac){
    return true;
}

// Evasion up (16)

bool AI_ExpertEvasionUp(BattleContext *bc, AiContext *ac) {
    if(AI_IfHpUnder(bc, ac, 90)) {
        return AI_ExpertEvasionUp_1(bc, ac);
    }
    if(AI_IfRndUnder(bc, 100)) {
        return AI_ExpertEvasionUp_1(bc, ac);
    }
    AI_INCDEC(ac, 3);
    return AI_ExpertEvasionUp_1(bc, ac);
}

bool AI_ExpertEvasionUp_1(BattleContext *bc, AiContext *ac) {
    if(AI_IfPara(bc, ac, 0, Stat::EVASION, 9)){
        return AI_ExpertEvasionUp_2(bc, ac);
    };
    if(AI_IfRndUnder(bc, 128)){
        return AI_ExpertEvasionUp_2(bc, ac);
    }
    AI_INCDEC(ac, -1);
    return AI_ExpertEvasionUp_2(bc, ac);
}

bool AI_ExpertEvasionUp_2(BattleContext *bc, AiContext *ac) {
    if(!AI_IfCond(bc, ac, MON_CONDITION_TOXIC, true)){
        return AI_ExpertEvasionUp_5(bc, ac);
    }
    if(AI_IfHpOver(bc, ac, 50)) {
        return AI_ExpertEvasionUp_4(bc, ac);
    }
    if(AI_IfRndUnder(bc, 80)) {
        return AI_ExpertEvasionUp_5(bc, ac);
    }
    return AI_ExpertEvasionUp_4(bc, ac);
}

bool AI_ExpertEvasionUp_4(BattleContext *bc, AiContext *ac) {
    if(AI_IfRndUnder(bc, 50)) {
        return AI_ExpertEvasionUp_5(bc, ac);
    }
    AI_INCDEC(ac, 3);
    return AI_ExpertEvasionUp_5(bc, ac);
}

bool AI_ExpertEvasionUp_5(BattleContext *bc, AiContext *ac) {
    if(!AI_IfMoveEffect(bc, ac, MOVE_EFFECT_LEECH_SEED, true)) {
        return AI_ExpertEvasionUp_6(bc, ac);
    }
    if(AI_IfRndUnder(bc, 70)) {
        return AI_ExpertEvasionUp_6(bc, ac);
    }
    AI_INCDEC(ac, 3);
    return AI_ExpertEvasionUp_6(bc, ac);
}

bool AI_ExpertEvasionUp_6(BattleContext *bc, AiContext *ac) {
    if(!AI_IfMoveEffect(bc, ac, MOVE_EFFECT_INGRAIN)) {
        if(AI_IfMoveEffect(bc, ac, MOVE_EFFECT_AQUA_RING)) {
            if(AI_IfRndUnder(bc, 128)) {
                return AI_ExpertEvasionUp_7(bc, ac);
            }
            AI_INCDEC(ac, 2);
        }
        return AI_ExpertEvasionUp_7(bc, ac);
    }
    if(AI_IfRndUnder(bc, 128)) {
        return AI_ExpertEvasionUp_7(bc, ac);
    }
    AI_INCDEC(ac, 2);
    return AI_ExpertEvasionUp_7(bc, ac);
}

bool AI_ExpertEvasionUp_7(BattleContext *bc, AiContext *ac) {
    if(AI_IfVolCondition(bc, ac, VOLATILE_CONDITION_CURSE, true)) {
        if(AI_IfRndUnder(bc, 70)) {
            return AI_ExpertEvasionUp_8(bc, ac);
        }
        AI_INCDEC(ac, 3);
    }
    return AI_ExpertEvasionUp_8(bc, ac);
}

bool AI_ExpertEvasionUp_8(BattleContext *bc, AiContext *ac) {
    if(AI_IfHpOver(bc, ac, 70)) {
        return AI_ExpertEvasionUp_end(bc, ac);
    }
    if(AI_IfPara(bc, ac, 1, Stat::EVASION, 6)) {
        return AI_ExpertEvasionUp_end(bc, ac);
    }
    if(AI_IfHpUnder(bc, ac, 40)) {
        return AI_ExpertEvasionUp_9(bc, ac);
    }
    if(AI_IfHpUnder(bc, ac, 40, true)) {
        return AI_ExpertEvasionUp_9(bc, ac);
    }
    if(AI_IfRndUnder(bc, 70)) {
        return AI_ExpertEvasionUp_end(bc, ac);
    }
    return AI_ExpertEvasionUp_9(bc, ac);
}
bool AI_ExpertEvasionUp_9(BattleContext *bc, AiContext *ac) {
    AI_INCDEC(ac, -2);
    return AI_ExpertEvasionUp_end(bc, ac);
}

bool AI_ExpertEvasionUp_end(BattleContext *bc, AiContext *ac) {
    return true;
}