#pragma once

// swaps the "majorness" of a matrix. the majorness of a matrix
// is effectively whether contiguous row elements are stored
// contiguously in memory, or contiguous column elements are stored
// contiguously in memory (i.e. reads from left to right vs reads
// from up to down).
template <size_t Rows, size_t Cols, typename ElemTy>
void convert_matrix_major(const ElemTy* source, ElemTy* dest)
{
   static_assert(Rows > 0 && Cols > 0, "Must be non-zero indices");
   constexpr static size_t MAX_ELEMS = Rows * Cols;
   for (size_t itr = 0; itr < MAX_ELEMS; ++itr)
   {
      // this will effectively tell us where to get dest's next
      // neighbor from our source index. we need to figure 
      size_t targetSrcIdx = (Cols * (itr / Cols)) + idx;
      dest[itr] = source[targetSrcIdx];
   }
}