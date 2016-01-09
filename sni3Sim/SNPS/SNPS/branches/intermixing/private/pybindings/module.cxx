#include <icetray/I3FrameObject.h>
#include <icetray/load_project.h>

#include <boost/preprocessor.hpp>

#define REGISTER_THESE_THINGS                     \
(Interaction)(SNGenerationWeight)(SNPSFileFilter) \
(SNGenerationWeightHelpers)(framemerger)

#define I3_REGISTRATION_FN_DECL(r, data, t) void BOOST_PP_CAT(register_,t)();
#define I3_REGISTER(r, data, t) BOOST_PP_CAT(register_,t)();
BOOST_PP_SEQ_FOR_EACH(I3_REGISTRATION_FN_DECL, ~, REGISTER_THESE_THINGS)


I3_PYTHON_MODULE(SNPS)
{
  load_project("libSNPS", false);
  BOOST_PP_SEQ_FOR_EACH(I3_REGISTER, ~, REGISTER_THESE_THINGS);
}