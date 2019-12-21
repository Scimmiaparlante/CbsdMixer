#ifndef FILTER_H
#define FILTER_H

#include <vector>
#include <fftw3.h>

class Filter
{
protected:
    unsigned int num_elem;
    double resolution;
    std::vector<double> frequencies;

    std::vector<double> filter_factors;

    double* filter;

    virtual void compute_filter() = 0;

public:
    Filter(unsigned int num_elem_, double resolution_, std::vector<double> frequencies_);
    virtual ~Filter();

    virtual void apply(fftw_complex* dest_buf, fftw_complex* source_buf, double extra_factor);
    virtual int set_filterValue(int n_filter, double value);
    virtual double* get_filter() { return filter; }
};

#endif // FILTER_H
