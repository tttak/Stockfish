//Definition of input feature quantity KK of NNUE evaluation function

#ifndef _NNUE_FEATURES_KK_H_
#define _NNUE_FEATURES_KK_H_

#if defined(EVAL_NNUE)

#include "../../../evaluate.h"
#include "features_common.h"

namespace Eval {

  namespace NNUE {

    namespace Features {

      // Feature KK
      class KK {
      public:
        // feature quantity name
        static constexpr const char* kName = "KK";
        // Hash value embedded in the evaluation function file
        static constexpr std::uint32_t kHashValue = 0x62924F92u;
        // number of feature dimensions
        static constexpr IndexType kDimensions = static_cast<IndexType>(SQUARE_NB) * static_cast<IndexType>(SQUARE_NB);
        // The maximum value of the number of indexes whose value is 1 at the same time among the feature values
        static constexpr IndexType kMaxActiveDimensions = 1;
        // Timing of full calculation instead of difference calculation
        static constexpr TriggerEvent kRefreshTrigger = TriggerEvent::kAnyKingMoved;

        // Get a list of indices with a value of 1 among the features
        static void AppendActiveIndices(const Position& pos, Color perspective,
          IndexList* active);

        // Get a list of indices whose values ??have changed from the previous one in the feature quantity
        static void AppendChangedIndices(const Position& pos, Color perspective,
          IndexList* removed, IndexList* added);

        // MakeIndex
        static IndexType MakeIndex(Color perspective, Square ksq_us, Square ksq_them);
      };

    }  // namespace Features

  }  // namespace NNUE

}  // namespace Eval

#endif  // defined(EVAL_NNUE)

#endif
