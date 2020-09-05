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

#ifndef NNUE_FEATURES_MOBILITY_H_INCLUDED
#define NNUE_FEATURES_MOBILITY_H_INCLUDED

#include "../../evaluate.h"
#include "features_common.h"

namespace Eval::NNUE::Features {

  // Feature Mobility
  class Mobility {
   public:
    // Feature name
    static constexpr const char* kName = "Mobility";
    // Hash value embedded in the evaluation file
    static constexpr std::uint32_t kHashValue = 0x62924F91u;

    static constexpr IndexType kMaxPieceCount = 2;
    static constexpr IndexType kMaxMobilityCount = 28 + 1;

    // Number of feature dimensions
    //  4 : Knight, Bishop, Rook, Queen
    static constexpr IndexType kDimensions = kMaxPieceCount * 4 * kMaxMobilityCount;

    // Maximum number of simultaneously active features
    //  4 : Knight, Bishop, Rook, Queen
    static constexpr IndexType kMaxActiveDimensions = kMaxPieceCount * 4;

    // Trigger for full calculation instead of difference calculation
    static constexpr TriggerEvent kRefreshTrigger = TriggerEvent::kNone;

    // Get a list of indices for active features
    static void AppendActiveIndices(const Position& pos, Color perspective,
                                    IndexList* active);

    // Get a list of indices for recently changed features
    static void AppendChangedIndices(const Position& pos, Color perspective,
                                     IndexList* removed, IndexList* added);

   private:
    static void CalcMobilityCount(int mobilityCount[4][2], const Position& pos, Color perspective);

    template<PieceType Pt>
    static void CalcMobilityCount(int mobilityCount[4][2], const Position& pos, Color perspective, const Bitboard& mobilityArea);

    static IndexType MakeIndex(int pt_index, int piece_count, int mobility_count);

    static Bitboard shift(Bitboard b, Direction D);
  };

}  // namespace Eval::NNUE::Features

#endif // #ifndef NNUE_FEATURES_MOBILITY_H_INCLUDED
