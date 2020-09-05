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

//Definition of input features HalfKPE4 of NNUE evaluation function

#include "half_kpe4.h"
#include "index_list.h"

#if defined(USE_MOBILITY_IN_STATEINFO)

//#define DEBUG_HALFKPE4

#if defined(DEBUG_HALFKPE4)
namespace UCI {
  std::string square(Square s);
}
#endif

namespace Eval::NNUE::Features {

  Piece sqBonaPieceToPiece[] = {
      W_PAWN  , B_PAWN
    , W_KNIGHT, B_KNIGHT
    , W_BISHOP, B_BISHOP
    , W_ROOK  , B_ROOK
    , W_QUEEN , B_QUEEN
    , W_KING  , B_KING
  };

  int mobilityThreshold[] = {
      PAWN    // PAWN
    , PAWN    // KNIGHT
    , PAWN    // BISHOP
    , BISHOP  // ROOK
    , ROOK    // QUEEN
    , KING    // KING
  };

  inline Bitboard MergeAllMobility(const StateInfo* st, Color perspective) {
    return   st->mobility[perspective][0]
           | st->mobility[perspective][1]
           | st->mobility[perspective][2]
           | st->mobility[perspective][3]
           | st->mobility[perspective][4]
           | st->mobility[perspective][5];
  }

  // CalcMobility
  template <Side AssociatedKing, EffectType EffectTypeUs, EffectType EffectTypeThem>
  inline void HalfKPE4<AssociatedKing, EffectTypeUs, EffectTypeThem>::CalcMobility(const StateInfo* st, Bitboard mobility[2][2][6]) {
    for (Color color : { WHITE, BLACK }) {
      for (EffectType et : { EffectType::kAll, EffectType::kFromSmallerPiecesOnly }) {
        if (EffectTypeUs == EffectType::kAll || EffectTypeThem == EffectType::kAll) {
          Bitboard b = MergeAllMobility(st, color);
          for (int i = 0; i < 6; i++) {
            mobility[color][(int)EffectType::kAll][i] = b;
          }
        }

        if (EffectTypeUs == EffectType::kFromSmallerPiecesOnly || EffectTypeThem == EffectType::kFromSmallerPiecesOnly) {
          mobility[color][(int)EffectType::kFromSmallerPiecesOnly][0] = st->mobility[color][0];
          for (int i = 1; i < 6; i++) {
            mobility[color][(int)EffectType::kFromSmallerPiecesOnly][i] = mobility[color][(int)EffectType::kFromSmallerPiecesOnly][i-1] | st->mobility[color][i];
          }
        }
      }
    }
  }

  inline bool IsDirty(const DirtyPiece& dp, Square sq) {
    for (int i = 0; i < dp.dirty_num; ++i) {
      if (sq == dp.to[i]) {
        return true;
      }
    }
    return false;
  }

  // Orient a square according to perspective (rotates by 180 for black)
  inline Square orient(Color perspective, Square s) {
    return Square(int(s) ^ (bool(perspective) * 63));
  }

  // Find the index of the feature quantity from the king position and PieceSquare and effect
  template <Side AssociatedKing, EffectType EffectTypeUs, EffectType EffectTypeThem>
  inline IndexType HalfKPE4<AssociatedKing, EffectTypeUs, EffectTypeThem>::MakeIndex(
      Color perspective, Square s, Piece pc, Square ksq, bool hasEffect_us, bool hasEffect_them) {

#if defined(DEBUG_HALFKPE4)
    std::cout << "hasEffect_us=" << hasEffect_us << ", hasEffect_them=" << hasEffect_them << std::endl;
#endif

    IndexType effect_index = static_cast<IndexType>(hasEffect_us * 2 + hasEffect_them);
    return IndexType(orient(perspective, s) + kpp_board_index[pc][perspective] + PS_END * ksq
           + effect_index * static_cast<IndexType>(SQUARE_NB) * static_cast<IndexType>(PS_END));
  }

