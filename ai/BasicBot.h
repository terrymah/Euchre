#ifndef BASICBOT_H
#define BASICBOT_H
#include <random>
#include <ctime>
#include "BotBase.h"

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

private:
    std::mt19937 gen;

    int m_ourMinOrder;
    int m_oppMinOrder;
    int m_minBid;
    int m_ourAloneOrder;
    int m_oppAloneOrder;
    int m_aloneBid;
};

#endif