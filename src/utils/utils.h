#ifndef __EMD_UTILS_H__
#define __EMD_UTILS_H__

#include <stddef.h>
#include <unistd.h>
#include <sys/time.h>
#include <cstdlib>
#include <tuple>
#include <string>
#include <vector>
#include <set>
#include <tuple>
#include "array1d.h"
#include "array2d.h"

using namespace std;

double getTime();

uint32_t uint32Rand();

// return <cost, db>
tuple<array2d_t<double>, array2d_t<double>> loadDB(const string &dbname);

struct Query
{
    array1d_t<double> q;
    double T;
};
vector<Query> loadQueries(const string &queries_file, int dims);

template <typename T>
array1d_t<T> getRow(array2d_t<T> mat, int r)
{
    array1d_t<T> ans = allocate1d<T>(mat.M);
    ans.n = mat.m;
    for (int i = 0; i < mat.m; i++)
    {
        at1d(ans, i) = at2d(mat, r, i);
    }
    return ans;
}

template <typename T>
T getSum(array1d_t<T> mat)
{
    T ans = 0;
    for (int i = 0; i < mat.n; i++)
    {
        ans += at1d(mat, i);
    }
    return ans;
}

#endif // __EMD_UTILS_H__
