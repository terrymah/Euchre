#ifndef DECK_H
#define DECK_H

enum Suit
{
    Hearts,
    Clubs,
    Diamonds,
    Spades
};

enum Value
{
    Nine = 9,
    Ten,
    Jack,
    Queen,
    King,
    Ace
};

struct Card
{
    Suit suit;
    Value value;

    bool operator==(const Card& other) const;
};

struct Deck
{
    Card m_cards[24];
    int m_position;

    Deck();
    void Shuffle();
    Card Deal();
};

#endif