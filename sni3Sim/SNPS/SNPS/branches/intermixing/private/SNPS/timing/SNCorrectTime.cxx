#include <SNPS/timing/SNCorrectTime.h>

#include <phys-services/I3RandomService.h>

void SNCorrectTime::CorrectTimeEventHeader(I3EventHeaderPtr header, double tdt, double dt)
{
    header->SetStartTime(header->GetStartTime() + tdt);
    header->SetEndTime(header->GetStartTime() + dt);

}

void SNCorrectTime::CorrectTimePrimaryDaugther(I3MCTreePtr tree, I3MCTree::sibling_iterator primit, double t)
{
    primit->SetTime(t);

    log_debug("Time Primary after correction [s] %e", primit->GetTime() / I3Units::s );
    if (t > primit->GetTime() ) log_fatal("Time calculations are off!");

    I3MCTree::sibling_iterator dit;
    for ( dit = tree->begin(primit); dit != tree->end(primit); dit++)
    {
        log_debug("Daughter Time before correction [s] %e", dit->GetTime() / I3Units::s);
        dit->SetTime(t);
        log_debug("Daughter Time after correction [s] %e", dit->GetTime() / I3Units::s);
    }
}

void SNCorrectTime::CorrectTimeMCTree( I3MCTreePtr tree, I3MCTree::sibling_iterator primit, double t_, double dt_, I3RandomServicePtr rng )
{
    double timeinbin = rng->Uniform(0., dt_);
    log_debug("Time in Bin [s] %e", timeinbin / I3Units::s);
    double t = t_ + timeinbin;

    CorrectTimePrimaryDaugther(tree, primit, t);

}

void SNCorrectTime::CorrectTimeMCTree( I3MCTreePtr tree, I3MCTree::sibling_iterator primit, double dt_, I3RandomServicePtr rng )
{
    double timeinbin = rng->Uniform(0., dt_);
    log_debug("Time in Bin [s] %e", timeinbin / I3Units::s);

    CorrectTimePrimaryDaugther(tree, primit, timeinbin);

}

void SNCorrectTime::CorrectTimeMCTree( I3MCTreePtr tree, double dt_, I3RandomServicePtr rng )
{
    I3MCTree::sibling_iterator primit;
    for ( primit = tree->begin(); primit != tree->end(); primit++ )
    {
        double timeinbin = rng->Uniform(0., dt_);
        log_debug("Time in Bin [s] %e", timeinbin / I3Units::s);
        CorrectTimePrimaryDaugther(tree, primit, timeinbin);
    }
}

void SNCorrectTime::CorrectTimeMCTree( I3MCTreePtr tree, double dt_, double t_, I3RandomServicePtr rng )
{
    I3MCTree::sibling_iterator primit;
    for ( primit = tree->begin(); primit != tree->end(); primit++ )
    {
        double timeinbin = rng->Uniform(0., dt_);
        log_debug("Time in Bin [s] %e", timeinbin / I3Units::s);
        double t = timeinbin + t_;
        CorrectTimePrimaryDaugther(tree, primit, t);
    }
}

void SNCorrectTime::CorrectTimeCORSIKA( I3MCTreePtr tree, I3MCPESeriesMapPtr mcpe, double dt_, I3RandomServicePtr rng )
{
    float tprime = 0;
    I3MCTree::iterator treeit;
    for ( treeit = tree->begin(); treeit != tree->end(); treeit++ )
    {
        if ( treeit->IsPrimary() )
        {
            float timeinbin = rng->Uniform(0., dt_);
            log_debug("Time in Bin CORSIKA [s] %e", timeinbin / I3Units::s);
            float tfirsthit = frameobjecthelpers::GetFirstHitTime(mcpe);
            float tprime = timeinbin - tfirsthit;
        }
        treeit->SetTime( treeit->GetTime() + tprime );
    }

    I3MCPESeriesMap::iterator omkey;
    I3MCPESeries::iterator mcpeit;
    for ( omkey = mcpe->begin(); omkey != mcpe->end(); omkey++ )
    {
        CorrectTimeMCPE( tree, omkey );
    }
}

