#ifndef EUCHREGAME_H
#define EUCHREGAME_H

#include <exception>
#include <vector>
#include <array>
#include "Deck.h"

enum Position
{
    North,
    East,
    South,
    West
};

enum Phase
{
    Pre,
    Ordering,
    Bidding,
    Discarding,
    Playing,
    Post
};

class IllegalMove : public std::exception
{
};

class IllegalOperation : public std::exception
{
};

struct Trick
{
    Position m_leader;
    int m_cardsPlayed;
    Card m_field[4];
    Position m_winner;

    Position GetCurrentWinner(Suit trump) const;
    Suit GetLeadSuit(Suit trump) const;
};

struct Round
{
    Position m_dealer;
    Card m_upCard;
    Suit m_trump;
    Phase m_callMethod;
    Position m_caller;
    bool m_alone;

    int m_tricksPlayed;
    Trick m_history[5];

    Suit GetTrump() const
    { return m_trump; }
};

struct CompletedRound : public Round
{
    Card m_hands[4][5];
    Card m_discard;
    int m_pointsScored[2];
};

class EuchreGame
{
public:
    EuchreGame(bool std = true, int winning_score = 10);

    // Accessors
    bool isScrewTheDealer() const
    { return m_std; }

    const Round& GetCurrentRound() const
    { return m_currentRound; }

    const Trick& GetCurrentTrick() const;

    const std::vector<CompletedRound>& GetRoundHistory() const
    { return m_completedRounds; }

    Phase GetCurrentPhase() const
    { return m_phase; }

    Position GetCurrentTurn() const
    { return m_turn; }

    const std::vector<Card>& GetCurrentHand() const
    { return m_currentHands[GetCurrentTurn()]; }

    int GetScore(int team) const
    { return m_score[team]; }

    int GetWinningScore() const
    { return m_winning_score; }

    // Actions
    void Start();
    void Pass();
    void OrderUp(bool alone);
    void Call(Suit s, bool alone);
    void Play(Card c);
    void Discard(Card c);

private:
    void CheckPhase(Phase one) const;
    void CheckPhase(Phase one, Phase two) const;
    void CheckPhase(Phase one, Phase two, Phase three) const;
    void CheckPhase(Phase one, Phase two, Phase three, Phase four) const;

    // Helper functions
    Trick& _GetCurrentTrick()
    { return const_cast<Trick&>(GetCurrentTrick()); }
    std::vector<Card>& _GetCurrentHand()
    { return m_currentHands[GetCurrentTurn()]; }
    void _InitRound(Position dealer);
    bool m_std;

    Phase m_phase;
    Position m_turn;
    int m_score[2];
    int m_winning_score;
    Deck m_deck;

    std::array<std::vector<Card>,4> m_currentHands;

    CompletedRound m_currentRound;
    std::vector<CompletedRound> m_completedRounds;
};

#endif
