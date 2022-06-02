#include "utils.h"
#include <iostream>
#include <cassert>
#include <cstdio>
#include <vector>
#include <cstdio>
#include <cmath>
#include <queue>
#include <cstdlib>

double getTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

uint32_t uint32Rand()
{
    return ((uint32_t)(rand() & 0xFF)) | ((uint32_t)(rand() & 0xFF) << 8) | ((uint32_t)(rand() & 0xFF) << 16) | ((uint32_t)(rand() & 0xFF) << 24);
}

tuple<array2d_t<double>, array2d_t<double>> loadDB(const string &dbpath)
{
    cout << "loadDB from " << dbpath << endl;

    // load cost
    int N;
    array2d_t<double> cost;
    FILE *cfd = fopen((dbpath + string("_cost")).c_str(), "r");
    assert(cfd);
    assert(fscanf(cfd, "%d", &N) == 1);
    cost = allocate2d<double>(N, N);
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            assert(fscanf(cfd, "%lf", &at2d(cost, i, j)) == 1);
        }
    }
    cost.n = N;
    cost.m = N;
    fclose(cfd);

    // load db
    FILE *dfd = fopen((dbpath + string("_features")).c_str(), "r");
    assert(dfd);
    char *filename = new char[1024];
    int line_num = 0;
    while (true)
    {
        if (fscanf(dfd, "%s", filename) == EOF)
            break;
        for (int i = 0; i < N; i++)
        {
            double x;
            assert(fscanf(dfd, "%lf", &x) == 1);
        }
        line_num++;
    }

    fseek(dfd, 0, SEEK_SET);
    array2d_t<double> db = allocate2d<double>(line_num, N);
    db.n = line_num;
    db.m = N;
    for (int x = 0; x < line_num; x++)
    {
        if (fscanf(dfd, "%s", filename) == EOF)
            break;
        for (int i = 0; i < N; i++)
        {
            assert(fscanf(dfd, "%lf", &at2d(db, x, i)) == 1);
        }
    }

    delete[] filename;
    fclose(dfd);

    cout << "db " << dbpath << " has total " << db.n << " records, each " << db.m << " dims" << endl;
    return make_tuple(cost, db);
}

vector<Query> loadQueries(const string &queries_file, int dims)
{
    cout << "load queries from " << queries_file << endl;

    vector<Query> ans;

    FILE *fd = fopen(queries_file.c_str(), "r");
    assert(fd);

    while (true)
    {
        Query query;
        if (fscanf(fd, "%lf", &query.T) == EOF)
            break;
        query.q = allocate1d<double>(dims);
        query.q.n = dims;
        for (int i = 0; i < dims; i++)
        {
            assert(fscanf(fd, "%lf", &at1d(query.q, i)) == 1);
        }
        ans.push_back(query);
    }

    fclose(fd);

    return ans;
}
