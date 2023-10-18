#include "search-strategies.h"
#include "memusage.h"

#include <queue>
#include <algorithm>
#include <set>
#include <stack>
#include <iostream>
#include <ctime>
#include <random>
using namespace std;

struct StateCallerDFS{
    std::shared_ptr<SearchState> state = nullptr;
    std::shared_ptr<SearchAction> action = nullptr;
    std::shared_ptr<StateCallerDFS> parentState = nullptr;

    int depthLevel = 0;

    bool operator<(const StateCallerDFS& other) const {
        return state < other.state;
    }
};

std::vector<SearchAction> BreadthFirstSearch::solve(const SearchState &init_state) {
    return {};
}

std::vector<SearchAction> DepthFirstSearch::solve(const SearchState &init_state){
    if(init_state.isFinal()) return {};

    stack<std::shared_ptr<StateCallerDFS>> openStack;
    vector<SearchAction> solutionActions;

    std::shared_ptr<StateCallerDFS> actualStatePtr;
    std::shared_ptr<StateCallerDFS> newStatePtr;
    std::shared_ptr<int> actualDepth;

    // init
    openStack.push(std::make_shared<StateCallerDFS>(StateCallerDFS{std::make_shared<SearchState>(init_state)}));

    while(!openStack.empty()){
        actualStatePtr = openStack.top(); openStack.pop();
        actualDepth = std::make_shared<int>(actualStatePtr->depthLevel);

        // memory watcher
        if (getCurrentRSS() + 50 * 1024 * 1024 > mem_limit_){
            return {};
        }

        // random access to vector
        auto actions = std::make_shared<vector<SearchAction>>(actualStatePtr->state->actions());
        std::vector<int> ind(actions->size());
        std::iota(ind.begin(), ind.end(), 0);
        std::random_shuffle(ind.begin(), ind.end());

        for(auto& i: ind){
            std::shared_ptr<SearchAction> action = std::make_shared<SearchAction>((*actions)[i]);
            newStatePtr = std::make_shared<StateCallerDFS>(StateCallerDFS{std::make_shared<SearchState>(action->execute(*actualStatePtr->state)), action,
                           actualStatePtr, *actualDepth + 1});

            // check whether the new node is final
            if(newStatePtr->state->isFinal()){
                actualStatePtr = newStatePtr;

                while(actualStatePtr->parentState != nullptr){
                    solutionActions.emplace_back(*actualStatePtr->action);
                    actualStatePtr = actualStatePtr->parentState;
                }

                std::reverse( solutionActions.begin(), solutionActions.end() );
                return solutionActions;
            }

            // max depth
            if(*actualDepth + 1 < depth_limit_){
                openStack.push(newStatePtr);
            }
        }
    }
    return {};
}

double StudentHeuristic::distanceLowerBound(const GameState &state) const {
    return 0;
}

std::vector<SearchAction> AStarSearch::solve(const SearchState &init_state) {
    return {};
}
