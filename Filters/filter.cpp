#include "filter.h"

Filter::Filter(unsigned int num_samples_, unsigned int sample_rate_, std::vector<double> frequencies_)
{
    num_samples = num_samples_;
    sample_rate = sample_rate_;
    frequencies = frequencies_;

    for(unsigned long i = 0; i < frequencies.size(); ++i)
        filter_factors.push_back(1);

    //allocate and initialize filter
    filter = new double[num_samples];
    for(unsigned int i = 0; i < num_samples; ++i)
        filter[i] = 1;
}

Filter::~Filter()
{
    delete[] filter;
}


void Filter::apply(fftw_complex* dest_buf, fftw_complex* source_buf, double extra_factor)
{
    for(unsigned int i = 0; i < num_samples; ++i)
    {
        dest_buf[i][0] = source_buf[i][0] * filter[i] * extra_factor;
        dest_buf[i][1] = source_buf[i][1] * filter[i] * extra_factor;
    }
}

int Filter::set_filterValue(int n_filter, double value)
{
    unsigned long len = frequencies.size();
    if (n_filter >= static_cast<int>(len))
        return -1;

    filter_factors[static_cast<unsigned long>(n_filter)] = value;

    //recompute the filter's values
    compute_filter();

    return 0;
}
