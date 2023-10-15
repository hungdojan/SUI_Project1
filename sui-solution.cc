#include "search-strategies.h"
#include "memusage.h"       // getCurrentRSS
#include "card.h"           // Card
#include "card-storage.h"   // HomeDestination
#include <set>              // std::set
#include <unordered_map>    // std::map, std::unordered_map
#include <algorithm>        // std::find
#include <iterator>         // ::iterator
#include <queue>            // std::priority_queue

#ifndef LOGGING
#define LOGGING 0
#endif

#if LOGGING
#define LOG_MOVE(_a, _pref, _suf)                       \
    do {                                                \
        std::cout << _pref << "[" << _a << "]" << _suf << std::endl;  \
    } while (0)
#define LOG_BOARD(_s, _pref, _suf)                      \
    do {                                                \
        std::cout << _pref << _s << _suf << std::endl;  \
    } while (0)
#else
#define LOG_MOVE(_a, _pref, _suf)   ;
#define LOG_BOARD(_s, _pref, _suf)  ;
#endif // LOGGING

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
    for (const HomeDestination &home : state.homes) {
        const std::optional<Card> opt_top = home.topCard();
        if (opt_top.has_value()) {
            cards_from_home[opt_top->color] = opt_top->value;
            distance -= opt_top->value;
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
            const std::vector<Card> storage = s.storage();
            auto it = std::find(storage.begin(), storage.end(), curr_card);

            // card not found in this stack
            if (it == storage.end())
                continue;
            distance += storage.size() - (it - storage.begin()) - 1;
            break;
        }
    }

    return distance;
}


typedef std::shared_ptr<SearchState> SearchStatePtr;
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
void reconstructPath(std::unordered_map<SearchStatePtr,
        std::pair<SearchStatePtr, std::shared_ptr<SearchAction>>> &moves,
        const SearchStatePtr &finalState,
        std::vector<SearchAction> &solution) {

    std::pair<SearchStatePtr, std::shared_ptr<SearchAction>> curr = moves[finalState];
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
    // using namespace std;

    // check final state
    if (init_state.isFinal()) {
        return {};
    }

    // priority queue for states
    // we want to select a SearchState with lowest f-score
    // the queue accepts a pair <f-score ; SearchStatePtr>
    std::priority_queue<std::pair<double, SearchStatePtr>,
                        std::vector<std::pair<double, SearchStatePtr>>,
                        std::greater<std::pair<double, SearchStatePtr>>> pq_open;

    // set of g-scores (number of steps from init_position)
    // this set also represents a close collection; visited states will have a gscore value
    std::map<SearchState, double> gscore;
    // map of transitions [dsc] -> <src, Action>
    std::unordered_map<SearchStatePtr,
                       std::pair<SearchStatePtr, std::shared_ptr<SearchAction>>> moves;

    // init all structures/collections
    SearchStatePtr currStatePtr = std::make_shared<SearchState>(init_state);
    gscore[init_state] = 0;
    pq_open.push(std::make_pair(compute_heuristic(init_state, *heuristic_), currStatePtr));
    moves[currStatePtr] = std::make_pair(nullptr, nullptr);

    // temp variables
    SearchStatePtr nextPtr = nullptr;
    double nextGscore = 0;

    while (!pq_open.empty()) {
        // pop a state with the lowest f-score from the queue
        const std::pair<double, SearchStatePtr> top = pq_open.top(); pq_open.pop();
        currStatePtr = top.second;

        // iterate over elements
        for (const SearchAction &action : currStatePtr->actions()) {
            const SearchState next = action.execute(*currStatePtr);
            nextPtr = std::make_shared<SearchState>(next);

            // check for final state
            if (next.isFinal()) {
                std::vector<SearchAction> actions;
                moves[nextPtr] = std::make_pair(currStatePtr,
                                                std::make_shared<SearchAction>(action));

                // generate a path
                reconstructPath(moves, nextPtr, actions);
                return actions;
            }

            // memory check
            if (getCurrentRSS() + 50*1024*1024 > mem_limit_) {
                return {};
            }

            nextGscore = gscore[*currStatePtr] + 1;
            std::map<SearchState, double>::iterator closedIt = gscore.find(next);
            // skip states that has already been visited but are further from the init_state
            if (closedIt != gscore.end()) {
                // uncomment line of code below to find the shortest path
                // if (nextGscore >= gscore[next]) continue;
                // uncomment line of code below to speed up the algorithm
                continue;
            }
            // update the next state
            // store transition, g-score and push next state to priority queue
            moves[nextPtr] = std::make_pair(currStatePtr, std::make_shared<SearchAction>(action));
            gscore[next] = nextGscore;
            pq_open.push(std::make_pair(compute_heuristic(next, *heuristic_) + nextGscore, nextPtr));
        }
    }
    return {};
}
