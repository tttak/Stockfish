// Specialization of NNUE evaluation function feature conversion class template for HalfKPKrank

#ifndef _NNUE_TRAINER_FEATURES_FACTORIZER_HALF_KPKRANK_H_
#define _NNUE_TRAINER_FEATURES_FACTORIZER_HALF_KPKRANK_H_

#if defined(EVAL_NNUE)

#include "../../features/half_kpkrank.h"
#include "../../features/half_kp.h"
#include "factorizer.h"

namespace Eval {

namespace NNUE {

namespace Features {

// Class template that converts input features into learning features
// Specialization for HalfKPKrank
template <Side AssociatedKing>
class Factorizer<HalfKPKrank<AssociatedKing>> {
 private:
  using FeatureType = HalfKPKrank<AssociatedKing>;

  // The maximum value of the number of indexes whose value is 1 at the same time among the feature values
  static constexpr IndexType kMaxActiveDimensions =
      FeatureType::kMaxActiveDimensions;

  // Type of learning feature
  enum TrainingFeatureType {
    kFeaturesHalfKPKrank,
    kFeaturesHalfKP,
    kNumTrainingFeatureTypes,
  };

  // Learning feature information
  static constexpr FeatureProperties kProperties[] = {
    // kFeaturesHalfKPKrank
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
    // kFeaturesHalfKPKrank
    IndexType index_offset = AppendBaseFeature<FeatureType>(
        kProperties[kFeaturesHalfKPKrank], base_index, training_features);

    IndexType index_HalfKP = base_index % (static_cast<IndexType>(fe_end) * static_cast<IndexType>(SQUARE_NB));

    // kFeaturesHalfKP
    index_offset += InheritFeaturesIfRequired<HalfKP<AssociatedKing>>(index_offset, kProperties[kFeaturesHalfKP], index_HalfKP, training_features);

    assert(index_offset == GetDimensions());
  }
};

template <Side AssociatedKing>
constexpr FeatureProperties Factorizer<HalfKPKrank<AssociatedKing>>::kProperties[];

}  // namespace Features

}  // namespace NNUE

}  // namespace Eval

#endif  // defined(EVAL_NNUE)

#endif
