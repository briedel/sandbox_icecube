//#define GRANULAR_GEOMETRY_SUPPORT
// #include <clsim/I3Photon.h>
#include <SNPS/HDFreaderwriter/I3FileBooking/converter/SNPSConverter.h>

#include <tableio/I3Converter.h>
#include <tableio/converter/I3VectorConverter.h>

#include <string>

I3TableRowDescriptionPtr SNPSConverter::CreateDescription(const SNPSHitRecord &r)
{
    I3TableRowDescriptionPtr desc(new I3TableRowDescription());

    desc->AddField<int>                         ("String", "", "String Number");
    desc->AddField<int>                         ("DOM", "", "DOM Number on String");

    desc->AddField<uint64_t>                    ("Driving_Time", "Nanoseconds since start of year", "Time reference for frame");
    
    MAKE_ENUM_VECTOR(type, I3Particle, I3Particle::ParticleType, I3PARTICLE_H_I3Particle_ParticleType);

    desc->AddEnumField<I3Particle::ParticleType>("Primary_Type", type, "Primary Particle Type", "");
    desc->AddField<double>                      ("Energy_Neutrino", "GeV", "Energy of neutrino");
    desc->AddField<double>                      ("Primary_Time", "Nanoseconds since Driving_Time", "Time primary started");
    desc->AddField<double>                      ("Zenith_Primary", "", "Zenith of momentum vector of primary");
    desc->AddField<uint64_t>                    ("Particle_Major_ID_Primary", "", "Particle Major ID");
    desc->AddField<int>                         ("Particle_Minor_ID_Primary", "", "Particle Minor ID");
    desc->AddField<double>                      ("InteractionWeight", "", "Interaction Weight");
    desc->AddField<double>                      ("GenerationWeight", "", "Generation Weight");
    desc->AddField<double>                      ("OscillationWeight", "", "OscillationWeight");

    MAKE_ENUM_VECTOR(type1, I3Particle, I3Particle::ParticleType, I3PARTICLE_H_I3Particle_ParticleType);

    desc->AddEnumField<I3Particle::ParticleType>("Product_Type", type1, "Product Particle Type", "");
    desc->AddField<double>                      ("Energy_Product", "GeV", "Energy of in product");
    desc->AddField<double>                      ("Product_Time", "Nanoseconds since Driving_Time", "Time product started");
    desc->AddField<double>                      ("X_Particle_Position", "meters", "X starting position of in ice particle");
    desc->AddField<double>                      ("Y_Particle_Position", "meters", "Y starting position of in ice particle");
    desc->AddField<double>                      ("Z_Particle_Position", "meters", "Z starting position of in ice particle");
    desc->AddField<double>                      ("Zenith", "", "Zenith of momentum vector");
    desc->AddField<double>                      ("Azimuth", "", "Azimuth of momentum vector");
    desc->AddField<uint64_t>                    ("Particle_Major_ID", "", "Particle Major ID");
    desc->AddField<int>                         ("Particle_Minor_ID", "", "Particle Minor ID");

    desc->AddField<double>                      ("Hit_Time", "Nanoseconds since Driving_Time", "Time when DOM was Hit");

    // desc->AddField<double>  ("hit_x", "meters", "photon position on the DOM surface (x coordinate)");
    // desc->AddField<double>  ("hit_y", "meters", "photon position on the DOM surface (y coordinate)");
    // desc->AddField<double>  ("hit_z", "meters", "photon position on the DOM surface (z coordinate)");
    // desc->AddField<double>  ("zen", "radian", "zenith angle of the photon direction vector");
    // desc->AddField<double>  ("azi", "radian", "azimuthal angle of the photon direction vector");
    // desc->AddField<double>  ("start_time", "ns", "time of photon emission");
    // desc->AddField<double>  ("hit_time", "ns", "time of photon hits the DOM");
    // desc->AddField<double>  ("X_Photon_Position", "meters", "photon emission position (x coordinate)");
    // desc->AddField<double>  ("Y_Photon_Position", "meters", "photon emission position (y coordinate)");
    // desc->AddField<double>  ("Z_Photon_Position", "meters", "photon emission position (z coordinate)");
    // desc->AddField<double>  ("start_zen", "radian", "zenith angle of the photon direction vector at emission");
    // desc->AddField<double>  ("start_azi", "radian", "azimuthal angle of the photon direction vector at emission");
    // desc->AddField<uint32_t>("num_scattered", "", "number of times this photon has been scattered");
    return desc;
}

