#ifndef COSINEFILTER_H
#define COSINEFILTER_H

#include "filter.h"

class CosineFilter : public Filter
{
protected:
    virtual void compute_filter();

public:
    CosineFilter(unsigned int num_samples_, unsigned int sample_rate_, std::vector<double> frequencies_);
};

#endif // COSINEFILTER_H
