#include "emds.h"
#include <vendor/emd.h>
#include <iostream>
#include <cmath>

using namespace std;

array2d_t<double> global_cost;
double cost_func(int *a, int *b)
{
    return at2d(global_cost, *a, *b);
}

double rubner_emd(array1d_t<double> q, array1d_t<double> p, array2d_t<double> cost)
{
    signature_t qs;
    qs.n = q.n;
    qs.Features = new int[q.N];
    qs.Weights = new double[q.N];
    for (int i = 0; i < q.n; i++)
    {
        qs.Features[i] = i;
        qs.Weights[i] = (double)at1d(q, i);
    }

    signature_t ps;
    ps.n = p.n;
    ps.Features = new int[p.N];
    ps.Weights = new double[p.N];
    for (int i = 0; i < p.n; i++)
    {
        ps.Features[i] = i;
        ps.Weights[i] = (double)at1d(p, i);
    }

    global_cost = cost;
    double ans = emd(&qs, &ps, cost_func, NULL, NULL);

    double sum = 0;
    for (int i = 0; i < q.n; i++)
    {
        sum += at1d(q, i);
    }
    ans = ans * sum;

    delete[] qs.Features;
    delete[] qs.Weights;
    delete[] ps.Features;
    delete[] ps.Weights;
    return ans;
}
