#include "triangularfilter.h"

TriangularFilter::TriangularFilter(unsigned int num_samples_, unsigned int sample_rate_, std::vector<double> frequencies_)
    : Filter(num_samples_, sample_rate_, frequencies_)
{

}


void TriangularFilter::compute_filter()
{
    unsigned long part = 0;
    unsigned long len = frequencies.size();

    for(unsigned long i = 0; i < num_samples; ++i)
    {
        double f = i * sample_rate / ((num_samples - 1)*2); //check this computation

        if(f < frequencies[0])
            filter[i] = 1 + (filter_factors[0]-1)*f/frequencies[0];
        else if(f > (frequencies[len-1]))
        {
            double n = (f - frequencies[len-1]) / (sample_rate/2 - frequencies[len-1]);
            filter[i] = 1 * n +  filter_factors[len-1] * (1-n);
        }
        else if(part > 0 && part < (len) && f >= frequencies[part-1] && f <= frequencies[part])
        {
            double n = (f - frequencies[part-1]) / (frequencies[part] - frequencies[part-1]);
            filter[i] = filter_factors[part] * n +  filter_factors[part-1] * (1-n);
        }
        else {
            part++; i--;
        }
    }
}
