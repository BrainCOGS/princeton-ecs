#ifndef __UTILITIES_H__
#define __UTILITIES_H__


#include <cmath>
#include <cassert>


template<typename Size, typename OutType, typename InType>
void copy(Size n, OutType* out, const InType* in, const OutType scale = 1, const OutType shift = 0)
{
  for (Size i = 0; i < n; ++i)
    out[i]  = static_cast<OutType>(in[i]) * scale + shift;
}

template<typename Size, typename Type, typename InitType>
void initialize(Size n, Type* array, const InitType& init)
{
  for (Size i = 0; i < n; ++i)
    array[i]  = init;
}

template<typename Size, typename Type>
bool equal(Size n, const Type* array, const Type& compare)
{
  for (Size i = 0; i < n; ++i)
    if (array[i] != compare)  return false;
  return true;
}

template<typename Type>
bool approx(const Type x, const Type y, const Type& tolerance)
{
  return 2*fabs(x - y) <= tolerance * fabs(x + y);
}

template<typename Size, typename Type>
Type prod(Size n, const Type* array)
{
  Type    product   = 1;
  for (Size i = 0; i < n; ++i)
    product        *= array[i];
  return product;
}

template<typename Size, typename Type>
void cprod(Size n, const Type* array, Type* product)
{
  if (n < 1)        return;

  product[0]        = 1;
  for (Size i = 1; i < n; ++i)
    product[i]      = product[i-1] * array[i-1];
}

template<typename Size, typename Type>
Type sum(Size n, const Type* array)
{
  Type    cumulant  = 0;
  for (Size i = 0; i < n; ++i)
    cumulant       += array[i];
  return cumulant;
}

template<typename Size, typename Type>
Type sumSquares(Size n, const Type* array)
{
  Type    cumulant  = 0;
  for (Size i = 0; i < n; ++i)
    cumulant       += sqr(array[i]);
  return cumulant;
}

template<typename Size, typename Type, typename Weight>
Type sumSquares(Size n, const Type* array, const Weight* weight)
{
  Type    cumulant  = 0;
  for (Size i = 0; i < n; ++i)
    cumulant       += sqr(array[i] * weight[i]);
  return cumulant;
}

template<typename Size, typename Type>
Type maximum(Size n, const Type* array)
{
  if (n < 1)  return std::numeric_limits<Type>::quiet_NaN();

  Type    maxValue  = array[0];
  for (Size i = 1; i < n; ++i)
    if (array[i] > maxValue)
      maxValue      = array[i];
  return maxValue;
}

template<typename Size, typename Type>
Type minimum(Size n, const Type* array)
{
  if (n < 1)  return std::numeric_limits<Type>::quiet_NaN();

  Type    minValue  = array[0];
  for (Size i = 1; i < n; ++i)
    if (array[i] < minValue)
      minValue      = array[i];
  return minValue;
}

template<typename Size, typename Type>
Type maximum(Size nX, Size nY, const Type* xArray, const Type* yArray)
{
  if (nX < 1 || nY < 1)
    return std::numeric_limits<Type>::quiet_NaN();

  Type    maxValue  = xArray[0] * yArray[0];
  for (Size iX = 0; iX < nX; ++iX)
    for (Size iY = 0; iY < nX; ++iY)
      maxValue      = std::max( xArray[iX]*yArray[iY], maxValue );
  return maxValue;
}


template<typename Size, typename Type>
Type mean(Size n, const Type* array)
{
  if (n < 1)        return 0;
  return sum(n, array) / n;
}

template<typename Size, typename Type, typename Weight>
Type mean(Size n, const Type* array, const Weight* w)
{
  Type    cumulant  = 0;
  Type    sumW      = 0;
  for (Size i = 0; i < n; ++i) {
    cumulant       += w[i] * array[i];
    sumW           += w[i];
  }

  if (sumW == 0)    return 0;
  return cumulant / sumW;
}

template<typename Size, typename Type, typename Weight>
double variance(Size n, const Type& array, const Weight* w, double* mean = 0)
{
  double            avg     = 0;
  double            M2      = 0;
  double            sumW    = 0;
  for (Size i = 0; i < n; ++i) {
    if (w[i] <= 0)  continue;

    const double    temp    = w[i] + sumW;
    const double    delta   = getAt(array,i) - avg;
    const double    R       = delta * w[i] / temp;
    avg            += R;
    M2             += sumW * delta * R;
    sumW            = temp;
  }

  if (mean)        *mean    = avg;
  if (sumW == 0)    return 0;
  return (M2 / sumW) * n / (n-1);
}

