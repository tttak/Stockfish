// Specialization of NNUE evaluation function feature conversion class template for Pawn

#ifndef _NNUE_TRAINER_FEATURES_FACTORIZER_PAWN_H_
#define _NNUE_TRAINER_FEATURES_FACTORIZER_PAWN_H_

#if defined(EVAL_NNUE)

#include "../../features/pawn.h"
#include "../../features/pawn_element.h"
#include "factorizer.h"

namespace Eval {

namespace NNUE {

namespace Features {

// Class template that converts input features into learning features
// Specialization for Pawn
template <>
class Factorizer<Pawn> {
 private:
  using FeatureType = Pawn;

  // The maximum value of the number of indexes whose value is 1 at the same time among the feature values
  static constexpr IndexType kMaxActiveDimensions =
      FeatureType::kMaxActiveDimensions;

  // Type of learning feature
  enum TrainingFeatureType {
    kFeaturesPawn,
    kFeaturesPawnElement_Neighbours,
    kFeaturesPawnElement_Stoppers  ,
    kFeaturesPawnElement_Support   ,
    kFeaturesPawnElement_Phalanx   ,
    kFeaturesPawnElement_Opposed   ,
    kFeaturesPawnElement_Lever     ,
    kFeaturesPawnElement_LeverPush ,
    kFeaturesPawnElement_Blocked   ,
    kFeaturesPawnElement_Backward  ,
    kFeaturesPawnElement_Passed    ,
    kFeaturesPawnElement_Doubled   ,
    //
    kNumTrainingFeatureTypes,
  };

  // Learning feature information
  static constexpr FeatureProperties kProperties[] = {
    // kFeaturesPawn
    {true, FeatureType::kDimensions},
    // kFeaturesPawnElement_XXX
    {true, Factorizer<PawnElement<PawnElementType::kNeighbours>>::GetDimensions()},
    {true, Factorizer<PawnElement<PawnElementType::kStoppers  >>::GetDimensions()},
    {true, Factorizer<PawnElement<PawnElementType::kSupport   >>::GetDimensions()},
    {true, Factorizer<PawnElement<PawnElementType::kPhalanx   >>::GetDimensions()},
    {true, Factorizer<PawnElement<PawnElementType::kOpposed   >>::GetDimensions()},
    {true, Factorizer<PawnElement<PawnElementType::kLever     >>::GetDimensions()},
    {true, Factorizer<PawnElement<PawnElementType::kLeverPush >>::GetDimensions()},
    {true, Factorizer<PawnElement<PawnElementType::kBlocked   >>::GetDimensions()},
    {true, Factorizer<PawnElement<PawnElementType::kBackward  >>::GetDimensions()},
    {true, Factorizer<PawnElement<PawnElementType::kPassed    >>::GetDimensions()},
    {true, Factorizer<PawnElement<PawnElementType::kDoubled   >>::GetDimensions()},
  };
  static_assert(GetArrayLength(kProperties) == kNumTrainingFeatureTypes, "");

 public:
  // Get the dimensionality of the learning feature
  static constexpr IndexType GetDimensions() {
    return GetActiveDimensions(kProperties);
  }

