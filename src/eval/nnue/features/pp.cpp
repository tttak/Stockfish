//Definition of input feature quantity PP of NNUE evaluation function

#if defined(EVAL_NNUE)

#include "pp.h"
#include "index_list.h"

namespace Eval {

  namespace NNUE {

    namespace Features {

      // Get a list of indices with a value of 1 among the features
      void PP::AppendActiveIndices(
        const Position& pos, Color perspective, IndexList* active) {
        // do nothing if array size is small to avoid compiler warning
        if (RawFeatures::kMaxActiveDimensions < kMaxActiveDimensions) return;

        const BonaPiece* pieces = (perspective == BLACK) ?
            pos.eval_list()->piece_list_fb() :
            pos.eval_list()->piece_list_fw();

        for (PieceNumber pn1 = PIECE_NUMBER_ZERO; pn1 < PIECE_NUMBER_KING; ++pn1) {
          if (pieces[pn1] != Eval::BONA_PIECE_ZERO) {
            for (PieceNumber pn2 = PIECE_NUMBER_ZERO; pn2 < pn1; ++pn2) {
              if (pieces[pn2] != Eval::BONA_PIECE_ZERO) {
                active->push_back(MakeIndex(pieces[pn1], pieces[pn2]));
              }
            }
          }
        }
      }

      // Get a list of indices whose values ??have changed from the previous one in the feature quantity
      void PP::AppendChangedIndices(
        const Position& pos, Color perspective,
        IndexList* removed, IndexList* added) {

        const auto& dp = pos.state()->dirtyPiece;

        const BonaPiece* pieces = (perspective == BLACK) ?
            pos.eval_list()->piece_list_fb() :
            pos.eval_list()->piece_list_fw();

        bool dirty_p[2] = { false, false };

        for (int i = 0; i < dp.dirty_num; ++i) {
          if (dp.pieceNo[i] < PIECE_NUMBER_KING) {
            dirty_p[i] = true;
          }
        }

        if (dirty_p[0]) {
          if (dirty_p[1]) {
            PieceNumber dirty_pn_p0 = dp.pieceNo[0];
            BonaPiece old_p0 = dp.changed_piece[0].old_piece.from[perspective];
            BonaPiece new_p0 = dp.changed_piece[0].new_piece.from[perspective];

            PieceNumber dirty_pn_p1 = dp.pieceNo[1];
            BonaPiece old_p1 = dp.changed_piece[1].old_piece.from[perspective];
            BonaPiece new_p1 = dp.changed_piece[1].new_piece.from[perspective];

            for (PieceNumber pn_p = PIECE_NUMBER_ZERO; pn_p < PIECE_NUMBER_KING; ++pn_p) {
              if (pieces[pn_p] != Eval::BONA_PIECE_ZERO) {
                if (pn_p != dirty_pn_p0) {
                  if (pn_p != dirty_pn_p1) {
                    if (old_p0 != Eval::BONA_PIECE_ZERO) {
                      removed->push_back(MakeIndex(pieces[pn_p], old_p0));
                    }
                    if (new_p0 != Eval::BONA_PIECE_ZERO) {
                      added->push_back(MakeIndex(pieces[pn_p], new_p0));
                    }
                    if (old_p1 != Eval::BONA_PIECE_ZERO) {
                      removed->push_back(MakeIndex(pieces[pn_p], old_p1));
                    }
                    if (new_p1 != Eval::BONA_PIECE_ZERO) {
                      added->push_back(MakeIndex(pieces[pn_p], new_p1));
                    }
                  }
                }
              }
            }

            if (old_p0 != Eval::BONA_PIECE_ZERO && old_p1 != Eval::BONA_PIECE_ZERO) {
              removed->push_back(MakeIndex(old_p0, old_p1));
            }
            if (new_p0 != Eval::BONA_PIECE_ZERO && new_p1 != Eval::BONA_PIECE_ZERO) {
              added->push_back(MakeIndex(new_p0, new_p1));
            }
          }
          else {
            PieceNumber dirty_pn_p = dp.pieceNo[0];
            BonaPiece old_p = dp.changed_piece[0].old_piece.from[perspective];
            BonaPiece new_p = dp.changed_piece[0].new_piece.from[perspective];

            for (PieceNumber pn_p = PIECE_NUMBER_ZERO; pn_p < PIECE_NUMBER_KING; ++pn_p) {
              if (pieces[pn_p] != Eval::BONA_PIECE_ZERO) {
                if (pn_p != dirty_pn_p) {
                  if (old_p != Eval::BONA_PIECE_ZERO) {
                    removed->push_back(MakeIndex(pieces[pn_p], old_p));
                  }
                  if (new_p != Eval::BONA_PIECE_ZERO) {
                    added->push_back(MakeIndex(pieces[pn_p], new_p));
                  }
                }
              }
            }
          }
        }
        else {
          if (dirty_p[1]) {
            PieceNumber dirty_pn_p = dp.pieceNo[1];
            BonaPiece old_p = dp.changed_piece[1].old_piece.from[perspective];
            BonaPiece new_p = dp.changed_piece[1].new_piece.from[perspective];

            for (PieceNumber pn_p = PIECE_NUMBER_ZERO; pn_p < PIECE_NUMBER_KING; ++pn_p) {
              if (pieces[pn_p] != Eval::BONA_PIECE_ZERO) {
                if (pn_p != dirty_pn_p) {
                  if (old_p != Eval::BONA_PIECE_ZERO) {
                    removed->push_back(MakeIndex(pieces[pn_p], old_p));
                  }
                  if (new_p != Eval::BONA_PIECE_ZERO) {
                    added->push_back(MakeIndex(pieces[pn_p], new_p));
                  }
                }
              }
            }
          }
        }
      }

      // MakeIndex
      inline IndexType PP::MakeIndex(BonaPiece bp1, BonaPiece bp2) {
        IndexType bp_max = std::max(bp1, bp2);
        IndexType bp_min = std::min(bp1, bp2);

        return bp_max * (bp_max - 1) / 2 + bp_min;
      }

    }  // namespace Features

  }  // namespace NNUE

}  // namespace Eval

#endif  // defined(EVAL_NNUE)
