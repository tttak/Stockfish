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

//Definition of input features HalfKPE4 of NNUE evaluation function

#ifndef NNUE_FEATURES_HALF_KPE4_H_INCLUDED
#define NNUE_FEATURES_HALF_KPE4_H_INCLUDED

#include "../../evaluate.h"
#include "features_common.h"

struct StateInfo;

namespace Eval::NNUE::Features {

  enum class EffectType {
    kAll,
    kFromSmallerPiecesOnly,
  };

  // Feature HalfKPE4: Combination of the position of own king
  // and the position of pieces other than kings
  // and effect
  template <Side AssociatedKing, EffectType EffectTypeUs, EffectType EffectTypeThem>
  class HalfKPE4 {
   public:
    // Feature name
    static constexpr const char* kName = "HalfKPE4(Friend)";
    // Hash value embedded in the evaluation file
    static constexpr std::uint32_t kHashValue =
        0x5D69D3B9u ^ (AssociatedKing == Side::kFriend);
    // Number of feature dimensions
    static constexpr IndexType kDimensions =
        static_cast<IndexType>(SQUARE_NB) * static_cast<IndexType>(PS_END) * 2 * 2;
    // Maximum number of simultaneously active features
    static constexpr IndexType kMaxActiveDimensions = 30; // Kings don't count
    // Trigger for full calculation instead of difference calculation
    static constexpr TriggerEvent kRefreshTrigger = TriggerEvent::kFriendKingMoved;

    // Get a list of indices for active features
    static void AppendActiveIndices(const Position& pos, Color perspective,
                                    IndexList* active);

    // Get a list of indices for recently changed features
    static void AppendChangedIndices(const Position& pos, Color perspective,
                                     IndexList* removed, IndexList* added);

   private:
    // Find the index of the feature quantity from the king position and PieceSquare and effect
    static IndexType MakeIndex(Color perspective, Square s, Piece pc, Square ksq, bool hasEffect_us, bool hasEffect_them);

    // CalcMobility
    static void CalcMobility(const StateInfo* st, Bitboard mobility[2][2][6]);
  };

}  // namespace Eval::NNUE::Features

#endif // #ifndef NNUE_FEATURES_HALF_KPE4_H_INCLUDED
