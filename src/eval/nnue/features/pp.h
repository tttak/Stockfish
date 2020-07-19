//Definition of input feature quantity PP of NNUE evaluation function

#ifndef _NNUE_FEATURES_PP_H_
#define _NNUE_FEATURES_PP_H_

#if defined(EVAL_NNUE)

#include "../../../evaluate.h"
#include "features_common.h"

namespace Eval {

  namespace NNUE {

    namespace Features {

      // Feature PP
      class PP {
      public:
        // feature quantity name
        static constexpr const char* kName = "PP";
        // Hash value embedded in the evaluation function file
        static constexpr std::uint32_t kHashValue = 0x62923F91u;
        // number of feature dimensions
        static constexpr IndexType kDimensions = static_cast<IndexType>(fe_end) * (static_cast<IndexType>(fe_end) - 1) / 2;
        // The maximum value of the number of indexes whose value is 1 at the same time among the feature values
        static constexpr IndexType kMaxActiveDimensions = static_cast<IndexType>(PIECE_NUMBER_KING) * (static_cast<IndexType>(PIECE_NUMBER_KING) - 1) / 2;
        // Timing of full calculation instead of difference calculation
        static constexpr TriggerEvent kRefreshTrigger = TriggerEvent::kNone;

        // Get a list of indices with a value of 1 among the features
        static void AppendActiveIndices(const Position& pos, Color perspective,
          IndexList* active);

        // Get a list of indices whose values ??have changed from the previous one in the feature quantity
        static void AppendChangedIndices(const Position& pos, Color perspective,
          IndexList* removed, IndexList* added);

        // MakeIndex
        static IndexType MakeIndex(BonaPiece bp1, BonaPiece bp2);
      };

    }  // namespace Features

  }  // namespace NNUE

}  // namespace Eval

#endif  // defined(EVAL_NNUE)

#endif
