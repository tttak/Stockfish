﻿// Specialization of NNUE evaluation function feature conversion class template for HalfKPE4

#ifndef _NNUE_TRAINER_FEATURES_FACTORIZER_HALF_KPE4_H_
#define _NNUE_TRAINER_FEATURES_FACTORIZER_HALF_KPE4_H_

#if defined(EVAL_NNUE)

#include "../../features/half_kpe4.h"
#include "../../features/half_kp.h"
#include "factorizer.h"

namespace Eval {

namespace NNUE {

namespace Features {

// Class template that converts input features into learning features
// Specialization for HalfKPE4
template <Side AssociatedKing, EffectType EffectTypeUs, EffectType EffectTypeThem>
class Factorizer<HalfKPE4<AssociatedKing, EffectTypeUs, EffectTypeThem>> {
 private:
  using FeatureType = HalfKPE4<AssociatedKing, EffectTypeUs, EffectTypeThem>;

  // The maximum value of the number of indexes whose value is 1 at the same time among the feature values
  static constexpr IndexType kMaxActiveDimensions =
      FeatureType::kMaxActiveDimensions;

  // Type of learning feature
  enum TrainingFeatureType {
    kFeaturesHalfKPE4,
    kFeaturesHalfKP,
    kNumTrainingFeatureTypes,
  };

  // Learning feature information
  static constexpr FeatureProperties kProperties[] = {
    // kFeaturesHalfKPE4
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
    // kFeaturesHalfKPE4
    IndexType index_offset = AppendBaseFeature<FeatureType>(
        kProperties[kFeaturesHalfKPE4], base_index, training_features);

    IndexType index_HalfKP = base_index % (static_cast<IndexType>(PS_END) * static_cast<IndexType>(SQUARE_NB));

    // kFeaturesHalfKP
    index_offset += InheritFeaturesIfRequired<HalfKP<AssociatedKing>>(index_offset, kProperties[kFeaturesHalfKP], index_HalfKP, training_features);

    assert(index_offset == GetDimensions());
  }
};

template <Side AssociatedKing, EffectType EffectTypeUs, EffectType EffectTypeThem>
constexpr FeatureProperties Factorizer<HalfKPE4<AssociatedKing, EffectTypeUs, EffectTypeThem>>::kProperties[];

}  // namespace Features

}  // namespace NNUE

}  // namespace Eval

#endif  // defined(EVAL_NNUE)

#endif
