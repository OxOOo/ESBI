#ifndef __EMD_ARRAY1D_H__
#define __EMD_ARRAY1D_H__

#include <stdint.h>

template<typename T>
struct array1d_t
{
    int64_t N;
    int64_t n;
    T* data;
};

template<typename T>
array1d_t<T> allocate1d(int64_t N)
{
    array1d_t<T> ans;
    ans.N = N;
    ans.n = 0;
    ans.data = new T[N];
    return ans;
}
template<typename T>
void destory1d(array1d_t<T> arr)
{
    delete[] arr.data;
}

#define at1d(arr, pos) ( (arr).data[(pos)] )

#endif // __EMD_ARRAY1D_H__
