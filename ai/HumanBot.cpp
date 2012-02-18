#include <vector>
#include <iostream>

#include "HumanBot.h"
#include "../EuchreGame.h"
#include "../ui/Text.h"

using namespace std;

void HumanBot::PreMove(EuchreGame& g)
{
    print_game(g);
}

void HumanBot::Order(EuchreGame& g)
{
    bool done = false;

    while(!done)
    {
        char choice;
        cout << "[O]rder, [P]ass: ";
        cin >> choice;
        choice = (char)toupper(choice);
        if(choice == 'O')
        {
            cout << "Alone? [YN]: ";
            cin >> choice;
            choice = (char)toupper(choice);
            g.OrderUp(choice == 'Y');
            done = true;
        }
        if(choice == 'P')
        {
            g.Pass();
            done = true;
        }
    }
}

void HumanBot::Bid(EuchreGame& g)
{
    bool done = false;
    while(!done)
    {
        char choice, alone;
        cout << "[O]rder, [P]ass: ";
        cin >> choice;
        choice = (char)toupper(choice);
        if(choice == 'O')
        {
            cout << "Suit? [CSDH]: ";
            cin >> choice;
            choice = (char)toupper(choice);
            cout << "Alone? [YN]: ";
            cin >> alone;
            alone = (char)toupper(alone);
            Suit s = choice == 'S' ? Spades : (choice == 'C' ? Clubs : (choice == 'D' ? Diamonds : Hearts));
            g.Call(s, alone == 'Y');
            done = true;
        }
        if(choice == 'P')
        {
            g.Pass();
            done = true;
        }
    }
}

void HumanBot::Discard(EuchreGame& g)
{
    char choice;
    int i = -1;
    while(i < 0 || i > 4)
    {
        cout << "Choose Card [12345]: ";
        cin >> choice;
        i = atoi(&choice);
        --i;
    }
    g.Discard(g.GetCurrentHand()[i]);
}

void HumanBot::Play(EuchreGame& g)
{
    bool done = false;
    while(!done)
    {
        try
        {
            char choice;
            size_t cards = g.GetCurrentHand().size();
            int i = -1;
            while(i < 0 || i > 4)
            {
                cout << "Choose Card [";
                for(size_t num = 0; num < cards; ++num)
                    cout << num+1;
                cout << "]:";
                cin >> choice;
                i = atoi(&choice);
                --i;
            }
            g.Play(g.GetCurrentHand()[i]);
            done = true;
        }
        catch(IllegalMove&)
        {
        }
    }
}