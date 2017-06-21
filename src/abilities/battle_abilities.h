#ifndef BATTLE_ABILITIES_TABLE_H_
#define BATTLE_ABILITIES_TABLE_H_

#include <pokeagb/pokeagb.h>
#include "battle_data/pkmn_bank.h"
#include "battle_state.h"
#include "moves/moves.h"

typedef u16 (*StatCallback)(u8, u16); // bank, stat amount
typedef void (*AbilityModMoveCallback)(u8, u16); // bank, move
typedef void (*AbilitySwitchInCallback)(u8);
typedef void (*AbilityResidualCallback)(u8);
typedef u8 (*AbilityTryHitCallback)(u8, u8, u16); // bank, tbank, move
typedef u16 (*AbilityOnDamageCallback)(u8, u8, u16, u16, u8, u16); // bank, tbank, move, damage taken, ability, item
typedef void (*AbilityAfterDamageCallback)(u8, u8, u16, u16, u8, u16); // bank, tbank, move, damage taken, ability, item
typedef void (*AbilityAfterMoveSecondaryCallback)(u8, u8, u16, u8, u16); // bank, tbank, move, ability, item
typedef void (*AbilityModifySecondaryCallback)(u8, u8, u16, u8, u16); // bank, tbank, move, ability, item
typedef void (*AbilityUpdateCallback)(u8);
typedef void (*AbilitySetStatusCallback)(u8, s8, enum Effect); //bank, atkbank, Effect
typedef void (*AbilityOnFaint)(u8, u8); //bank, fainted


typedef u8 (*AbilityOnDraggedOutCallback)(u8);

struct b_ability {
    StatCallback on_priority;
    StatCallback on_attack;
    StatCallback on_defense;
    StatCallback on_speed;
    StatCallback on_sp_attack;
    StatCallback on_sp_defense;
    StatCallback on_accuracy;
    StatCallback on_evasion;
    StatCallback on_critchance;
    
    AbilitySwitchInCallback on_switch;
    AbilityModMoveCallback on_modify_move;  
    AbilityTryHitCallback on_tryhit;
    AbilityOnDamageCallback on_damage; // right before damage is applied. Can modify dmg
    AbilityAfterDamageCallback on_after_damage;
    AbilitySetStatusCallback on_set_status;
    AbilityModifySecondaryCallback on_mod_secondary;
    AbilityAfterMoveSecondaryCallback on_after_move_secondary;
    AbilityResidualCallback on_residual;
    AbilityUpdateCallback on_update;
    AbilityOnFaint on_faint;
    
    AbilityOnDraggedOutCallback on_dragout;
};

#define ABILITIES_MAX 5
extern struct b_ability* abilities_table[11];








#endif /* BATTLE_ABILITIES_TABLE_H_ */