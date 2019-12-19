#include "rectangularfilter.h"


RectangularFilter::RectangularFilter(unsigned int num_samples_, unsigned int sample_rate_, std::vector<double> frequencies_)
                    : Filter(num_samples_, sample_rate_, frequencies_)
{

}


void RectangularFilter::compute_filter()
{
    unsigned long part = 0;
    unsigned long len = frequencies.size();

    for(unsigned long i = 0; i < num_samples; ++i)
    {
        double f = i * sample_rate / ((num_samples - 1)*2); //check this computation

        if(f < (frequencies[0] + frequencies[1])/2)
            filter[i] = filter_factors[0];
        else if(f > (frequencies[len-1] + frequencies[len-2])/2)
            filter[i] = filter_factors[len - 1];
        else if(part > 0 && part < (len-1) && f > (frequencies[part] + frequencies[part-1])/2 && f < (frequencies[part]+frequencies[part+1])/2)
            filter[i] = filter_factors[part];
        else {
            part++; i--;
        }
    }

}
