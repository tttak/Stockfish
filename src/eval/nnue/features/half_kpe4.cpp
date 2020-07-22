//Definition of input features HalfKPE4 of NNUE evaluation function

#if defined(EVAL_NNUE)

#include "half_kpe4.h"
#include "index_list.h"

namespace Eval {

namespace NNUE {

namespace Features {

inline Bitboard MergeAllMobility(const StateInfo* st, Color perspective) {
  return   st->mobility[perspective][0]
         | st->mobility[perspective][1]
         | st->mobility[perspective][2]
         | st->mobility[perspective][3]
         | st->mobility[perspective][4]
         | st->mobility[perspective][5];
}

inline Square GetSquareFromBonaPiece(BonaPiece p, Color perspective) {
  Square sq = static_cast<Square>((p - fe_hand_end) % SQUARE_NB);
  if (perspective == BLACK) {
    sq = Inv(sq);
  }
  return sq;
}

inline bool IsDirty(const Eval::DirtyPiece& dp, PieceNumber pn) {
  for (int i = 0; i < dp.dirty_num; ++i) {
    if (pn == dp.pieceNo[i]) {
      return true;
    }
  }
  return false;
}

// Find the index of the feature quantity from the ball position and BonaPiece
template <Side AssociatedKing>
inline IndexType HalfKPE4<AssociatedKing>::MakeIndex(Square sq_k, BonaPiece p, bool hasEffect_us, bool hasEffect_them) {
  //std::cout << "hasEffect_us=" << hasEffect_us << ", hasEffect_them=" << hasEffect_them << std::endl;
  return (static_cast<IndexType>(fe_end) * static_cast<IndexType>(sq_k) + p)
       + (static_cast<IndexType>(fe_end) * static_cast<IndexType>(SQUARE_NB) * (hasEffect_us * 2 + hasEffect_them));
}

// Get the piece information
template <Side AssociatedKing>
inline void HalfKPE4<AssociatedKing>::GetPieces(
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
void HalfKPE4<AssociatedKing>::AppendActiveIndices(
    const Position& pos, Color perspective, IndexList* active) {
  // do nothing if array size is small to avoid compiler warning
  if (RawFeatures::kMaxActiveDimensions < kMaxActiveDimensions) return;

  BonaPiece* pieces;
  Square sq_target_k;
  GetPieces(pos, perspective, &pieces, &sq_target_k);

  Bitboard mobility_us   = MergeAllMobility(pos.state(), perspective);
  Bitboard mobility_them = MergeAllMobility(pos.state(), ~perspective);

  for (PieceNumber i = PIECE_NUMBER_ZERO; i < PIECE_NUMBER_KING; ++i) {
    BonaPiece p = pieces[i];
    if (p != Eval::BONA_PIECE_ZERO) {
      Square sq_p = GetSquareFromBonaPiece(p, perspective);
      active->push_back(MakeIndex(sq_target_k, p
                                  , mobility_us   & sq_p
                                  , mobility_them & sq_p));
    }
  }
}

// Get a list of indices whose values ​​have changed from the previous one in the feature quantity
template <Side AssociatedKing>
void HalfKPE4<AssociatedKing>::AppendChangedIndices(
    const Position& pos, Color perspective,
    IndexList* removed, IndexList* added) {
  BonaPiece* pieces;
  Square sq_target_k;
  GetPieces(pos, perspective, &pieces, &sq_target_k);
  const auto& dp = pos.state()->dirtyPiece;

  const StateInfo* st_now  = pos.state();
  const StateInfo* st_prev = st_now->previous;

  Bitboard mobility_now_us    = MergeAllMobility(st_now , perspective);
  Bitboard mobility_now_them  = MergeAllMobility(st_now , ~perspective);
  Bitboard mobility_prev_us   = MergeAllMobility(st_prev, perspective);
  Bitboard mobility_prev_them = MergeAllMobility(st_prev, ~perspective);

  for (int i = 0; i < dp.dirty_num; ++i) {
    if (dp.pieceNo[i] >= PIECE_NUMBER_KING) continue;

    const auto old_p = static_cast<BonaPiece>(dp.changed_piece[i].old_piece.from[perspective]);
    if (old_p != Eval::BONA_PIECE_ZERO) {
      Square old_sq_p = GetSquareFromBonaPiece(old_p, perspective);
      removed->push_back(MakeIndex(sq_target_k, old_p
                                   , mobility_prev_us   & old_sq_p
                                   , mobility_prev_them & old_sq_p));
    }

    const auto new_p = static_cast<BonaPiece>(dp.changed_piece[i].new_piece.from[perspective]);
    if (new_p != Eval::BONA_PIECE_ZERO) {
      Square new_sq_p = GetSquareFromBonaPiece(new_p, perspective);
      added->push_back(MakeIndex(sq_target_k, new_p
                                   , mobility_now_us   & new_sq_p
                                   , mobility_now_them & new_sq_p));
    }
  }

  for (PieceNumber i = PIECE_NUMBER_ZERO; i < PIECE_NUMBER_KING; ++i) {
    if (IsDirty(dp, i)) {
      continue;
    }

    BonaPiece p = pieces[i];
    if (p == Eval::BONA_PIECE_ZERO) {
      continue;
    }

    Square sq_p = GetSquareFromBonaPiece(p, perspective);

    bool hasEffect_now_us    = mobility_now_us    & sq_p;
    bool hasEffect_now_them  = mobility_now_them  & sq_p;
    bool hasEffect_prev_us   = mobility_prev_us   & sq_p;
    bool hasEffect_prev_them = mobility_prev_them & sq_p;

    if (   hasEffect_now_us   != hasEffect_prev_us
        || hasEffect_now_them != hasEffect_prev_them) {
      removed->push_back(MakeIndex(sq_target_k, p, hasEffect_prev_us, hasEffect_prev_them));
      added  ->push_back(MakeIndex(sq_target_k, p, hasEffect_now_us , hasEffect_now_them ));
    }
  }
}

template class HalfKPE4<Side::kFriend>;
template class HalfKPE4<Side::kEnemy>;

}  // namespace Features

}  // namespace NNUE

}  // namespace Eval

#endif  // defined(EVAL_NNUE)
