#ifndef EUCHRELOGIC_H
#define EUCHRELOGIC_H

#include <vector>

struct Card;
enum Suit;
enum Position;


bool HandHasCard(const std::vector<Card>& hand, Card c);
Suit EffectiveSuit(Card c, Suit trump);
Suit SisterSuit(Suit s);
bool HandHasSuit(const std::vector<Card>& hand, Suit s, Suit trump);
Position Partner(Position p);
int Team(Position p);
Position GetNextPosition(Position p);
int CardValue(Card c, Suit trump, Suit leadsuit);
int PointsScored(int tricksWon, int team, int callingTeam, bool alone);

struct IsCardSuit
{
    IsCardSuit(Suit targetsuit, Suit trump);
    Suit m_targetSuit;
    Suit m_trump;

    bool operator()(const Card& c);
};
#endif
