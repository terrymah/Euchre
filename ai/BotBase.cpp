#include <assert.h>
#include "botbase.h"
#include "../EuchreGame.h"
#include "../EuchreLogic.h"

Stats::Stats()
{
    memset(&m_roundFlagged, 0, sizeof(Stats) - offsetof(Stats, m_roundFlagged));
}

void Stats::RoundBegin(const Round&)
{
    m_roundFlagged = false;
}

void Stats::RoundComplete(const CompletedRound& r)
{
    ++m_totalRounds;
    if(m_roundFlagged)
    {
        int maker = r.m_caller - r.m_dealer; 

        if(maker < 0)
            maker += 4;
        ++m_numRoundsFlagged;
        ++m_maker[maker];
        if( (r.m_pointsScored[0] == 4) 
        ||  (r.m_pointsScored[1] == 4)
        )
        {
            ++m_lonerRanboard[maker];
        }
        else if( (r.m_pointsScored[0] == 1) 
        ||  (r.m_pointsScored[1] == 1)
        )
        {
            if(r.m_alone)
                ++m_lonerPoint[maker];
            else
                ++m_makerPoint[maker];
        }
        else if( (r.m_pointsScored[0] == 2) 
        ||  (r.m_pointsScored[1] == 2)
        )
        {
            if(r.m_alone)
            {
                if(score > 11 && val == 0)
                    score = score;
                ++m_lonerEuched[maker];
            }
            else if( ((r.m_pointsScored[0] == 2) 
            &&  (Team(r.m_caller) == 0))
          
            ||
                ( (r.m_pointsScored[1] == 2) 
            &&  (Team(r.m_caller) == 1)
            )
            )
                ++m_makerRanboard[maker];
            else
            {
                if(score > 11 && val == 0)
                    score = score;
                ++m_makerEuched[maker];
            }
        }
    }
}

void Stats::TrickBegin(const Round&, const Trick&)
{
    m_trickFlagged = false;
}

void Stats::TrickComplete(const Round& r, const Trick& t)
{
    ++m_totalTricks;
    if(m_trickFlagged)
    {
        ++m_numTricksFlagged;
        ++m_winner[(t.GetCurrentWinner(r.GetTrump()) - t.m_leader) % 4];
    }
}

void Stats::FlagRound()
{
    m_roundFlagged = true;
}

void Stats::FlagTrick()
{
    m_trickFlagged = true;
}

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