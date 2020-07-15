//Definition of input feature quantity KK of NNUE evaluation function

#if defined(EVAL_NNUE)

#include "kk.h"
#include "index_list.h"

namespace Eval {

  namespace NNUE {

    namespace Features {

      // Get a list of indices with a value of 1 among the features
      void KK::AppendActiveIndices(
        const Position& pos, Color perspective, IndexList* active) {
        // do nothing if array size is small to avoid compiler warning
        if (RawFeatures::kMaxActiveDimensions < kMaxActiveDimensions) return;

        active->push_back(MakeIndex(perspective, pos.square<KING>(perspective), pos.square<KING>(~perspective)));
      }

      // Get a list of indices whose values ??have changed from the previous one in the feature quantity
      void KK::AppendChangedIndices(
        const Position& pos, Color perspective,
        IndexList* removed, IndexList* added) {
        // do nothing
      }

      // MakeIndex
      inline IndexType KK::MakeIndex(Color perspective, Square ksq_us, Square ksq_them) {
        if (perspective == BLACK) {
          ksq_us = Inv(ksq_us);
          ksq_them = Inv(ksq_them);
        }

        return static_cast<IndexType>(ksq_us) * static_cast<IndexType>(SQUARE_NB) + ksq_them;
      }

    }  // namespace Features

  }  // namespace NNUE

}  // namespace Eval

#endif  // defined(EVAL_NNUE)
