#ifndef HUMANBOT_H
#define HUMANBOT_H

#include "BotBase.h"

class HumanBot : public Bot
{
public:
    HumanBot(Position p) : Bot(p) { }

    virtual void PreMove(EuchreGame& g);

    virtual void Order(EuchreGame& g);
    virtual void Bid(EuchreGame& g);
    virtual void Discard(EuchreGame& g);
    virtual void Play(EuchreGame& g);
};

#endif