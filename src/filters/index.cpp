#include "filters.h"
#include <algorithm>
#include <queue>
#include <vector>

using namespace std;

struct PData
{
    array2d_t<int> permutation;
    array2d_t<double> flow;
};

void *filter_index_preprocessing(array2d_t<double> cost, array2d_t<double> db)
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
array1d_t<int> filter_index(array1d_t<double> q, double T, array2d_t<double> cost, array2d_t<double> db, void *preprocessing_data)
{
    PData *pdata = (PData *)preprocessing_data;

    double *im = new double[db.N];
    for (int i = 0; i < db.n; i++)
    {
        im[i] = 0;
    }

    for (int y = 0; y < db.m; y++)
    {
        typedef pair<double, int> Node; // cost, from
        priority_queue<Node, vector<Node>, greater<Node>> sourceH;
        for (int x = 0; x < q.n; x++)
        {
            sourceH.push(Node(at2d(cost, x, y), x));
        }
        double flowMoved = 0;
        double costMoved = 0;
        int currentSource = sourceH.top().second;
        double currentCost = sourceH.top().first;
        double currentFlowRemain = at1d(q, currentSource);

        int *permutation = &at2d(pdata->permutation, y, 0);
        double *flow = &at2d(pdata->flow, y, 0);
        for (int k = 0; k < db.n; k++)
        {
            int i = *permutation;
            permutation++;

            double needFlow = *flow;
            flow++;
            while (flowMoved + currentFlowRemain < needFlow)
            {
                flowMoved += currentFlowRemain;
                costMoved += currentFlowRemain * currentCost;
                sourceH.pop();
                currentSource = sourceH.top().second;
                currentCost = sourceH.top().first;
                currentFlowRemain = at1d(q, currentSource);
            }

            double thisFlow = needFlow - flowMoved;
            currentFlowRemain -= thisFlow;
            flowMoved = needFlow;
            costMoved += thisFlow * currentCost;
            im[i] += costMoved;
        }
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
void filter_index_dpreprocessing(array2d_t<double> cost, array2d_t<double> db, void *preprocessing_data)
{
    PData *pdata = (PData *)preprocessing_data;
    destory2d(pdata->permutation);
    destory2d(pdata->flow);
    delete pdata;
}
