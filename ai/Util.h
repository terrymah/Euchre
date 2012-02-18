#ifndef UTIL_H
#define UTIL_H

#include <vector>

struct Card;
class EuchreGame;
enum Suit;

int SimpleScore(const std::vector<Card>& hand, Suit trump);

// Each function here reduces the set of cards you should consider
// playing in a given situation. Order may matter, choose wisely.
// All functions know not to reduce cards.size() to zero.
typedef void (*Rule)(std::vector<Card>&, const EuchreGame&);
// Don't consider illegal cards. You probably want to call this rule first
void LegalCards(std::vector<Card>& cards, const EuchreGame& game);

// If losing trick, don't consider losing cards if you have at least one winning card
void TryToWinTrick(std::vector<Card>& cards, const EuchreGame& game); 
// Euchre Commandment #3
void DontTrumpParntersAce(std::vector<Card>& cards, const EuchreGame& game);
// Euchre Commandment #4
void AlwaysTrumpPartnersKing(std::vector<Card>& cards, const EuchreGame& game);
// Euchre Commandment #5
void LeadTrumpToParnersCall(std::vector<Card>& cards, const EuchreGame& game);
// Euchre Commandment #6
void DontLeadTrumpToOppCall(std::vector<Card>& cards, const EuchreGame& game);
// Euchre Commandment #7
void LeadHighestTrumpWhenCalling(std::vector<Card>& cards, const EuchreGame& game);
// Just lead your best card
void LeadBestCard(std::vector<Card>& cards, const EuchreGame& game);

// You probably want to call these rules last
void ShortSuitSelf(std::vector<Card>& cards, const EuchreGame& game);
void PlayLowestCard(std::vector<Card>& cards, const EuchreGame& game);


#endif