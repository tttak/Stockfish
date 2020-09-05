// Definition of input features and network structure used in NNUE evaluation function

#ifndef HALFKPE4_256X2_32_32_H
#define HALFKPE4_256X2_32_32_H

// HalfKPE4
#define USE_MOBILITY_IN_STATEINFO

#include "../features/feature_set.h"
#include "../features/half_kpe4.h"

#include "../layers/input_slice.h"
#include "../layers/affine_transform.h"
#include "../layers/clipped_relu.h"

namespace Eval {

namespace NNUE {

// Input features used in evaluation function
using RawFeatures = Features::FeatureSet<
    Features::HalfKPE4<Features::Side::kFriend, Features::EffectType::kFromSmallerPiecesOnly, Features::EffectType::kFromSmallerPiecesOnly>>;

// Number of input feature dimensions after conversion
constexpr IndexType kTransformedFeatureDimensions = 256;

namespace Layers {

// define network structure
using InputLayer = InputSlice<kTransformedFeatureDimensions * 2>;
using HiddenLayer1 = ClippedReLU<AffineTransform<InputLayer, 32>>;
using HiddenLayer2 = ClippedReLU<AffineTransform<HiddenLayer1, 32>>;
using OutputLayer = AffineTransform<HiddenLayer2, 1>;

}  // namespace Layers

using Network = Layers::OutputLayer;

}  // namespace NNUE

}  // namespace Eval

#endif // #ifndef HALFKPE4_256X2_32_32_H
