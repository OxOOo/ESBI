#include <utils/utils.h>
#include <emds/emds.h>
#include <filters/filters.h>
#include <verifications/verifications.h>
#include <iostream>
#include <cstring>

using namespace std;

int main()
{
    filter_t filter = loadFilter("naive");             // can be naive, dim, index, batch
    verification_t verify = loadVerification("naive"); // can be naive, singleflow, multiflow

    array2d_t<double> cost, db;
    tie(cost, db) = loadDB("../data/example");

    void *filter_data = filter.preprocessing(cost, db);

    auto queries = loadQueries("../data/example_queries", cost.n);
    for (int i = 0; i < (int)queries.size(); i++)
    {
        cout << "============ running query " << i << endl;
        auto query = queries[i];

        array1d_t<int> candidates = filter.run(query.q, query.T, cost, db, filter_data);
        cout << "total " << candidates.n << " candidates" << endl;

        array2d_t<double> vdb = allocate2d<double>(db.N, db.M);
        vdb.n = candidates.n;
        vdb.m = db.m;

        for (int k = 0; k < candidates.n; k++)
        {
            int idx = at1d(candidates, k);
            memcpy(&at2d(vdb, k, 0), &at2d(db, idx, 0), sizeof(double) * db.m);
        }

        array1d_t<Record> results = verify.run(query.q, query.T, cost, vdb);
        cout << "total " << results.n << " results" << endl;
        for (int k = 0; k < results.n; k++)
        {
            const auto &r = at1d(results, k);
            cout << "\t"
                 << "distance to data " << at1d(candidates, r.id) << " is " << r.dist << endl;
        }

        destory1d(candidates);
        destory2d(vdb);
    }

    filter.dpreprocessing(cost, db, filter_data);

    return 0;
}