  // Get index of learning feature and scale of learning rate
  static void AppendTrainingFeatures(
      IndexType base_index, std::vector<TrainingFeature>* training_features) {
    // kFeaturesPawn
    IndexType index_offset = AppendBaseFeature<FeatureType>(
        kProperties[kFeaturesPawn], base_index, training_features);

    // kFeaturesPawnElement_XXX
    IndexType pawn_count = base_index / (2048 + 1);
    IndexType tmp        = base_index % (2048 + 1);

    if (tmp == 0) {
      index_offset += InheritFeaturesIfRequired<PawnElement<PawnElementType::kNeighbours>>(index_offset, kProperties[kFeaturesPawnElement_Neighbours], PawnElement<PawnElementType::kNeighbours>::MakeIndexOfNoPiece(pawn_count), training_features);
      index_offset += InheritFeaturesIfRequired<PawnElement<PawnElementType::kStoppers  >>(index_offset, kProperties[kFeaturesPawnElement_Stoppers  ], PawnElement<PawnElementType::kStoppers  >::MakeIndexOfNoPiece(pawn_count), training_features);
      index_offset += InheritFeaturesIfRequired<PawnElement<PawnElementType::kSupport   >>(index_offset, kProperties[kFeaturesPawnElement_Support   ], PawnElement<PawnElementType::kSupport   >::MakeIndexOfNoPiece(pawn_count), training_features);
      index_offset += InheritFeaturesIfRequired<PawnElement<PawnElementType::kPhalanx   >>(index_offset, kProperties[kFeaturesPawnElement_Phalanx   ], PawnElement<PawnElementType::kPhalanx   >::MakeIndexOfNoPiece(pawn_count), training_features);
      index_offset += InheritFeaturesIfRequired<PawnElement<PawnElementType::kOpposed   >>(index_offset, kProperties[kFeaturesPawnElement_Opposed   ], PawnElement<PawnElementType::kOpposed   >::MakeIndexOfNoPiece(pawn_count), training_features);
      index_offset += InheritFeaturesIfRequired<PawnElement<PawnElementType::kLever     >>(index_offset, kProperties[kFeaturesPawnElement_Lever     ], PawnElement<PawnElementType::kLever     >::MakeIndexOfNoPiece(pawn_count), training_features);
      index_offset += InheritFeaturesIfRequired<PawnElement<PawnElementType::kLeverPush >>(index_offset, kProperties[kFeaturesPawnElement_LeverPush ], PawnElement<PawnElementType::kLeverPush >::MakeIndexOfNoPiece(pawn_count), training_features);
      index_offset += InheritFeaturesIfRequired<PawnElement<PawnElementType::kBlocked   >>(index_offset, kProperties[kFeaturesPawnElement_Blocked   ], PawnElement<PawnElementType::kBlocked   >::MakeIndexOfNoPiece(pawn_count), training_features);
      index_offset += InheritFeaturesIfRequired<PawnElement<PawnElementType::kBackward  >>(index_offset, kProperties[kFeaturesPawnElement_Backward  ], PawnElement<PawnElementType::kBackward  >::MakeIndexOfNoPiece(pawn_count), training_features);
      index_offset += InheritFeaturesIfRequired<PawnElement<PawnElementType::kPassed    >>(index_offset, kProperties[kFeaturesPawnElement_Passed    ], PawnElement<PawnElementType::kPassed    >::MakeIndexOfNoPiece(pawn_count), training_features);
      index_offset += InheritFeaturesIfRequired<PawnElement<PawnElementType::kDoubled   >>(index_offset, kProperties[kFeaturesPawnElement_Doubled   ], PawnElement<PawnElementType::kDoubled   >::MakeIndexOfNoPiece(pawn_count), training_features);
    }
    else {
      tmp -= 1;
      bool doubled    = tmp % 2; tmp /= 2;
      bool passed     = tmp % 2; tmp /= 2;
      bool backward   = tmp % 2; tmp /= 2;
      bool blocked    = tmp % 2; tmp /= 2;
      bool leverPush  = tmp % 2; tmp /= 2;
      bool lever      = tmp % 2; tmp /= 2;
      bool opposed    = tmp % 2; tmp /= 2;
      bool phalanx    = tmp % 2; tmp /= 2;
      bool support    = tmp % 2; tmp /= 2;
      bool stoppers   = tmp % 2; tmp /= 2;
      bool neighbours = tmp % 2; tmp /= 2;

      index_offset += InheritFeaturesIfRequired<PawnElement<PawnElementType::kNeighbours>>(index_offset, kProperties[kFeaturesPawnElement_Neighbours], PawnElement<PawnElementType::kNeighbours>::MakeIndex(pawn_count, neighbours), training_features);
      index_offset += InheritFeaturesIfRequired<PawnElement<PawnElementType::kStoppers  >>(index_offset, kProperties[kFeaturesPawnElement_Stoppers  ], PawnElement<PawnElementType::kStoppers  >::MakeIndex(pawn_count, stoppers  ), training_features);
      index_offset += InheritFeaturesIfRequired<PawnElement<PawnElementType::kSupport   >>(index_offset, kProperties[kFeaturesPawnElement_Support   ], PawnElement<PawnElementType::kSupport   >::MakeIndex(pawn_count, support   ), training_features);
      index_offset += InheritFeaturesIfRequired<PawnElement<PawnElementType::kPhalanx   >>(index_offset, kProperties[kFeaturesPawnElement_Phalanx   ], PawnElement<PawnElementType::kPhalanx   >::MakeIndex(pawn_count, phalanx   ), training_features);
      index_offset += InheritFeaturesIfRequired<PawnElement<PawnElementType::kOpposed   >>(index_offset, kProperties[kFeaturesPawnElement_Opposed   ], PawnElement<PawnElementType::kOpposed   >::MakeIndex(pawn_count, opposed   ), training_features);
      index_offset += InheritFeaturesIfRequired<PawnElement<PawnElementType::kLever     >>(index_offset, kProperties[kFeaturesPawnElement_Lever     ], PawnElement<PawnElementType::kLever     >::MakeIndex(pawn_count, lever     ), training_features);
      index_offset += InheritFeaturesIfRequired<PawnElement<PawnElementType::kLeverPush >>(index_offset, kProperties[kFeaturesPawnElement_LeverPush ], PawnElement<PawnElementType::kLeverPush >::MakeIndex(pawn_count, leverPush ), training_features);
      index_offset += InheritFeaturesIfRequired<PawnElement<PawnElementType::kBlocked   >>(index_offset, kProperties[kFeaturesPawnElement_Blocked   ], PawnElement<PawnElementType::kBlocked   >::MakeIndex(pawn_count, blocked   ), training_features);
      index_offset += InheritFeaturesIfRequired<PawnElement<PawnElementType::kBackward  >>(index_offset, kProperties[kFeaturesPawnElement_Backward  ], PawnElement<PawnElementType::kBackward  >::MakeIndex(pawn_count, backward  ), training_features);
      index_offset += InheritFeaturesIfRequired<PawnElement<PawnElementType::kPassed    >>(index_offset, kProperties[kFeaturesPawnElement_Passed    ], PawnElement<PawnElementType::kPassed    >::MakeIndex(pawn_count, passed    ), training_features);
      index_offset += InheritFeaturesIfRequired<PawnElement<PawnElementType::kDoubled   >>(index_offset, kProperties[kFeaturesPawnElement_Doubled   ], PawnElement<PawnElementType::kDoubled   >::MakeIndex(pawn_count, doubled   ), training_features);
    }

    assert(index_offset == GetDimensions());
  }
};

template <>
constexpr FeatureProperties Factorizer<Pawn>::kProperties[];

}  // namespace Features

}  // namespace NNUE

}  // namespace Eval

#endif  // defined(EVAL_NNUE)

#endif
