#include "../include/search.h"

int search::eval_board(const bitboards::Board &board)
{
    int eval = 0;
    for(int i = 0; i < 42; i++)
    {
        if(board.bitboards[0] & (1ULL << i))
            eval += search::occupation_eval[i];
        else if(board.bitboards[1] & (1ULL << i))
            eval -= search::occupation_eval[i];
    }
    return eval;
}

search::Eval search::minimax(const bitboards::Board& board, bool maximizing, uint8_t depth, int alpha, int beta)
{
    const int winner = bitboards::get_winner(board);

    if(depth == 0 || winner != 0) // TODO add check to see if board is full
    {
        return {(winner * 100) + search::eval_board(board) + (maximizing ? -((int)depth) : (int)depth), 0};
    }

    search::Eval best_eval;

    const uint64_t cb = board.bitboards[0] | board.bitboards[1];

    if(maximizing)
    {
        best_eval = {INT32_MIN, 0};

        for(int i = 0; i < 7; i++)
        {
            const uint64_t fc = (FULL_COLUMN << (6-i));

            if((cb & fc) != fc)
            {
                bitboards::Board hypothetical_board = board;

                bitboards::place_disc(hypothetical_board, i);
                
                search::Eval hypothetical_eval = search::minimax(hypothetical_board, false, depth-1, alpha, beta);

                if(hypothetical_eval.eval > best_eval.eval)
                {
                    best_eval = hypothetical_eval;
                    best_eval.move = i;
                }

                alpha = std::max(alpha, best_eval.eval);
                if(beta <= alpha)
                {
                    break;
                }
            }
        }
    }
    else {
        best_eval = {INT32_MAX, 0};

        for(int i = 0; i < 7; i++)
        {
            const uint64_t fc = (FULL_COLUMN << (6-i));

            if((cb & fc) != fc)
            {
                bitboards::Board hypothetical_board = board;

                bitboards::place_disc(hypothetical_board, i);

                search::Eval hypothetical_eval = search::minimax(hypothetical_board, true, depth-1, alpha, beta);

                if(hypothetical_eval.eval < best_eval.eval)
                {
                    best_eval.eval = hypothetical_eval.eval;
                    best_eval.move = i;
                }

                beta = std::min(beta, best_eval.eval);
                if(beta <= alpha) {
                    break;
                }
            }
        }
    }

    return best_eval;
}