size_t SNPSConverter::FillRows(const SNPSHitRecord &r, I3TableRowPtr rows)
{

    rows->Set<int>                     ("String", r.k.GetString());
    rows->Set<int>                     ("DOM", r.k.GetOM());

    rows->Set<uint64_t>                ("Driving_Time", r.dtime.GetUTCDaqTime());

    rows->Set<I3Particle::ParticleType>("Primary_Type", r.prim.GetType());
    rows->Set<double>                  ("Energy_Neutrino", r.prim.GetEnergy());
    rows->Set<double>                  ("Primary_Time", r.prim.GetTime());
    rows->Set<double>                  ("Zenith_Primary", r.prim.GetDir().GetZenith() / I3Units::rad);
    rows->Set<uint64_t>                ("Particle_Major_ID_Primary", r.prim.GetMajorID());
    rows->Set<int>                     ("Particle_Minor_ID_Primary", r.prim.GetMinorID());
    rows->Set<double>                  ("InteractionWeight", r.w.InteractionWeight);
    rows->Set<double>                  ("GenerationWeight", r.w.GenerationWeight);
    rows->Set<double>                  ("OscillationWeight", r.w.OscillationWeight);

    rows->Set<I3Particle::ParticleType>("Product_Type", r.part.GetType());
    rows->Set<double>                  ("Energy_Product", r.part.GetEnergy() / I3Units::GeV);
    rows->Set<double>                  ("Product_Time", r.part.GetTime());
    rows->Set<double>                  ("X_Particle_Position", r.part.GetPos().GetX() / I3Units::m);
    rows->Set<double>                  ("Y_Particle_Position", r.part.GetPos().GetY() / I3Units::m);
    rows->Set<double>                  ("Z_Particle_Position", r.part.GetPos().GetZ() / I3Units::m);
    rows->Set<double>                  ("Zenith", r.part.GetDir().GetZenith() / I3Units::rad);
    rows->Set<double>                  ("Azimuth", r.part.GetDir().GetAzimuth() / I3Units::rad);
    rows->Set<uint64_t>                ("Particle_Major_ID", r.part.GetMajorID());
    rows->Set<int>                     ("Particle_Minor_ID", r.part.GetMinorID());

    rows->Set<double>                  ("Hit_Time", r.mcpe.time);

    // rows->Set<double>  ("hit_x", r.phot.GetPos().GetX() / I3Units::m);
    // rows->Set<double>  ("hit_y", r.phot.GetPos().GetY() / I3Units::m);
    // rows->Set<double>  ("hit_z", r.phot.GetPos().GetZ() / I3Units::m);
    // rows->Set<double>  ("zen", r.phot.GetDir().GetZenith() / I3Units::rad);
    // rows->Set<double>  ("azi", r.phot.GetDir().GetAzimuth() / I3Units::rad);
    // rows->Set<double>  ("start_time", r.phot.GetStartTime() / I3Units::ns);
    // rows->Set<double>  ("hit_time", r.phot.GetTime() / I3Units::ns);
    // rows->Set<double>  ("X_Photon_Position", r.phot.GetStartPos().GetX() / I3Units::m);
    // rows->Set<double>  ("Y_Photon_Position", r.phot.GetStartPos().GetY() / I3Units::m);
    // rows->Set<double>  ("Z_Photon_Position", r.phot.GetStartPos().GetZ() / I3Units::m);
    // rows->Set<double>  ("start_zen", r.phot.GetStartDir().GetZenith() / I3Units::rad);
    // rows->Set<double>  ("start_azi", r.phot.GetStartDir().GetAzimuth() / I3Units::rad);
    // rows->Set<uint32_t>("num_scattered", r.phot.GetNumScattered());
    return (1);
}

// I3_CONVERTER(SNPSConverter, SNPSHitRecord);
// I3_CONVERTER(I3VectorConverter<SNPSConverter>, I3Vector<SNPSHitRecord>);
