#include <algorithm>

#include "EuchreLogic.h"
#include "EuchreGame.h"

bool HandHasCard(const std::vector<Card>& hand, Card c)
{
    return std::find(hand.begin(), hand.end(), c) != hand.end();
}

Suit EffectiveSuit(Card c, Suit trump)
{
    if(c.value == Jack)
    {
        if( (trump == Hearts && c.suit == Diamonds)
        ||  (trump == Diamonds && c.suit == Hearts)
        ||  (trump == Spades && c.suit == Clubs)
        ||  (trump == Clubs && c.suit == Spades)
        )
            return trump;
    }

    return c.suit;
}

IsCardSuit::IsCardSuit(Suit targetsuit, Suit trump) 
    : m_targetSuit(targetsuit), m_trump(trump) 
{ 
}

bool IsCardSuit::operator()(const Card& c)
{
    return EffectiveSuit(c, m_trump) == m_targetSuit;
}

bool HandHasSuit(const std::vector<Card>& hand, Suit s, Suit trump)
{
    return std::find_if(hand.begin(), hand.end(), IsCardSuit(s, trump)) != hand.end();
}

Position GetNextPosition(Position p)
{
    return static_cast<Position>((p+1) % 4);
}

Position Partner(Position p)
{
    return static_cast<Position>((p+2) % 4);
}

int Team(Position p)
{
    return static_cast<Position>(p % 2);
}

int CardValue(Card c, Suit trump, Suit leadsuit)
{
    Suit effectiveSuit = EffectiveSuit(c, trump);
    int value = 0;

    // Non-trump, off-suit == worthless
    if((effectiveSuit != leadsuit) && (effectiveSuit != trump))
    {
        return 0;
    }

    // Start with the cards face value
    value += c.value;

    // Add 100 points if it's trump
    if(effectiveSuit == trump)
        value += 100;
    
    // Add 1000 points if it's the left bower
    Card on = { trump, Jack };
    if(c == on)
        value += 1000;

    return value;
}

int PointsScored(int tricksWon, int team, int callingTeam, bool alone)
{
    if(tricksWon < 3)
        return 0;

    // Euchre!
    if(tricksWon > 2 && team != callingTeam)
        return 2;

    // Ran the board?
    if(tricksWon == 5)
        return alone ? 4 : 2;

    // By process of elimination...
    return 1;
}