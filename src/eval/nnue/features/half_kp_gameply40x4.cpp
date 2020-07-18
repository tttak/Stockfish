//Definition of input features HalfKP_GamePly40x4 of NNUE evaluation function

#if defined(EVAL_NNUE)

#include "half_kp_gameply40x4.h"
#include "index_list.h"

namespace Eval {

namespace NNUE {

namespace Features {

// Find the index of the feature quantity from the ball position and BonaPiece
template <Side AssociatedKing>
inline IndexType HalfKP_GamePly40x4<AssociatedKing>::MakeIndex(Square sq_k, BonaPiece p, int ply) {
  IndexType ply_index = static_cast<IndexType>(std::min(std::max((ply - 1) / 40, 0), 3));
  return static_cast<IndexType>(fe_end) * static_cast<IndexType>(sq_k) + p
         + ply_index * static_cast<IndexType>(SQUARE_NB) * static_cast<IndexType>(fe_end);
}

// Get the piece information
template <Side AssociatedKing>
inline void HalfKP_GamePly40x4<AssociatedKing>::GetPieces(
    const Position& pos, Color perspective,
    BonaPiece** pieces, Square* sq_target_k) {
  *pieces = (perspective == BLACK) ?
      pos.eval_list()->piece_list_fb() :
      pos.eval_list()->piece_list_fw();
  const PieceNumber target = (AssociatedKing == Side::kFriend) ?
      static_cast<PieceNumber>(PIECE_NUMBER_KING + perspective) :
      static_cast<PieceNumber>(PIECE_NUMBER_KING + ~perspective);
  *sq_target_k = static_cast<Square>(((*pieces)[target] - f_king) % SQUARE_NB);
}

// Get a list of indices with a value of 1 among the features
template <Side AssociatedKing>
void HalfKP_GamePly40x4<AssociatedKing>::AppendActiveIndices(
    const Position& pos, Color perspective, IndexList* active) {
  // do nothing if array size is small to avoid compiler warning
  if (RawFeatures::kMaxActiveDimensions < kMaxActiveDimensions) return;

  BonaPiece* pieces;
  Square sq_target_k;
  GetPieces(pos, perspective, &pieces, &sq_target_k);
  for (PieceNumber i = PIECE_NUMBER_ZERO; i < PIECE_NUMBER_KING; ++i) {
    if (pieces[i] != Eval::BONA_PIECE_ZERO) {
      active->push_back(MakeIndex(sq_target_k, pieces[i], pos.game_ply()));
    }
  }
}

// Get a list of indices whose values ​​have changed from the previous one in the feature quantity
template <Side AssociatedKing>
void HalfKP_GamePly40x4<AssociatedKing>::AppendChangedIndices(
    const Position& pos, Color perspective,
    IndexList* removed, IndexList* added) {
  BonaPiece* pieces;
  Square sq_target_k;
  GetPieces(pos, perspective, &pieces, &sq_target_k);
  const auto& dp = pos.state()->dirtyPiece;
  for (int i = 0; i < dp.dirty_num; ++i) {
    if (dp.pieceNo[i] >= PIECE_NUMBER_KING) continue;
    const auto old_p = static_cast<BonaPiece>(
        dp.changed_piece[i].old_piece.from[perspective]);
    if (old_p != Eval::BONA_PIECE_ZERO) {
      removed->push_back(MakeIndex(sq_target_k, old_p, pos.game_ply() - 1));
    }
    const auto new_p = static_cast<BonaPiece>(
        dp.changed_piece[i].new_piece.from[perspective]);
    if (new_p != Eval::BONA_PIECE_ZERO) {
      added->push_back(MakeIndex(sq_target_k, new_p, pos.game_ply()));
    }
  }
}

template class HalfKP_GamePly40x4<Side::kFriend>;
template class HalfKP_GamePly40x4<Side::kEnemy>;

}  // namespace Features

}  // namespace NNUE

}  // namespace Eval

#endif  // defined(EVAL_NNUE)
