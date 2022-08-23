#include "StateMachine.hpp"

StateMachine::StateMachine()
{

}

void StateMachine::updateState(State&& state)
{
    if (currentState > 0)
    {
        states.removeHead(currentState);
        currentState = 0;
    }
    states.add(std::forward<State>(state));
}

void StateMachine::updateState(State& state)
{
    updateState(std::forward<State>(state));
}

std::optional<State> StateMachine::previousState()
{
    if (currentState + 1 < states.size())
    {
        return states.at(++currentState);
    }
    return std::nullopt;
}

std::optional<State> StateMachine::nextState()
{
    if (currentState > 0)
    {
        return states.at(--currentState);
    }
    return std::nullopt;
}

State StateMachine::latestState()
{
    return states.at(currentState);
}
