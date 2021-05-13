#ifndef KAI_MAZE_PLAYER
#define KAI_MAZE_PLAYER

#include "tile.h"
#include "tileEntityComponents.h"
#include <string>
#include <iostream>

constexpr short INIT_HP = 20,
                INIT_GP = 0,
                INIT_IQ = 0, //haha 0 iq
                INIT_INV_SZ = 12;

class Player{
    
    T_EntityTraits playerTraits;
    EntityMovement playerMovement;
    EntityInventory playerInventory;
    
    Player(point loc {0,0})
    : playerTraits(INIT_HP, INIT_GP, INIT_IQ, color("38;5;203;48;22m@"), true)
    , playerMovement(loc, "walk")
    , EntityInventory(INIT_INV_SZ)
    { }
    
};

#endif