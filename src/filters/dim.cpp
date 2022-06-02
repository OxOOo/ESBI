#include "filters.h"
#include <algorithm>
#include <queue>
#include <vector>
#include <cstdlib>
#include <iostream>

using namespace std;

array1d_t<int> filter_dim(array1d_t<double> q, double T, array2d_t<double> cost, array2d_t<double> db, void *)
{
    double *im = new double[db.N];
    for (int i = 0; i < db.n; i++)
    {
        im[i] = 0;
    }
    for (int y = 0; y < db.m; y++)
    {
        int *from = new int[q.N];
        double *flowMoved = new double[q.N];
        double *costMoved = new double[q.N];

        typedef pair<double, int> Node; // cost, from
        priority_queue<Node, vector<Node>, greater<Node>> heap;
        for (int x = 0; x < q.n; x++)
        {
            heap.push(Node(at2d(cost, x, y), x));
        }
        double _flowMoved = 0;
        double _costMoved = 0;
        for (int i = 0; i < q.n; i++)
        {
            int x = heap.top().second;
            from[i] = x;
            flowMoved[i] = _flowMoved;
            costMoved[i] = _costMoved;
            _flowMoved += at1d(q, x);
            _costMoved += heap.top().first * at1d(q, x);
            heap.pop();
        }

        for (int i = 0; i < db.n; i++)
        {
            int idx = upper_bound(flowMoved, flowMoved + q.n, at2d(db, i, y)) - flowMoved - 1;
            im[i] += costMoved[idx] + (at2d(db, i, y) - flowMoved[idx]) * at2d(cost, from[idx], y);
        }

        delete[] from;
        delete[] flowMoved;
        delete[] costMoved;
    }

    array1d_t<int> ans = allocate1d<int>(db.N);
    ans.n = 0;
    for (int i = 0; i < db.n; i++)
    {
        if (im[i] <= T)
        {
            at1d(ans, ans.n) = i;
            ans.n++;
        }
    }
    delete[] im;
    return ans;
}