// void SNCorrectTime::CorrectTimeMCPE( I3MCTreePtr tree, I3MCPESeriesMap::iterator omkey )
// {
//     I3MCPESeries::iterator mcpeit;
//     for ( mcpeit = omkey->second.begin(); mcpeit != omkey->second.end(); mcpeit++ )
//     {

//         log_debug("Time Hit before correction [s] %e", mcpeit->time / I3Units::s );
//         I3Particle pepart = frameobjecthelpers::GetParticle(*tree, *mcpeit);
//         I3ParticlePtr primarypart = I3MCTreeUtils::GetPrimary(*tree, pepart);
//         log_debug("Time primary [s] %e", primarypart->GetTime() / I3Units::s );
//         double newhittime = primarypart->GetTime() + mcpeit->time;
//         mcpeit->time = newhittime;
//         log_debug("Time Hit after correction [s] %e", mcpeit->time / I3Units::s );
//     }
// }

void SNCorrectTime::CorrectTimeMCPE( I3MCTreePtr tree, I3MCPESeriesMap::iterator omkey )
{
    I3MCPESeries::iterator mcpeit;
    for ( mcpeit = omkey->second.begin(); mcpeit != omkey->second.end(); mcpeit++ )
    {
        log_debug("Time Hit before correction [s] %e", mcpeit->time / I3Units::s );
        I3Particle pepart = frameobjecthelpers::GetParticle(*tree, *mcpeit);
        log_debug("Time primary [s] %e", pepart.GetTime() / I3Units::s );
        double newhittime = pepart.GetTime() + mcpeit->time;
        mcpeit->time = newhittime;
        log_debug("Time Hit after correction [s] %e", mcpeit->time / I3Units::s );
    }
}

void SNCorrectTime::CorrectTimePhotonSeries( I3MCTreePtr tree, I3PhotonSeriesMap::iterator modulekey )
{
    I3PhotonSeries::iterator phothit;
    for ( phothit = modulekey->second.begin(); phothit != modulekey->second.end(); phothit++)
    {
        uint64_t majid = phothit->GetParticleMajorID();
        int minid = phothit->GetParticleMinorID();
        I3Particle inicephotsource = frameobjecthelpers::FindPart(minid, majid, *tree);

        log_debug("Time Photon Hit before correction [s] %e", phothit->GetTime() / I3Units::s );
        log_debug("Time Photon Source [s] %e", inicephotsource.GetTime() / I3Units::s );
        double newtime = phothit->GetTime() + inicephotsource.GetTime();
        phothit->SetTime(newtime);
        log_debug("Time Photon Hit after correction [s] %e", phothit->GetTime() / I3Units::s );

        log_debug("Start Time Photon Hit before correction [s] %e", phothit->GetStartTime() / I3Units::s );
        log_debug("Time Photon Source [s] %e", inicephotsource.GetTime() / I3Units::s );
        double newstarttime = phothit->GetStartTime() + inicephotsource.GetTime();
        phothit->SetStartTime(newstarttime);
        log_debug("Start Time Photon Hit after correction [s] %e", phothit->GetStartTime() / I3Units::s );

    }
}


void SNCorrectTime::ShiftMCTree(I3MCTreePtr tree, double dt )
{
    I3MCTree::sibling_iterator primit;
    for ( primit = tree->begin(); primit != tree->end(); primit++ )
    {
        primit->SetTime( primit->GetTime() + dt );
        I3MCTree::sibling_iterator dit;
        for ( dit = tree->begin(primit); dit != tree->end(primit); dit++)
        {
            dit->SetTime(dit->GetTime() + dt);
        }
    }
}


void SNCorrectTime::ShiftMCPESeriesMap(I3MCPESeriesMapPtr mcpemap, double dt)
{
    I3MCPESeriesMap::iterator omkey;
    I3MCPESeries::iterator mcpeit;
    for ( omkey = mcpemap->begin(); omkey != mcpemap->end(); omkey++)
    {
        for ( mcpeit = omkey->second.begin(); mcpeit != omkey->second.end(); mcpeit++ )
        {
            mcpeit->time += dt;
        }
    }
}
