#include "../include/bitboard.h"

uint64_t bitboards::win_masks[69];

uint8_t bitboards::get_top(const Board &board, uint8_t column)
{
    const uint64_t combinedBoard = board.bitboards[0] | board.bitboards[1];
    for(int i = 0; i < 6; i++)
    {
        if(!(combinedBoard & (1ULL << ((i*7) + (6-column)))))
        {
            return i;
        }
    }
    return 6;
}

void bitboards::place_disc(Board &board, uint8_t column)
{
    board.bitboards[!(board.flags & 0x1)] |= (1ULL << (((bitboards::get_top(board, column) * 7)) + (6 - column)));
    board.flags ^= 0x1; // Switch turns
}

void bitboards::print_bitboard(const uint64_t& bitboard)
{
    for(int i = 41; i >= 0; i--)
    {
        if(((i+1) % 7) == 0 && i > 0)
        {
            std::cout << "\n";
        }
        std::cout << ((bitboard & (1ULL << i)) ? "1" : "0") << " ";
    }
    std::cout << "\n";
}

int bitboards::get_winner(const bitboards::Board &board)
{
    for(int i = 0; i < 69; i++)
    {
        if((board.bitboards[0] & bitboards::win_masks[i]) == bitboards::win_masks[i])
        {
            return 1;
        }
        else if((board.bitboards[1] & bitboards::win_masks[i]) == bitboards::win_masks[i])
        {
            return -1;
        }
    }
    return 0;
}

void bitboards::load_win_masks()
{
    // o
    // o
    // o
    // o
    for(int i = 0; i < 21; i++)
    {
        bitboards::win_masks[i] = 0x204081ULL << i;
    }

    // o o o o
    for(int i = 0; i < 6; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            bitboards::win_masks[(i*4+j) + 21] = (0xfULL << (i*7+j));
        }
    }

    //       o
    //     o
    //   o
    // o
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            bitboards::win_masks[(i*4+j) + 45] = (0x208208ULL << (i*7+j));
        }
    }

    // o
    //   o
    //     o
    //       o
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            bitboards::win_masks[(i*4+j) + 57] = (0x1010101ULL << (i*7+j));
        }
    }
}
