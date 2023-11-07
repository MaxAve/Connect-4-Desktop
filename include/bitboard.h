#pragma once

#include <iostream>

#define FULL 0x3ffffffffffULL
#define FULL_COLUMN 0x810203081ULL

namespace bitboards
{
    typedef struct
    {
        uint64_t bitboards[2];
        uint8_t flags;
    } Board;

    extern uint64_t win_masks[69];

    /* Returns row of the free slot over the top of a culumn (returns 6 if column is occupied) */
    uint8_t get_top(const Board& board, uint8_t column);

    /* Places a disc in the specified column and modifies the necessary flags */
    void place_disc(Board& bitboard, uint8_t column);

    // Prints the first 42 digits of the bitboard in binary
    void print_bitboard(const uint64_t& bitboard);

    // Returns winner (1 for player 1, 0 for draw, -1 for player 2)
    int get_winner(const Board& board);
    
    /* Loads bitmasks for all possible Fours */
    void load_win_masks();
}
