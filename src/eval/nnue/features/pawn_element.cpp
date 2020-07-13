//Definition of input feature quantity PawnElement of NNUE evaluation function

#if defined(EVAL_NNUE)

#include "pawn_element.h"
#include "index_list.h"
#include "../../../pawns.h"

namespace Eval {

  namespace NNUE {

    namespace Features {

      // Get a list of indices with a value of 1 among the features
      template <PawnElementType PEType>
      void PawnElement<PEType>::AppendActiveIndices(
        const Position& pos, Color perspective, IndexList* active) {
        // do nothing if array size is small to avoid compiler warning
        if (RawFeatures::kMaxActiveDimensions < kMaxActiveDimensions) return;

        Color Us     = perspective;
        Color Them   = ~Us;
        Direction Up = pawn_push(Us);

        Bitboard neighbours, stoppers, support, phalanx, opposed;
        Bitboard lever, leverPush, blocked;
        Square s;
        bool backward, passed, doubled;
        const Square* pl = pos.squares<PAWN>(perspective);

        Bitboard ourPawns   = pos.pieces(  Us, PAWN);
        Bitboard theirPawns = pos.pieces(Them, PAWN);

        Bitboard doubleAttackThem = pawn_double_attacks_bb(theirPawns, Them);

        int pawn_count = 0;

        // Loop through all pawns of the current color and score each pawn
        while ((s = *pl++) != SQ_NONE)
        {
            assert(pos.piece_on(s) == make_piece(Us, PAWN));

            Rank r = relative_rank(Us, s);

            // Flag the pawn
            opposed    = theirPawns & forward_file_bb(Us, s);
            blocked    = theirPawns & (s + Up);
            stoppers   = theirPawns & passed_pawn_span(Us, s);
            lever      = theirPawns & pawn_attacks_bb(Us, s);
            leverPush  = theirPawns & pawn_attacks_bb(Us, s + Up);
            doubled    = ourPawns   & (s - Up);
            neighbours = ourPawns   & adjacent_files_bb(s);
            phalanx    = neighbours & rank_bb(s);
            support    = neighbours & rank_bb(s - Up);

            // A pawn is backward when it is behind all pawns of the same color on
            // the adjacent files and cannot safely advance.
            backward =  !(neighbours & forward_ranks_bb(Them, s + Up))
                      && (leverPush | blocked);

            // A pawn is passed if one of the three following conditions is true:
            // (a) there is no stoppers except some levers
            // (b) the only stoppers are the leverPush, but we outnumber them
            // (c) there is only one front stopper which can be levered.
            //     (Refined in Evaluation::passed)
            passed =   !(stoppers ^ lever)
                    || (   !(stoppers ^ leverPush)
                        && popcount(phalanx) >= popcount(leverPush))
                    || (   stoppers == blocked && r >= RANK_5
                        && (shift_(support, Up) & ~(theirPawns | doubleAttackThem)));

            passed &= !(forward_file_bb(Us, s) & ourPawns);

            IndexType index = MakeIndex(pawn_count
                                        , neighbours, stoppers, support, phalanx, opposed
                                        , lever, leverPush, blocked
                                        , backward, passed, doubled
                                        );
#if 0
            std::cout << "perspective="  << perspective
                      << ", pawn_count=" << pawn_count
                      << ", neighbours=" << popcount(neighbours)
                      << ", stoppers="   << popcount(stoppers)
                      << ", support="    << popcount(support)
                      << ", phalanx="    << popcount(phalanx)
                      << ", opposed="    << popcount(opposed)
                      << ", lever="      << popcount(lever)
                      << ", leverPush="  << popcount(leverPush)
                      << ", blocked="    << popcount(blocked)
                      << ", backward="   << backward
                      << ", passed="     << passed
                      << ", doubled="    << doubled
                      << ", index="      << index
                      << std::endl;
#endif
            active->push_back(index);
            pawn_count++;
        }

        while (pawn_count < 8) {
          IndexType index = MakeIndexOfNoPiece(pawn_count);
#if 0
          std::cout << "perspective="  << perspective
                    << ", pawn_count=" << pawn_count
                    << ", index="      << index
                    << std::endl;
#endif
          active->push_back(index);
          pawn_count++;
        }
      }

