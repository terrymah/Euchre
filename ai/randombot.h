#ifndef RANDOMBOT_H
#define RANDOMBOT_H

#include <random>
#include <ctime>

#include "BotBase.h"

class RandomBot : public Bot
{
public:
    RandomBot(Position p, unsigned long seed = time(nullptr)) : Bot(p), gen(seed) { }

    virtual void Order(EuchreGame& g);
    virtual void Bid(EuchreGame& g);
    virtual void Discard(EuchreGame& g);
    virtual void Play(EuchreGame& g);

private:
    std::mt19937 gen;
};

#endif