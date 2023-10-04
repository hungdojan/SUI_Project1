#include "search-strategies.h"

#include <queue>
#include <algorithm>
#include <set>
#include <stack>
#include <iostream>

using namespace std;

struct StateCaller{
    SearchState state;
    std::shared_ptr<SearchAction> action = nullptr;
    std::shared_ptr<StateCaller> parentState = nullptr;

    int depthLevel = 0;

    bool operator<(const StateCaller& other) const {
        return state < other.state;
    }
};

std::vector<SearchAction> DepthFirstSearch::solve(const SearchState &init_state) {

    if(init_state.isFinal()) return {};

    set<StateCaller> visitedStates;
    stack<std::shared_ptr<StateCaller>> openStack;
    vector<SearchAction> solutionActions;

    std::shared_ptr<StateCaller> actualStatePtr;
    std::shared_ptr<int> actualDepth;

    // init
    StateCaller actualState = {init_state};
    visitedStates.insert(actualState);
    openStack.push(std::make_shared<StateCaller>(actualState));

    while(!openStack.empty()){
        actualStatePtr = openStack.top(); openStack.pop();
        actualDepth = std::make_shared<int>(actualStatePtr->depthLevel);

        // memory watch
        if (getCurrentRSS() + actualStatePtr->state.actions().size() * sizeof(StateCaller) > mem_limit_){
            return {};
        }

        // check its followers
        for(const auto& action: actualStatePtr->state.actions()){
            actualState = {action.execute(actualStatePtr->state), std::make_shared<SearchAction>(action),
                           actualStatePtr, *actualDepth + 1};

            // check if the selected element is final
            if(actualState.state.isFinal()){
                actualStatePtr = std::make_shared<StateCaller>(actualState);

                while(actualStatePtr->parentState != nullptr){
                    solutionActions.emplace_back(*actualStatePtr->action);
                    actualStatePtr = actualStatePtr->parentState;
                }

                std::reverse( solutionActions.begin(), solutionActions.end() );
                return solutionActions;
            }

            // max depth
            if(*actualDepth + 1 < depth_limit_){

                // check whether the state is visited
                if(visitedStates.find(actualState) == visitedStates.end()){
                    visitedStates.insert(actualState);
                    openStack.push(std::make_shared<StateCaller>(actualState));
                }
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
