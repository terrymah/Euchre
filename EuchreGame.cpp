#include <assert.h>
#include <algorithm>

#include "EuchreGame.h"
#include "EuchreLogic.h"

int dealer;
int other;

Suit Trick::GetLeadSuit(Suit trump) const
{ 
    return EffectiveSuit(m_field[m_leader], trump); 
}

Position Trick::GetCurrentWinner(Suit trump) const
{
    if(m_cardsPlayed < 2)
        return m_leader;

    Position bestPosition = m_leader; 
    Position p = bestPosition;
    Card bestCard = m_field[m_leader];
    
    for(int i = 1; i < 4; ++i)
    {
        p = GetNextPosition(p);

        Card pcard = m_field[p];
        
        // If this position didn't play a card, skip
        if(pcard.value == 0)
            continue;

        if(CardValue(pcard, trump, GetLeadSuit(trump)) > CardValue(bestCard, trump, GetLeadSuit(trump)))
        {
            bestCard = pcard;
            bestPosition = p;
        }
    }

    return bestPosition;
}

EuchreGame::EuchreGame(bool std, int winning_score)
    : m_std(std), m_winning_score(winning_score), m_phase(Pre)
{
    m_score[0] = m_score[1] = 0;
}

void EuchreGame::CheckPhase(Phase one) const
{
    if(m_phase != one) 
        throw IllegalOperation();
}

void EuchreGame::CheckPhase(Phase one, Phase two) const
{
    if((m_phase != one) && (m_phase != two)) 
        throw IllegalOperation();
}

void EuchreGame::CheckPhase(Phase one, Phase two, Phase three) const
{
    if((m_phase != one) && (m_phase != two) && (m_phase != three))
        throw IllegalOperation();
}

void EuchreGame::CheckPhase(Phase one, Phase two, Phase three, Phase four) const
{
    if((m_phase != one) && (m_phase != two) && (m_phase != three) && (m_phase != four))
        throw IllegalOperation();
}

const Trick& EuchreGame::GetCurrentTrick() const
{
    CheckPhase(Playing);
    return GetCurrentRound().m_history[GetCurrentRound().m_tricksPlayed];
}

void EuchreGame::Start()
{
    CheckPhase(Pre);

    // Initialize the first round
    _InitRound(static_cast<Position>(rand() % 4));
}

void EuchreGame::Pass()
{
    CheckPhase(Ordering, Bidding);

    // Can't pass if you're the dealer in an STD game
    if( isScrewTheDealer()
    &&  (GetCurrentPhase() == Bidding)
    &&  (GetCurrentTurn() == GetCurrentRound().m_dealer)
    )
    {
        throw IllegalOperation();
    }

    if(GetCurrentTurn() == GetCurrentRound().m_dealer)
    {
        if(GetCurrentPhase() == Ordering)
        {
            m_phase = Bidding;
            m_turn = GetNextPosition(GetCurrentTurn());
        }
        else
        {
            m_completedRounds.push_back(m_currentRound);
            _FireRoundComplete();
            _InitRound(GetNextPosition(GetCurrentTurn()));
        }
    }
    else
    {
        m_turn = GetNextPosition(GetCurrentTurn());
    }
}

void EuchreGame::OrderUp(bool alone)
{
    CheckPhase(Ordering);

    // Record the call
    m_currentRound.m_callMethod = Ordering;
    m_currentRound.m_caller = GetCurrentTurn();
    m_currentRound.m_alone = alone;
    m_currentRound.m_trump = m_currentRound.m_upCard.suit;

    // Dealer needs to discard now
    m_phase = Discarding;
    m_turn = GetCurrentRound().m_dealer;
}

void EuchreGame::Call(Suit s, bool alone)
{
    CheckPhase(Bidding);
    
    // Can't call the suit of the upcard
    if(s == GetCurrentRound().m_upCard.suit)
    {
        throw IllegalMove();
    }

    // Record the call
    m_currentRound.m_callMethod = Bidding;
    m_currentRound.m_caller = GetCurrentTurn();
    m_currentRound.m_alone = alone;
    m_currentRound.m_trump = s;

    // Start the first trick
    m_phase = Playing;
    assert(m_currentRound.m_tricksPlayed == 0);
    _GetCurrentTrick().m_cardsPlayed = 0;
    m_turn = GetNextPosition(GetCurrentRound().m_dealer);

    // Partner doesn't play during a loner
    if(GetCurrentRound().m_alone && (GetCurrentTurn() == Partner(GetCurrentRound().m_caller)))
    {
        m_turn = GetNextPosition(GetCurrentTurn());
    }

    _GetCurrentTrick().m_leader = GetCurrentTurn();
}

