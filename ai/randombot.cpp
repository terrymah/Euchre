#include <vector>

#include "RandomBot.h"
#include "../EuchreGame.h"

void RandomBot::Order(EuchreGame& g)
{
    // Order 25% of the time
    if(gen() % 4 == 0)
    {
        g.OrderUp(gen() % 10 == 0);
    }
    else
    {
        try 
        {
            g.Pass();
        }
        catch(const IllegalOperation&)
        {
            g.OrderUp(gen() % 10 == 0);
        }
    }
}

void RandomBot::Bid(EuchreGame& g)
{
    // Order 25% of the time
    bool done = false;
    while(!done)
    {
        try
        {
            if(gen() % 4 == 0)
            {
                Suit s = static_cast<Suit>(gen() % 4);
                g.Call(s, gen() % 10 == 0);
                done = true;
            }
            else
            {
                try 
                {
                    g.Pass();
                    done = true;
                }
                catch(const IllegalOperation&)
                {
                    Suit s = static_cast<Suit>(gen() % 4);
                    g.Call(s, gen() % 10 == 0);
                    done = true;
                }
            }
        }
        catch(const IllegalMove&)
        {
        }
    }
}

void RandomBot::Discard(EuchreGame& g)
{
    g.Discard(g.GetCurrentHand()[gen() % g.GetCurrentHand().size()]);
}

void RandomBot::Play(EuchreGame& g)
{
    // Play a random card
    int card = gen();
    bool done = false;
    while(!done)
    {
        try
        {
            g.Play(g.GetCurrentHand()[card++ % g.GetCurrentHand().size()]);
            done = true;
        }
        catch(const IllegalMove&)
        {
        }
    }
}