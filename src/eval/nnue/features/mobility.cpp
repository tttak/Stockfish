//Definition of input feature quantity Mobility of NNUE evaluation function

#if defined(EVAL_NNUE)

#include "mobility.h"
#include "index_list.h"
#include "../../../pawns.h"

namespace Eval {

  namespace NNUE {

    namespace Features {

      // Get a list of indices with a value of 1 among the features
      void Mobility::AppendActiveIndices(
        const Position& pos, Color perspective, IndexList* active) {
        // do nothing if array size is small to avoid compiler warning
        if (RawFeatures::kMaxActiveDimensions < kMaxActiveDimensions) return;

        // mobilityCount[PieceType(Knight, Bishop, Rook, Queen)][Piece Count]
        int mobilityCount[4][2];
        CalcMobilityCount(mobilityCount, pos, perspective);

        for (int pt_index = 0; pt_index < 4; pt_index++) {
          for (int piece_count = 0; piece_count < 2; piece_count++) {
            active->push_back(MakeIndex(pt_index, piece_count, mobilityCount[pt_index][piece_count]));
            pos.state()->mobilityCount[perspective][pt_index][piece_count] = mobilityCount[pt_index][piece_count];
          }
        }
      }

      // Get a list of indices whose values have changed from the previous one in the feature quantity
      void Mobility::AppendChangedIndices(
        const Position& pos, Color perspective,
        IndexList* removed, IndexList* added) {

        // mobilityCount[PieceType(Knight, Bishop, Rook, Queen)][Piece Count]
        int mobilityCount[4][2];
        CalcMobilityCount(mobilityCount, pos, perspective);

        const auto prev = pos.state()->previous;

        for (int pt_index = 0; pt_index < 4; pt_index++) {
          for (int piece_count = 0; piece_count < 2; piece_count++) {
            if (prev->mobilityCount[perspective][pt_index][piece_count] != mobilityCount[pt_index][piece_count]) {
              removed->push_back(MakeIndex(pt_index, piece_count, prev->mobilityCount[perspective][pt_index][piece_count]));
              added  ->push_back(MakeIndex(pt_index, piece_count, mobilityCount[pt_index][piece_count]));
            }
            pos.state()->mobilityCount[perspective][pt_index][piece_count] = mobilityCount[pt_index][piece_count];
          }
        }
      }

      // CalcMobilityCount
      void Mobility::CalcMobilityCount(int mobilityCount[4][2], const Position& pos, Color perspective) {
        Direction Up   = pawn_push(perspective);
        Direction Down = -Up;
        Bitboard LowRanks = (perspective == WHITE ? Rank2BB | Rank3BB : Rank7BB | Rank6BB);

        // Find our pawns that are blocked or on the first two ranks
        Bitboard b = pos.pieces(perspective, PAWN) & (shift(pos.pieces(), Down) | LowRanks);

        Bitboard pawnAttacks_them = (perspective == WHITE) ? pawn_attacks_bb<BLACK>(pos.pieces(~perspective, PAWN))
                                                           : pawn_attacks_bb<WHITE>(pos.pieces(~perspective, PAWN));

        // Squares occupied by those pawns, by our king or queen, by blockers to attacks on our king
        // or controlled by enemy pawns are excluded from the mobility area.
        Bitboard mobilityArea = ~(b | pos.pieces(perspective, KING, QUEEN) | pos.blockers_for_king(perspective) | pawnAttacks_them);

        CalcMobilityCount<KNIGHT>(mobilityCount, pos, perspective, mobilityArea);
        CalcMobilityCount<BISHOP>(mobilityCount, pos, perspective, mobilityArea);
        CalcMobilityCount<ROOK>  (mobilityCount, pos, perspective, mobilityArea);
        CalcMobilityCount<QUEEN> (mobilityCount, pos, perspective, mobilityArea);
      }

      // CalcMobilityCount
      template<PieceType Pt>
      void Mobility::CalcMobilityCount(int mobilityCount[4][2], const Position& pos, Color perspective, const Bitboard& mobilityArea) {
        const Square* pl = pos.squares<Pt>(perspective);

        Bitboard b;
        int piece_count = 0;

        for (Square s = *pl; s != SQ_NONE; s = *++pl)
        {
          // Find attacked squares, including x-ray attacks for bishops and rooks
          b = Pt == BISHOP ? attacks_bb<BISHOP>(s, pos.pieces() ^ pos.pieces(QUEEN))
            : Pt ==   ROOK ? attacks_bb<  ROOK>(s, pos.pieces() ^ pos.pieces(QUEEN) ^ pos.pieces(perspective, ROOK))
                           : attacks_bb<Pt>(s, pos.pieces());

          int mob = popcount(b & mobilityArea);

          mobilityCount[Pt - 2][piece_count] = mob + 1;
          piece_count++;

          if (piece_count >= kMaxPieceCount) {
            break;
          }
        }

        while (piece_count < kMaxPieceCount) {
          mobilityCount[Pt - 2][piece_count] = 0;
          piece_count++;
        }
      }

      // MakeIndex
      IndexType Mobility::MakeIndex(int pt_index, int piece_count, int mobility_count) {
        return (pt_index * kMaxPieceCount + piece_count) * kMaxMobilityCount + mobility_count;
      }

      // shift() moves a bitboard one step along direction D
      Bitboard Mobility::shift(Bitboard b, Direction D) {
        return  D == NORTH      ?  b             << 8 : D == SOUTH      ?  b             >> 8
              : D == NORTH+NORTH?  b             <<16 : D == SOUTH+SOUTH?  b             >>16
              : D == EAST       ? (b & ~FileHBB) << 1 : D == WEST       ? (b & ~FileABB) >> 1
              : D == NORTH_EAST ? (b & ~FileHBB) << 9 : D == NORTH_WEST ? (b & ~FileABB) << 7
              : D == SOUTH_EAST ? (b & ~FileHBB) >> 7 : D == SOUTH_WEST ? (b & ~FileABB) >> 9
              : 0;
      }

    }  // namespace Features

  }  // namespace NNUE

}  // namespace Eval

#endif  // defined(EVAL_NNUE)
