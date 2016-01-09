#include <SNPS/HDFreaderwriter/inputfilereader.h>

template<typename T>
std::vector<T> InputFileReader::readTableBlock(hid_t tableLoc, const std::string &tableName)
{
    hsize_t nFields, nRecords;
    std::vector<T> data;
    T dummyT;
    size_t fieldsizes[T::nFields];
    for (int i = 0; i < T::nFields; i++ ) fieldsizes[i] = dummyT.getSizes()[i];
    H5TBget_table_info( tableLoc, tableName.c_str(), &nFields, &nRecords);
    //TODO: check for errors

    assert( nFields >= T::nFields );
    const unsigned int maxBlockSize = 1 << 20; //1 MB
    unsigned int recordsPerBlock = maxBlockSize / sizeof(T);
    if ( !recordsPerBlock ) recordsPerBlock = 1;

    // empty data array
    std::vector<T> intermediate(recordsPerBlock);

    T *dataPtr = &intermediate[0];

    hsize_t recordsRead = 0;

    while ( recordsRead < nRecords )
    {

        hsize_t toRead = recordsPerBlock;
        if ( recordsRead + toRead > nRecords ) toRead = nRecords - recordsRead;
        intermediate.resize( toRead );
        assert( &intermediate[0] == dataPtr );
        H5TBread_fields_name( tableLoc, tableName.c_str(), T::getFields(), recordsRead, toRead, sizeof(T), T::getOffsets(), fieldsizes, dataPtr );
        data.insert( data.end(), intermediate.begin(), intermediate.end());
        recordsRead += toRead;

    }

    return data;

}


InputFileReader::DataModel InputFileReader::openFileandreadModel(std::string model)
{

    hid_t file_id, group_id;

    DataModel d;

    file_id = H5Fopen(filename.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);

    #if (H5_VERS_MAJOR >= 1)&&(H5_VERS_MINOR >= 8)
        group_id = H5Gopen(file_id, model.c_str(), H5P_DEFAULT);
    #else
        group_id = H5Gopen(file_id, model.c_str());
    #endif


    std::vector<LandmE> lumins = readTableBlock<LandmE>(group_id, "LandmE");

    std::vector<PinchParameter> alphas = readTableBlock<PinchParameter>(group_id, "Pinch_Parameter");

    for ( std::vector<LandmE>::iterator it = lumins.begin(); it != lumins.end(); it++ )
    {

        d.Luminosity_nue.push_back( it->Luminosity_nue );
        d.Luminosity_nuebar.push_back( it->Luminosity_nuebar );
        d.Luminosity_nux.push_back( it->Luminosity_nux );
        d.Mean_Energy_nue.push_back( it->Mean_Energy_nue );
        d.Mean_Energy_nuebar.push_back( it->Mean_Energy_nuebar );
        d.Mean_Energy_nux.push_back( it->Mean_Energy_nux );
        d.pbtime_LandmE.push_back( it->Post_Bounce_Time );

    }

    for ( std::vector<PinchParameter>::iterator it = alphas.begin(); it != alphas.end(); it++ )
    {

        d.alpha_nue.push_back( it->alpha_nue );
        d.alpha_nuebar.push_back( it->alpha_nuebar );
        d.alpha_nux.push_back( it->alpha_nux );
        d.pbtime_a.push_back( it->Post_Bounce_Time );

    }

    H5Gclose(group_id);

    return d;

}


InputFileReader::DataShock InputFileReader::openFileandreadShock(std::string shocktype)
{

    hid_t file_id, group_id;

    DataShock d;

    file_id = H5Fopen(filename.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);

    #if (H5_VERS_MAJOR >= 1)&&(H5_VERS_MINOR >= 8)
        group_id = H5Gopen(file_id, "Shock_Types", H5P_DEFAULT);
    #else
        group_id = H5Gopen(file_id, "Shock_Types");
    #endif


    std::vector<ShockTypes> shock = readTableBlock<ShockTypes>(group_id, shocktype.c_str());

    for ( std::vector<ShockTypes>::iterator it = shock.begin(); it != shock.end(); it++ )
    {

        d.rho_outer.push_back( it->Density_Outer );
        d.rho_inner.push_back( it->Density_Inner );
        d.y_e_outer.push_back( it->Y_e_Outer );
        d.y_e_inner.push_back( it->Y_e_Inner );
        d.pbtime_rho_y_e.push_back( it->Post_Bounce_Time );

    }

    H5Gclose(group_id);

    H5Fclose(file_id);

    return d;

}


// int main()
// {

//     std::string build_path = std::getenv("I3_BUILD");

//     InputFileReader T(build_path + "/SNPS/resources/supernovaspectra.h5");

//     InputFileReader::DataModel d;

//     InputFileReader::DataShock ds;

//     d = T.openFileandreadModel("/8.8_M_solar_O-NE-MG_Full_Shen_Equation_of_State");

//     ds = T.openFileandreadShock("Reverse");

//     return 0;

// }