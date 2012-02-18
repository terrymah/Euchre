#include <cassert>
#include "BasicBot.h"
#include "Util.h"
#include "../EuchreGame.h"
#include "../EuchreLogic.h"

void BasicBot::Order(EuchreGame& g)
{
    int handStrength = SimpleScore(g.GetCurrentHand(), g.GetCurrentRound().m_upCard.suit);

    int orderScore = Team(g.GetCurrentRound().m_dealer) == Team(g.GetCurrentTurn()) ? m_ourMinOrder : m_oppMinOrder;
    int aloneScore = Team(g.GetCurrentRound().m_dealer) == Team(g.GetCurrentTurn()) ? m_ourAloneOrder : m_oppAloneOrder;

    // Don't be that guy
    if(g.GetScore(Team(g.GetCurrentTurn())) >= (g.GetWinningScore() - 2))
        aloneScore = 100;

    if(handStrength >= aloneScore) 
    {
        g.OrderUp(true);
    }
    else if(handStrength >= orderScore)
    {
        g.OrderUp(false);
    }
    else
    {
        g.Pass();
    }
}

void BasicBot::Bid(EuchreGame& g)
{
    // Identify the best suit in our hand
    Suit bestSuit = Hearts;
    int bestScore = 0;

    for(int s = Hearts; s <= Spades; ++s)
    {
        Suit suit = static_cast<Suit>(s);
        if(suit == g.GetCurrentRound().m_upCard.suit)
            continue;

        int score = SimpleScore(g.GetCurrentHand(), suit);
        if( (score > bestScore)
        ||  (score == bestScore && gen() % 2)
        )
        {
            bestScore = score;
            bestSuit = suit;
        }
    }

    if( (bestScore >= m_aloneBid) 
    &&  (g.GetScore(Team(g.GetCurrentTurn())) < (g.GetWinningScore() - 2))
    )
    {
        g.Call(bestSuit, true);
    }
    else if(bestScore >= m_minBid)
    {
        g.Call(bestSuit, false);
    }
    else if((g.GetCurrentRound().m_dealer == g.GetCurrentTurn()) && g.isScrewTheDealer())
    {
        g.Call(bestSuit, false);
    }
    else
    {
        g.Pass();
    }
}

void BasicBot::Discard(EuchreGame& g)
{
    // Discard down to the best possible hand
    int bestHand = 0;
    int bestScore = 0;

    for(int i = 0; i < 5; ++i)
    {
        // What would the hand look like if we discarded this card?
        std::vector<Card> hand = g.GetCurrentHand();
        hand[i] = g.GetCurrentRound().m_upCard;

        int score = SimpleScore(hand, g.GetCurrentRound().m_upCard.suit);
        if( (score > bestScore)
        ||  (score == bestScore && gen() % 2)
        )
        {
            bestScore = score;
            bestHand = i;
        }
    }

    g.Discard(g.GetCurrentHand()[bestHand]);
}

Rule rules[] = {
    &LegalCards,
    &TryToWinTrick,
    &DontTrumpParntersAce,
    &AlwaysTrumpPartnersKing,
    &LeadTrumpToParnersCall,
    &DontLeadTrumpToOppCall,
    &LeadHighestTrumpWhenCalling,
    &LeadBestCard,
    &PlayLowestCard
};

void BasicBot::Play(EuchreGame& g)
{
    std::vector<Card> hand = g.GetCurrentHand();

    for(int i = 0; i < sizeof(rules) / sizeof(rules[0]); ++i)
    {
        (*rules[i])(hand, g);
        if(hand.size() == 1)
            break;
    }

    assert(hand.size() > 0);
    g.Play(hand.size() > 1 ? hand[gen() % hand.size()] : hand[0]);
}