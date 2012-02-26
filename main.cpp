#include <iostream>
#include <sstream>
#include <time.h>

#if _MSC_VER >= 1600
//#define ENABLE_PARALLEL_PROCESSING
//#include <ppl.h>
#endif

#include "EuchreGame.h"
#include "ai/BasicBot.h"
#include "ai/randombot.h"
#include "ui/Text.h"

const int TOTAL_PLAYERS = 2;

std::string PlayerName(int player)
{
    if(player == 1)
        return "ScratchBot";
    else
        return "BasicBot";
}

Bot* GenPlayer(int player, Position p)
{
    if(player == 1)
        return new ScratchBot(p);
    else
        return new BasicBot(p);
}

void ReturnPlayer(Bot* bot)
{
    delete bot;
}

// Any individual element is only written to by one thread
double results[TOTAL_PLAYERS][TOTAL_PLAYERS] = {0};

int main()
{
    static_assert(TOTAL_PLAYERS > 1, "Need to define at least two players");

    srand((unsigned int)time(NULL));

// Switch between these two #defines to flip between round robin (where every player 
// plays all other players, don't attempt for large Ns!) or a gauntlet style where
// the first player (a control of some type) plays everyone else.
//#define OUTER_LOOP 1 // Gauntlet
#define OUTER_LOOP TOTAL_PLAYERS // Round Robin

#ifdef ENABLE_PARALLEL_PROCESSING
    Concurrency::parallel_for(0, OUTER_LOOP, [&](int first) {
        Concurrency::parallel_for(first+1, TOTAL_PLAYERS, [&](int second) {
#else
    for(int first = 0; first < OUTER_LOOP; ++first) {
        for(int second = first+1; second < TOTAL_PLAYERS; ++second) {
#endif
            Bot* players[4];
            players[North] = GenPlayer(first, North);
            players[East] = GenPlayer(second, East);
            players[South] = GenPlayer(first, South);
            players[West] = GenPlayer(second, West);
            Stats call, pass;

            players[North]->SetStatsObjects(&call, &pass);
            players[East]->SetStatsObjects(&call, &pass);
            players[South]->SetStatsObjects(&call, &pass);
            players[West]->SetStatsObjects(&call, &pass);

            int firstWins = 0;
            int secondWins = 0;
            for(int i = 0; i < 100000; ++i)
            {
                EuchreGame g;
                g.AddObserver(&call);
                g.AddObserver(&pass);
                g.Start();
                while(!(g.GetCurrentPhase() == Post))
                {
                    //print_game(g);
                    players[g.GetCurrentTurn()]->Move(g);
                }

                if(g.GetScore(0) >= g.GetWinningScore())
                    firstWins++;
                else
                    secondWins++;
            }

            results[first][second] = (double)firstWins / (firstWins + secondWins);
            results[second][first] = (double)secondWins / (firstWins + secondWins);

            std::cout << "Count\tPlay\tPass\n";
            std::cout.precision(3);

            std::cout << call.m_numRoundsFlagged + pass.m_numRoundsFlagged;

            // Call
            double netPoints = 0.0;
            for(int j = 0; j < 4; ++j)
            {
                double points = call.m_makerPoint[j] + (call.m_makerRanboard[j]*2) +
                    (call.m_makerEuched[j]*-2) + call.m_lonerPoint[j] + (call.m_lonerRanboard[j]*4) +
                    (call.m_lonerEuched[j]*-2);
                if(j % 2)
                    netPoints += points;
                else
                    netPoints -= points;
            }
            if(call.m_numRoundsFlagged)
                std::cout << "\t" << netPoints/call.m_numRoundsFlagged;
            else
                std::cout << "\t...";

            // Pass
            netPoints = 0.0;
            for(int j = 0; j < 4; ++j)
            {
                double points = pass.m_makerPoint[j] + (pass.m_makerRanboard[j]*2) +
                    (pass.m_makerEuched[j]*-2) + pass.m_lonerPoint[j] + (pass.m_lonerRanboard[j]*4) +
                    (pass.m_lonerEuched[j]*-2);
                if(j % 2)
                    netPoints += points;
                else
                    netPoints -= points;
            }
            if(pass.m_numRoundsFlagged)
                std::cout << "\t" << netPoints/pass.m_numRoundsFlagged;
            else
                std::cout << "\t...";

            ReturnPlayer(players[North]);
            ReturnPlayer(players[East]);
            ReturnPlayer(players[South]);
            ReturnPlayer(players[West]);

#ifdef ENABLE_PARALLEL_PROCESSING
        });
    });
#else
        }
    }
#endif

    // Print Top Row
    std::cout << "\n";
    for(int i = 0; i < OUTER_LOOP; ++i)
        std::cout << "\t" << i;
    std::cout << "\n";

    // Print out the array
    for(int i = 0; i < TOTAL_PLAYERS; ++i)
    {
        std::cout << i;
        for(int j = 0; j < OUTER_LOOP; ++j)
        {
            std::cout.precision(4);
            std::cout << "\t" << results[i][j];
        }
        std::cout << "\n";
    }
    std::cout << "\n";

    // Print out the names
    for(int i = 0; i < TOTAL_PLAYERS; ++i)
    {
        std::cout << i << "\t" << PlayerName(i).c_str();

        // Calculate average win%
        double win = 0;
        int games = 0;
        for(int j = 0; j < TOTAL_PLAYERS; ++j)
        {
            if(results[i][j] != 0)
                ++games;
            win += results[i][j];
        }
        std::cout << "\t(" << win/games << ")\n";
    }
    char c; std::cin >> c;
}