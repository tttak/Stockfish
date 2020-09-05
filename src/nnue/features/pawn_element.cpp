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

//Definition of input features PawnElement of NNUE evaluation function

#include "pawn_element.h"
#include "index_list.h"
#include "../../pawns.h"

//#define DEBUG_PAWN_ELEMENT

namespace Eval::NNUE::Features {

  // Get a list of indices for active features
  template <PawnElementType PEType>
  void PawnElement<PEType>::AppendActiveIndices(
    const Position& pos, Color perspective, IndexList* active) {

    Color Us     = perspective;
    Color Them   = ~Us;
    Direction Up = pawn_push(Us);

    Bitboard neighbours, stoppers, support, phalanx, opposed;
    Bitboard lever, leverPush, blocked;
    Square s;
    bool backward, passed, doubled;
    const Square* pl = pos.squares<PAWN>(perspective);

    Bitboard ourPawns   = pos.pieces(  Us, PAWN);
    Bitboard theirPawns = pos.pieces(Them, PAWN);

    Bitboard doubleAttackThem = pawn_double_attacks_bb(theirPawns, Them);

    int pawn_count = 0;

    // Loop through all pawns of the current color and score each pawn
    while ((s = *pl++) != SQ_NONE)
    {
      assert(pos.piece_on(s) == make_piece(Us, PAWN));

      Rank r = relative_rank(Us, s);

      // Flag the pawn
      opposed    = theirPawns & forward_file_bb(Us, s);
      blocked    = theirPawns & (s + Up);
      stoppers   = theirPawns & passed_pawn_span(Us, s);
      lever      = theirPawns & pawn_attacks_bb(Us, s);
      leverPush  = theirPawns & pawn_attacks_bb(Us, s + Up);
      doubled    = ourPawns   & (s - Up);
      neighbours = ourPawns   & adjacent_files_bb(s);
      phalanx    = neighbours & rank_bb(s);
      support    = neighbours & rank_bb(s - Up);

      // A pawn is backward when it is behind all pawns of the same color on
      // the adjacent files and cannot safely advance.
      backward =  !(neighbours & forward_ranks_bb(Them, s + Up))
                && (leverPush | blocked);

      // A pawn is passed if one of the three following conditions is true:
      // (a) there is no stoppers except some levers
      // (b) the only stoppers are the leverPush, but we outnumber them
      // (c) there is only one front stopper which can be levered.
      //     (Refined in Evaluation::passed)
      passed =   !(stoppers ^ lever)
              || (   !(stoppers ^ leverPush)
                  && popcount(phalanx) >= popcount(leverPush))
              || (   stoppers == blocked && r >= RANK_5
                  && (shift_(support, Up) & ~(theirPawns | doubleAttackThem)));

      passed &= !(forward_file_bb(Us, s) & ourPawns);

      IndexType index = MakeIndex(pawn_count
                                  , neighbours, stoppers, support, phalanx, opposed
                                  , lever, leverPush, blocked
                                  , backward, passed, doubled
                                  );
#if defined(DEBUG_PAWN_ELEMENT)
      std::cout << "perspective="  << perspective
                << ", pawn_count=" << pawn_count
                << ", neighbours=" << popcount(neighbours)
                << ", stoppers="   << popcount(stoppers)
                << ", support="    << popcount(support)
                << ", phalanx="    << popcount(phalanx)
                << ", opposed="    << popcount(opposed)
                << ", lever="      << popcount(lever)
                << ", leverPush="  << popcount(leverPush)
                << ", blocked="    << popcount(blocked)
                << ", backward="   << backward
                << ", passed="     << passed
                << ", doubled="    << doubled
                << ", index="      << index
                << std::endl;
#endif
      active->push_back(index);
      pawn_count++;
    }

    while (pawn_count < 8) {
      IndexType index = MakeIndexOfNoPiece(pawn_count);
#if defined(DEBUG_PAWN_ELEMENT)
      std::cout << "perspective="  << perspective
                << ", pawn_count=" << pawn_count
                << ", index="      << index
                << std::endl;
#endif
      active->push_back(index);
      pawn_count++;
    }
  }

