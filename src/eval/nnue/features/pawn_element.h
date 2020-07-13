//Definition of input feature quantity PawnElement of NNUE evaluation function

#ifndef _NNUE_FEATURES_PAWN_ELEMENT_H_
#define _NNUE_FEATURES_PAWN_ELEMENT_H_

#if defined(EVAL_NNUE)

#include "../../../evaluate.h"
#include "features_common.h"

namespace Eval {

  namespace NNUE {

    namespace Features {

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
        // feature quantity name
        static constexpr const char* kName = "PawnElement";
        // Hash value embedded in the evaluation function file
        static constexpr std::uint32_t kHashValue = 0x82924F91u;

        // number of feature dimensions
        static constexpr IndexType kDimensions = (2 + 1) * 8;

        // The maximum value of the number of indexes whose value is 1 at the same time among the feature values
        static constexpr IndexType kMaxActiveDimensions = 8;

        // Timing of full calculation instead of difference calculation
        // TODO : difference calculation
        static constexpr TriggerEvent kRefreshTrigger = TriggerEvent::kAnyPieceMoved;

        // Get a list of indices with a value of 1 among the features
        static void AppendActiveIndices(const Position& pos, Color perspective,
          IndexList* active);

        // Get a list of indices whose values ??have changed from the previous one in the feature quantity
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

    }  // namespace Features

  }  // namespace NNUE

}  // namespace Eval

#endif  // defined(EVAL_NNUE)

#endif
