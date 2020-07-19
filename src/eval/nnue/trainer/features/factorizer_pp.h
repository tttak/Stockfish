// Specialization of NNUE evaluation function feature conversion class template for PP

#ifndef _NNUE_TRAINER_FEATURES_FACTORIZER_PP_H_
#define _NNUE_TRAINER_FEATURES_FACTORIZER_PP_H_

#if defined(EVAL_NNUE)

#include "../../features/pp.h"
#include "../../features/p.h"
#include "factorizer.h"

namespace Eval {

namespace NNUE {

extern int fibonacci[fe_end + 1];

namespace Features {

// Class template that converts input features into learning features
// Specialization for PP
template <>
class Factorizer<PP> {
 private:
  using FeatureType = PP;

  // The maximum value of the number of indexes whose value is 1 at the same time among the feature values
  static constexpr IndexType kMaxActiveDimensions =
      FeatureType::kMaxActiveDimensions;

  // Type of learning feature
  enum TrainingFeatureType {
    kFeaturesPP,
    kFeaturesP1,
    kFeaturesP2,
    kNumTrainingFeatureTypes,
  };

  // Learning feature information
  static constexpr FeatureProperties kProperties[] = {
    // kFeaturesPP
    {true, FeatureType::kDimensions},
    // kFeaturesP1
    {true, Factorizer<P>::GetDimensions()},
    // kFeaturesP2
    {true, Factorizer<P>::GetDimensions()},
  };
  static_assert(GetArrayLength(kProperties) == kNumTrainingFeatureTypes, "");

  static void GetP1P2Index(IndexType base_index, IndexType& p1, IndexType& p2) {
    for (int i = sqrt(base_index); true; i++) {
      if ((int)base_index < fibonacci[i]) {
        p1 = i;
        p2 = i - (fibonacci[i] - base_index);
        return;
      }
    }
  }

 public:
  // Get the dimensionality of the learning feature
  static constexpr IndexType GetDimensions() {
    return GetActiveDimensions(kProperties);
  }

  // Get index of learning feature and scale of learning rate
  static void AppendTrainingFeatures(
      IndexType base_index, std::vector<TrainingFeature>* training_features) {
    // kFeaturesPP
    IndexType index_offset = AppendBaseFeature<FeatureType>(
        kProperties[kFeaturesPP], base_index, training_features);

    IndexType p1, p2;
    GetP1P2Index(base_index, p1, p2);

    // kFeaturesP1
    index_offset += InheritFeaturesIfRequired<P>(index_offset, kProperties[kFeaturesP1], p1, training_features);
    // kFeaturesP2
    index_offset += InheritFeaturesIfRequired<P>(index_offset, kProperties[kFeaturesP2], p2, training_features);

    assert(index_offset == GetDimensions());
  }
};

template <>
constexpr FeatureProperties Factorizer<PP>::kProperties[];

}  // namespace Features

}  // namespace NNUE

}  // namespace Eval

#endif  // defined(EVAL_NNUE)

#endif
