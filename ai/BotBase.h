#ifndef BOTBASE_H
#define BOTBASE_H

#include "../EuchreGame.h"

// For collecting stats
class Stats : public GameObserver
{
public:
    Stats();

    // GameObserver
    void RoundBegin(const Round& r);
    void RoundComplete(const CompletedRound& r);

    void TrickBegin(const Round& r, const Trick& t);
    void TrickComplete(const Round& r, const Trick& t);

    // Track interesting rounds, tricks
    void FlagRound();
    void FlagTrick();

    // Round stats
    bool m_roundFlagged;
    int m_numRoundsFlagged;
    int m_totalRounds;
    int m_makerPoint[4];
    int m_makerRanboard[4];
    int m_makerEuched[4];
    int m_lonerPoint[4];
    int m_lonerRanboard[4];
    int m_lonerEuched[4];
    int m_maker[4]; // Who was the maker, relative to the dealer (dealer = 0)

    // Trick stats
    bool m_trickFlagged;
    int m_numTricksFlagged;
    int m_totalTricks;
    int m_winner[4]; // What position won, relative to leader (leader = 0)

    int score;
    int val;
};

class Bot
{
public:
    Bot(Position p) : m_position(p), m_play(nullptr), m_pass(nullptr) { }
    virtual ~Bot() {}
    void Move(EuchreGame& g);

    // Hooks
    virtual void PreMove(EuchreGame&) {}
    virtual void PostMove(EuchreGame&) {}

    virtual void Order(EuchreGame& g) = 0;
    virtual void Bid(EuchreGame& g) = 0;
    virtual void Discard(EuchreGame& g) = 0;
    virtual void Play(EuchreGame& g) = 0;

    // Stats support
    void SetStatsObjects(Stats* call, Stats* pass)
    { m_play = call; m_pass = pass; }

    Stats* GetStatsObject() const
    { return m_play; }

protected:
    Position m_position;
    Stats* m_play;
    Stats* m_pass;
};

#endif