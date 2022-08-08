#pragma once
#include "CircularBuffer.hpp"
#include "State.hpp"
#include <optional>

class StateMachine
{
public:
    StateMachine();
    void updateState(State&);
    std::optional<State> previousState();
    std::optional<State> nextState();

private:
    CircularBuffer<State> states;
    std::size_t currentState{0};
};