  // Get a list of indices for active features
  template <Side AssociatedKing, EffectType EffectTypeUs, EffectType EffectTypeThem>
  void HalfKPE4<AssociatedKing, EffectTypeUs, EffectTypeThem>::AppendActiveIndices(
      const Position& pos, Color perspective, IndexList* active) {

    Bitboard mobility[2][2][6];
    CalcMobility(pos.state(), mobility);

#if defined(DEBUG_HALFKPE4)
    std::cout << "pos=" << std::endl;
    std::cout << pos << std::endl;
    std::cout << "perspective=" << perspective << std::endl;

    for (Color c : { WHITE, BLACK }) {
      for (EffectType et : { EffectType::kAll, EffectType::kFromSmallerPiecesOnly }) {
        for (int i = 0; i < 6; i++) {
          std::cout << "c=" << c << ", et=" << (int)et << ", i=" << i << std::endl;
          std::cout << Bitboards::pretty(mobility[c][(int)et][i]) << std::endl;
        }
      }
    }
#endif

    Square ksq = orient(perspective, pos.square<KING>(perspective));
    Bitboard bb = pos.pieces() & ~pos.pieces(KING);
    while (bb) {
      Square sq = pop_lsb(&bb);
      Piece pc = pos.piece_on(sq);
      PieceType pt = type_of(pc);
      Color color = color_of(pc);

#if defined(DEBUG_HALFKPE4)
      std::cout << "EffectTypeUs=" << (int)EffectTypeUs << std::endl;
      std::cout << "EffectTypeThem=" << (int)EffectTypeThem << std::endl;
      std::cout << "perspective=" << perspective << std::endl;
      std::cout << "sq=" << UCI::square(sq) << std::endl;
      std::cout << "pc=" << pc << std::endl;
      std::cout << "pt=" << pt << std::endl;
      std::cout << "color=" << color << std::endl;
      std::cout << "mobility[" <<  color << "][" << (int)EffectTypeUs   << "][" << mobilityThreshold[pt - 1] - 1 << "]=" << std::endl << Bitboards::pretty(mobility[ color][(int)EffectTypeUs  ][mobilityThreshold[pt - 1] - 1]) << std::endl;
      std::cout << "mobility[" << ~color << "][" << (int)EffectTypeThem << "][" << mobilityThreshold[pt - 1] - 1 << "]=" << std::endl << Bitboards::pretty(mobility[~color][(int)EffectTypeThem][mobilityThreshold[pt - 1] - 1]) << std::endl;
#endif

      active->push_back(MakeIndex(perspective, sq, pc, ksq
                                  , mobility[ color][(int)EffectTypeUs  ][mobilityThreshold[pt - 1] - 1] & sq
                                  , mobility[~color][(int)EffectTypeThem][mobilityThreshold[pt - 1] - 1] & sq));
    }
  }

  // Get a list of indices for recently changed features
  template <Side AssociatedKing, EffectType EffectTypeUs, EffectType EffectTypeThem>
  void HalfKPE4<AssociatedKing, EffectTypeUs, EffectTypeThem>::AppendChangedIndices(
      const Position& pos, Color perspective,
      IndexList* removed, IndexList* added) {

    Bitboard mobility_now [2][2][6];
    Bitboard mobility_prev[2][2][6];

    CalcMobility(pos.state(), mobility_now);
    CalcMobility(pos.state()->previous, mobility_prev);

    Square ksq = orient(perspective, pos.square<KING>(perspective));
    const auto& dp = pos.state()->dirtyPiece;

    for (int i = 0; i < dp.dirty_num; ++i) {
      Piece pc = dp.piece[i];
      if (type_of(pc) == KING) continue;

      if (dp.from[i] != SQ_NONE) {
        Square sq = dp.from[i];
        PieceType pt = type_of(pc);
        Color color = color_of(pc);
        removed->push_back(MakeIndex(perspective, sq, pc, ksq
                                     , mobility_prev[ color][(int)EffectTypeUs  ][mobilityThreshold[pt - 1] - 1] & sq
                                     , mobility_prev[~color][(int)EffectTypeThem][mobilityThreshold[pt - 1] - 1] & sq));
      }

      if (dp.to[i] != SQ_NONE) {
        Square sq = dp.to[i];
        PieceType pt = type_of(pc);
        Color color = color_of(pc);
        added->push_back(MakeIndex(perspective, sq, pc, ksq
                                     , mobility_now[ color][(int)EffectTypeUs  ][mobilityThreshold[pt - 1] - 1] & sq
                                     , mobility_now[~color][(int)EffectTypeThem][mobilityThreshold[pt - 1] - 1] & sq));
      }
    }

    Bitboard bb = pos.pieces() & ~pos.pieces(KING);
    while (bb) {
      Square sq = pop_lsb(&bb);

      if (IsDirty(dp, sq)) {
        continue;
      }

      Piece pc = pos.piece_on(sq);
      PieceType pt = type_of(pc);
      Color color = color_of(pc);

      bool hasEffect_now_us    = mobility_now [ color][(int)EffectTypeUs  ][mobilityThreshold[pt - 1] - 1] & sq;
      bool hasEffect_now_them  = mobility_now [~color][(int)EffectTypeThem][mobilityThreshold[pt - 1] - 1] & sq;
      bool hasEffect_prev_us   = mobility_prev[ color][(int)EffectTypeUs  ][mobilityThreshold[pt - 1] - 1] & sq;
      bool hasEffect_prev_them = mobility_prev[~color][(int)EffectTypeThem][mobilityThreshold[pt - 1] - 1] & sq;

      if (   hasEffect_now_us   != hasEffect_prev_us
          || hasEffect_now_them != hasEffect_prev_them) {
        removed->push_back(MakeIndex(perspective, sq, pc, ksq, hasEffect_prev_us, hasEffect_prev_them));
        added  ->push_back(MakeIndex(perspective, sq, pc, ksq, hasEffect_now_us , hasEffect_now_them ));
      }
    }
  }

  template class HalfKPE4<Side::kFriend, EffectType::kAll                  , EffectType::kAll                  >;
  template class HalfKPE4<Side::kFriend, EffectType::kAll                  , EffectType::kFromSmallerPiecesOnly>;
  template class HalfKPE4<Side::kFriend, EffectType::kFromSmallerPiecesOnly, EffectType::kAll                  >;
  template class HalfKPE4<Side::kFriend, EffectType::kFromSmallerPiecesOnly, EffectType::kFromSmallerPiecesOnly>;

}  // namespace Eval::NNUE::Features

#endif  // defined(USE_MOBILITY_IN_STATEINFO)
