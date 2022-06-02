#ifndef __EMD_FILTERS_H__
#define __EMD_FILTERS_H__

#include <utils/utils.h>

// Input: database & query
// Output: candidate
struct filter_t
{
    void *(*preprocessing)(array2d_t<double> cost, array2d_t<double> db);
    array1d_t<int> (*run)(array1d_t<double> q, double T, array2d_t<double> cost, array2d_t<double> db, void *preprocessing_data);
    void (*dpreprocessing)(array2d_t<double> cost, array2d_t<double> db, void *preprocessing_data);
};

filter_t loadFilter(const string &filter_name);

// naive: naive IM-Sig
array1d_t<int> filter_naiveIM(array1d_t<double> q, double T, array2d_t<double> cost, array2d_t<double> db, void *);

// dim: Dimension-aware Candidate Generation
array1d_t<int> filter_dim(array1d_t<double> q, double T, array2d_t<double> cost, array2d_t<double> db, void *);

// index: Index-based Filtering
void *filter_index_preprocessing(array2d_t<double> cost, array2d_t<double> db);
array1d_t<int> filter_index(array1d_t<double> q, double T, array2d_t<double> cost, array2d_t<double> db, void *preprocessing_data);
void filter_index_dpreprocessing(array2d_t<double> cost, array2d_t<double> db, void *preprocessing_data);

// batch: Progressive Batch Pruning
void *filter_batch_preprocessing(array2d_t<double> cost, array2d_t<double> db);
array1d_t<int> filter_batch(array1d_t<double> q, double T, array2d_t<double> cost, array2d_t<double> db, void *preprocessing_data);
void filter_batch_dpreprocessing(array2d_t<double> cost, array2d_t<double> db, void *preprocessing_data);

#endif // __EMD_FILTERS_H__
