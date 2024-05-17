#include <iostream>
#include <map>
#include <string>
#include <cmath>
#include "battle.h"
#include "battle_effects.h"

bool checkMoveEffect(Pokemon *p, int mc) {
    if(p->bVal.substituteHp > 0) {
        return false; // substitute blocks conditions
    }
    if(p->bVal.moveEffectsMask & mc) {
        return false; // cannot apply an effect that's already active
    }
    // normally we would apply the condition here, but some moves have special protocol that isn't standardized, so just do it in the function
    return true;
}
bool applyStatus(Pokemon *p, int sc, BattleContext *bc){
    if(p->bVal.substituteHp > 0) {
        return false; // substitute blocks conditions
    }
    if(p->bVal.condition == 0) {
        p->bVal.condition |= sc;
        return true;
    }
    return false;

}
bool applyVolatileStatus(Pokemon *p, int vc, BattleContext *bc) {
    bool canBeAdded = true;
    if(p->bVal.substituteHp > 0) {
        canBeAdded = false; // substitute blocks most volatile conditions
    }
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
            return true; // prevented by mist // still successful, apparently, though I would think this is not true ?  TODO
        }
        p = &bc->defender.team[bc->defender.battler];
        if(p->bVal.substituteHp > 0) {
            return false; // prevented by substitute
        }
    }
    int stageModifier = getStatStage(p, stat);
    if(stageModifier == 12) {
        return true; // TODO might need more testing, but I'm pretty sure this doesn't count as true failure for the purpose of advancing rng twice, so we return true still
    } else if(stageModifier == 0) {
        return true;
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
    bool appliedBurn = applyStatus(p, MON_CONDITION_BURN, bc);
    return appliedBurn;
}
bool applyPoison(Pokemon *p, BattleContext *bc) {
    bool appliedPoison = applyStatus(p, MON_CONDITION_POISON, bc);
    if(p->bVal.item == ITEM_PECHA_BERRY && appliedPoison) {
        if(DEBUG) {
            std::cout << "\tPecha Berry Prevents Poison" << std::endl;
        }
        p->bVal.item = 0; // consume pecha berry
        p->bVal.condition = 0; // reset condition
        return true; // the poisoning effect still technically succeeds
    }
    return appliedPoison;
}
bool applyParalysis(Pokemon *p, BattleContext *bc) {
    bool appliedParalysis = applyStatus(p, MON_CONDITION_PARALYSIS, bc);
    if(p->bVal.item == ITEM_CHERI_BERRY && appliedParalysis) {
        if(DEBUG) {
            std::cout << "\tCheri Berry Prevents Paralysis" << std::endl;
        }
        p->bVal.item = 0; // consume cheri berry
        p->bVal.condition = 0; // reset condition
        return true; // the paralysis effect still technically succeeds
    }
    return appliedParalysis;
}

