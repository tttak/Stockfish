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

//Definition of input features Mobility of NNUE evaluation function

#include "mobility.h"
#include "index_list.h"
#include "../../pawns.h"

#if defined(USE_MOBILITY_COUNT_IN_STATEINFO)

namespace Eval::NNUE::Features {

  // Get a list of indices for active features
  void Mobility::AppendActiveIndices(
    const Position& pos, Color perspective, IndexList* active) {

    // mobilityCount[PieceType(Knight, Bishop, Rook, Queen)][Piece Count]
    int mobilityCount[4][2];
    CalcMobilityCount(mobilityCount, pos, perspective);

    for (int pt_index = 0; pt_index < 4; pt_index++) {
      for (int piece_count = 0; piece_count < 2; piece_count++) {
        active->push_back(MakeIndex(pt_index, piece_count, mobilityCount[pt_index][piece_count]));
        pos.state()->mobilityCount[perspective][pt_index][piece_count] = mobilityCount[pt_index][piece_count];
      }
    }
  }

  // Get a list of indices for recently changed features
  void Mobility::AppendChangedIndices(
    const Position& pos, Color perspective,
    IndexList* removed, IndexList* added) {

    // mobilityCount[PieceType(Knight, Bishop, Rook, Queen)][Piece Count]
    int mobilityCount[4][2];
    CalcMobilityCount(mobilityCount, pos, perspective);

    const auto prev = pos.state()->previous;

    for (int pt_index = 0; pt_index < 4; pt_index++) {
      for (int piece_count = 0; piece_count < 2; piece_count++) {
        if (prev->mobilityCount[perspective][pt_index][piece_count] != mobilityCount[pt_index][piece_count]) {
          removed->push_back(MakeIndex(pt_index, piece_count, prev->mobilityCount[perspective][pt_index][piece_count]));
          added  ->push_back(MakeIndex(pt_index, piece_count, mobilityCount[pt_index][piece_count]));
        }
        pos.state()->mobilityCount[perspective][pt_index][piece_count] = mobilityCount[pt_index][piece_count];
      }
    }
  }

  // CalcMobilityCount
  void Mobility::CalcMobilityCount(int mobilityCount[4][2], const Position& pos, Color perspective) {
    Direction Up   = pawn_push(perspective);
    Direction Down = -Up;
    Bitboard LowRanks = (perspective == WHITE ? Rank2BB | Rank3BB : Rank7BB | Rank6BB);

    // Find our pawns that are blocked or on the first two ranks
    Bitboard b = pos.pieces(perspective, PAWN) & (shift(pos.pieces(), Down) | LowRanks);

    Bitboard pawnAttacks_them = (perspective == WHITE) ? pawn_attacks_bb<BLACK>(pos.pieces(~perspective, PAWN))
                                                       : pawn_attacks_bb<WHITE>(pos.pieces(~perspective, PAWN));

    // Squares occupied by those pawns, by our king or queen, by blockers to attacks on our king
    // or controlled by enemy pawns are excluded from the mobility area.
    Bitboard mobilityArea = ~(b | pos.pieces(perspective, KING, QUEEN) | pos.blockers_for_king(perspective) | pawnAttacks_them);

    CalcMobilityCount<KNIGHT>(mobilityCount, pos, perspective, mobilityArea);
    CalcMobilityCount<BISHOP>(mobilityCount, pos, perspective, mobilityArea);
    CalcMobilityCount<ROOK>  (mobilityCount, pos, perspective, mobilityArea);
    CalcMobilityCount<QUEEN> (mobilityCount, pos, perspective, mobilityArea);
  }

  // CalcMobilityCount
  template<PieceType Pt>
  void Mobility::CalcMobilityCount(int mobilityCount[4][2], const Position& pos, Color perspective, const Bitboard& mobilityArea) {
    const Square* pl = pos.squares<Pt>(perspective);

    Bitboard b;
    int piece_count = 0;

    for (Square s = *pl; s != SQ_NONE; s = *++pl)
    {
      // Find attacked squares, including x-ray attacks for bishops and rooks
      b = Pt == BISHOP ? attacks_bb<BISHOP>(s, pos.pieces() ^ pos.pieces(QUEEN))
        : Pt ==   ROOK ? attacks_bb<  ROOK>(s, pos.pieces() ^ pos.pieces(QUEEN) ^ pos.pieces(perspective, ROOK))
                       : attacks_bb<Pt>(s, pos.pieces());

      int mob = popcount(b & mobilityArea);

      mobilityCount[Pt - 2][piece_count] = mob + 1;
      piece_count++;

      if (piece_count >= kMaxPieceCount) {
        break;
      }
    }

    while (piece_count < kMaxPieceCount) {
      mobilityCount[Pt - 2][piece_count] = 0;
      piece_count++;
    }
  }

  // MakeIndex
  IndexType Mobility::MakeIndex(int pt_index, int piece_count, int mobility_count) {
    return (pt_index * kMaxPieceCount + piece_count) * kMaxMobilityCount + mobility_count;
  }

  // shift() moves a bitboard one step along direction D
  Bitboard Mobility::shift(Bitboard b, Direction D) {
    return  D == NORTH      ?  b             << 8 : D == SOUTH      ?  b             >> 8
          : D == NORTH+NORTH?  b             <<16 : D == SOUTH+SOUTH?  b             >>16
          : D == EAST       ? (b & ~FileHBB) << 1 : D == WEST       ? (b & ~FileABB) >> 1
          : D == NORTH_EAST ? (b & ~FileHBB) << 9 : D == NORTH_WEST ? (b & ~FileABB) << 7
          : D == SOUTH_EAST ? (b & ~FileHBB) >> 7 : D == SOUTH_WEST ? (b & ~FileABB) >> 9
          : 0;
  }

}  // namespace Eval::NNUE::Features

#endif  // defined(USE_MOBILITY_COUNT_IN_STATEINFO)
