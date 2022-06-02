#include "verifications.h"
#include <iostream>

using namespace std;

void *verification_preprocessing_empty(array2d_t<double>, array2d_t<double>)
{
    return NULL;
}

void verification_dpreprocessing_empty(array2d_t<double>, array2d_t<double>, void *)
{
    // NOTHING
}

verification_t loadVerification(const string &verification_name)
{
    cout << "using verification " << verification_name << endl;

    verification_t verification;

    if (verification_name == "naive")
    {
        verification.run = verification_naive;
    }
    else if (verification_name == "singleflow")
    {
        verification.run = verification_singleflow;
    }
    else if (verification_name == "multiflow")
    {
        verification.run = verification_multiflow;
    }
    else
    {
        cerr << "Unknown verification : " << verification_name << endl;
        exit(1);
    }
    return verification;
}
