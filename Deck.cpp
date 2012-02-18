#include "Deck.h"

#include <algorithm>

bool Card::operator==(const Card& other) const
{
    return suit == other.suit && value == other.value;
}

Deck::Deck()
{
    int location = 0;
    for(int i = Hearts; i <= Spades; ++i)
    {
        for(int j = Nine; j <= Ace; ++j)
        {
            m_cards[location].suit = static_cast<Suit>(i);
            m_cards[location].value = static_cast<Value>(j);
            ++location;
        }
    }

    Shuffle();
}

void Deck::Shuffle()
{
    std::random_shuffle(&m_cards[0], &m_cards[24]);
    m_position = 0;
}

Card Deck::Deal()
{
    return m_cards[m_position++];
}