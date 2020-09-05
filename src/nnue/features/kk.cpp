/*
  Stockfish, a UCI chess playing engine derived from Glaurung 2.1
  Copyright (C) 2004-2020 The Stockfish developers (see AUTHORS file)

  Stockfish is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Stockfish is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

//Definition of input features KK of NNUE evaluation function

#include "kk.h"
#include "index_list.h"

namespace Eval::NNUE::Features {

  // MakeIndex
  inline IndexType KK::MakeIndex(Color perspective, Square ksq_us, Square ksq_them) {
    if (perspective == BLACK) {
      ksq_us = rotate180(ksq_us);
      ksq_them = rotate180(ksq_them);
    }
    return static_cast<IndexType>(ksq_us) * static_cast<IndexType>(SQUARE_NB) + ksq_them;
  }

  // Get a list of indices for active features
  void KK::AppendActiveIndices(
      const Position& pos, Color perspective, IndexList* active) {

    active->push_back(MakeIndex(perspective, pos.square<KING>(perspective), pos.square<KING>(~perspective)));
  }

  // Get a list of indices for recently changed features
  void KK::AppendChangedIndices(
      const Position& pos, Color perspective,
      IndexList* removed, IndexList* added) {
    // do nothing
  }

}  // namespace Eval::NNUE::Features
