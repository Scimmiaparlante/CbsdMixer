#include "cosinefilter.h"
#include <cmath>

CosineFilter::CosineFilter(unsigned int num_samples_, unsigned int sample_rate_, std::vector<double> frequencies_)
    : Filter(num_samples_, sample_rate_, frequencies_)
{

}


void CosineFilter::compute_filter()
{
    unsigned long part = 0;
    unsigned long len = frequencies.size();

    for(unsigned long i = 0; i < num_samples; ++i)
    {
        double f = i * sample_rate / ((num_samples - 1)*2); //check this computation

        if(f < frequencies[0])
        {
            double n = f / frequencies[0];
            double n2 = (1 - cos(n * M_PI)) / 2;
            filter[i] = filter_factors[0] * n2 + 1 * (1-n2);
        }
        else if(f > (frequencies[len-1]))
        {
            double n = (f - frequencies[len-1]) / (sample_rate/2 - frequencies[len-1]);
            double n2 = (1 - cos(n * M_PI)) / 2;
            filter[i] = 1 * n2 + filter_factors[len-1] * (1-n2);
        }
        else if(part > 0 && part < (len) && f >= frequencies[part-1] && f <= frequencies[part])
        {
            double n = (f - frequencies[part-1]) / (frequencies[part] - frequencies[part-1]);
            double n2 = (1 - cos(n * M_PI)) / 2;
            filter[i] = filter_factors[part] * n2 + filter_factors[part-1] * (1-n2);
        }
        else {
            part++; i--;
        }
    }
}
