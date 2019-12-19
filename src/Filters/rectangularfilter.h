#ifndef RECTANGULARFILTER_H
#define RECTANGULARFILTER_H

#include "filter.h"

class RectangularFilter : public Filter
{
protected:
    virtual void compute_filter();

public:
    RectangularFilter(unsigned int num_samples_, unsigned int sample_rate_, std::vector<double> frequencies_);
};

#endif // RECTANGULARFILTER_H
