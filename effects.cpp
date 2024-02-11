#include <iostream>
#include <map>
#include <string>
#include <cmath>
#include "battle.h"
#include "battle_effects.h"
bool applyStatus(Pokemon *p, int sc, BattleContext *bc){
    if(p->bVal.condition == 0) {
        p->bVal.condition |= sc;
        return true;
    }
    return false;

}
bool applyVolatileStatus(Pokemon *p, int vc, BattleContext *bc) {
    bool canBeAdded = true;
    if(vc && p->bVal.volConditions){
        canBeAdded = false;
    }
    if(canBeAdded) {
        p->bVal.volConditions |= vc;
        return true;
    }
    return false;
}
int getStatStage(Pokemon *p, Stat stat) {
    switch(stat) {
        case Stat::ATTACK:
            return p->bVal.atkStg;
        case Stat::DEFENSE:
            return p->bVal.defStg;
        case Stat::SPECIAL_ATTACK:
            return p->bVal.spAtkStg;
        case Stat::SPECIAL_DEFENSE:
            return p->bVal.spDefStg;
        case Stat::SPEED:
            return p->bVal.spdStg;
        case Stat::ACCURACY:
            return p->bVal.accStg;
        case Stat::EVASION:
            return p->bVal.evaStg;
        default:
            return 6;
    }
}
bool modifyStat(bool affectSelf, Stat stat, int change, BattleContext *bc) {
    Pokemon* p;
    if(affectSelf) {
        p = &bc->attacker.team[bc->attacker.battler];
    } else {
        if(bc->defender.sideConditions & SIDE_CONDITION_MIST) {
            if(DEBUG) {
                std::cout << "\tStat reduction prevented by Mist" << std::endl;
            }
            return false; // prevented by mist
        }
        p = &bc->defender.team[bc->defender.battler];
    }
    int stageModifier = getStatStage(p, stat);
    if(stageModifier == 12) {
        return false;
    } else if(stageModifier == 0) {
        return false;
    } else {
        stageModifier = stageModifier + change;
        if(stageModifier > 12) {
            stageModifier = 12;
        } else if(stageModifier < 0) {
            stageModifier = 0;
        }
        switch(stat) {
            case Stat::ATTACK:
                p->bVal.atkStg = stageModifier;
                break;
            case Stat::DEFENSE:
                p->bVal.defStg = stageModifier;
                break;
            case Stat::SPECIAL_ATTACK:
                p->bVal.spAtkStg = stageModifier;
                break;
            case Stat::SPECIAL_DEFENSE:
                p->bVal.spDefStg = stageModifier;
                break;
            case Stat::SPEED:
                p->bVal.spdStg = stageModifier;
                break;
            case Stat::ACCURACY:
                p->bVal.accStg = stageModifier;
                break;
            case Stat::EVASION:
                p->bVal.evaStg = stageModifier;
                break;
            default:
                return false;
        }
        return true;

    }
    return false;
}
bool applyBurn(Pokemon *p, BattleContext *bc) {
    if(p->info.primaryType == Type::Fire || p->info.secondaryType == Type::Fire) {
        return false;
    }
    return applyStatus(p, MON_CONDITION_BURN, bc);
}
bool applyPoison(Pokemon *p, BattleContext *bc) {
    if(p->bVal.item == ITEM_PECHA_BERRY) {
        if(DEBUG) {
            std::cout << "\tPecha Berry Prevents Poison" << std::endl;
        }
        p->bVal.item = 0; // consume pecha berry
        return true; // the poisoning effect still technically succeeds
    }
    return applyStatus(p, MON_CONDITION_POISON, bc);
}
bool applyParalysis(Pokemon *p, BattleContext *bc) {
    // TODO: Condition application should check for mitigating items AFTER checking
    // if the condition could even apply.  If you tried to paralyze a pokemon
    // that was already posioned and had a cheri berry this would consume the 
    // berry even though they can't even get paralyzed
    if(p->bVal.item == ITEM_CHERI_BERRY) {
        if(DEBUG) {
            std::cout << "\tCheri Berry Prevents Paralysis" << std::endl;
        }
        p->bVal.item = 0; // consume cheri berry
        return true; // the paralysis effect still technically succeeds
    }
    return applyStatus(p, MON_CONDITION_PARALYSIS, bc);
}
bool applyTaunt(Pokemon *p, BattleContext *bc) {
    if(p->bVal.turnsTaunted > 0) {
        return false;
    } else {
        p->bVal.turnsTaunted = 3 + (advanceSeed(bc) % 3);
    }
    if(DEBUG) {
        std::cout << "\tTaunt will last " << p->bVal.turnsTaunted << " turns" << std::endl;
    }
    return true;
}
bool applyMist(BattleContext *bc) {
    if(bc->attacker.sideConditions & SIDE_CONDITION_MIST) {
        return false; // can't reapply mist until it's over
    } else {
        bc->attacker.mistTurns = 5;
        bc->attacker.sideConditions |= SIDE_CONDITION_MIST;
        return true;
    }
}
bool applyEndeavor(BattleContext *bc) {
    int aHp = bc->attacker.team[bc->attacker.battler].bVal.bHp;
    int dHp = bc->defender.team[bc->defender.battler].bVal.bHp;
    if(aHp < dHp){
        bc->cDmg = dHp - aHp;
        return true;
    }
    return false;
}
bool applyEffect(Move move, BattleContext *bc) {
    switch(move.effect) {
        case BATTLE_EFFECT_BURN_HIT:
        case BATTLE_EFFECT_STATUS_BURN:
            return applyBurn(&bc->defender.team[bc->defender.battler], bc);
        case BATTLE_EFFECT_STATUS_POISON:
            return applyPoison(&bc->defender.team[bc->defender.battler], bc);
        case BATTLE_EFFECT_ATK_DOWN_2:
            return modifyStat(false, Stat::ATTACK, -2, bc);
        case BATTLE_EFFECT_DEF_DOWN_2:
            return modifyStat(false, Stat::DEFENSE, -2, bc);
        case BATTLE_EFFECT_ACC_DOWN:
            return modifyStat(false, Stat::ACCURACY, -1, bc);
        case BATTLE_EFFECT_EVA_UP:
            return modifyStat(true, Stat::EVASION, 1, bc);
        case BATTLE_EFFECT_TAUNT:
            return applyTaunt(&bc->defender.team[bc->defender.battler], bc);
        case BATTLE_EFFECT_PREVENT_STAT_REDUCTION:
            return applyMist(bc);
        case BATTLE_EFFECT_SET_HP_EQUAL_TO_USER:
            return applyEndeavor(bc);
        default:
            return false;
    }
    return false;
}


