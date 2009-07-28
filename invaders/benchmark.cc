#include <iostream>

#include <invaders.h>

using namespace std;

int main(void)
{
  Board board;
  RandomPolicy policy;

  srand(Timer::now() % 1000000);

  FastMap<Player, uint> winners;
  winners.fill(0);
  Timer timer;

  for (uint i = 0; i < 10000; ++i) {
    board.clear();
  
    Playout<RandomPolicy> playout(board, policy);
    playout.do_playout();
    
    ++winners[playout.winner()];
  }

  uint msec = timer.toc();
  cout << "winners: " << winners[0] << " / " << winners[1] << " in " << msec << " msec" << endl;
  
  return 0;
}

