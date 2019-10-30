/*!*****************************************************************************
 * @file main.cpp
 * @brief main unit/entry point for game
 *
 * @author Christian Kranz
 * 
 * @copyright Copyright 2019 by Christian Kranz
 *            All rights reserved.
 *            None of this file or parts of it may be
 *            copied, redistributed or used in any other way
 *            without written approval of Christian Kranz
 *
 ******************************************************************************/

#include <fastdict.h>


#include <unistd.h>
#include <ios>
#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <cstdlib>
#include <chrono>


class Time
{
public:
    using TimePoint = std::chrono::steady_clock::time_point;
    using TimeType  = std::chrono::nanoseconds;

    /******************************************************************************
     *
     *****************************************************************************/
    static TimeType from_seconds( const size_t s )
    {
        return std::chrono::duration_cast<TimeType>( std::chrono::seconds( s ) );
    }

    /******************************************************************************
     *
     *****************************************************************************/
    static TimeType from_milliseconds( const size_t s )
    {
        return std::chrono::duration_cast<TimeType>( std::chrono::milliseconds( s ) );
    }

    /******************************************************************************
     *
     *****************************************************************************/
    static TimeType from_microseconds( const size_t s )
    {
        return std::chrono::duration_cast<TimeType>( std::chrono::microseconds( s ) );
    }

    /******************************************************************************
     *
     *****************************************************************************/
    static TimePoint get_time()
    {
        return std::chrono::steady_clock::now();
    }

    /******************************************************************************
     *
     *****************************************************************************/
    static TimeType get_time_elapsed( TimePoint start, TimePoint end )
    {
        return std::chrono::duration_cast<TimeType>( end - start );
    }

    /******************************************************************************
     *
     *****************************************************************************/
    static struct timespec get_timespec()
    {
        struct timespec time;

        clock_gettime(CLOCK_MONOTONIC, &time);

        return time;
    }
};


//////////////////////////////////////////////////////////////////////////////
//
// process_mem_usage(double &, double &) - takes two doubles by reference,
// attempts to read the system-dependent data for a process' virtual memory
// size and resident set size, and return the results in KB.
//
// On failure, returns 0.0, 0.0
void process_mem_usage(double& vm_usage, double& resident_set);
void process_mem_usage(double& vm_usage, double& resident_set)
{
   using std::ios_base;
   using std::ifstream;
   using std::string;

   vm_usage     = 0.0;
   resident_set = 0.0;

   // 'file' stat seems to give the most reliable results
   //
   ifstream stat_stream("/proc/self/stat",ios_base::in);

   // dummy vars for leading entries in stat that we don't care about
   //
   string pid, comm, state, ppid, pgrp, session, tty_nr;
   string tpgid, flags, minflt, cminflt, majflt, cmajflt;
   string utime, stime, cutime, cstime, priority, nice;
   string O, itrealvalue, starttime;

   // the two fields we want
   //
   unsigned long vsize;
   long rss;

   stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
               >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
               >> utime >> stime >> cutime >> cstime >> priority >> nice
               >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

   stat_stream.close();

   long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
   vm_usage     = vsize / 1024.0;
   resident_set = rss * page_size_kb;
}




/******************************************************************************
 *
 *****************************************************************************/
bool find_dic( fastdict::FastDict& dic, std::string word, bool verbose )
{
    bool result = false;
    const std::vector<std::string>& words = dic;

    Time::TimePoint before_deliver = Time::get_time();
    std::vector<std::string> results = dic.get_contained_words( word );

    for( std::string w : results )
    {
        if ( verbose )
            std::cout << "found " << w << std::endl;
        if ( w == word )
        {
            result = true;
        }
    }

    if (!result)
    {
        if ( results.size() > 0 )
        {
            result = true;
        }
        else
        {
            std::cout << "found nothing on " << word << std::endl;
        }

    }
    if (verbose)
    {
        Time::TimeType elapsed = Time::get_time_elapsed( before_deliver, Time::get_time() );
        std::cout << "find_dic needed " << elapsed.count() <<  "ns\n";
    }



    return result;
}

/******************************************************************************
 *
 *****************************************************************************/
int main( int, char** )
{


    fastdict::FastDict improved;

    Time::TimePoint before_deliver = Time::get_time();

    improved.load_from_list( "xxl_list_unique_sorted.txt" );
    //improved.load_from_list( env_info, "demo.txt" );
    const std::vector<std::string>& words = improved;

    Time::TimeType elapsed = Time::get_time_elapsed( before_deliver, Time::get_time() );
    std::cout << "load from list needed " << elapsed.count() <<  "ns\n";


    before_deliver = Time::get_time();
    
    for( size_t w_idx = 0; w_idx < words.size(); w_idx++ )
    {
        if ( !find_dic( improved, words[w_idx], false ) )
        {
            std::cout << "\ndidnt found " << words[w_idx] << std::endl;
            break;
        }
        else
        {
            std::cout << "\r" << std::to_string( w_idx+1 ) << " / " << words.size();
        }
    }
    std::cout << std::endl;
    elapsed = Time::get_time_elapsed( before_deliver, Time::get_time() );
    std::cout << "loop needed " << elapsed.count() <<  "ns\n";



    find_dic( improved, "aneuronesa", true );
    find_dic( improved, "suesaures", true );
    find_dic( improved, "suessaures", true );
    find_dic( improved, "ss", true );
    find_dic( improved, "renht", true );
    find_dic( improved, "ren", true );



    
    {
        double vm, rss;
        process_mem_usage(vm, rss);
        std::cout << "VM: " << vm << "; RSS: " << rss << std::endl;
    }

    return 0;
}


