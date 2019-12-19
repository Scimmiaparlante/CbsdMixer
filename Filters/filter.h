#ifndef FILTER_H
#define FILTER_H

#include <vector>
#include <fftw3.h>

class Filter
{
protected:
    unsigned int num_samples;
    unsigned int sample_rate;
    std::vector<double> frequencies;

    std::vector<double> filter_factors;

    double* filter;

    virtual void compute_filter() = 0;

public:
    Filter(unsigned int num_samples_, unsigned int sample_rate_, std::vector<double> frequencies_);
    virtual ~Filter();

    virtual void apply(fftw_complex* buf) = 0;
    virtual int set_filterValue(int n_filter, double value);
};

#endif // FILTER_H
