#include "verifications.h"
#include <emds/emds.h>
#include <cstdlib>
#include <algorithm>
#include <queue>
#include <cassert>
#include <iostream>

using namespace std;

const static double EPS = 1e-5;

// return <true,dist> if dist <= T
pair<bool, double> singleflow(array1d_t<double> q, array1d_t<double> p, array2d_t<double> c, double T)
{
    static const int MAX_DIM = 3000;
    const double INF = 1e100;
    static double flow[MAX_DIM][MAX_DIM];
    static double cost[MAX_DIM][MAX_DIM];
    static double sum[MAX_DIM];

    assert(q.n <= MAX_DIM);
    assert(p.n <= MAX_DIM);
    for (int i = 0; i < q.n; i++)
    {
        for (int j = 0; j < p.n; j++)
        {
            flow[i][j] = 0;
            cost[i][j] = at2d(c, i, j);
        }
        sum[i] = 0;
    }

    double dist = 0;
    for (int y = 0; y < p.n; y++)
    {
        typedef pair<double, int> Node; // cost, from
        Node nodes[MAX_DIM];
        for (int x = 0; x < q.n; x++)
        {
            nodes[x] = Node(cost[x][y], x);
        }
        sort(nodes, nodes + q.n);

        double currentFlow = 0;
        for (int i = 0; i < q.n; i++)
        {
            double thisFlow = -1;
            int x = nodes[i].second;
            if (currentFlow + at1d(q, x) < at1d(p, y))
            {
                thisFlow = at1d(q, x);
            }
            else
            {
                thisFlow = at1d(p, y) - currentFlow;
            }
            flow[x][y] = thisFlow;
            dist += cost[x][y] * thisFlow;
            currentFlow += thisFlow;
            if (currentFlow >= at1d(p, y))
                break;
        }
    }
    if (dist > T)
    {
        return make_pair(false, dist);
    }

    for (int x = 0; x < q.n; x++)
    {
        sum[x] = 0;
        for (int y = 0; y < p.n; y++)
        {
            sum[x] += flow[x][y];
        }
    }

    double last_mindis = 0;
    while (dist <= T)
    {
        typedef pair<int, int> Point; // pos, type=0 -> r, type=1 -> c
        double disr[MAX_DIM], disc[MAX_DIM];
        int usedr[MAX_DIM], usedc[MAX_DIM];
        int fromr[MAX_DIM], fromc[MAX_DIM];
        queue<Point> que;
        for (int i = 0; i < q.n; i++)
        {
            disr[i] = INF;
            usedr[i] = 0;
            fromr[i] = -1;
        }
        for (int i = 0; i < p.n; i++)
        {
            disc[i] = INF;
            usedc[i] = 0;
            fromc[i] = -1;
        }
        double remain_sum = 0;
        for (int i = 0; i < q.n; i++)
        {
            if (sum[i] + EPS < at1d(q, i))
            {
                remain_sum += at1d(q, i) - sum[i];
                disr[i] = 0;
                usedr[i] = 1;
                que.emplace(i, 0);
            }
        }
        if (que.empty())
            break;
        if (dist + remain_sum * last_mindis > T + EPS)
        {
            return make_pair(false, dist + remain_sum * last_mindis);
        }
        while (!que.empty())
        {
            Point u = que.front();
            que.pop();
            if (u.second == 0)
            { // r
                usedr[u.first] = 0;
                for (int c = 0; c < p.n; c++)
                {
                    if (flow[u.first][c] + EPS < at1d(q, u.first) && disc[c] > disr[u.first] + cost[u.first][c] + EPS)
                    {
                        disc[c] = disr[u.first] + cost[u.first][c];
                        fromc[c] = u.first;
                        if (!usedc[c])
                        {
                            usedc[c] = 1;
                            que.emplace(c, 1);
                        }
                    }
                }
            }
            else
            { // c
                usedc[u.first] = 0;
                for (int r = 0; r < q.n; r++)
                {
                    if (flow[r][u.first] > EPS && disr[r] > disc[u.first] - cost[r][u.first] + EPS)
                    {
                        disr[r] = disc[u.first] - cost[r][u.first];
                        fromr[r] = u.first;
                        if (!usedr[r])
                        {
                            usedr[r] = 1;
                            que.emplace(r, 0);
                        }
                    }
                }
            }
        }
        double mindis = -1;
        int mindim = -1;
        for (int i = 0; i < q.n; i++)
        {
            if (sum[i] > at1d(q, i) + EPS && fromr[i] >= 0)
            {
                if (mindim < 0 || disr[i] + EPS < mindis)
                {
                    mindis = disr[i];
                    mindim = i;
                }
            }
        }
        assert(mindim >= 0);
        assert(last_mindis <= mindis);
        last_mindis = mindis;

        double maxflow = sum[mindim] - at1d(q, mindim);
        int r = mindim;
        while (fromr[r] >= 0)
        {
            int c = fromr[r];
            maxflow = min(maxflow, flow[r][c]);
            r = fromc[c];
            maxflow = min(maxflow, at1d(q, r) - flow[r][c]);
        }
        maxflow = min(maxflow, at1d(q, r) - sum[r]);
        dist += mindis * maxflow;
        for (r = mindim; fromr[r] >= 0;)
        {
            int c = fromr[r];
            flow[r][c] -= maxflow;
            r = fromc[c];
            flow[r][c] += maxflow;
        }
        sum[r] += maxflow;
        sum[mindim] -= maxflow;
    }
    return dist <= T ? make_pair(true, dist) : make_pair(false, dist);
}
array1d_t<Record> verification_singleflow(array1d_t<double> q, double T, array2d_t<double> cost, array2d_t<double> db)
{
    array1d_t<Record> ans = allocate1d<Record>(db.N);
    ans.n = 0;
    for (int i = 0; i < db.n; i++)
    {
        array1d_t<double> p = getRow(db, i);
        auto rst = singleflow(q, p, cost, T);
        if (rst.first)
        {
            Record record;
            record.dist = rst.second;
            record.id = i;
            at1d(ans, ans.n) = record;
            ans.n++;
        }
        destory1d(p);
    }
    return ans;
}
