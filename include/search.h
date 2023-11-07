#pragma once

#include "bitboard.h"

namespace search
{
    typedef struct
    {
        int eval;
        uint8_t move;
    } Eval;

    const int occupation_eval[42] {
        10, 12, 15, 16, 15, 12, 10,
        10, 12, 15, 16, 15, 12, 10,
        10, 12, 15, 16, 15, 12, 10,
        10, 12, 15, 16, 15, 12, 10,
        10, 12, 15, 16, 15, 12, 10,
        10, 12, 15, 16, 15, 12, 10
    };

    int eval_board(const bitboards::Board& board);

    Eval minimax(const bitboards::Board& board, bool maximizing, uint8_t depth, int alpha, int beta);
}
