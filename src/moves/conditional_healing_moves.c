#include <pokeagb/pokeagb.h>
#include "../battle_data/pkmn_bank.h"
#include "../battle_data/pkmn_bank_stats.h"
#include "../battle_data/battle_state.h"

extern void dprintf(const char * str, ...);
extern bool enqueue_message(u16 move, u8 bank, enum battle_string_ids id, u16 effect);
extern void do_heal(u8 bank_index, u8 heal);
void set_status(u8 bank, enum Effect status);
extern void flat_heal(u8 bank, u16 heal);
extern u8 get_ability(struct Pokemon* p);


/* Purify */
void purify_on_after_move(u8 user, u8 src, u16 move, struct anonymous_callback* acb)
{
    if (user != src) return;
    do_heal(user, 50);
    enqueue_message(NULL, user, STRING_HEAL, 0);
}

u8 purify_on_effect(u8 user, u8 src, u16 move, struct anonymous_callback* acb)
{
    if (user != src) return true;
    if (B_STATUS(user) != AILMENT_NONE) {
        set_status(user, EFFECT_CURE);
        add_callback(CB_ON_AFTER_MOVE, 0, 0, user, (u32)purify_on_after_move);
        return true;
    } else {
        return false;
    }
}

/* Strength sap */
void strength_sap_on_after_move(u8 user, u8 src, u16 move, struct anonymous_callback* acb)
{
    if (user != src) return;
    flat_heal(user, acb->data_ptr);
    enqueue_message(NULL, user, STRING_HEAL, 0);
}

u8 strength_sap_on_effect(u8 user, u8 src, u16 move, struct anonymous_callback* acb)
{
    if (user != src) return true;
    u8 id = add_callback(CB_ON_AFTER_MOVE, 0, 0, user, (u32)strength_sap_on_after_move);
    CB_MASTER[id].data_ptr = B_ATTACK_STAT(TARGET_OF(user));
    return true;
}


/* Sparkling Aria */
u8 sparkling_aria_on_effect(u8 user, u8 src, u16 move, struct anonymous_callback* acb)
{
    if (user != src) return true;
    u8 target = TARGET_OF(user);
    if (B_STATUS(target) == AILMENT_BURN) {
        set_status(target, EFFECT_CURE);
    }
    return true;
}


/* Shore up */
u8 shore_up_on_modify_move(u8 user, u8 src, u16 move, struct anonymous_callback* acb)
{
    if (user != src) return true;
    if (battle_master->field_state.is_sandstorm)
        B_HEAL(user) = 67;
    return true;
}


/* Aromatherapy */
extern void clear_ailments_silent(u8 bank);
u8 aromatherapy_on_effect(u8 user, u8 src, u16 move, struct anonymous_callback* acb)
{
    if (user != src) return true;
    u8 partner_bank;
    if (SIDE_OF(user)) {
        // opp
        partner_bank = (user > 2) ? 2 : 3;
    } else {
        // player
        partner_bank = (user > 0) ? 0 : 1;
    }
    bool success = false;
    struct Pokemon* p;
    struct Pokemon* user_p;
    struct Pokemon* partner;

    // get party & battle side members
    if (SIDE_OF(user)) {
        p =  &party_opponent[0];
        user_p = p_bank[user]->this_pkmn;
        partner = p_bank[((user > 2) ? 3 : 2)]->this_pkmn;
    } else {
        dprintf("executing for player side. %d is partner\n", partner_bank);
        p =  &party_player[0];
        user_p = p_bank[user]->this_pkmn;
        partner = p_bank[((user > 0) ? 0 : 1)]->this_pkmn;
    }

    // Heal party
    for (u8 i = 0; i < 6; i++) {
        // don't check field pokemon
        if ((&p[i] == user_p) || (&p[i] == partner))
            continue;
        // don't heal sapsipper
        if (get_ability(&p[i]) == ABILITY_SAP_SIPPER)
            continue;
        // heal conditions
        if (pokemon_getattr(&p[i], REQUEST_STATUS_AILMENT, NULL)) {
            u8 zero = 0;
            pokemon_setattr(&p[i], REQUEST_STATUS_AILMENT, &zero);
            success = true;
        }
    }

    // heal field members
    dprintf("checking user for ailment\n");
    if (B_STATUS(user) != AILMENT_NONE) {
        clear_ailments_silent(user);
        success = true;
    }
    
    dprintf("checking user for ailment, user was %d\n", success);
    if (B_STATUS(partner_bank) != AILMENT_NONE) {
        clear_ailments_silent(partner_bank);
    }
    if (success) {
        enqueue_message(NULL, user, STRING_SOOTHING_AROMA, 0);
    }
    return success;
}
