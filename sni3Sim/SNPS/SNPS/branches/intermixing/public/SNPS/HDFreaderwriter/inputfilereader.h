#ifndef INPUTFILEREADER_H_INCLUDED
#define INPUTFILEREADER_H_INCLUDED

#include <hdf5.h>

#if (H5_VERS_MAJOR >= 1)&&(H5_VERS_MINOR >= 8)
    #include <hdf5_hl.h>
#else
    #include <H5TA.h>
#endif

#include <H5Dpublic.h>
#include <H5Gpublic.h>
#include <H5Fpublic.h>
#include <H5Tpublic.h>

#include <vector>
#include <string>
#include <iostream>

/**
 * Class to read in HDF5 file with model information
 */

class InputFileReader
{

public:

    InputFileReader(std::string fname)
    {

        filename = fname;

    };

    virtual ~InputFileReader() {};


    /**
     * Struct to describe the table for the 
     * Luminsoity and mean energy table
     */
    struct LandmE
    {

        enum {nFields = 7};

        double Luminosity_nue;
        double Luminosity_nuebar;
        double Luminosity_nux;
        double Mean_Energy_nue;
        double Mean_Energy_nuebar;
        double Mean_Energy_nux;
        double Post_Bounce_Time;


        static const size_t( &getOffsets() )[nFields]
        {

            const static size_t offsets[nFields] = {

                HOFFSET(LandmE, Luminosity_nue),
                HOFFSET(LandmE, Luminosity_nuebar),
                HOFFSET(LandmE, Luminosity_nux),
                HOFFSET(LandmE, Mean_Energy_nue),
                HOFFSET(LandmE, Mean_Energy_nuebar),
                HOFFSET(LandmE, Mean_Energy_nux),
                HOFFSET(LandmE, Post_Bounce_Time)

            };

            return (offsets);

        }

        const size_t( &getSizes() )[nFields]
        {

            const static size_t sizes[nFields] = {

                sizeof(Luminosity_nue),
                sizeof(Luminosity_nuebar),
                sizeof(Luminosity_nux),
                sizeof(Mean_Energy_nue),
                sizeof(Mean_Energy_nuebar),
                sizeof(Mean_Energy_nux),
                sizeof(Post_Bounce_Time)

            };

            return (sizes);

        }

        static const char *getFields()
        {
            return ("Luminosity_nue,Luminosity_nuebar,Luminosity_nux,Mean_Energy_nue,Mean_Energy_nuebar,Mean_Energy_nux,Post_Bounce_Time");
        }

    };

    /**
     * Struct to describe the table for the 
     * pinch parameter
     */
    struct PinchParameter
    {

        enum {nFields = 4};

        double Post_Bounce_Time;
        double alpha_nue;
        double alpha_nuebar;
        double alpha_nux;

        static const size_t(&getOffsets())[nFields]
        {

            const static size_t offsets[nFields] = {

                HOFFSET(PinchParameter, Post_Bounce_Time),
                HOFFSET(PinchParameter, alpha_nue),
                HOFFSET(PinchParameter, alpha_nuebar),
                HOFFSET(PinchParameter, alpha_nux),

            };

            return (offsets);

        }

        const size_t(&getSizes())[nFields]
        {

            const static size_t sizes[nFields] = {

                sizeof(Post_Bounce_Time),
                sizeof(alpha_nue),
                sizeof(alpha_nuebar),
                sizeof(alpha_nux),

            };

            return (sizes);

        }

        static const char *getFields()
        {
            return ("Post_Bounce_Time,alpha_nue,alpha_nuebar,alpha_nux");
        }

    };

    /**
     * Struct to describe the table for the 
     * shock types
     * Currently not needed
     */
    struct ShockTypes
    {

        enum {nFields = 5};

        double Density_Inner;
        double Density_Outer;
        double Post_Bounce_Time;
        double Y_e_Inner;
        double Y_e_Outer;

        static const size_t(&getOffsets())[nFields]
        {

            const static size_t offsets[nFields] = {

                HOFFSET(ShockTypes, Density_Inner),
                HOFFSET(ShockTypes, Density_Outer),
                HOFFSET(ShockTypes, Post_Bounce_Time),
                HOFFSET(ShockTypes, Y_e_Inner),
                HOFFSET(ShockTypes, Y_e_Outer),


            };

            return (offsets);

        }

        const size_t(&getSizes())[nFields]
        {

            const static size_t sizes[nFields] = {

                sizeof(Density_Inner),
                sizeof(Density_Outer),
                sizeof(Post_Bounce_Time),
                sizeof(Y_e_Inner),
                sizeof(Y_e_Outer),


            };

            return (sizes);

        }

        static const char *getFields()
        {
            return ("Density_Inner,Density_Outer,Post_Bounce_Time,Y_e_Inner,Y_e_Outer");
        }

    };

    /**
     * Struct to hold the model data
     */
    struct DataModel
    {

        std::vector<double> Luminosity_nue;
        std::vector<double> Luminosity_nuebar;
        std::vector<double> Luminosity_nux;
        std::vector<double> Mean_Energy_nue;
        std::vector<double> Mean_Energy_nuebar;
        std::vector<double> Mean_Energy_nux;
        std::vector<double> pbtime_LandmE;
        std::vector<double> alpha_nue;
        std::vector<double> alpha_nuebar;
        std::vector<double> alpha_nux;
        std::vector<double> pbtime_a;

    };

    /**
     * Struct to hold the shock data
     */
    struct DataShock
    {

        std::vector<double> rho_outer;
        std::vector<double> rho_inner;
        std::vector<double> y_e_outer;
        std::vector<double> y_e_inner;
        std::vector<double> pbtime_rho_y_e;

    };

    std::string filename;


    /**
     * Reading the in the table block
     * 
     * @params file id
     * @params table name
     * @return data in table
     */
    template<typename T>
    std::vector<T> readTableBlock(hid_t, const std::string &);

    /**
     * Reading in model data
     */
    DataModel openFileandreadModel(std::string);

    /**
     * Reading in shock data
     */
    DataShock openFileandreadShock(std::string);

private:

};

#endif //INPUTFILEREADER_H_INCLUDED