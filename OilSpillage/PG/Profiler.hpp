#pragma once

#include <chrono>
#include <algorithm>
#include <limits>
#include <iostream>
#include <vector>
#include "utils.hpp"
#include "defs.hpp"

// generates a probe with a scoped RAII lifetime if in debug mode
#define DBG_PROBE(x)  UPtr<Profiler::Probe> __generated_profiler_probe__ { nullptr };          \
                      if constexpr ( isDebugging )                                             \
                         __generated_profiler_probe__ = std::make_unique<Profiler::Probe>(#x); \

#define DBG_PROFILE_OUTPUT(x)   if constexpr (isDebugging) Profiler::Probe::outputAll(x);

#define DBG_PROFILE_RESET()     if constexpr (isDebugging) Profiler::Probe::reset();

namespace Profiler {
   using namespace std::chrono;

   using Time    = high_resolution_clock;
   using Counter = Size;

   struct ProbeData {
   #undef max
      U64      runtimeMicrosecsMin   { std::numeric_limits<U64>::max() },
               runtimeMicrosecsMax   {                               0 },
               runtimeMicrosecsTotal {                               0 };
      Counter  callCount             {                               0 };
      String   label                 {                           "N/A" };
   };

   struct ProbeClock {
      ProbeClock( ProbeData *data = { nullptr } );
      ~ProbeClock() noexcept;
   private:
      decltype(Time::now())  start;
      ProbeData             *data;
   };

   struct Probe {
      using DataMap    = HashMap<String, ProbeData>;
      using DataVector = Vector<ProbeData>;
   public:
      Probe( String &&label );
     ~Probe() noexcept;
      Probe(           Probe const  & ) = delete;
      Probe(           Probe       && ) = delete;
      auto &operator=( Probe const  & ) = delete;
      auto &operator=( Probe       && ) = delete;
      static void outputCounters(      std::ostream &output = std::cout );
      static void outputTotalRuntimes( std::ostream &output = std::cout );
      static void outputMinRuntimes(   std::ostream &output = std::cout );
      static void outputMaxRuntimes(   std::ostream &output = std::cout );
      static void outputAvgRuntimes(   std::ostream &output = std::cout );
      static void outputAll(           std::ostream &output = std::cout );
      static void reset();
   private:
      String const    label;
      ProbeClock      timer;
      static DataMap  data;
   };
};