bool applyConfusion(Pokemon *p, BattleContext *bc) {
    if(p->bVal.volConditions & VOLATILE_CONDITION_CONFUSION) {
        return false;
    }
    if(p->bVal.substituteHp > 0) {
        return false; // prevented by substitute
    }
    int turnsRemaining = 2 + advanceSeed(bc, "determine how long to be confused for") % 4;
    if(DEBUG) {
        std::cout << "Confused for " << turnsRemaining << " turns" << std::endl;
    }
    turnsRemaining = turnsRemaining << VOLATILE_CONDITION_CONFUSION_SHIFT;
    return applyVolatileStatus(p, turnsRemaining, bc);
}
bool applySleep(Pokemon *p, BattleContext *bc, bool bypass) {
    // 0018 subscript
    if(p->bVal.condition & MON_CONDITION_ANY) {
        return false;
    }
    // bypass substitute if this is a bypassed sleep, eg from yawn
    if(p->bVal.substituteHp > 0 && !bypass) {
        return false; // prevented by substitute
    }
    // TODO logic for various interactions if required eg vital spirit, insomnia

    int turnsRemaining = 2 + advanceSeed(bc, "determine how long to be asleep for") % 4;
    if(DEBUG) {
        std::cout << turnsRemaining << " turns asleep" << std::endl;
    }
    return applyStatus(p, turnsRemaining, bc);
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
bool applySubstitute(BattleContext *bc) {
    Pokemon attacker = bc->attacker.team[bc->attacker.battler];
    if(attacker.bVal.substituteHp == 0) {
        int subHealth = attacker.cHp / 4;
        if(attacker.bVal.bHp > subHealth) {
            dealDamage(&attacker, subHealth, false);
            attacker.bVal.substituteHp = subHealth;
            return true;
        }
    }
    return false;
}
bool applyFlinch(Pokemon *p, BattleContext *bc) {
    bool appliedFlinch = applyVolatileStatus(p, VOLATILE_CONDITION_FLINCH, bc);
    return appliedFlinch;
}
bool applyRecoilQuarter(BattleContext *bc) {
    int recoil = bc->realDmg / 4;
    dealDamage(&bc->attacker.team[bc->attacker.battler], recoil, false); // apply indirect damage equal to a quarter of real damage dealt
    return true; // this effect always succeeds if its triggered -- unless the pokemon has an ability that cancels it but we'll TODO that if it comes up
}

bool applyYawn(BattleContext *bc) {
    bool appliedYawn = checkMoveEffect(&bc->defender.team[bc->defender.battler], MOVE_EFFECT_YAWN);
    if(appliedYawn) {
        bc->defender.team[bc->defender.battler].bVal.moveEffectsMask |= (1 << (MOVE_EFFECT_YAWN_SHIFT + 1));
    }
    return appliedYawn;
}
bool applyToxic(Pokemon *p, BattleContext *bc) {
    bool appliedPoison = applyStatus(p, MON_CONDITION_TOXIC, bc);
    if(p->bVal.item == ITEM_PECHA_BERRY && appliedPoison) {
        if(DEBUG) {
            std::cout << "\tPecha Berry Prevents Poison" << std::endl;
        }
        p->bVal.item = 0; // consume pecha berry
        p->bVal.condition = 0; // reset condition
        return true; // the poisoning effect still technically succeeds
    }
    return appliedPoison;
}
bool applyEffect(Move move, BattleContext *bc) {
    // gigantic switch/case lol
    switch(move.effect) {
        case BATTLE_EFFECT_BURN_HIT:
        case BATTLE_EFFECT_STATUS_BURN:
            return applyBurn(&bc->defender.team[bc->defender.battler], bc);
        case BATTLE_EFFECT_PARALYZE_HIT:
            return applyParalysis(&bc->defender.team[bc->defender.battler], bc);
        case BATTLE_EFFECT_STATUS_POISON:
            return applyPoison(&bc->defender.team[bc->defender.battler], bc);
        case BATTLE_EFFECT_STATUS_CONFUSE:
        case BATTLE_EFFECT_CONFUSE_HIT:
            return applyConfusion(&bc->defender.team[bc->defender.battler], bc);
        case BATTLE_EFFECT_FLINCH_HIT:
            return applyFlinch(&bc->defender.team[bc->defender.battler], bc);
        case BATTLE_EFFECT_ATK_DOWN_2:
            return modifyStat(false, Stat::ATTACK, -2, bc);
        case BATTLE_EFFECT_DEF_DOWN_2:
            return modifyStat(false, Stat::DEFENSE, -2, bc);
        case BATTLE_EFFECT_ACC_DOWN:
            return modifyStat(false, Stat::ACCURACY, -1, bc);
        case BATTLE_EFFECT_LOWER_SP_DEF_HIT:
            return modifyStat(false, Stat::SPECIAL_DEFENSE, -1, bc);
        case BATTLE_EFFECT_EVA_UP:
            return modifyStat(true, Stat::EVASION, 1, bc);
        case BATTLE_EFFECT_TAUNT:
            return applyTaunt(&bc->defender.team[bc->defender.battler], bc);
        case BATTLE_EFFECT_PREVENT_STAT_REDUCTION:
            return applyMist(bc);
        case BATTLE_EFFECT_SET_HP_EQUAL_TO_USER:
            return applyEndeavor(bc);
        case BATTLE_EFFECT_SET_SUBSTITUTE:
            return applySubstitute(bc);
        case BATTLE_EFFECT_RECOIL_QUARTER:
            return applyRecoilQuarter(bc);
        case BATTLE_EFFECT_STATUS_SLEEP_NEXT_TURN: 
            return applyYawn(bc);
        case BATTLE_EFFECT_STATUS_BADLY_POISON:
            return applyToxic(&bc->defender.team[bc->defender.battler], bc);
        default:
            return false;
    }
    return false;
}


