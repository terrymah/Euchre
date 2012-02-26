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
        if(ShouldSandbag(g))
            g.Pass();
        else
            g.OrderUp(true);
    }
    else if(handStrength >= orderScore)
    {
        if(ShouldSandbag(g))
            g.Pass();
        else
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
        if(!SituationalBid(g))
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


Rule bbrules[] = {
    &LegalCards,
    &DontTrumpParntersAce,
    &AlwaysTrumpPartnersKing,
    &LeadTrumpToParnersCall,
    &DontLeadTrumpToOppCall,
    &LeadHighestTrumpWhenCalling,
    &LeadBestCard,
    &TryToWinTrick,
    &PlayLowestCard
};

Rule* BasicBot::GetRules() const
{
    return bbrules;
}

int BasicBot::GetRuleCount() const
{
    return sizeof(bbrules) / sizeof(bbrules[0]);
}

void BasicBot::Play(EuchreGame& g)
{
    std::vector<Card> hand = g.GetCurrentHand();
    Rule* rules = GetRules();

    for(int i = 0; GetRuleCount(); ++i)
    {
        (*rules[i])(hand, g);
        if(hand.size() == 1)
            break;
    }

    assert(hand.size() > 0);
    g.Play(hand.size() > 1 ? hand[gen() % hand.size()] : hand[0]);
}
/*
bool ScratchBot::ShouldSandbag(EuchreGame& g) const
{
    // If we're to the right of the dealer, and we have a better suit to 
    // call, pass and instead prefer to bid our better suit.
    if(GetNextPosition(g.GetCurrentRound().m_dealer) == m_position)
    {
        Suit upCard = g.GetCurrentRound().m_upCard.suit;
        int upValue = SimpleScore(g.GetCurrentHand(), upCard);

        // Find the value of the other suits
        Suit bestSuit = upCard;
        int bestValue = 0;
        for(int i = Hearts; i <= Spades; ++i)
        {
            if(static_cast<Suit>(i) == upCard)
                continue;

            int value = SimpleScore(g.GetCurrentHand(), static_cast<Suit>(i));

            if(value > bestValue)
            {
                bestValue = value;
                bestSuit = static_cast<Suit>(i);
            }
        }

        // Ok, decision time. Also consider that ordering up gives opponents a trump.
        upValue -= 2;

        if(bestValue >= upValue)
            return true; // Sandbag and call bestSuit when it's bidding time
    }
    return false;
}
*/

bool ScratchBot::SituationalBid(EuchreGame& g) const
{
    if(GetNextPosition(g.GetCurrentRound().m_dealer) == m_position)
    {
        if((g.GetCurrentRound().m_upCard.value == Jack) &&
             (SimpleScore(g.GetCurrentHand(), SisterSuit(g.GetCurrentRound().m_upCard.suit)) >= 4)
            )
        {
            if(gen() % 2)
            {
                m_play->FlagRound();
                g.Call(SisterSuit(g.GetCurrentRound().m_upCard.suit), false);
            }
            else
            {
                m_pass->FlagRound();
                g.Pass();
            }
            return true;
        }
    }
    return false;
}
