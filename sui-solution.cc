#include "search-strategies.h"

#include <queue>
#include <algorithm>
#include <set>
#include <iostream>

using namespace std;

struct StateCaller{
    SearchState state;
    std::shared_ptr<SearchAction> action;
    std::shared_ptr<StateCaller> parentState;

    bool operator<(const StateCaller& other) const {
        return state < other.state;
    }
};

std::vector<SearchAction> BreadthFirstSearch::solve(const SearchState &init_state) {

    if(init_state.isFinal())
        return {};

    set<StateCaller> visitedStates;
    queue<std::shared_ptr<StateCaller>> openQueue;
    vector<SearchAction> solutionActions;

    std::shared_ptr<StateCaller> actualStatePtr;

    // init
    StateCaller actualState = {init_state, nullptr, nullptr};
    visitedStates.insert(actualState);
    openQueue.push(std::make_shared<StateCaller>(actualState));

    while(!openQueue.empty()){
        actualStatePtr = openQueue.front(); openQueue.pop();

        // check its followers
        for(const auto& action: actualStatePtr->state.actions()){
            actualState = {action.execute(actualStatePtr->state), std::make_shared<SearchAction>(action),
                    actualStatePtr};

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

            // check whether the state is visited
            if(visitedStates.find(actualState) == visitedStates.end()){
                visitedStates.insert(actualState);
                openQueue.push(std::make_shared<StateCaller>(actualState));
            }
        }
    }
    return {};
}

std::vector<SearchAction> DepthFirstSearch::solve(const SearchState &init_state) {
	return {};
}

double StudentHeuristic::distanceLowerBound(const GameState &state) const {
    return 0;
}

std::vector<SearchAction> AStarSearch::solve(const SearchState &init_state) {
	return {};
}
