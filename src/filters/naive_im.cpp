#include "filters.h"
#include <queue>
#include <vector>
#include <cstdlib>
#include <iostream>

using namespace std;

double imLB(array1d_t<double> q, array1d_t<double> p, array2d_t<double> cost)
{
    double ans = 0;
    for (int y = 0; y < p.n; y++)
    {
        double earthMoved = 0;
        double this_ans = 0;
        typedef pair<double, int> Node; // cost, target
        priority_queue<Node, vector<Node>, greater<Node>> heap;
        for (int x = 0; x < q.n; x++)
        {
            heap.push(Node(at2d(cost, x, y), x));
        }
        int x = heap.top().second;
        while (at1d(p, y) - earthMoved > at1d(q, x))
        {
            this_ans += heap.top().first * at1d(q, x);
            earthMoved += at1d(q, x);
            heap.pop();
            x = heap.top().second;
        }
        this_ans += heap.top().first * (at1d(p, y) - earthMoved);
        ans += this_ans;
        // cout << "dim " << y << " " << this_ans << " ans = " << ans << endl;
    }
    return ans;
}
array1d_t<int> filter_naiveIM(array1d_t<double> q, double T, array2d_t<double> cost, array2d_t<double> db, void *)
{
    array1d_t<int> ans = allocate1d<int>(db.N);

    ans.n = 0;
    array1d_t<double> p = allocate1d<double>(db.M);
    for (int i = 0; i < db.n; i++)
    {
        p.n = db.m;
        for (int j = 0; j < db.m; j++)
        {
            at1d(p, j) = at2d(db, i, j);
        }
        // cout << "running im for " << i << endl;
        if (imLB(q, p, cost) <= T)
        {
            at1d(ans, ans.n) = i;
            ans.n++;
        }
        // cout << i << " imLB(q, p, cost) = " << imLB(q, p, cost) << endl;
    }
    destory1d(p);

    return ans;
}
