/*
 *  This Quickselect routine is based on the algorithm described in
 *  "Numerical recipes in C", Second Edition,
 *  Cambridge University Press, 1992, Section 8.5, ISBN 0-521-43108-5
 *  This code by Nicolas Devillard - 1998. Public domain.
 *
 *  Obtained from http://ndevilla.free.fr/median/median/ and modified
 *  by Sue Ann Koay (koay@princeton.edu).
 */


#ifndef QUICKSELECT_H
#define QUICKSELECT_H

#include <utility>
#include <vector>

template<typename Element>
Element quickSelect(std::vector<Element>& arr, const size_t n) 
{
  int             low       = 0;
  int             high      = n - 1;
  int             median    = (low + high) / 2;

  while (true)
  {
    if (high <= low) /* One element only */
      return arr[median] ;

    if (high == low + 1) {  /* Two elements only */
      if (arr[low] > arr[high])
        std::swap(arr[low], arr[high]) ;
      return arr[median] ;
    }

    /* Find median of low, middle and high items; swap into position low */
    const int     middle    = (low + high) / 2;
    if (arr[middle] > arr[high])    std::swap(arr[middle], arr[high]) ;
    if (arr[low]    > arr[high])    std::swap(arr[low]   , arr[high]) ;
    if (arr[middle] > arr[low])     std::swap(arr[middle], arr[low] ) ;

    /* Swap low item (now in position middle) into position (low+1) */
    std::swap(arr[middle], arr[low+1]) ;

    /* Nibble from each end towards middle, swapping items when stuck */
    int           ll        = low + 1;
    int           hh        = high;
    while (true)
    {
      do ++ll;    while (arr[low] > arr[ll] );
      do --hh;    while (arr[hh]  > arr[low]);

      if (hh < ll)
        break;

      std::swap(arr[ll], arr[hh]) ;
    }

    /* Swap middle item (in position low) back into correct position */
    std::swap(arr[low], arr[hh]) ;

    /* Re-set active partition */
    if (hh <= median)       low   = ll;
    if (hh >= median)       high  = hh - 1;
  }
}

#endif //QUICKSELECT_H
