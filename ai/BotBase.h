#ifndef BOTBASE_H
#define BOTBASE_H

class EuchreGame;
enum Position;

class Bot
{
public:
    Bot(Position p) : m_position(p) { }
    virtual ~Bot() {}
    void Move(EuchreGame& g);

    // Hooks
    virtual void PreMove(EuchreGame&) {}
    virtual void PostMove(EuchreGame&) {}

    virtual void Order(EuchreGame& g) = 0;
    virtual void Bid(EuchreGame& g) = 0;
    virtual void Discard(EuchreGame& g) = 0;
    virtual void Play(EuchreGame& g) = 0;

private:
    Position m_position;
};

#endif