  // Get a list of indices for recently changed features
  template <PawnElementType PEType>
  void PawnElement<PEType>::AppendChangedIndices(
    const Position& pos, Color perspective,
    IndexList* removed, IndexList* added) {
    // TODO : difference calculation
    // Not implemented.
    assert(false);
  }

  // MakeIndex
  template <PawnElementType PEType>
  inline IndexType PawnElement<PEType>::MakeIndex(int pawn_count
                                   , Bitboard neighbours, Bitboard stoppers, Bitboard support ,Bitboard phalanx, Bitboard opposed
                                   , Bitboard lever, Bitboard leverPush, Bitboard blocked
                                   , bool backward, bool passed, bool doubled
                                  ) {
    static Bitboard ZERO_BB = Bitboard(0);

    return MakeIndex(pawn_count,
               (PEType == PawnElementType::kNeighbours) ? (ZERO_BB != neighbours) :
               (PEType == PawnElementType::kStoppers  ) ? (ZERO_BB != stoppers  ) :
               (PEType == PawnElementType::kSupport   ) ? (ZERO_BB != support   ) :
               (PEType == PawnElementType::kPhalanx   ) ? (ZERO_BB != phalanx   ) :
               (PEType == PawnElementType::kOpposed   ) ? (ZERO_BB != opposed   ) :
               (PEType == PawnElementType::kLever     ) ? (ZERO_BB != lever     ) :
               (PEType == PawnElementType::kLeverPush ) ? (ZERO_BB != leverPush ) :
               (PEType == PawnElementType::kBlocked   ) ? (ZERO_BB != blocked   ) :
               (PEType == PawnElementType::kBackward  ) ? backward :
               (PEType == PawnElementType::kPassed    ) ? passed   :
               (PEType == PawnElementType::kDoubled   ) ? doubled  :
               false
             );
  }

  // MakeIndex
  template <PawnElementType PEType>
  inline IndexType PawnElement<PEType>::MakeIndex(int pawn_count, bool element) {
    return MakeIndexOfNoPiece(pawn_count) + 1 + element;
  }

  // MakeIndexOfNoPiece
  template <PawnElementType PEType>
  inline IndexType PawnElement<PEType>::MakeIndexOfNoPiece(int pawn_count) {
    return (2 + 1) * pawn_count;
  }

  // pawn_double_attacks_bb() returns the squares doubly attacked by pawns of the
  // given color from the squares in the given bitboard.
  template <PawnElementType PEType>
  Bitboard PawnElement<PEType>::pawn_double_attacks_bb(Bitboard b, Color C) {
    return C == WHITE ? shift<NORTH_WEST>(b) & shift<NORTH_EAST>(b)
                      : shift<SOUTH_WEST>(b) & shift<SOUTH_EAST>(b);
  }

  // shift() moves a bitboard one step along direction D
  template <PawnElementType PEType>
  Bitboard PawnElement<PEType>::shift_(Bitboard b, Direction D) {
    return  D == NORTH      ?  b             << 8 : D == SOUTH      ?  b             >> 8
          : D == NORTH+NORTH?  b             <<16 : D == SOUTH+SOUTH?  b             >>16
          : D == EAST       ? (b & ~FileHBB) << 1 : D == WEST       ? (b & ~FileABB) >> 1
          : D == NORTH_EAST ? (b & ~FileHBB) << 9 : D == NORTH_WEST ? (b & ~FileABB) << 7
          : D == SOUTH_EAST ? (b & ~FileHBB) >> 7 : D == SOUTH_WEST ? (b & ~FileABB) >> 9
          : 0;
  }

  template class PawnElement<PawnElementType::kNeighbours>;
  template class PawnElement<PawnElementType::kStoppers  >;
  template class PawnElement<PawnElementType::kSupport   >;
  template class PawnElement<PawnElementType::kPhalanx   >;
  template class PawnElement<PawnElementType::kOpposed   >;
  template class PawnElement<PawnElementType::kLever     >;
  template class PawnElement<PawnElementType::kLeverPush >;
  template class PawnElement<PawnElementType::kBlocked   >;
  template class PawnElement<PawnElementType::kBackward  >;
  template class PawnElement<PawnElementType::kPassed    >;
  template class PawnElement<PawnElementType::kDoubled   >;

}  // namespace Eval::NNUE::Features
