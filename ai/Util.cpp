#include "Util.h"
#include "../EuchreGame.h"
#include "../EuchreLogic.h"

int SimpleScore(const std::vector<Card>& hand, Suit trump)
{
    // Jack (on/off) = 3 points
    // Other Trump = 2 points
    // Ace = 1 point
    // 2 Suited = 2 points
    // 3 Suited = 1 point

    int suitCount[4] = {0};
    int score = 0;

    for(size_t i = 0; i < hand.size(); ++i)
    {
        const Card& c = hand[i];
        ++suitCount[EffectiveSuit(c, trump)];
        if(EffectiveSuit(c,trump) == trump)
        {
            if(c.value == Jack)
                score += 3;
            else
                score += 2;
        }
        else if(c.value == Ace)
        {
            score += 1;
        }
    }

    // Count total suits
    int totalSuits = 0;
    for(int i = 0; i < 4; ++i)
    {
        if(suitCount[i] > 0)
            ++totalSuits;
    }

    if(totalSuits == 3)
        ++score;
    else if(totalSuits == 2)
        score += 2;

    return score;
}

void LegalCards(std::vector<Card>& cards, const EuchreGame& game)
{
    // If you're leading, all cards are legal
    if(game.GetCurrentTrick().m_cardsPlayed == 0)
        return;

    // If not leading, and you have a card of the lead suit,
    // only those cards are legal.
    Suit trump = game.GetCurrentRound().GetTrump();
    Suit leadSuit = game.GetCurrentTrick().GetLeadSuit(trump);

    if(HandHasSuit(cards, leadSuit, trump))
    {
        // Elminate all cards that are not that suit
        cards.erase(std::partition(cards.begin(), cards.end(), IsCardSuit(leadSuit, trump)), cards.end());
    }
    // else all cards are legal
}

void TryToWinTrick(std::vector<Card>& cards, const EuchreGame& game)
{
    // Rule not relevent if leading
    if(game.GetCurrentTrick().m_cardsPlayed == 0)
        return;

    // Find the current leader
    Suit trump = game.GetCurrentRound().GetTrump();
    Suit leadSuit = game.GetCurrentTrick().GetLeadSuit(trump);

    Position bestPosition = game.GetCurrentTrick().GetCurrentWinner(trump);

    // No need to try and win trick if we're last and our partner is winning
    if( (game.GetCurrentTrick().m_cardsPlayed == 3) 
    &&  (bestPosition == Partner(game.GetCurrentTurn()))
    )
        return;

    int bestScore = CardValue(game.GetCurrentTrick().m_field[bestPosition], trump, leadSuit);

    // Do we have any cards that can beat bestScore?
    for(size_t i = 0; i < cards.size(); ++i)
    {
        // If so, remove all ones that can't beat it, and return
        if(CardValue(cards[i], trump, leadSuit) > bestScore)
        {
            cards.erase(std::remove_if(cards.begin(), cards.end(), [&](const Card& c) {
                return CardValue(c, trump, leadSuit) < bestScore;
            }), cards.end());
            return;
        }
    }

}

void DontTrumpParntersAce(std::vector<Card>& cards, const EuchreGame& game)
{
    // Rule not relevant if not 3rd
    if(game.GetCurrentTrick().m_cardsPlayed != 2)
        return;

    Suit trump = game.GetCurrentRound().GetTrump();

    // Rule not relevant if trump is led
    if(game.GetCurrentTrick().GetLeadSuit(trump) == trump)
        return;

    // Rule not relevant if opp trumps partner
    if(EffectiveSuit(game.GetCurrentTrick().m_field[GetNextPosition(game.GetCurrentTrick().m_leader)], trump) == trump)
        return;

    // As the name implies, only consider when partner leads an ace
    if(game.GetCurrentTrick().m_field[game.GetCurrentTrick().m_leader].value != Ace)
        return;

    // Eliminate all cards that are trump (assuming we don't have all trump)
    for(size_t i = 0; i < cards.size(); ++i)
    {
        if(EffectiveSuit(cards[i], trump) != trump)
        {
            cards.erase(std::remove_if(cards.begin(), cards.end(), IsCardSuit(trump, trump)), cards.end());
            break;
        }
    }
}

