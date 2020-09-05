/*
  Stockfish, a UCI chess playing engine derived from Glaurung 2.1
  Copyright (C) 2004-2020 The Stockfish developers (see AUTHORS file)

  Stockfish is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Stockfish is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// Input features and network structure used in NNUE evaluation function

#ifndef NNUE_ARCHITECTURE_H_INCLUDED
#define NNUE_ARCHITECTURE_H_INCLUDED

// Defines the network structure
//#include "architectures/halfkp_256x2-32-32.h"
//#include "architectures/k-p_256x2-32-32.h"
//#include "architectures/k-p-cr_256x2-32-32.h"
//#include "architectures/k-p-cr-ep_256x2-32-32.h"
//#include "architectures/halfkp-cr-ep_256x2-32-32.h"
//#include "architectures/halfkp_384x2-32-32.h"
//#include "architectures/halfkp-mobility_256x2-32-32.h"
//#include "architectures/halfkp-pawn_256x2-32-32.h"
//#include "architectures/halfkp-pawnelement_test_256x2-32-32.h"
//#include "architectures/halfkp-mobility-pawn_256x2-32-32.h"
//#include "architectures/halfkp-kk_256x2-32-32.h"
//#include "architectures/halfkp_gameply40x4_256x2-32-32.h"
//#include "architectures/halfkpkfile_256x2-32-32.h"
//#include "architectures/halfkpkrank_256x2-32-32.h"
//#include "architectures/halfkp-pp_256x2-32-32.h"
//#include "architectures/halfkpe4aa_256x2-32-32.h"
//#include "architectures/halfkpe4as_256x2-32-32.h"
//#include "architectures/halfkpe4sa_256x2-32-32.h"
//#include "architectures/halfkpe4ss_256x2-32-32.h"
#include "architectures/halfkp_piececount_256x2-32-32.h"

namespace Eval::NNUE {

  static_assert(kTransformedFeatureDimensions % kMaxSimdWidth == 0, "");
  static_assert(Network::kOutputDimensions == 1, "");
  static_assert(std::is_same<Network::OutputType, std::int32_t>::value, "");

  // Trigger for full calculation instead of difference calculation
  constexpr auto kRefreshTriggers = RawFeatures::kRefreshTriggers;

}  // namespace Eval::NNUE

#endif // #ifndef NNUE_ARCHITECTURE_H_INCLUDED
