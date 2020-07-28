// Specialization of NNUE evaluation function feature conversion class template for HalfKP_PieceCount

#ifndef _NNUE_TRAINER_FEATURES_FACTORIZER_HALF_KP_PIECECOUNT_H_
#define _NNUE_TRAINER_FEATURES_FACTORIZER_HALF_KP_PIECECOUNT_H_

#if defined(EVAL_NNUE)

#include "../../features/half_kp_piececount.h"
#include "../../features/half_kp.h"
#include "factorizer.h"

namespace Eval {

namespace NNUE {

namespace Features {

// Class template that converts input features into learning features
// Specialization for HalfKP_PieceCount
template <Side AssociatedKing>
class Factorizer<HalfKP_PieceCount<AssociatedKing>> {
 private:
  using FeatureType = HalfKP_PieceCount<AssociatedKing>;

  // The maximum value of the number of indexes whose value is 1 at the same time among the feature values
  static constexpr IndexType kMaxActiveDimensions =
      FeatureType::kMaxActiveDimensions;

  // Type of learning feature
  enum TrainingFeatureType {
    kFeaturesHalfKP_PieceCount,
    kFeaturesHalfKP,
    kNumTrainingFeatureTypes,
  };

  // Learning feature information
  static constexpr FeatureProperties kProperties[] = {
    // kFeaturesHalfKP_PieceCount
    {true, FeatureType::kDimensions},
    // kFeaturesHalfKP
    {true, Factorizer<HalfKP<AssociatedKing>>::GetDimensions()},
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
    // kFeaturesHalfKP_PieceCount
    IndexType index_offset = AppendBaseFeature<FeatureType>(
        kProperties[kFeaturesHalfKP_PieceCount], base_index, training_features);

    IndexType index_HalfKP = base_index % (static_cast<IndexType>(fe_end) * static_cast<IndexType>(SQUARE_NB));

    // kFeaturesHalfKP
    index_offset += InheritFeaturesIfRequired<HalfKP<AssociatedKing>>(index_offset, kProperties[kFeaturesHalfKP], index_HalfKP, training_features);

    assert(index_offset == GetDimensions());
  }
};

template <Side AssociatedKing>
constexpr FeatureProperties Factorizer<HalfKP_PieceCount<AssociatedKing>>::kProperties[];

}  // namespace Features

}  // namespace NNUE

}  // namespace Eval

#endif  // defined(EVAL_NNUE)

#endif
