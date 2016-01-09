#include <SNPS/eventselection/SNEventSelectionBaseClass.h>

#include <dataclasses/I3Constants.h>

#include <icetray/I3Logging.h>

#include <sys/stat.h>

#include <numeric>

std::string SNEventSelectionBaseClass::GetGroupName(std::string model)
{

    if ( model == "Garching_8.8_FullShen" ) return "/O_NE_MG_Full_Shen_Equation_of_State_8_8_M_solar";
    if ( model == "Garching_8.8_ReduShen" ) return "/O_NE_MG_Reduced_Shen_Equation_of_State_8_8_M_solar";
    if ( model == "Garching_810_LS" ) return "/Lattimer_and_Swesty_Equation_of_State_8_10_M_solar";
    if ( model == "Garching_810_WH" ) return "/Wollf_and_Hillebrandt_Equation_of_State_8_10_M_solar";
    if ( model == "Lawrence-Livermore_20_H" ) return "/Lawrence_Livermore_Model";
    if ( model == "BlackHole_40_Shen" ) return "/Black_Hole_2009_Shen_Equation_of_State_40_M_solar";
    if ( model == "BlackHole_40_LS" ) return "/Black_Hole_2009_Lattimer_Swesty_Equation_of_State_40_M_solar";

    log_fatal("Select a supported model");

}

std::string SNEventSelectionBaseClass::GetPhysSpec(std::string model)
{

    if ( model == "Garching_8.8_FullShen"
            || model == "Garching_8.8_ReduShen"
            || model == "Garching_810_LS"
            || model == "Garching_810_WH"
            || model == "Lawrence-Livermore_20_H"
            || model == "BlackHole_40_Shen"
            || model == "BlackHole_40_LS")
    {
        return "Garching";
    }
    else
    {
        log_fatal("Selected model is not yet supported");
    }

}

void SNEventSelectionBaseClass::CorrectUnits(std::vector<double> &pbtime_a, std::vector<double> &pbtime_LandmE, std::vector<double> &Luminosity_nuebar, std::vector<double> &Mean_Energy_nuebar)
{
    CorrectTimeUnits(pbtime_a);
    CorrectTimeUnits(pbtime_LandmE);
    CorrectLuminUnits(Luminosity_nuebar);
    CorrectMeanEUnits(Mean_Energy_nuebar);
}

void SNEventSelectionBaseClass::CorrectLuminUnits(std::vector<double> &lumin)
{
    std::vector<double>::iterator vecit;
    for ( vecit = lumin.begin(); vecit != lumin.end(); vecit++)
    {
        (*vecit) *= 1e-7 * I3Units::joule / I3Units::s;
    }
}

void SNEventSelectionBaseClass::CorrectTimeUnits(std::vector<double> &time)
{
    std::vector<double>::iterator vecit;
    for ( vecit = time.begin(); vecit != time.end(); vecit++)
    {
        (*vecit) *= I3Units::s;
    }
}

void SNEventSelectionBaseClass::CorrectMeanEUnits(std::vector<double> &mE)
{
    std::vector<double>::iterator vecit;
    for ( vecit = mE.begin(); vecit != mE.end(); vecit++)
    {
        (*vecit) *= I3Units::MeV;
    }
}

void SNEventSelectionBaseClass::CorrectUnits(InputFileReader::DataModel &d)
{
    CorrectTimeUnits(d.pbtime_LandmE);
    CorrectTimeUnits(d.pbtime_a);
    CorrectMeanEUnits(d.Mean_Energy_nuebar);
    CorrectMeanEUnits(d.Mean_Energy_nue);
    CorrectMeanEUnits(d.Mean_Energy_nux);
    CorrectLuminUnits(d.Luminosity_nuebar);
    CorrectLuminUnits(d.Luminosity_nue);
    CorrectLuminUnits(d.Luminosity_nux);
}

std::pair<double, double> SNEventSelectionBaseClass::GetTimeRange(std::vector<double> pbtime_a, std::vector<double> pbtime_LandmE)
{

    std::pair<double, double> startendtime;

    if ( std::abs( pbtime_a.front() ) > std::abs( pbtime_LandmE.front() ) )
    {
        startendtime.first = pbtime_LandmE.front();
    }
    else
    {
        startendtime.first = pbtime_a.front();
    }

    if ( std::abs( pbtime_a.back() ) > std::abs( pbtime_LandmE.back() ) )
    {
        startendtime.second = pbtime_LandmE.back();
    }
    else
    {
        startendtime.second = pbtime_a.back();
    }

    return startendtime;

}

