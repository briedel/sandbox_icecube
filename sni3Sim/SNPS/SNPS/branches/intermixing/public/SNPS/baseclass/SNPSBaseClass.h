#ifndef SNPSBASECLASS_H_INCLUDED
#define SNPSBASECLASS_H_INCLUDED

#include <SNPS/helpers/frameobjects.h>
#include <SNPS/helpers/gsl_interp_wrapper.h>
#include <SNPS/HDFreaderwriter/inputfilereader.h>
#include <SNPS/helpers/frameobjecthelpers.h>

#include <dataclasses/physics/I3MCTree.h>
#include <dataclasses/physics/I3MCTreeUtils.h>
#include <dataclasses/physics/I3Particle.h>
#include <dataclasses/I3TreeUtils.h>
#include <dataclasses/physics/I3Particle.h>
#include <dataclasses/I3Direction.h>
#include <dataclasses/external/tree.hh>
#include <dataclasses/physics/I3EventHeader.h>

#include <simclasses/I3MCPE.h>

#include <vector>
#include <string>

/**
 * General base class
 * That provides structs for different parts of SNPS
 */

class SNPSBaseClass
{
public:

    SNPSBaseClass() {};

    virtual ~SNPSBaseClass() {};


    /**
     * Struct that holds information 
     * for generating weights for 
     * given time bin
     */
    struct genweigtinfo
    {
        double Enu; // Neutrino energy
        double N_SN; // Number of expected neutrinos
        double mEnu; // Mean neutrino energy
        double alpha; // Pinch parameter
        double N_gen; // Number of generated neutrinos
        std::string physicsspectrum; // Spectrum type due to model
        std::string inputspectrum; // Generation spectrum 
        std::string interaction; // Type of interaction
        double gammaalpha; // Generation spetrum parameter
        double gammameanE;
        double maxE; // Maximum energy
        int noFramesNeeded; // No of generation frames needed
    } gwi;
    
    /**
     * Struct that holds new frame objects
     * when needed
     */
    struct newframeobjects
    {
        
        I3MCTreePtr newmctree;
        I3EventHeaderPtr neweventheader;
        I3MCPESeriesMapPtr newmchitseriesmap;
        // I3PhotonSeriesMapPtr newphotonseries;
        SNWeightMapPtr newweightmap;
        
    } nfo;


    /**
     * Struct that holds information about model
     */
    struct modelintconfiguration
    {

        std::string model;
        std::string modelfile;
        std::vector<double> itdt; // Bin size of model
        std::pair<double, double> startendtime; // Start and end time of model
        // Splines of model quanities
        boost::shared_ptr<gsl_linear_wrapper> Lnue_cspline_ptr;
        boost::shared_ptr<gsl_linear_wrapper> Lnuebar_cspline_ptr;
        boost::shared_ptr<gsl_linear_wrapper> Lnux_cspline_ptr;
        boost::shared_ptr<gsl_linear_wrapper> mEnue_cspline_ptr;
        boost::shared_ptr<gsl_linear_wrapper> mEnuebar_cspline_ptr;
        boost::shared_ptr<gsl_linear_wrapper> mEnux_cspline_ptr;
        boost::shared_ptr<gsl_linear_wrapper> anue_cspline_ptr;
        boost::shared_ptr<gsl_linear_wrapper> anuebar_cspline_ptr;
        boost::shared_ptr<gsl_linear_wrapper> anux_cspline_ptr;

    } miconfig;


};

#endif //SNWEIGHTINGBASECLASS_H_INCLUDED

