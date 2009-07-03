/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                           *
 *  This file is part of Library of Effective GO routines - EGO library      *
 *                                                                           *
 *  Copyright 2006 and onwards, Lukasz Lew                                   *
 *                                                                           *
 *  EGO library is free software; you can redistribute it and/or modify      *
 *  it under the terms of the GNU General Public License as published by     *
 *  the Free Software Foundation; either version 2 of the License, or        *
 *  (at your option) any later version.                                      *
 *                                                                           *
 *  EGO library is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU General Public License for more details.                             *
 *                                                                           *
 *  You should have received a copy of the GNU General Public License        *
 *  along with EGO library; if not, write to the Free Software               *
 *  Foundation, Inc., 51 Franklin St, Fifth Floor,                           *
 *  Boston, MA  02110-1301  USA                                              *
 *                                                                           *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <fstream>

#include "basic_gtp.h"

#include "testing.h"

BasicGtp::BasicGtp (Gtp& gtp, FullBoard& board_) : board (board_) { 
  gtp.add_gtp_command (this, "boardsize");
  gtp.add_gtp_command (this, "clear_board");
  gtp.add_gtp_command (this, "komi");
  gtp.add_gtp_command (this, "play");
  gtp.add_gtp_command (this, "undo");
  gtp.add_gtp_command (this, "showboard");
}

GtpResult BasicGtp::exec_command (const string& command, istream& params) {
  if (command == "boardsize") {
    int new_board_size;;
    if (!(params >> new_board_size)) return GtpResult::syntax_error ();
    if (new_board_size != int (board_size)) { 
      return GtpResult::failure ("unacceptable size"); 
    }
    return GtpResult::success ();
  }

  if (command == "clear_board") {
    board.clear ();
    return GtpResult::success ();
  }

  if (command == "komi") {
    float new_komi;
    if (!(params >> new_komi)) return GtpResult::syntax_error ();
    board.set_komi (-new_komi);
    return GtpResult::success ();
  }

  if (command == "play") {
    Player pl;
    Vertex v;
    if (!(params >> pl >> v))
      return GtpResult::syntax_error ();
  
    if (v != Vertex::resign () && board.try_play (pl, v) == false)
      return GtpResult::failure ("illegal move");

    return GtpResult::success ();
  }

  if (command == "undo") {
    if (board.undo () == false)
      return GtpResult::failure ("too many undo");

    return GtpResult::success ();
  }
    
  if (command == "showboard") {
    return GtpResult::success ("\n" + board.board().to_string());
  }

  assert(false);
} 