      // Get a list of indices whose values ??have changed from the previous one in the feature quantity
      template <PawnElementType PEType>
      void PawnElement<PEType>::AppendChangedIndices(
        const Position& pos, Color perspective,
        IndexList* removed, IndexList* added) {
        // TODO : difference calculation
        // Not implemented.
        assert(false);
      }

      // MakeIndex
      template <PawnElementType PEType>
      inline IndexType PawnElement<PEType>::MakeIndex(int pawn_count
                                       , Bitboard neighbours, Bitboard stoppers, Bitboard support ,Bitboard phalanx, Bitboard opposed
                                       , Bitboard lever, Bitboard leverPush, Bitboard blocked
                                       , bool backward, bool passed, bool doubled
                                      ) {
        static Bitboard ZERO_BB = Bitboard(0);

        return MakeIndex(pawn_count,
                   (PEType == PawnElementType::kNeighbours) ? (ZERO_BB != neighbours) :
                   (PEType == PawnElementType::kStoppers  ) ? (ZERO_BB != stoppers  ) :
                   (PEType == PawnElementType::kSupport   ) ? (ZERO_BB != support   ) :
                   (PEType == PawnElementType::kPhalanx   ) ? (ZERO_BB != phalanx   ) :
                   (PEType == PawnElementType::kOpposed   ) ? (ZERO_BB != opposed   ) :
                   (PEType == PawnElementType::kLever     ) ? (ZERO_BB != lever     ) :
                   (PEType == PawnElementType::kLeverPush ) ? (ZERO_BB != leverPush ) :
                   (PEType == PawnElementType::kBlocked   ) ? (ZERO_BB != blocked   ) :
                   (PEType == PawnElementType::kBackward  ) ? backward :
                   (PEType == PawnElementType::kPassed    ) ? passed   :
                   (PEType == PawnElementType::kDoubled   ) ? doubled  :
                   false
                 );
      }

      // MakeIndex
      template <PawnElementType PEType>
      inline IndexType PawnElement<PEType>::MakeIndex(int pawn_count, bool element) {
        return MakeIndexOfNoPiece(pawn_count) + 1 + element;
      }

      // MakeIndexOfNoPiece
      template <PawnElementType PEType>
      inline IndexType PawnElement<PEType>::MakeIndexOfNoPiece(int pawn_count) {
        return (2 + 1) * pawn_count;
      }

      // pawn_double_attacks_bb() returns the squares doubly attacked by pawns of the
      // given color from the squares in the given bitboard.
      template <PawnElementType PEType>
      Bitboard PawnElement<PEType>::pawn_double_attacks_bb(Bitboard b, Color C) {
        return C == WHITE ? shift<NORTH_WEST>(b) & shift<NORTH_EAST>(b)
                          : shift<SOUTH_WEST>(b) & shift<SOUTH_EAST>(b);
      }

      // shift() moves a bitboard one step along direction D
      template <PawnElementType PEType>
      Bitboard PawnElement<PEType>::shift_(Bitboard b, Direction D) {
        return  D == NORTH      ?  b             << 8 : D == SOUTH      ?  b             >> 8
              : D == NORTH+NORTH?  b             <<16 : D == SOUTH+SOUTH?  b             >>16
              : D == EAST       ? (b & ~FileHBB) << 1 : D == WEST       ? (b & ~FileABB) >> 1
              : D == NORTH_EAST ? (b & ~FileHBB) << 9 : D == NORTH_WEST ? (b & ~FileABB) << 7
              : D == SOUTH_EAST ? (b & ~FileHBB) >> 7 : D == SOUTH_WEST ? (b & ~FileABB) >> 9
              : 0;
      }

      template class PawnElement<PawnElementType::kNeighbours>;
      template class PawnElement<PawnElementType::kStoppers  >;
      template class PawnElement<PawnElementType::kSupport   >;
      template class PawnElement<PawnElementType::kPhalanx   >;
      template class PawnElement<PawnElementType::kOpposed   >;
      template class PawnElement<PawnElementType::kLever     >;
      template class PawnElement<PawnElementType::kLeverPush >;
      template class PawnElement<PawnElementType::kBlocked   >;
      template class PawnElement<PawnElementType::kBackward  >;
      template class PawnElement<PawnElementType::kPassed    >;
      template class PawnElement<PawnElementType::kDoubled   >;

    }  // namespace Features

  }  // namespace NNUE

}  // namespace Eval

#endif  // defined(EVAL_NNUE)
