﻿/*
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

//Definition of input features HalfKPKfile of NNUE evaluation function

#include "half_kpkfile.h"
#include "index_list.h"

namespace Eval::NNUE::Features {

  // Orient a square according to perspective (rotates by 180 for black)
  inline Square orient(Color perspective, Square s) {
    return Square(int(s) ^ (bool(perspective) * 63));
  }

  // Find the index of the feature quantity from the king position and PieceSquare and the other king position
  template <Side AssociatedKing>
  inline IndexType HalfKPKfile<AssociatedKing>::MakeIndex(
      Color perspective, Square s, Piece pc, Square ksq, Square other_ksq) {

    return IndexType(orient(perspective, s) + kpp_board_index[pc][perspective] + PS_END * ksq
           + static_cast<IndexType>(SQUARE_NB) * static_cast<IndexType>(PS_END) * static_cast<IndexType>(file_of(other_ksq)));
  }

  // Get a list of indices for active features
  template <Side AssociatedKing>
  void HalfKPKfile<AssociatedKing>::AppendActiveIndices(
      const Position& pos, Color perspective, IndexList* active) {

    Square ksq = orient(perspective, pos.square<KING>(perspective));
    Square other_ksq = orient(perspective, pos.square<KING>(~perspective));
    Bitboard bb = pos.pieces() & ~pos.pieces(KING);
    while (bb) {
      Square s = pop_lsb(&bb);
      active->push_back(MakeIndex(perspective, s, pos.piece_on(s), ksq, other_ksq));
    }
  }

  // Get a list of indices for recently changed features
  template <Side AssociatedKing>
  void HalfKPKfile<AssociatedKing>::AppendChangedIndices(
      const Position& pos, Color perspective,
      IndexList* removed, IndexList* added) {

    Square ksq = orient(perspective, pos.square<KING>(perspective));
    Square other_ksq = orient(perspective, pos.square<KING>(~perspective));
    const auto& dp = pos.state()->dirtyPiece;
    for (int i = 0; i < dp.dirty_num; ++i) {
      Piece pc = dp.piece[i];
      if (type_of(pc) == KING) continue;
      if (dp.from[i] != SQ_NONE)
        removed->push_back(MakeIndex(perspective, dp.from[i], pc, ksq, other_ksq));
      if (dp.to[i] != SQ_NONE)
        added->push_back(MakeIndex(perspective, dp.to[i], pc, ksq, other_ksq));
    }
  }

  template class HalfKPKfile<Side::kFriend>;

}  // namespace Eval::NNUE::Features