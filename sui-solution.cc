#include "search-strategies.h"

#include <queue>
#include <unordered_map>
#include <algorithm>
#include <iostream>

using namespace std;

bool operator==(const SearchState &a, const SearchState &b) {
    return a.state_ == b.state_;
}

struct callers{
    SearchState state;
    std::shared_ptr<SearchAction> action;
    std::shared_ptr<callers> parentState;

};

std::vector<SearchAction> BreadthFirstSearch::solve(const SearchState &init_state) {
    cout << "start" << endl;

    queue<std::shared_ptr<callers>> open;
    vector<callers> usedCallers;
    vector<SearchAction> solution;

    usedCallers.push_back({init_state, nullptr, nullptr});
    open.push(std::make_shared<callers>(usedCallers.back()));

    bool expand;

    while(!open.empty()){
        auto actualCaller = open.front(); open.pop();

        // check if the selected element is final
        if(actualCaller->state.isFinal()){

            cout << "je final" << endl;

#ifdef PRINT_BOARD
            SearchState in_progress(actualCaller->state);
            std::cout << "### Starting Position ###" << std::endl;
    std::cout << in_progress << std::endl;
    std::cout << "#########################" << std::endl << std::endl;
#endif // PRINT_BOARD

            while(actualCaller->parentState != nullptr){
                solution.push_back(*actualCaller->action);
                actualCaller = actualCaller->parentState;
            }

            std::reverse( solution.begin(), solution.end() );
            cout << "solution" << endl;
            return solution;
        }

       //cout << "1" << endl;

        // check theirs followers
        for(const auto& action: actualCaller->state.actions()){
//            callers newCaller = {newState, }
            auto newState = action.execute(actualCaller->state);

            // check if the selected element is final
            if(newState.isFinal()){
                cout << "je final" << endl;

                usedCallers.push_back({newState, std::make_shared<SearchAction>(action), actualCaller});
                actualCaller = std::make_shared<callers>(usedCallers.back());

                while(actualCaller->parentState != nullptr){
                    solution.emplace_back(*actualCaller->action);
                    actualCaller = actualCaller->parentState;
                }

                std::reverse( solution.begin(), solution.end() );
                cout << "solution" << endl;
                return solution;
            }

            bool visited = std::any_of(usedCallers.begin(), usedCallers.end(),
                                       [newState](auto cal) { return cal.state == newState; }
            );

            if(!visited){
                usedCallers.push_back({std::move(newState), std::make_shared<SearchAction>(action), actualCaller});
                open.push(std::make_shared<callers>(usedCallers.back()));
            }
        }
    }

    cout << "empty solution" << endl;

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
