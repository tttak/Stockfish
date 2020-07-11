//Definition of input feature quantity Mobility of NNUE evaluation function

#ifndef _NNUE_FEATURES_MOBILITY_H_
#define _NNUE_FEATURES_MOBILITY_H_

#if defined(EVAL_NNUE)

#include "../../../evaluate.h"
#include "features_common.h"

namespace Eval {

  namespace NNUE {

    namespace Features {

      // Feature Mobility
      class Mobility {
      public:
        // feature quantity name
        static constexpr const char* kName = "Mobility";
        // Hash value embedded in the evaluation function file
        static constexpr std::uint32_t kHashValue = 0x62924F91u;

        static constexpr IndexType kMaxPieceCount = 3;
        static constexpr IndexType kMaxMobilityCount = 28 + 1;

        // number of feature dimensions
        //  4 : Knight, Bishop, Rook, Queen
        static constexpr IndexType kDimensions = kMaxPieceCount * 4 * kMaxMobilityCount;

        // The maximum value of the number of indexes whose value is 1 at the same time among the feature values
        //  4 : Knight, Bishop, Rook, Queen
        static constexpr IndexType kMaxActiveDimensions = kMaxPieceCount * 4;

        // Timing of full calculation instead of difference calculation
        // TODO : difference calculation
        static constexpr TriggerEvent kRefreshTrigger = TriggerEvent::kAnyPieceMoved;

        // Get a list of indices with a value of 1 among the features
        static void AppendActiveIndices(const Position& pos, Color perspective,
          IndexList* active);

        // Get a list of indices whose values ??have changed from the previous one in the feature quantity
        static void AppendChangedIndices(const Position& pos, Color perspective,
          IndexList* removed, IndexList* added);

      private:
        template<PieceType Pt>
        static void AppendActiveIndices(const Position& pos, Color perspective, IndexList* active, const Bitboard& mobilityArea);

        template<PieceType Pt>
        static IndexType MakeIndex(int piece_count, int mob);

        static Bitboard shift(Bitboard b, Direction D);
      };

    }  // namespace Features

  }  // namespace NNUE

}  // namespace Eval

#endif  // defined(EVAL_NNUE)

#endif
