// Definition of input features and network structure used in NNUE evaluation function

#ifndef HALFKP_MOBILITY_PAWN_256X2_32_32_H
#define HALFKP_MOBILITY_PAWN_256X2_32_32_H

// Mobility
#define USE_MOBILITY_COUNT_IN_STATEINFO
// Pawn
#define USE_PAWN_INDEX_IN_STATEINFO

#include "../features/feature_set.h"
#include "../features/half_kp.h"
#include "../features/mobility.h"
#include "../features/pawn.h"

#include "../layers/input_slice.h"
#include "../layers/affine_transform.h"
#include "../layers/clipped_relu.h"

namespace Eval {

namespace NNUE {

// Input features used in evaluation function
using RawFeatures = Features::FeatureSet<
    Features::HalfKP<Features::Side::kFriend>, Features::Mobility, Features::Pawn>;

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

#endif // #ifndef HALFKP_MOBILITY_PAWN_256X2_32_32_H