void AlwaysTrumpPartnersKing(std::vector<Card>& cards, const EuchreGame& game)
{
    Suit trump = game.GetCurrentRound().GetTrump();

    // Rule not relevant if not 3rd
    if(game.GetCurrentTrick().m_cardsPlayed != 2)
        return;

    // Rule not relevant if trump is led
    if(game.GetCurrentTrick().GetLeadSuit(trump) == trump)
        return;

    // As the name implies, only consider when partner leads a king (or less)
    if(game.GetCurrentTrick().m_field[game.GetCurrentTrick().m_leader].value < Ace)
        return;

    // If you have a trump, remove all non-trump cards
    if(HandHasSuit(cards, trump, trump))
    {
        // Elminate all cards that are not trump
        cards.erase(std::partition(cards.begin(), cards.end(), IsCardSuit(trump, trump)), cards.end());
    }
}

void LeadTrumpToParnersCall(std::vector<Card>& cards, const EuchreGame& game)
{
    Suit trump = game.GetCurrentRound().GetTrump();

    // Rule not relevant if not leading
    if(game.GetCurrentTrick().m_cardsPlayed != 0)
        return;

    // Only relevant on first trick
    if(game.GetCurrentRound().m_tricksPlayed != 0)
        return;

    // Only relevant if partner called
    if(game.GetCurrentRound().m_caller != Partner(game.GetCurrentTurn()))
        return;

    // If you have a trump, remove all non-trump cards
    if(HandHasSuit(cards, trump, trump))
    {
        // Elminate all cards that are not trump
        cards.erase(std::partition(cards.begin(), cards.end(), IsCardSuit(trump, trump)), cards.end());
    }
}

void DontLeadTrumpToOppCall(std::vector<Card>& cards, const EuchreGame& game)
{
    Suit trump = game.GetCurrentRound().GetTrump();

    // Rule not relevant if not leading
    if(game.GetCurrentTrick().m_cardsPlayed != 0)
        return;

    // Only relevant if opp called
    if(Team(game.GetCurrentRound().m_caller) == Team(game.GetCurrentTurn()))
        return;

    // Eliminate all cards that are trump (assuming we don't have all trump)
    for(size_t i = 0; i < cards.size(); ++i)
    {
        if(EffectiveSuit(cards[i], trump) != trump)
        {
            cards.erase(std::remove_if(cards.begin(), cards.end(), IsCardSuit(trump, trump)), cards.end());
            break;
        }
    }
}

void LeadHighestTrumpWhenCalling(std::vector<Card>& cards, const EuchreGame& game)
{
    // Rule not relevant if not leading
    if(game.GetCurrentTrick().m_cardsPlayed != 0)
        return;

    // If we called, and we have the on, definately play that
    Card target = { game.GetCurrentRound().GetTrump(), Jack };

    if(std::find(cards.begin(), cards.end(), target) != cards.end())
    {
        // We have the on. Play it!
        cards.clear();
        cards.push_back(target);
    }
}

void LeadBestCard(std::vector<Card>& cards, const EuchreGame& game)
{
    // Rule not relevant if not leading
    if(game.GetCurrentTrick().m_cardsPlayed != 0)
        return;

    // Just find the highest value card in our hand and play that
    Suit trump = game.GetCurrentRound().GetTrump();
    Suit leadSuit = game.GetCurrentTrick().GetLeadSuit(trump);

    size_t bestCard = 0;
    size_t bestScore = 0;
    for(size_t i = 0; i < cards.size(); ++i)
    {
        size_t score = CardValue(cards[i], trump, leadSuit);
        if(score > bestScore)
        {
            bestScore = score;
            bestCard = i;
        }
    }

    Card c = cards[bestCard];
    cards.clear();
    cards.push_back(c);
}

void PlayLowestCard(std::vector<Card>& cards, const EuchreGame& game)
{
    // Play whatever cards leaves us with the stongest hand
    Suit trump = game.GetCurrentRound().GetTrump();
    int bestDiscard = 0;
    int bestScore = 0;

    // TODO: Refactor this function (with SimpleBot::Discard)
    for(size_t i = 0; i < cards.size(); ++i)
    {
        // What would the hand look like if we discarded this card?
        std::vector<Card> hand = cards;
        std::swap(hand[i], hand[hand.size()-1]);
        hand.pop_back();

        int score = SimpleScore(hand, trump);
        if(score > bestScore)
        {
            bestScore = score;
            bestDiscard = i;
        }
    }

    Card c = cards[bestDiscard];
    cards.clear();
    cards.push_back(c);
}