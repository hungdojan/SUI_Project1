#include "search-strategies.h"
#include <set>

std::vector<SearchAction> BreadthFirstSearch::solve(const SearchState &init_state) {
	return {};
}

std::vector<SearchAction> DepthFirstSearch::solve(const SearchState &init_state) {
	return {};
}

double StudentHeuristic::distanceLowerBound(const GameState &state) const {
    return 0;
}

double compute_heuristic(const SearchState &state, const AStarHeuristicItf &heuristic);

std::vector<SearchAction> AStarSearch::solve(const SearchState &init_state) {
    // the final solution (sequence of actions)
    std::vector<SearchAction> actions;

    // set of (un)visited states
    std::vector<SearchState> open;
    std::set<SearchState> close;

    // g-score and f-score
    std::map<SearchState, double> gscore;
    std::map<SearchState, double> fscore;

#if PRINT_BOARD
    std::cout << "### Starting Position ###" << std::endl;
    std::cout << init_state << std::endl;
    std::cout << "#########################" << std::endl;
#endif // PRINT_BOARD

    // init the structure
    gscore[init_state] = 0;
    fscore[init_state] = gscore[init_state] + compute_heuristic(init_state, *heuristic_);
    open.push_back(init_state);

#if LOGGING
    std::cout << "Moves:" << std::endl << std::endl;
#endif

    // TODO: fix the algorithm
    while(!open.empty()) {
        // get state with lowest f-score
        // f-score is computed as g-score (num_of_moves) + h-score (heuristic function)
        SearchState s = open.front();

        // check for final state
        if (s.isFinal()) {
            return actions;
        }

#if LOGGING
        std::cout << "------------------------" << std::endl;
        std::cout << "[" << s.actions()[4] << "]" << std::endl;
#if PRINT_BOARD
        std::cout << std::endl << s.actions()[4].execute(s);
#endif
        std::cout << "------------------------" << std::endl;
#endif

        // generate new states through actions
        // iterate states/actions
        // calculate current g-score (curr_num_of_moves+1)
        // if the state has been already discovered, update the g-score
        // if the new score is lower than existing one -> we can get to this state sooner
        //
        // templete functions: compute_heuristic(s, *heuristic_);

        // add new states into open
        // add current state to close
        break;
    }
	return {};
}
