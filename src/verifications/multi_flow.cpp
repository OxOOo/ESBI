#include "verifications.h"
#include <emds/emds.h>
#include <cstdlib>
#include <algorithm>
#include <queue>
#include <cassert>
#include <iostream>
#include <cmath>

using namespace std;

const static double EPS = 1e-5;

// return <true,dist> if dist <= T
pair<bool, double> multiflow(array1d_t<double> q, array1d_t<double> p, array2d_t<double> c, double T)
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

    double disr[MAX_DIM], disc[MAX_DIM];
    double hr[MAX_DIM], hc[MAX_DIM];
    int usedr[MAX_DIM], usedc[MAX_DIM];
    int fromr[MAX_DIM], fromc[MAX_DIM];

    {
        typedef pair<int, int> Node; // pos, type=0 -> q, type=1 -> p
        queue<Node> que;
        for (int i = 0; i < q.n; i++)
        {
            hr[i] = 0;
            que.push(Node(i, 0));
        }
        for (int i = 0; i < p.n; i++)
        {
            hc[i] = 0;
            que.push(Node(i, 1));
        }
        while (!que.empty())
        {
            Node item = que.front();
            que.pop();
            int u = item.first;
            if (item.second == 0)
            { // q
                for (int v = 0; v < p.n; v++)
                {
                    if (flow[u][v] + EPS < at1d(q, u) && hc[v] > hr[u] + cost[u][v] + EPS)
                    {
                        hc[v] = hr[u] + cost[u][v];
                        que.push(Node(v, 1));
                    }
                }
            }
            else
            { // p
                for (int v = 0; v < q.n; v++)
                {
                    if (flow[v][u] > EPS && hr[v] > hc[u] - cost[v][u] + EPS)
                    {
                        hr[v] = hc[u] - cost[v][u];
                        que.push(Node(v, 0));
                    }
                }
            }
        }
    }

    double last_mindis = 0;
    int iter = 0;
    bool early_stop = false;
    while (dist <= T)
    {
        iter++;
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
        bool empty = true;
        double inbalance_sum = 0;
        for (int i = 0; i < q.n; i++)
        {
            if (sum[i] + EPS < at1d(q, i))
            {
                disr[i] = -hr[i];
                empty = false;
                inbalance_sum += at1d(q, i) - sum[i];
            }
        }
        if (empty)
            break;
        if (inbalance_sum * last_mindis + dist > T)
        {
            early_stop = true;
            break;
        }
        for (int t = 0; t < q.n + p.n; t++)
        {
            int type = -1;
            int u = -1;
            double mindis = -1;
            for (int i = 0; i < q.n; i++)
            {
                if (!usedr[i])
                {
                    if (u < 0 || mindis > disr[i] + EPS)
                    {
                        type = 0;
                        u = i;
                        mindis = disr[i];
                    }
                }
            }
            for (int i = 0; i < p.n; i++)
            {
                if (!usedc[i])
                {
                    if (u < 0 || mindis > disc[i] + EPS)
                    {
                        type = 1;
                        u = i;
                        mindis = disc[i];
                    }
                }
            }
            if (type == 0)
            { // q
                usedr[u] = 1;
                for (int v = 0; v < p.n; v++)
                {
                    if (flow[u][v] + EPS < at1d(q, u) && disc[v] > disr[u] + hr[u] + cost[u][v] - hc[v] + EPS)
                    {
                        assert(hr[u] + cost[u][v] - hc[v] >= 0);
                        assert(!usedc[v]);
                        disc[v] = disr[u] + hr[u] + cost[u][v] - hc[v];
                        fromc[v] = u;
                    }
                }
            }
            else
            { // p
                usedc[u] = 1;
                for (int v = 0; v < q.n; v++)
                {
                    if (flow[v][u] > EPS && disr[v] > disc[u] + hc[u] - cost[v][u] - hr[v] + EPS)
                    {
                        assert(hc[u] - cost[v][u] - hr[v] >= 0);
                        assert(!usedr[v]);
                        disr[v] = disc[u] + hc[u] - cost[v][u] - hr[v];
                        fromr[v] = u;
                    }
                }
            }
        }

        double mindis = -1;
        for (int i = 0; i < q.n; i++)
            if (sum[i] > at1d(q, i) + EPS && fromr[i] >= 0)
            {
                if (mindis < 0 || mindis > disr[i] + hr[i] + EPS)
                {
                    mindis = disr[i] + hr[i];
                }
            }
        assert(mindis >= 0);
        assert(last_mindis <= mindis);
        last_mindis = mindis;
        // cout << "======= " <<  iter << " " << mindis << endl;

        for (int i = 0; i < q.n; i++)
            if (sum[i] > at1d(q, i) + EPS && fromr[i] >= 0 && fabs(disr[i] + hr[i] - mindis) < EPS)
            {
                double maxflow = sum[i] - at1d(q, i);
                int r = i;
                while (fromr[r] >= 0)
                {
                    int c = fromr[r];
                    maxflow = min(maxflow, flow[r][c]);
                    r = fromc[c];
                    maxflow = min(maxflow, at1d(q, r) - flow[r][c]);
                }
                maxflow = min(maxflow, at1d(q, r) - sum[r]);
                dist += mindis * maxflow;
                for (r = i; fromr[r] >= 0;)
                {
                    int c = fromr[r];
                    flow[r][c] -= maxflow;
                    r = fromc[c];
                    flow[r][c] += maxflow;
                }
                sum[r] += maxflow;
                sum[i] -= maxflow;
            }

        for (int i = 0; i < q.n; i++)
            if (disr[i] < INF)
            {
                hr[i] += disr[i];
            }
        for (int i = 0; i < p.n; i++)
            if (disc[i] < INF)
            {
                hc[i] += disc[i];
            }
    }
    return (!early_stop && dist <= T) ? make_pair(true, dist) : make_pair(false, dist);
}
array1d_t<Record> verification_multiflow(array1d_t<double> q, double T, array2d_t<double> cost, array2d_t<double> db)
{
    array1d_t<Record> ans = allocate1d<Record>(db.N);
    ans.n = 0;
    for (int i = 0; i < db.n; i++)
    {
        array1d_t<double> p = getRow(db, i);
        auto rst = multiflow(q, p, cost, T);
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
