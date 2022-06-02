#ifndef __EMD_ARRAY2D_H__
#define __EMD_ARRAY2D_H__

#include <stdint.h>

template<typename T>
struct array2d_t
{
    int64_t N, M;
    int64_t n, m;
    T* data;
};

template<typename T>
array2d_t<T> allocate2d(int64_t N, int64_t M)
{
    array2d_t<T> ans;
    ans.N = N;
    ans.M = M;
    ans.n = 0;
    ans.m = 0;
    ans.data = new T[N*M];
    return ans;
}
template<typename T>
void destory2d(array2d_t<T> arr)
{
    delete[] arr.data;
}

#define at2d(arr, x, y) ( (arr).data[(x)*(arr).M+(y)] )

#endif // __EMD_ARRAY2D_H__