std::pair<double, double> SNEventSelectionBaseClass::GetTimeRangeEdges(std::vector<double> pbtime_a, std::vector<double> pbtime_LandmE)
{

    std::pair<double, double> startendtime;


    if ( std::abs( pbtime_a.front() ) > std::abs( pbtime_LandmE.front() ) )
    {
        startendtime.first = pbtime_a.front();
    }
    else
    {
        startendtime.first = pbtime_LandmE.front();
    }

    if ( std::abs( pbtime_a.back() ) > std::abs( pbtime_LandmE.back() ) )
    {
        startendtime.second = pbtime_a.back();
    }
    else
    {
        startendtime.second = pbtime_LandmE.back();
    }

    return startendtime;

}

void SNEventSelectionBaseClass::AdjustTimeVector(std::vector<double> &pbtime_a, std::vector<double> &pbtime_LandmE, \
        std::vector<double> &L, std::vector<double> &mE, std::vector<double> &a, std::pair<double, double> &edgestimes)
{
    edgestimes = GetTimeRangeEdges(pbtime_a, pbtime_LandmE);

    if ( pbtime_a.front() == edgestimes.first )
    {
        pbtime_LandmE.insert(pbtime_LandmE.begin(), edgestimes.first);
        L.insert( L.begin(), L.front() );
        mE.insert( mE.begin(), mE.front() );
    }
    else
    {
        pbtime_a.insert( pbtime_a.begin(), edgestimes.first );
        a.insert( a.begin(), a.front() );
    }

    if ( pbtime_a.back() == edgestimes.second )
    {
        pbtime_LandmE.push_back( edgestimes.second );
        L.push_back( L.back() );
        mE.push_back( mE.back() );
    }
    else
    {
        pbtime_a.push_back( edgestimes.second );
        a.push_back( a.back() );
    }
}

bool SNEventSelectionBaseClass::fileNotExists(const std::string &filename)
{
    struct stat buf;
    if (stat(filename.c_str(), &buf) != -1)
    {
        return false;
    }
    return true;
}

void SNEventSelectionBaseClass::ConfigureTimeInterpolation(SNPSBaseClass::modelintconfiguration &c)
{
    if ( fileNotExists(c.modelfile) ) log_fatal("NO MODEL FILE!");

    std::string modelgroup = GetGroupName(c.model);

    InputFileReader T(c.modelfile);

    InputFileReader::DataModel d = T.openFileandreadModel(modelgroup);

    CorrectUnits(d);
    
    /// TODO: needs to be adjusted for different interactions that do not involve nuebar
    AdjustTimeVector(d.pbtime_a, d.pbtime_LandmE, d.Luminosity_nuebar, d.Mean_Energy_nuebar, d.alpha_nuebar, c.startendtime);

    std::vector<double> dtLandmE;
    std::vector<double> dtalpha;

    std::adjacent_difference( d.pbtime_LandmE.begin(), d.pbtime_LandmE.end(), std::back_inserter(dtLandmE) );
    std::adjacent_difference( d.pbtime_a.begin(), d.pbtime_a.end(), std::back_inserter(dtalpha) );

    if ( dtLandmE.size() >= dtalpha.size() )
    {
        c.itdt = dtLandmE;
    }
    else
    {
        c.itdt = dtalpha;
    }

    log_trace("Start time lumin, mean E= %f", d.pbtime_LandmE.front());
    log_trace("Start time alpha = %f", d.pbtime_LandmE.front());
    
    c.Lnue_cspline_ptr = boost::shared_ptr<gsl_linear_wrapper> ( new gsl_linear_wrapper( d.pbtime_LandmE, d.Luminosity_nue ) );
    c.Lnuebar_cspline_ptr = boost::shared_ptr<gsl_linear_wrapper> ( new gsl_linear_wrapper( d.pbtime_LandmE, d.Luminosity_nuebar) );
    c.Lnux_cspline_ptr = boost::shared_ptr<gsl_linear_wrapper> ( new gsl_linear_wrapper( d.pbtime_LandmE, d.Luminosity_nux ) );
    c.mEnue_cspline_ptr = boost::shared_ptr<gsl_linear_wrapper> ( new gsl_linear_wrapper( d.pbtime_LandmE, d.Mean_Energy_nue ) );
    c.mEnuebar_cspline_ptr = boost::shared_ptr<gsl_linear_wrapper> ( new gsl_linear_wrapper( d.pbtime_LandmE, d.Mean_Energy_nuebar ) );
    c.mEnux_cspline_ptr = boost::shared_ptr<gsl_linear_wrapper> ( new gsl_linear_wrapper( d.pbtime_LandmE, d.Mean_Energy_nux ) );
    c.anue_cspline_ptr = boost::shared_ptr<gsl_linear_wrapper> ( new gsl_linear_wrapper( d.pbtime_a, d.alpha_nue ) );
    c.anuebar_cspline_ptr = boost::shared_ptr<gsl_linear_wrapper> ( new gsl_linear_wrapper( d.pbtime_a, d.alpha_nuebar ) );
    c.anux_cspline_ptr = boost::shared_ptr<gsl_linear_wrapper> ( new gsl_linear_wrapper( d.pbtime_a, d.alpha_nux ) );

}
