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

#ifndef NNUE_FEATURES_PAWN_ELEMENT_H_INCLUDED
#define NNUE_FEATURES_PAWN_ELEMENT_H_INCLUDED

#include "../../evaluate.h"
#include "features_common.h"

namespace Eval::NNUE::Features {

  enum class PawnElementType {
    kNeighbours,
    kStoppers,
    kSupport,
    kPhalanx,
    kOpposed,
    kLever,
    kLeverPush,
    kBlocked,
    kBackward,
    kPassed,
    kDoubled,
  };

  // Feature PawnElement
  template <PawnElementType PEType>
  class PawnElement {
   public:
    // Feature name
    static constexpr const char* kName = "PawnElement";
    // Hash value embedded in the evaluation file
    static constexpr std::uint32_t kHashValue = 0x82924F91u;

    // Number of feature dimensions
    static constexpr IndexType kDimensions = (2 + 1) * 8;

    // Maximum number of simultaneously active features
    static constexpr IndexType kMaxActiveDimensions = 8;

    // Trigger for full calculation instead of difference calculation
    // TODO : difference calculation
    static constexpr TriggerEvent kRefreshTrigger = TriggerEvent::kAnyPieceMoved;

    // Get a list of indices for active features
    static void AppendActiveIndices(const Position& pos, Color perspective,
                                    IndexList* active);

    // Get a list of indices for recently changed features
    static void AppendChangedIndices(const Position& pos, Color perspective,
                                     IndexList* removed, IndexList* added);

    // MakeIndex
    static IndexType MakeIndex(int pawn_count
                               , Bitboard neighbours, Bitboard stoppers, Bitboard support, Bitboard phalanx, Bitboard opposed
                               , Bitboard lever, Bitboard leverPush, Bitboard blocked
                               , bool backward, bool passed, bool doubled
                              );

    // MakeIndex
    static IndexType MakeIndex(int pawn_count, bool element);

    // MakeIndexOfNoPiece
    static IndexType MakeIndexOfNoPiece(int pawn_count);

   private:
    static Bitboard pawn_double_attacks_bb(Bitboard b, Color C);
    static Bitboard shift_(Bitboard b, Direction D);
  };

}  // namespace Eval::NNUE::Features

#endif // #ifndef NNUE_FEATURES_PAWN_ELEMENT_H_INCLUDED