template<typename Size>
double getAt(const double& array, Size i)
{
  return array;
}
template<typename Size, typename Type>
double getAt(const Type* array, Size i)
{
  return getAt(array[i], 0);
}


//-------------------------------------------------------------------------
template<typename Type>
int sign(Type x)
{
  return ( x < 0 ? -1 : 1 );
}
template<typename Type>
Type sqr(Type x)
{
  return x*x;
}

//-------------------------------------------------------------------------
template<typename Type>
Type safe(Type x, Type boundary, Type tolerance)
{
  if (fabs(x - boundary) < tolerance)
    return boundary;
  return x;
}

//-------------------------------------------------------------------------
//  Computes sum = sA * a + sB * b
template<typename Size, typename Type>
void add(Size n, Type* sum, const Type* a, const Type* b, double sA = 1, double sB = 1)
{
  for (Size i = 0; i < n; ++i)
    sum[i]  = sA * a[i] + sB * b[i];
}

//-------------------------------------------------------------------------
//  Computes the 2-norm
template<typename Size, typename Type>
Type norm(Size n, const Type* x)
{
  Type  value = 0;
  for (Size i = 0; i < n; ++i)
    value  += x[i] * x[i];
  return sqrt(value);
}

//-------------------------------------------------------------------------
template<typename Size, typename Type>
Type normalize(Size n, Type* x, Type scale = 1)
{
  const Type  length  = norm(n, x);
  if (length) {
    scale    /= length;
    for (Size i = 0; i < n; ++i)
      x[i]   *= scale;
  }
  return scale / length;
}

//-------------------------------------------------------------------------
//  Computes the distance between x and y
template<typename Size, typename Type>
Type distance(Size n, const Type* x, const Type* y)
{
  Type  value = 0;
  for (Size i = 0; i < n; ++i)
    value    += sqr(x[i] - y[i]);
  return sqrt(value);
}

template<typename Size, typename Type>
Type distance(Size n, const Type** pos, size_t item1, size_t item2)
{
  Type  value = 0;
  for (Size i = 0; i < n; ++i)
    value    += sqr(pos[i][item1] - pos[i][item2]);
  return sqrt(value);
}

//-------------------------------------------------------------------------
template<typename Size, typename Type>
Type dot(Size n, const Type* x, const Type* y)
{
  Type  value = 0;
  for (Size i = 0; i < n; ++i)
    value    += x[i] * y[i];
  return value;
}

//-------------------------------------------------------------------------
template<typename Type>
bool lessByDeref(Type const* a, Type const* b)
{
  return *a < *b; 
}

template<typename Type>
bool greaterByDeref(Type const* a, Type const* b)
{
  return *a > *b; 
}

//-------------------------------------------------------------------------
template<typename Type, typename Weight>
Type accumulateMean(Type currentMean, Weight currentWeight, Type x, Weight weight = 1)
{
  weight       /= weight + currentWeight;
  currentMean  += (x - currentMean) * weight;
  
  return currentMean;
}


//-------------------------------------------------------------------------
template<typename Size, typename Index>
Index start(Index* subscripts, Size nDims, const Index* firsts = 0, const Index* metric = 0)
{
  if (firsts) {
    copy(nDims, subscripts, firsts);

    if (metric) {
      Index           linearIndex   = 0;
      for (mwSize iDim = 0; iDim < nDims; ++iDim)
        linearIndex  += metric[iDim] * subscripts[iDim];
      return linearIndex;
    }
  }
  else  initialize(nDims, subscripts, 0);

  return 0;
}

//-------------------------------------------------------------------------
// Advances the given subscripts by one step
template<typename Size, typename Index>
bool advance(Index* subscripts, Size nDims, const Index* dimensions, const Index* firsts = 0, const Index* steps = 0, bool includeBounds = false, Index* linearIndex = 0, const Index* metric = 0)
{
  if (linearIndex)      assert(metric);

  for (Size iDim = 0; iDim < nDims; ++iDim) {
    if (steps && steps[iDim] == 0)    continue;
    
    // If incrementing in this dimension is within bounds, have succeeded
    const Index         increment     = (steps ? steps[iDim] : 1);
    const Index         direction     = sign(increment);
    subscripts[iDim]   += increment;

    if  ( includeBounds 
        ? direction * subscripts[iDim] <= direction * dimensions[iDim]
        : direction * subscripts[iDim] <  direction * dimensions[iDim]
        ) {
      if (linearIndex) *linearIndex  += metric[iDim] * increment;
      return true;
    }
    
    // Otherwise rewind to the first and increment in the next dimension
    if (linearIndex) {
      *linearIndex     += metric[iDim] *  ( (firsts ? firsts[iDim] : 0)
                                          - subscripts[iDim] + increment  // undo increment
                                          );
    }
    subscripts[iDim]    = (firsts ? firsts[iDim] : 0);
  } // end loop over dimensions  
  
  // If we've reached this point, there is nothing within bounds.
  // Actually we should also have wrapped subscripts back to firsts entirely.
  return false;
}

