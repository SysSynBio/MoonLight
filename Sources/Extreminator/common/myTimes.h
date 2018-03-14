//
// Filename     : myTimes.h
// Description  : Defining namespace for timing functions
// Author(s)    : Henrik Jonsson (henrik@thep.lu.se)
// Created      : 
// Revision     : $Id: myTimes.h 474 2010-08-30 15:23:20Z henrik $
//
#ifndef MYTIMES_H
#define MYTIMES_H

#include "common.h"

#include <stdio.h>
#ifdef WINDOWS
#include <windows.h>
#else
#include <sys/times.h>
#endif
#include <unistd.h>

///
/// @brief Namespace with functions for measuring time aspects
///
namespace myTimes {
  
  long clocktick();
  
  ///
  /// @brief Returns user time since start.
  ///
  double getTime(void);
  
  ///
  /// @brief Returns system time since start
  ///
  double getSysTime(void);
  
  ///
  /// @brief Prints result from calling getTime() and getSysTime()
  ///
  void showTime(void);
  
  ///
  /// @brief Returns user time since last call to getDiffTime()
  ///
  double getDiffTime(void);
  
}
#endif
