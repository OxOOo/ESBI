#ifndef __EMD_VERIFICATIONS_H__
#define __EMD_VERIFICATIONS_H__

#include <utils/utils.h>

struct Record
{
    int id;
    double dist;
};

struct verification_t
{
    array1d_t<Record> (*run)(array1d_t<double> q, double T, array2d_t<double> cost, array2d_t<double> db);
};

verification_t loadVerification(const string &verification_name);

// naive: rubner
array1d_t<Record> verification_naive(array1d_t<double> q, double T, array2d_t<double> cost, array2d_t<double> db);

// singleflow: Single-source Flow Adjustment
array1d_t<Record> verification_singleflow(array1d_t<double> q, double T, array2d_t<double> cost, array2d_t<double> db);

// multiflow: Multi-source Flow Adjustment
array1d_t<Record> verification_multiflow(array1d_t<double> q, double T, array2d_t<double> cost, array2d_t<double> db);

#endif // __EMD_VERIFICATIONS_H__
