//Definition of input features HalfKPE4 of NNUE evaluation function

#if defined(EVAL_NNUE)

#include "half_kpe4.h"
#include "index_list.h"

//#define DEBUG_HALFKPE4

#if defined(DEBUG_HALFKPE4)
namespace UCI {
  std::string square(Square s);
}
#endif

namespace Eval {

namespace NNUE {

namespace Features {

Piece sqBonaPieceToPiece[] = {
    W_PAWN  , B_PAWN
  , W_KNIGHT, B_KNIGHT
  , W_BISHOP, B_BISHOP
  , W_ROOK  , B_ROOK
  , W_QUEEN , B_QUEEN
  , W_KING  , B_KING
};

int mobilityThreshold[] = {
    PAWN    // PAWN
  , PAWN    // KNIGHT
  , PAWN    // BISHOP
  , BISHOP  // ROOK
  , ROOK    // QUEEN
  , KING    // KING
};

inline Bitboard MergeAllMobility(const StateInfo* st, Color perspective) {
  return   st->mobility[perspective][0]
         | st->mobility[perspective][1]
         | st->mobility[perspective][2]
         | st->mobility[perspective][3]
         | st->mobility[perspective][4]
         | st->mobility[perspective][5];
}

// CalcMobility
template <Side AssociatedKing, EffectType EffectTypeUs, EffectType EffectTypeThem>
inline void HalfKPE4<AssociatedKing, EffectTypeUs, EffectTypeThem>::CalcMobility(const StateInfo* st, Bitboard mobility[2][2][6]) {
  for (Color color : { WHITE, BLACK }) {
    for (EffectType et : { EffectType::kAll, EffectType::kFromSmallerPiecesOnly }) {
      if (EffectTypeUs == EffectType::kAll || EffectTypeThem == EffectType::kAll) {
        Bitboard b = MergeAllMobility(st, color);
        for (int i = 0; i < 6; i++) {
          mobility[color][(int)EffectType::kAll][i] = b;
        }
      }

      if (EffectTypeUs == EffectType::kFromSmallerPiecesOnly || EffectTypeThem == EffectType::kFromSmallerPiecesOnly) {
        mobility[color][(int)EffectType::kFromSmallerPiecesOnly][0] = st->mobility[color][0];
        for (int i = 1; i < 6; i++) {
          mobility[color][(int)EffectType::kFromSmallerPiecesOnly][i] = mobility[color][(int)EffectType::kFromSmallerPiecesOnly][i-1] | st->mobility[color][i];
        }
      }
    }
  }
}

inline void GetSquarePieceFromBonaPiece(BonaPiece p, Square &sq, Piece &pc, Color perspective) {
  sq = static_cast<Square>((p - fe_hand_end) % SQUARE_NB);
  pc = sqBonaPieceToPiece[(p - fe_hand_end) / SQUARE_NB];

  if (perspective == BLACK) {
    sq = Inv(sq);
    pc = make_piece(~color_of(pc), type_of(pc));
  }
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
template <Side AssociatedKing, EffectType EffectTypeUs, EffectType EffectTypeThem>
inline IndexType HalfKPE4<AssociatedKing, EffectTypeUs, EffectTypeThem>::MakeIndex(Square sq_k, BonaPiece p, bool hasEffect_us, bool hasEffect_them) {
#if defined(DEBUG_HALFKPE4)
  std::cout << "hasEffect_us=" << hasEffect_us << ", hasEffect_them=" << hasEffect_them << std::endl;
#endif
  return (static_cast<IndexType>(fe_end) * static_cast<IndexType>(sq_k) + p)
       + (static_cast<IndexType>(fe_end) * static_cast<IndexType>(SQUARE_NB) * (hasEffect_us * 2 + hasEffect_them));
}

// Get the piece information
template <Side AssociatedKing, EffectType EffectTypeUs, EffectType EffectTypeThem>
inline void HalfKPE4<AssociatedKing, EffectTypeUs, EffectTypeThem>::GetPieces(
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
template <Side AssociatedKing, EffectType EffectTypeUs, EffectType EffectTypeThem>
void HalfKPE4<AssociatedKing, EffectTypeUs, EffectTypeThem>::AppendActiveIndices(
    const Position& pos, Color perspective, IndexList* active) {
  // do nothing if array size is small to avoid compiler warning
  if (RawFeatures::kMaxActiveDimensions < kMaxActiveDimensions) return;

  BonaPiece* pieces;
  Square sq_target_k;
  GetPieces(pos, perspective, &pieces, &sq_target_k);

  Bitboard mobility[2][2][6];
  CalcMobility(pos.state(), mobility);

#if defined(DEBUG_HALFKPE4)
  std::cout << "pos=" << std::endl;
  std::cout << pos << std::endl;
  std::cout << "perspective=" << perspective << std::endl;

  for (Color c : { WHITE, BLACK }) {
    for (EffectType et : { EffectType::kAll, EffectType::kFromSmallerPiecesOnly }) {
      for (int i = 0; i < 6; i++) {
        std::cout << "c=" << c << ", et=" << (int)et << ", i=" << i << std::endl;
        std::cout << Bitboards::pretty(mobility[c][(int)et][i]) << std::endl;
      }
    }
  }
#endif

  for (PieceNumber i = PIECE_NUMBER_ZERO; i < PIECE_NUMBER_KING; ++i) {
    BonaPiece p = pieces[i];
    if (p != Eval::BONA_PIECE_ZERO) {
      Square sq_p;
      Piece pc_p;
      GetSquarePieceFromBonaPiece(p, sq_p, pc_p, perspective);

      PieceType pt = type_of(pc_p);
      Color color = color_of(pc_p);

#if defined(DEBUG_HALFKPE4)
      std::cout << "EffectTypeUs=" << (int)EffectTypeUs << std::endl;
      std::cout << "EffectTypeThem=" << (int)EffectTypeThem << std::endl;
      std::cout << "perspective=" << perspective << std::endl;
      std::cout << "sq_p=" << UCI::square(sq_p) << std::endl;
      std::cout << "pc_p=" << pc_p << std::endl;
      std::cout << "pt=" << pt << std::endl;
      std::cout << "color=" << color << std::endl;
      std::cout << "mobility[" <<  color << "][" << (int)EffectTypeUs   << "][" << mobilityThreshold[pt - 1] - 1 << "]=" << std::endl << Bitboards::pretty(mobility[ color][(int)EffectTypeUs  ][mobilityThreshold[pt - 1] - 1]) << std::endl;
      std::cout << "mobility[" << ~color << "][" << (int)EffectTypeThem << "][" << mobilityThreshold[pt - 1] - 1 << "]=" << std::endl << Bitboards::pretty(mobility[~color][(int)EffectTypeThem][mobilityThreshold[pt - 1] - 1]) << std::endl;
#endif

      active->push_back(MakeIndex(sq_target_k, p
                                  , mobility[ color][(int)EffectTypeUs  ][mobilityThreshold[pt - 1] - 1] & sq_p
                                  , mobility[~color][(int)EffectTypeThem][mobilityThreshold[pt - 1] - 1] & sq_p));
    }
  }
}

// Get a list of indices whose values ​​have changed from the previous one in the feature quantity
template <Side AssociatedKing, EffectType EffectTypeUs, EffectType EffectTypeThem>
void HalfKPE4<AssociatedKing, EffectTypeUs, EffectTypeThem>::AppendChangedIndices(
    const Position& pos, Color perspective,
    IndexList* removed, IndexList* added) {
  BonaPiece* pieces;
  Square sq_target_k;
  GetPieces(pos, perspective, &pieces, &sq_target_k);
  const auto& dp = pos.state()->dirtyPiece;

  Bitboard mobility_now[2][2][6];
  Bitboard mobility_prev[2][2][6];

  CalcMobility(pos.state(), mobility_now);
  CalcMobility(pos.state()->previous, mobility_prev);

  for (int i = 0; i < dp.dirty_num; ++i) {
    if (dp.pieceNo[i] >= PIECE_NUMBER_KING) continue;

    const auto old_p = static_cast<BonaPiece>(dp.changed_piece[i].old_piece.from[perspective]);
    if (old_p != Eval::BONA_PIECE_ZERO) {
      Square sq_p;
      Piece pc_p;
      GetSquarePieceFromBonaPiece(old_p, sq_p, pc_p, perspective);
      PieceType pt = type_of(pc_p);
      Color color = color_of(pc_p);
      removed->push_back(MakeIndex(sq_target_k, old_p
                                   , mobility_prev[ color][(int)EffectTypeUs  ][mobilityThreshold[pt - 1] - 1] & sq_p
                                   , mobility_prev[~color][(int)EffectTypeThem][mobilityThreshold[pt - 1] - 1] & sq_p));
    }

    const auto new_p = static_cast<BonaPiece>(dp.changed_piece[i].new_piece.from[perspective]);
    if (new_p != Eval::BONA_PIECE_ZERO) {
      Square sq_p;
      Piece pc_p;
      GetSquarePieceFromBonaPiece(new_p, sq_p, pc_p, perspective);
      PieceType pt = type_of(pc_p);
      Color color = color_of(pc_p);
      added->push_back(MakeIndex(sq_target_k, new_p
                                   , mobility_now[ color][(int)EffectTypeUs  ][mobilityThreshold[pt - 1] - 1] & sq_p
                                   , mobility_now[~color][(int)EffectTypeThem][mobilityThreshold[pt - 1] - 1] & sq_p));
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

    Square sq_p;
    Piece pc_p;
    GetSquarePieceFromBonaPiece(p, sq_p, pc_p, perspective);
    PieceType pt = type_of(pc_p);
    Color color = color_of(pc_p);

    bool hasEffect_now_us    = mobility_now [ color][(int)EffectTypeUs  ][mobilityThreshold[pt - 1] - 1] & sq_p;
    bool hasEffect_now_them  = mobility_now [~color][(int)EffectTypeThem][mobilityThreshold[pt - 1] - 1] & sq_p;
    bool hasEffect_prev_us   = mobility_prev[ color][(int)EffectTypeUs  ][mobilityThreshold[pt - 1] - 1] & sq_p;
    bool hasEffect_prev_them = mobility_prev[~color][(int)EffectTypeThem][mobilityThreshold[pt - 1] - 1] & sq_p;

    if (   hasEffect_now_us   != hasEffect_prev_us
        || hasEffect_now_them != hasEffect_prev_them) {
      removed->push_back(MakeIndex(sq_target_k, p, hasEffect_prev_us, hasEffect_prev_them));
      added  ->push_back(MakeIndex(sq_target_k, p, hasEffect_now_us , hasEffect_now_them ));
    }
  }
}

template class HalfKPE4<Side::kFriend, EffectType::kAll                  , EffectType::kAll                  >;
template class HalfKPE4<Side::kFriend, EffectType::kAll                  , EffectType::kFromSmallerPiecesOnly>;
template class HalfKPE4<Side::kFriend, EffectType::kFromSmallerPiecesOnly, EffectType::kAll                  >;
template class HalfKPE4<Side::kFriend, EffectType::kFromSmallerPiecesOnly, EffectType::kFromSmallerPiecesOnly>;
template class HalfKPE4<Side::kEnemy , EffectType::kAll                  , EffectType::kAll                  >;
template class HalfKPE4<Side::kEnemy , EffectType::kAll                  , EffectType::kFromSmallerPiecesOnly>;
template class HalfKPE4<Side::kEnemy , EffectType::kFromSmallerPiecesOnly, EffectType::kAll                  >;
template class HalfKPE4<Side::kEnemy , EffectType::kFromSmallerPiecesOnly, EffectType::kFromSmallerPiecesOnly>;

}  // namespace Features

}  // namespace NNUE

}  // namespace Eval

#endif  // defined(EVAL_NNUE)
