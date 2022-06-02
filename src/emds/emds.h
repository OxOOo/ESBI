#ifndef __EMD_EMDS_H__
#define __EMD_EMDS_H__

#include <utils/utils.h>
#include <tuple>
#include <atomic>

double rubner_emd(array1d_t<double> q, array1d_t<double> p, array2d_t<double> cost);

#endif // __EMD_EMDS_H__
