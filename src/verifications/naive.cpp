#include "verifications.h"
#include <emds/emds.h>

array1d_t<Record> verification_naive(array1d_t<double> q, double T, array2d_t<double> cost, array2d_t<double> db)
{
    array1d_t<Record> ans = allocate1d<Record>(db.N);
    ans.n = 0;
    for (int i = 0; i < db.n; i++)
    {
        array1d_t<double> p = getRow(db, i);
        double dist = rubner_emd(q, p, cost);
        if (dist <= T)
        {
            Record record;
            record.dist = dist;
            record.id = i;
            at1d(ans, ans.n) = record;
            ans.n++;
        }
        destory1d(p);
    }
    return ans;
}
