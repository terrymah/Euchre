#ifndef BASICBOT_H
#define BASICBOT_H
#include <random>
#include <ctime>
#include "BotBase.h"
#include "Util.h"

class BasicBot : public Bot
{
public:
    BasicBot(Position p, int ourMinOrder = 5, int oppMinOrder = 9, int minBid = 7, 
        int ourAloneOrder = 10, int oppAloneOrder = 10, int aloneBid = 10, 
        unsigned long seed = time(nullptr)) 
        : Bot(p), gen(seed), m_ourMinOrder(ourMinOrder), m_oppMinOrder(oppMinOrder), 
        m_minBid(minBid), m_ourAloneOrder(ourAloneOrder), m_oppAloneOrder(oppAloneOrder),
        m_aloneBid(aloneBid) { }

    virtual void Order(EuchreGame& g);
    virtual void Bid(EuchreGame& g);
    virtual void Discard(EuchreGame& g);
    virtual void Play(EuchreGame& g);

    // Hooks for Play
    virtual Rule* GetRules() const;
    virtual int GetRuleCount() const;

    // Hooks for Order
    virtual bool ShouldSandbag(EuchreGame&) const { return false; }

    // Hooks for Bid
    virtual bool SituationalBid(EuchreGame&) const { return false; }

protected:
    mutable std::mt19937 gen;

    int m_ourMinOrder;
    int m_oppMinOrder;
    int m_minBid;
    int m_ourAloneOrder;
    int m_oppAloneOrder;
    int m_aloneBid;
};

// ScratchBot is the guy I play around with to try and improve over
// the current version of BasicBot. Generally if I find something
// I'll push it back into BasicBot.
class ScratchBot : public BasicBot
{
public:
    ScratchBot(Position p) : BasicBot(p) { }

    bool ShouldSandbag(EuchreGame&) const;
    //bool SituationalBid(EuchreGame&) const;
};

#endif