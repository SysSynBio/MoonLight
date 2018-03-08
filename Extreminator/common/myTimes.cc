//
// Filename     : myTimes.cc
// Description  : Defining functions for runtime measurements
// Author(s)    : Henrik Jonsson (henrik@thep.lu.se)
// Created      : 
// Revision     : $Id: myTimes.cc 474 2010-08-30 15:23:20Z henrik $
//

#include"myTimes.h"

long myTimes::clocktick()
{
    #ifdef WINDOWS
        return 1000.0; //static_cast<long>(GetTickCount())/1000.0;
    #else
        static long CLK_TCK = 0;
        if( !CLK_TCK )
            CLK_TCK = sysconf(_SC_CLK_TCK);
        return CLK_TCK;
    #endif
}

double myTimes::getTime(void)
{
#ifdef WINDOWS
    return static_cast<double>(GetTickCount())/1000.0;
#else
  struct tms My_Time_Buffer;
  double ntime;

  times( &My_Time_Buffer );
  ntime = My_Time_Buffer.tms_utime;
  return ntime / clocktick();
#endif
}

double myTimes::getSysTime(void)
{
#ifdef WINDOWS
    return static_cast<double>(GetTickCount())/1000.0;
#else
  struct tms My_Time_Buffer;
  double ntime;

  times( &My_Time_Buffer );
  ntime = My_Time_Buffer.tms_stime;
  return ntime / clocktick();
#endif
}

void myTimes::showTime(void)
{
  printf("******************************\n");
  printf("*  User Time: %8.1f sec.  *\n", getTime());
  printf("*  Sys. Time: %8.1f sec.  *\n", getSysTime());
  printf("******************************\n");
}

double myTimes::getDiffTime(void)
{
  static double otime = 0;
  double dtime, ntime;

  ntime = getTime();
  dtime = ntime - otime;
  otime = ntime;
  return dtime;
}
