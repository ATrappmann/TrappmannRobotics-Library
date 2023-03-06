// NAME: CallWithTimeout.h
//
#ifndef CALLWITHTIMEOUT_H
#define CALLWITHTIMEOUT_H

#include <TrappmannRobotics/Watchdog.h>

#if !defined(TEENSYDUINO)
#define DEFINE_TIMEOUT_ISR

bool callWithWatchdog(void (*func)(void *, void *), void *args, void *results, WatchdogPrescalerValue timeout);
#endif

#endif