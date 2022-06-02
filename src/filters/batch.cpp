#include "filters.h"
#include <algorithm>
#include <queue>
#include <vector>
#include <cmath>
#include <iostream>

using namespace std;

struct PData
{
    array2d_t<int> permutation;
    array2d_t<double> flow;
};

void *filter_batch_preprocessing(array2d_t<double> cost, array2d_t<double> db)
{
    PData *pdata = new PData;

    pdata->permutation = allocate2d<int>(db.M, db.N);
    pdata->permutation.n = db.m;
    pdata->permutation.m = db.n;
    for (int y = 0; y < db.m; y++)
    {
        typedef pair<double, int> Node; // target flow, record
        Node *records = new Node[db.N];
        for (int i = 0; i < db.n; i++)
        {
            records[i] = Node(at2d(db, i, y), i);
        }
        sort(records, records + db.n);
        for (int i = 0; i < db.n; i++)
        {
            at2d(pdata->permutation, y, i) = records[i].second;
        }
        delete[] records;
    }

    pdata->flow = allocate2d<double>(db.M, db.N);
    pdata->flow.n = db.m;
    pdata->flow.m = db.n;
    for (int y = 0; y < db.m; y++)
    {
        for (int i = 0; i < db.n; i++)
        {
            at2d(pdata->flow, y, i) = at2d(db, at2d(pdata->permutation, y, i), y);
        }
    }

    return (void *)pdata;
}
array1d_t<int> filter_batch(array1d_t<double> q, double T, array2d_t<double> cost, array2d_t<double> db, void *preprocessing_data)
{
    PData *pdata = (PData *)preprocessing_data;

    struct EachInfo
    {
        double imCost;
        double roundCost;
        int16_t valueCount;
        int16_t filterOut;
    };

    EachInfo *each = new EachInfo[db.N];
    for (int i = 0; i < db.n; i++)
    {
        each[i].imCost = 0;
        each[i].roundCost = 0;
        each[i].valueCount = 0;
        each[i].filterOut = 0;
    }

    int logd = 1;
    while ((1 << logd) <= db.m)
        logd++;
    const double R = (double)T * (double)logd / (double)db.n;
    const int RT = floor(double(T) / R);

    typedef pair<double, int> Item; // im, dim
    priority_queue<Item, vector<Item>, greater<Item>> heap;

    typedef pair<double, int> Node; // cost, from
    struct DimInfo
    {
        priority_queue<Node, vector<Node>, greater<Node>> sourceH;
        double flowMoved;
        double costMoved;
        int currentSource;
        double currentCost;
        double currentFlowRemain;
    };
    DimInfo *infos = new DimInfo[db.M];
    int *pos = new int[db.M];
    for (int y = 0; y < db.m; y++)
    {
        for (int x = 0; x < q.n; x++)
        {
            infos[y].sourceH.push(Node(at2d(cost, x, y), x));
        }
        infos[y].flowMoved = 0;
        infos[y].costMoved = 0;
        infos[y].currentSource = infos[y].sourceH.top().second;
        infos[y].currentCost = infos[y].sourceH.top().first;
        infos[y].currentFlowRemain = at1d(q, infos[y].currentSource);

        pos[y] = 0;
        int needFlow = at2d(pdata->flow, y, pos[y]);
        while (infos[y].flowMoved + infos[y].currentFlowRemain < needFlow)
        {
            infos[y].flowMoved += infos[y].currentFlowRemain;
            infos[y].costMoved += infos[y].currentFlowRemain * infos[y].currentCost;
            infos[y].sourceH.pop();
            infos[y].currentSource = infos[y].sourceH.top().second;
            infos[y].currentCost = infos[y].sourceH.top().first;
            infos[y].currentFlowRemain = at1d(q, infos[y].currentSource);
        }
        int thisFlow = needFlow - infos[y].flowMoved;
        infos[y].currentFlowRemain -= thisFlow;
        infos[y].flowMoved = needFlow;
        infos[y].costMoved += thisFlow * infos[y].currentCost;
        heap.push(Item(infos[y].costMoved, y));
    }

    int64_t totalLeft = (int64_t)db.n * (int64_t)db.m;
    int64_t filterOutLeft = 0;
    while (totalLeft > filterOutLeft)
    {
        int minDim = heap.top().second;
        double minValue = heap.top().first;
        heap.pop();
        double thisValue = floor(minValue / R);
        double nextValue = minValue;
        int i = at2d(pdata->permutation, minDim, pos[minDim]);
        do
        {
            if (each[i].filterOut)
            {
                totalLeft--;
                filterOutLeft--;
            }
            else
            {
                totalLeft--;
                each[i].valueCount++;
                each[i].imCost += nextValue;
                each[i].roundCost += thisValue;

                if (each[i].roundCost + thisValue * (db.m - each[i].valueCount) > RT)
                {
                    each[i].filterOut = 1;
                    filterOutLeft += db.m - each[i].valueCount;
                }
            }
            pos[minDim]++;
            if (pos[minDim] >= db.n)
            {
                nextValue = -1;
                break;
            }
            else
            {
                i = at2d(pdata->permutation, minDim, pos[minDim]);
                double needFlow = at2d(pdata->flow, minDim, pos[minDim]);
                while (infos[minDim].flowMoved + infos[minDim].currentFlowRemain < needFlow)
                {
                    infos[minDim].flowMoved += infos[minDim].currentFlowRemain;
                    infos[minDim].costMoved += infos[minDim].currentFlowRemain * infos[minDim].currentCost;
                    infos[minDim].sourceH.pop();
                    infos[minDim].currentSource = infos[minDim].sourceH.top().second;
                    infos[minDim].currentCost = infos[minDim].sourceH.top().first;
                    infos[minDim].currentFlowRemain = at1d(q, infos[minDim].currentSource);
                }
                double thisFlow = needFlow - infos[minDim].flowMoved;
                infos[minDim].currentFlowRemain -= thisFlow;
                infos[minDim].flowMoved = needFlow;
                infos[minDim].costMoved += thisFlow * infos[minDim].currentCost;
                nextValue = infos[minDim].costMoved;
                if (thisValue != floor(nextValue / R))
                    break;
            }
        } while (true);
        if (nextValue >= 0)
        {
            heap.push(Item(nextValue, minDim));
        }
    }

    delete[] infos;
    delete[] pos;

    array1d_t<int> ans = allocate1d<int>(db.N);
    ans.n = 0;
    for (int i = 0; i < db.n; i++)
    {
        if (!each[i].filterOut && each[i].imCost <= T)
        {
            at1d(ans, ans.n) = i;
            ans.n++;
        }
    }
    delete[] each;
    return ans;
}
void filter_batch_dpreprocessing(array2d_t<double> cost, array2d_t<double> db, void *preprocessing_data)
{
    PData *pdata = (PData *)preprocessing_data;
    destory2d(pdata->permutation);
    destory2d(pdata->flow);
    delete pdata;
}
