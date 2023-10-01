#include "search-strategies.h"
#include <set>
#include <unordered_map>
#include <algorithm>
#include <iterator>

#if LOGGING
#define LOG_MOVE(_a)                                 \
    do {                                             \
        std::cout << "[" << _a << "]" << std::endl;  \
    } while (0)
#else
#define LOG_MOVE(_a) ;
#endif // LOGGING

#if PRINT_BOARD
#define LOG_BOARD(_s)                                \
    do {                                             \
        std::cout << _s << std::endl;  \
    } while (0)
#else
#define LOG_BOARD(_s) ;
#endif // PRINT_BOARD

bool operator==(const SearchState &a, const SearchState &b) {
    return a.state_ == b.state_;
}

std::vector<SearchAction>
BreadthFirstSearch::solve(const SearchState &init_state) {
    (void)init_state;
    return {};
}

std::vector<SearchAction>
DepthFirstSearch::solve(const SearchState &init_state) {
    (void)init_state;
    return {};
}

double StudentHeuristic::distanceLowerBound(const GameState &state) const {
    (void)state;
    return 0;
}

double compute_heuristic(const SearchState &state, const AStarHeuristicItf &heuristic);

void reconstructPath(std::unordered_map<std::shared_ptr<SearchState>,
        std::pair<std::shared_ptr<SearchState>, std::shared_ptr<SearchAction>>> &moves,
        const std::shared_ptr<SearchState> &finalState,
        std::vector<SearchAction> &solution) {

    std::pair<std::shared_ptr<SearchState>,
              std::shared_ptr<SearchAction>> curr = moves[finalState];
    // reconstructing the path by back tracking
    // the map `moves` contains [child_state] -> <parent_state, action>
    // each parent state is a child state of the successor
    // we iterate it until we get the first state (parent_state == nullptr)
    // we collect all the actions and store it in the given `solution` vector
    while (curr.first != nullptr) {
        solution.insert(solution.begin(), *curr.second);
        curr = moves[curr.first];
    }
}

std::vector<SearchAction> AStarSearch::solve(const SearchState &init_state) {
    using namespace std;
    // set of (un)visited states
    vector<shared_ptr<SearchState>> open;
    vector<shared_ptr<SearchState>> close;

    // number of moves from start
    unordered_map<shared_ptr<SearchState>, double> gscore;
    // total score (gscore + hscore)
    unordered_map<shared_ptr<SearchState>, double> fscore;
    // map of transitions [dsc] -> <src, Action>
    unordered_map<shared_ptr<SearchState>,
        pair<shared_ptr<SearchState>, shared_ptr<SearchAction>>> moves;

    // init the structure
    shared_ptr<SearchState> curr_state = make_shared<SearchState>(init_state);
    gscore[curr_state] = 0;
    fscore[curr_state] = compute_heuristic(*curr_state, *heuristic_);
    open.push_back(curr_state);
    moves[curr_state] = make_pair(nullptr, nullptr);

    // temp variables
    double min_fscore, curr_fscore;
    vector<shared_ptr<SearchState>>::iterator min_it;

    while (!open.empty()) {
        // get a state with the lowest f-score
        min_it = open.begin();
        min_fscore = fscore[*min_it];

        for (auto curr_it = open.begin(); curr_it != open.end(); curr_it++) {
            curr_fscore = fscore[*curr_it];
            if (curr_fscore < min_fscore) {
                min_it = curr_it;
                min_fscore = curr_fscore;
            }
        }

        // pop the state with the lowest f-score from the OPEN vector
        curr_state = *min_it;
        open.erase(min_it);

        // check for final state
        if (curr_state->isFinal()) {
            // the final solution (sequence of actions)
            vector<SearchAction> actions;
            reconstructPath(moves, curr_state, actions);
            return actions;
        }

        // iterate over elements
        for (SearchAction &action : curr_state->actions()) {
            shared_ptr<SearchState> next_state =
                make_shared<SearchState>(action.execute(*curr_state));
            double next_gscore = gscore[curr_state] + 1;

            // check if element has already been discovered
            auto open_item = find_if(open.begin(), open.end(),
                    [next_state](const shared_ptr<SearchState> &_s)
                    { return *_s == *next_state; });
            auto close_item = find_if(close.begin(), close.end(),
                    [next_state](const shared_ptr<SearchState> &_s)
                    { return *_s == *next_state; });
            if (open_item != open.end()) {   // in open
                // skip the next state if the path to it
                // is longer than what currently OPEN offers
                if (gscore[*open_item] <= next_gscore)
                    continue;
                next_state = *open_item;
            } else if (close_item != close.end()) {          // in close
                // skip the next state if the path to it
                // is longer than what currently OPEN offers
                if (gscore[*close_item] <= next_gscore)
                    continue;

                // move back from CLOSE to OPEN
                next_state = *close_item;
                close.erase(close_item);
                open.push_back(next_state);
            } else {                    // not discovered
                open.push_back(next_state);
            }

            // calculate score of the neighbours and store the transition between states
            gscore[next_state] = next_gscore;
            fscore[next_state] = next_gscore + compute_heuristic(*next_state, *heuristic_);
            moves[next_state] = make_pair(curr_state, make_shared<SearchAction>(action));
        }
        // close states that we have visited
        close.push_back(curr_state);
    }
    return {};
}
