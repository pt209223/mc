#ifndef _PLAYOUT_H_
#define _PLAYOUT_H_

#include "utils.h"
#include "board.h"
#include "fast_stack.h"

#include <cmath>

// mercy rule

const bool use_mercy_rule      = false;
const uint mercy_threshold     = 25;
const uint max_playout_length  = board_area * 2;

enum PlayoutStatus { pass_pass, mercy, too_long};

template <typename Policy> class Playout {
public:
  Policy*  policy;
  Board*   board;
  FastStack<Move, max_game_length>  move_history;
  PlayoutStatus status;

  Playout (Policy* policy_, Board*  board_) : policy (policy_), board (board_) {}

  all_inline flatten
  bool playout_end() {
    if (board->both_player_pass ()) {
      status = pass_pass;
      return true;
    }

    if (board->move_no >= max_playout_length) {
      status = too_long;
      return true;
    }

    if (use_mercy_rule &&
        uint (abs (float(board->approx_score ()))) > mercy_threshold) {
      status = mercy;
      return true;
    }
    return false;
  }

  all_inline
  PlayoutStatus run () {
    move_history.clear();

    while (!playout_end()) {
      policy->play_move (board);
      Move m = board->last_move();
      move_history.push_back(m);
    }

    return status;
  }
  
};

// -----------------------------------------------------------------------------

class SimplePolicy {
public:
  SimplePolicy(FastRandom& random_) : random(random_) { 
  }

  all_inline
  void play_move (Board* board) {
    uint ii_start = random.rand_int (board->empty_v_cnt); 
    uint ii = ii_start;
    Player act_player = board->act_player ();

    Vertex v;
    while (true) {
      v = board->empty_v [ii];
      if (!board->is_eyelike (act_player, v) &&
          board->is_pseudo_legal (act_player, v)) { 
        board->play_legal(act_player, v);
        return;
      }
      ii += 1;
      ii &= ~(-(ii == board->empty_v_cnt)); // if (ii==board->empty_v_cnt) ii=0;
      if (ii == ii_start) {
        board->play_legal(act_player, Vertex::pass());
        return;
      }
    }
  }

protected:
  FastRandom& random;
};

typedef Playout<SimplePolicy> SimplePlayout;

// -----------------------------------------------------------------------------

// TODO simplify and test performance
class LocalPolicy : protected SimplePolicy {
public:

  LocalPolicy(FastRandom& random_) : SimplePolicy(random_) { 
  }

  all_inline
  void play_move (Board* board) {
    if (play_local(board)) return;
    SimplePolicy::play_move(board);
  }

  struct Perms { int off[24][4]; };

private:
  static const Perms perms[3];
  static RandChance chances[3];

  all_inline
  bool play_local(Board *board) {
    Vertex center = board->last_play();
    if ((center == Vertex::any()) |
        (center == Vertex::pass())) return false;

    Player act_player = board->act_player ();
    const int *off;

    for (uint i = 0; i < 3; ++i) {
      if (chances[i].test()) {
        off = perms[i].off[random.rand_int(24)];

        for (uint j = 0; j < 4; ++j) {
          Vertex v = Vertex(center.get_idx()+off[j]);
          if (v.in_range() && 
              board->color_at[v] == Color::empty() &&
              !board->is_eyelike(act_player, v) &&
              board->is_pseudo_legal(act_player, v)) {
            board->play_legal(act_player, v);
            return true; 
          }
        }
      } 
    }

    // No possibility of a local move
    return false;
  }

};

#endif
