#include "search-strategies.h"
#include <set>
#include <unordered_map>
#include <algorithm>
#include <iterator>

#if LOGGING
#define LOG_MOVE(_a, _pref, _suf)                    \
    do {                                             \
        std::cout << _pref << "[" << _a << "]" << _suf << std::endl;  \
    } while (0)
#define LOG_BOARD(_s, _pref, _suf)                      \
    do {                                                \
        std::cout << _pref << _s << _suf << std::endl;  \
    } while (0)
#endif // LOGGING

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
    // distance = non_homes + card_steps_from_home
    // card_steps_from_home == how deep is the follow-up card in the stack
    // 1h 5c 4d == 2
    std::unordered_map<Color, int> cards_from_home{
        {Color::Heart, 0},
        {Color::Diamond, 0},
        {Color::Club, 0},
        {Color::Spade, 0}
    };
    // cards at home
    double distance = king_value * colors_list.size();
    for (const auto &home : state.homes) {
        auto opt_top = home.topCard();
        if (opt_top.has_value()) {
            cards_from_home[opt_top->color] = opt_top->value;
            distance -= opt_top->value;
        } else {
        }
    }

    // number of cards blocking next rank card in the stack
    for (const auto &p : cards_from_home) {
        if (p.second == king_value)
            continue;
        Card curr_card(p.first, p.second + 1);
        for (auto &s : state.stacks) {
            if (s.storage().size() == 0)
                continue;
            auto storage = s.storage();
            auto it = std::find_if(storage.begin(), storage.end(),
                    [curr_card](const Card &a) { return a == curr_card; });
            // card not found in this stack
            if (it == storage.end())
                continue;
            distance += storage.size() - (it - storage.begin());
            break;
        }
    }

    return distance;
}

double compute_heuristic(const SearchState &state, const AStarHeuristicItf &heuristic);

/**
 * @brief Reconstruct the path from start to finish.
 *
 * This function populates `solution` vector with the list of action
 * to get from the initial state to the final state. `moves` collection contains
 * mapping between a destination state and the pair of <source state, selected action>.
 *
 * @param moves      Map that stores possible actions [dst] <- <stc, act>
 * @param finalState The final state of the game.
 * @param solution   Reference of the action vector.
 */
void reconstructPath(std::unordered_map<std::shared_ptr<SearchState>,
        std::pair<std::shared_ptr<SearchState>, std::shared_ptr<SearchAction>>> &moves,
        const std::shared_ptr<SearchState> &finalState,
        std::vector<SearchAction> &solution) {

    auto curr = moves[finalState];
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

/**
 * @brief Solving the game from initial state using A* algorithm.
 *
 * @param init_state Initial state.
 * @return Vector of action to get from the initial to final state.
 */
std::vector<SearchAction> AStarSearch::solve(const SearchState &init_state) {
    using namespace std;
    // int iter = 0;
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

    // init all structures/collections
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

#if 0
        iter++;
        auto m = moves[curr_state];
        auto act = m.second;
        auto src = m.first;
        LOG_BOARD(*curr_state, "DST:" << std::endl, "---" << std::endl);
        cout << "iter: " << iter << endl << "---" << endl;
        if (act == nullptr) {
            LOG_MOVE("", "ACTION: ", std::endl << "---");
        } else {
            LOG_MOVE(*act, "ACTION: ", std::endl << "---");
        }
        if (src == nullptr) {
            LOG_BOARD("null" << std::endl, "SRC:" << std::endl, "#" << std::endl);
        } else {
            LOG_BOARD(*src, "SRC:" << std::endl, "#" << std::endl);
        }
#endif

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
