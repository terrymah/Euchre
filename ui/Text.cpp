#include <iostream>
#include "../EuchreGame.h"

using namespace std;

const char* ToString(Phase p)
{
    switch(p)
    {
    case Pre:
        return "Pre";
    case Ordering:
        return "Ordering";
    case Bidding:
        return "Bidding";
    case Discarding:
        return "Discarding";
    case Playing:
        return "Playing";
    case Post:
        return "Post";
    }
    return "????";
}

const char* ToString(Value v)
{
    switch(v)
    {
    case Nine:
        return "9";
    case Ten:
        return "10";
    case Jack:
        return "J";
    case Queen:
        return "Q";
    case King:
        return "K";
    case Ace:
        return "A";
    }
    return "?";
}

const char* ToString(Suit s)
{
    switch(s)
    {
    case Hearts:
        return "H";
    case Diamonds:
        return "D";
    case Spades:
        return "S";
    case Clubs:
        return "C";
    }
    return "?";
}

void print(const Card& c)
{
    if(c.value == 0)
        cout << "...";
    else
        cout << "(" << ToString(c.value) << " " << ToString(c.suit) << ")";
}

void print(const std::vector<Card>& hand)
{
    for(size_t i = 0; i < hand.size(); ++i)
    {
        print(hand[i]);
        cout << " ";
    }
}

void print(Position p)
{
    switch(p)
    {
    case North:
        cout << "\n X\n. .\n .\n";
        break;
    case East:
        cout << "\n .\n. X\n .\n";
        break;
    case South:
        cout << "\n .\n. .\n X\n";
        break;
    case West:
        cout << "\n .\nX .\n .\n";
        break;
    }
}

void print_game(const EuchreGame& game)
{
    cout << "\n\n";
    cout << "Score: [" << game.GetScore(0) << "-" << game.GetScore(1) << "]\n";
    cout << "Phase: " << ToString(game.GetCurrentPhase()) << "\n";
    cout << "Dealer:\n";
    print(game.GetCurrentRound().m_dealer);
    cout << "Turn:\n";
    print(game.GetCurrentTurn());
    
    switch(game.GetCurrentPhase())
    {
    case Discarding:
    case Ordering:
        cout << "Up Card: ";
        print(game.GetCurrentRound().m_upCard);
        cout << "\n";
        break;
    case Bidding:
        cout << "Former Up Card: ";
        print(game.GetCurrentRound().m_upCard);
        cout << "\n";
        break;
    case Playing:
        cout << "Trump: " << ToString(game.GetCurrentRound().GetTrump()) << "\n";
        cout << "Field:\n";
        cout << "\t";
        print(game.GetCurrentTrick().m_field[0]);
        cout << "\n";
        print(game.GetCurrentTrick().m_field[3]);
        cout << "\t\t";
        print(game.GetCurrentTrick().m_field[1]);
        cout << "\n\t";
        print(game.GetCurrentTrick().m_field[2]);
        cout << "\n";
        break;
    }
    cout << "Current Hand: ";
    print(game.GetCurrentHand());
    cout << "\n";
}
