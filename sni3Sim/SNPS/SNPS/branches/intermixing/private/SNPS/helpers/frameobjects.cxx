#include <SNPS/helpers/frameobjects.h>


template <class Archive>
void SNWeight::serialize (Archive &ar, unsigned version)
{
    ar &make_nvp("InteractionWeight", InteractionWeight);
    ar &make_nvp("GenerationWeight", GenerationWeight);
    ar &make_nvp("OscillationWeight", OscillationWeight);
    ar &make_nvp("TotalWeight", TotalWeight);
    ar &make_nvp("GenOscWeight", GenOscWeight);
}

I3_SERIALIZABLE(SNWeight);

I3_SERIALIZABLE(SNWeightMap);