//-------------------------------------------------------------------------
//  Row and column indices for a given linear index and matrix dimensions.
//  The linear index is assumed to go from 1 at (row,column) = (1,1), and
//  increment in the column-wise direction.
template<typename Size, typename Index, typename Dimension>
Index toLinearIndex(Size nDims, const Index* indices, const Dimension* dimensions)
{
  Index     slot    = 0;
  Index     metric  = 1;
  for (Size iDim = 0; iDim < nDims; ++iDim) {
    slot     += metric * indices[iDim];
    metric   *= dimensions[iDim];
  }
  return slot;
}

//-------------------------------------------------------------------------
//  Converts FORTRAN -> C indices, plus support for negative indexing to 
//  count backwards from the last element.
template<typename Index, typename Size>
Index normalIndex(Index index, Size maxIndex)
{
  return ( index < 0 ? maxIndex + index : index - 1 );
}


//-------------------------------------------------------------------------
//    Missing math functions
//-------------------------------------------------------------------------

template<typename Number>
Number asinh(Number x)
{
  return log(x + sqrt(x*x + 1));
}

template<typename Number>
Number acosh(Number x)
{
  return log(x + sqrt(x*x - 1));
}

template<typename Number>
Number atanh(Number x)
{
  return ( log(1+x) - log(1-x) )/2;
}



//-------------------------------------------------------------------------
//    Statistics
//-------------------------------------------------------------------------

class SampleStatistics {
protected:
  int         numEntries;
  double      sumWeights;
  double      sumWeights2;
  double      mean;
  double      M2;
  double      minimum;
  double      maximum;

public:
  SampleStatistics() { clear(); }
  void clear()  { numEntries  = 0;  sumWeights  = 0;  sumWeights2 = 0;  mean  = 0;  M2  = 0;  minimum = 1e308;  maximum = -1e308;  }


  //---------------------------------------------------------------------------
  /// @name   Interface
  //---------------------------------------------------------------------------
  ///@{
public:
  int         getNumEntries         () const  { return numEntries;  }
  double      getSumWeights         () const  { return sumWeights;  }
  double      getSumWeights2        () const  { return sumWeights2; }
  double      getMean               () const  { return mean;        }
  double      getMeanUncertainty    () const  { return getRMS() * sqrt(sumWeights2) / sumWeights; }
  double      getM2                 () const  { return M2;          }
  double      getSampleVariance     () const  { return ( numEntries > 1 ? (M2/sumWeights) * numEntries / (numEntries-1) : 0 ); }
  double      getPopulationVariance () const  { return ( numEntries > 1 ? M2/sumWeights                                 : 0 ); }
  double      getRMS                () const  { return sqrt(getSampleVariance()); }
  double      getMinimum            () const  { return minimum;     }
  double      getMaximum            () const  { return maximum;     }
  ///@}


  //---------------------------------------------------------------------------
  /// @name   Combination
  //---------------------------------------------------------------------------
  ///@{
public:
  virtual void  add(double x, double weight = 1)
  {
    if (!weight)      return;

    sumWeights2      += weight * weight;

    const double      temp    = weight + sumWeights;
    const double      delta   = x - mean;
    const double      R       = delta * weight / temp;
    mean             += R;
    if (sumWeights > 0)
      M2             += sumWeights * delta * R;
    sumWeights        = temp;
    ++numEntries;

    if (x < minimum)  minimum = x;
    if (x > maximum)  maximum = x;
  }

  virtual void  add(const SampleStatistics& other, const double weight = 1)
  {
    numEntries       += other.numEntries;
    const double      otherW  = other.sumWeights * weight;
    const double      total   = sumWeights + otherW;
    if (total <= 0)   return;

    const double      delta   = other.mean - mean;
    mean             += delta * otherW / total;
    M2               += other.M2 + delta*delta * sumWeights * otherW / total;
    sumWeights       += otherW;

    if (other.minimum < minimum)
      minimum         = other.minimum;
    if (other.maximum > maximum)
      maximum         = other.maximum;
  }
  ///@}
};


#endif //__UTILITIES_H__
