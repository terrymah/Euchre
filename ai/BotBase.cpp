#include <assert.h>
#include "botbase.h"
#include "../EuchreGame.h"

void Bot::Move(EuchreGame& game)
{
    assert(m_position == game.GetCurrentTurn());

    PreMove(game);
    switch(game.GetCurrentPhase())
    {
    case Pre:
        throw IllegalOperation();
    case Ordering:
        return Order(game);
    case Bidding:
        return Bid(game);
    case Discarding:
        return Discard(game);
    case Playing:
        return Play(game);
    case Post:
        throw IllegalOperation();
    }
    PostMove(game);
}