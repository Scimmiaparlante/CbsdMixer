#ifndef TRIANGULARFILTER_H
#define TRIANGULARFILTER_H

#include "filter.h"

class TriangularFilter : public Filter
{
protected:
        virtual void compute_filter();

public:
    TriangularFilter(unsigned int num_elem_, double resolution_, std::vector<double> frequencies_);
};

#endif // TRIANGULARFILTER_H
