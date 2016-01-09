#ifndef INTERP_GEN_H_INCLUDED
#define INTERP_GEN_H_INCLUDED

#include <SNPS/HDFreaderwriter/inputfilereader.h>


/**
 * Class to check interpolation and file reader works
 */

class interp_gen
{

public:

    interp_gen(InputFileReader & T, std::string model, std::string shock)
    {

        d = T.openFileandreadModel(model);

        if ( shock != "Off" ) ds = T.openFileandreadShock(shock);

    };

    virtual ~interp_gen() {};

    std::string model_;

    InputFileReader::DataModel d;

    InputFileReader::DataShock ds;

private:

};

#endif //INTERP_GEN_H_INCLUDED