void EuchreGame::Play(Card c)
{
    CheckPhase(Playing);

    Suit trump = GetCurrentRound().GetTrump();
    // Card must be in your hand
    if(!HandHasCard(GetCurrentHand(), c))
    {
        throw IllegalMove();
    }

    // Must follow suit (if you have that suit)
    if( (GetCurrentTrick().m_leader != GetCurrentTurn())
    &&  (EffectiveSuit(c, trump) != EffectiveSuit(GetCurrentTrick().m_field[GetCurrentTrick().m_leader], trump))
    &&  HandHasSuit(GetCurrentHand(), GetCurrentTrick().GetLeadSuit(trump), trump)
    )
    {
        throw IllegalMove();
    }

    // Play the card
    _GetCurrentTrick().m_field[GetCurrentTurn()] = c;

    // Remove the card from the hand
    _GetCurrentHand().erase(std::remove(_GetCurrentHand().begin(), _GetCurrentHand().end(), c), _GetCurrentHand().end());

    ++_GetCurrentTrick().m_cardsPlayed;
    
    // If all 4 tricks (3 on a loner) have been played, score the trick
    if( (GetCurrentTrick().m_cardsPlayed == 4)
    ||  (   (GetCurrentTrick().m_cardsPlayed == 3)
        &&  m_currentRound.m_alone
        )
    )
    {
        Position trickWinner = GetCurrentTrick().GetCurrentWinner(trump);
        _GetCurrentTrick().m_winner = trickWinner;
        _FireTrickComplete();
        m_currentRound.m_tricksPlayed++;

        // Wrap up the round
        if(m_currentRound.m_tricksPlayed == 5)
        {
            int tricksWon[2] = {0};
            for(int i = 0; i < 5; ++i)
            {
                ++tricksWon[Team(m_currentRound.m_history[i].m_winner)];
            }

            for(int i = 0; i < 2; ++i)
            {
                m_currentRound.m_pointsScored[i] = PointsScored(tricksWon[i], i, Team(m_currentRound.m_caller), m_currentRound.m_alone);
                m_score[i] += m_currentRound.m_pointsScored[i];

                if(i == Team(m_currentRound.m_dealer))
                    dealer += m_currentRound.m_pointsScored[i];
                else
                    other += m_currentRound.m_pointsScored[i];
            }

            // Store off this round
            m_completedRounds.push_back(m_currentRound);
            _FireRoundComplete();
            if(m_score[0] >= m_winning_score || m_score[1] >= m_winning_score)
                m_phase = Post;
            else
                _InitRound(GetNextPosition(m_currentRound.m_dealer));
        }
        // Just start the next trick
        else
        {
            _GetCurrentTrick().m_leader = trickWinner;
            assert(_GetCurrentTrick().m_cardsPlayed == 0);
            m_turn = trickWinner;
            _FireTrickBegin();
        }
    }
    // Otherwise, just move on to the next player
    else
    {
        m_turn = GetNextPosition(GetCurrentTurn());

        // Partner doesn't play during a loner
        if(GetCurrentRound().m_alone && (GetCurrentTurn() == Partner(GetCurrentRound().m_caller)))
        {
            m_turn = GetNextPosition(GetCurrentTurn());
        }
    }

}

void EuchreGame::Discard(Card c)
{
    CheckPhase(Discarding);
    assert(GetCurrentTurn() == GetCurrentRound().m_dealer);
    
    if(!HandHasCard(GetCurrentHand(), c))
    {
        throw IllegalMove();
    }

    // Store off the discarded card for posterity's sake
    m_currentRound.m_discard = c;

    // Remove the discarded card from the hand
    _GetCurrentHand().erase(std::remove(_GetCurrentHand().begin(), _GetCurrentHand().end(), c), _GetCurrentHand().end());

    // Add in the top card
    _GetCurrentHand().push_back(m_currentRound.m_upCard);

    // Start the first trick
    m_phase = Playing;
    assert(m_currentRound.m_tricksPlayed == 0);
    _GetCurrentTrick().m_cardsPlayed = 0;
    m_turn = GetNextPosition(GetCurrentRound().m_dealer);

    // Partner doesn't play during a loner
    if(GetCurrentRound().m_alone && (GetCurrentTurn() == Partner(GetCurrentRound().m_caller)))
    {
        m_turn = GetNextPosition(GetCurrentTurn());
    }

    _GetCurrentTrick().m_leader = GetCurrentTurn();
}

void EuchreGame::_InitRound(Position dealer)
{
    memset(&m_currentRound, 0, sizeof(CompletedRound));
    m_currentRound.m_dealer = dealer;
 
    m_turn = GetNextPosition(dealer);
    m_phase = Ordering;
    m_deck.Shuffle();

    /*
    for(int i = North; i <= West; ++i)
    {
        std::vector<Card> hand(5);
        for(int j = 0; j < 5; ++j)
            hand[j] = m_deck.Deal();
        m_currentHands[i] = hand;
    }
    */
    memcpy(m_currentRound.m_hands, m_deck.m_cards, sizeof(Card) * 20);
    m_deck.m_position = 20;

    for(int i = North; i <= West; ++i)
    {
        m_currentHands[i].swap(std::vector<Card>(&m_currentRound.m_hands[i][0], &m_currentRound.m_hands[i][5]));
    }

    m_currentRound.m_upCard = m_deck.Deal();
    _FireRoundBegin();
}

void EuchreGame::_FireRoundBegin()
{
    std::for_each(m_observers.begin(), m_observers.end(), [&](GameObserver* go){
        go->RoundBegin(GetCurrentRound());
    });
}
void EuchreGame::_FireRoundComplete()
{
    std::for_each(m_observers.begin(), m_observers.end(), [&](GameObserver* go){
        go->RoundComplete(m_currentRound);
    });
}
void EuchreGame::_FireTrickBegin()
{
    std::for_each(m_observers.begin(), m_observers.end(), [&](GameObserver* go){
        go->TrickBegin(GetCurrentRound(), GetCurrentTrick());
    });
}
void EuchreGame::_FireTrickComplete()
{
    std::for_each(m_observers.begin(), m_observers.end(), [&](GameObserver* go){
        go->TrickComplete(GetCurrentRound(), GetCurrentTrick());
    });

}