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

#include "playout.h"
#include <algorithm>
#include <cassert>

static const LocalPolicy::Perms gen_perms(int a1, int a2, int a3, int a4) {
  int i = 0, tab[4] = { a1, a2, a3, a4 };
  LocalPolicy::Perms p;
  
  // first permutation is sorted (ascending order)
  std::sort(tab, tab+4);
  
  do { // for each permutation
    memcpy(p.off[i], tab, sizeof(int)*4);
    ++i;
  } while (std::next_permutation(tab, tab+4));

  // last permutation is sorted (descending order)
  
  assert(i == 24);
  return p;
}

const LocalPolicy::Perms LocalPolicy::perms[3] = {
  // N,  S,  W,  E
  gen_perms(
      Vertex::dNS, -Vertex::dNS, 
      Vertex::dWE, -Vertex::dWE)
    ,
  // NE, NW, SE, SW
  gen_perms(
      Vertex::dNS+Vertex::dWE, Vertex::dNS-Vertex::dWE, 
      -Vertex::dNS+Vertex::dWE, -Vertex::dNS-Vertex::dWE)
    ,
  // NN, SS, WW, EE
  gen_perms(
      2*Vertex::dNS, -2*Vertex::dNS, 
      2*Vertex::dWE, -2*Vertex::dWE)
};

RandChance LocalPolicy::chances[3] = {
  RandChance(0.25), RandChance(0.25), RandChance(0.25) 
};
