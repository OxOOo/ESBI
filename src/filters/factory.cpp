#include "filters.h"
#include <iostream>

using namespace std;

void *filter_preprocessing_empty(array2d_t<double>, array2d_t<double>)
{
    return NULL;
}

void filter_preprocessing_empty(array2d_t<double>, array2d_t<double>, void *)
{
    // nothing
}

filter_t loadFilter(const string &filter_name)
{
    cout << "using filter " << filter_name << endl;

    filter_t filter;

    if (filter_name == "naive")
    {
        filter.preprocessing = filter_preprocessing_empty;
        filter.run = filter_naiveIM;
        filter.dpreprocessing = filter_preprocessing_empty;
    }
    else if (filter_name == "dim")
    {
        filter.preprocessing = filter_preprocessing_empty;
        filter.run = filter_dim;
        filter.dpreprocessing = filter_preprocessing_empty;
    }
    else if (filter_name == "index")
    {
        filter.preprocessing = filter_index_preprocessing;
        filter.run = filter_index;
        filter.dpreprocessing = filter_index_dpreprocessing;
    }
    else if (filter_name == "batch")
    {
        filter.preprocessing = filter_batch_preprocessing;
        filter.run = filter_batch;
        filter.dpreprocessing = filter_batch_dpreprocessing;
    }
    else
    {
        cerr << "Unknown filter : " << filter_name << endl;
        exit(1);
    }
    return filter;
}
