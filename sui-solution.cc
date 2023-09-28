#include "search-strategies.h"
#include <set>
#include <unordered_map>
#include <unordered_set>

#if LOGGING
#define LOG_MOVE(_a)                             \
  do {                                           \
    std::cout < < _a << std::endl;               \
  } while (0)
#else
#define LOG_MOVE(_a) ;
#endif // LOGGING

#if PRINT_BOARD
#define LOG_BOARD(_s)                            \
  do {                                           \
    std::cout << "[" << _s << "]" << std::endl;  \
  } while (0)
#else
#define LOG_BOARD(_s) ;
#endif // PRINT_BOARD

std::vector<SearchAction>
BreadthFirstSearch::solve(const SearchState &init_state) {
  return {};
}

std::vector<SearchAction>
DepthFirstSearch::solve(const SearchState &init_state) {
  return {};
}

double StudentHeuristic::distanceLowerBound(const GameState &state) const {
  return 0;
}

double compute_heuristic(const SearchState &state,
                         const AStarHeuristicItf &heuristic);

void createPath(const std::unordered_map<std::shared_ptr<SearchState>,
                                         std::shared_ptr<SearchAction>> &moves,
                const std::shared_ptr<SearchState> &finalState,
                std::vector<SearchAction> &solution) {
    // TODO:
    // std::shared_ptr<SearchState> currentState = finalState;
    // while(currentState != nullptr) {

    // }
}

std::vector<SearchAction> AStarSearch::solve(const SearchState &init_state) {
  // the final solution (sequence of actions)
  std::vector<SearchAction> actions;

  // set of (un)visited states
  std::vector<std::shared_ptr<SearchState>> open;
  std::vector<std::shared_ptr<SearchState>> close;

  // g-score and f-score
  std::unordered_map<std::shared_ptr<SearchState>, double> gscore;
  std::unordered_map<std::shared_ptr<SearchState>, double> fscore;
  std::unordered_map<std::shared_ptr<SearchState>,
                     std::shared_ptr<SearchAction>>
      moves;

  // init the structure
  gscore[std::make_shared<SearchState>(init_state)] = 0;
  fscore[std::make_shared<SearchState>(init_state)] =
      compute_heuristic(init_state, *heuristic_);
  open.push_back(std::make_shared<SearchState>(init_state));

#if LOGGING
  std::cout << "Moves:" << std::endl << std::endl;
#endif

  // TODO: fix the algorithm
  while (!open.empty()) {
    // get state with lowest f-score
    // f-score is computed as g-score (num_of_moves) + h-score (heuristic
    // function)
    std::shared_ptr<SearchState> s = open.back();
    open.pop_back();

    // check for final state
    if (s->isFinal()) {
      // TODO: create path

      createPath(moves, s, actions);
      return actions;
    }

    // for (SearchAction &action : s->actions()) {
    // }

    // #if LOGGING
    //         std::cout << "------------------------" << std::endl;
    //         std::cout << "[" << s.actions()[4] << "]" << std::endl;
    // #if PRINT_BOARD
    //         std::cout << std::endl << s.actions()[4].execute(s);
    // #endif
    //         std::cout << "------------------------" << std::endl;
    // #endif

    // generate new states through actions
    // iterate states/actions
    // calculate current g-score (curr_num_of_moves+1)
    // if the state has been already discovered, update the g-score
    // if the new score is lower than existing one -> we can get to this state
    // sooner
    //
    // templete functions: compute_heuristic(s, *heuristic_);

    // add new states into open
    // add current state to close
    break;
  }
  return {};
}
