#include "Profiler.hpp" 
#include <cassert>
#include <iomanip>

// TODO: replace static data with ProfilerContext?

namespace Profiler {

   static constexpr auto labelWidth { 32U };

   ProbeClock::ProbeClock( ProbeData *data ):
      start ( Time::now() ),
      data  ( data )
   {}

   ProbeClock::~ProbeClock() noexcept {
      assert( data and "Bug!" );

      auto  end     = Time::now();
      auto  usTime  = std::chrono::duration_cast<microseconds>(end - start);

      data->runtimeMicrosecsMin    = util::minValue( data->runtimeMicrosecsMin, static_cast<U64>(usTime.count()) );
      data->runtimeMicrosecsMax    = util::maxValue( data->runtimeMicrosecsMax, static_cast<U64>(usTime.count()) );
      data->runtimeMicrosecsTotal += static_cast<U64>(usTime.count());
   }

   Probe::Probe( String &&label ):
      label ( label        ),
      timer ( &data[label] )
   {
      auto &entry = data[label];
      entry.label = label;
      ++( entry.callCount );
   }

   Probe::~Probe() noexcept {}

   void  Probe::outputCounters( std::ostream &output ) {
      DataVector  sortedByTotalRunCount {};
      sortedByTotalRunCount.reserve( data.size() );
      for ( auto const &[_,data] : data )
         sortedByTotalRunCount.emplace_back( data );

      auto predicate = []( auto const &lhs, auto const &rhs ) {
                          return lhs.callCount < rhs.callCount;
                       };
      std::sort( sortedByTotalRunCount.begin(), sortedByTotalRunCount.end(), predicate );

      output << "\nTOTAL CALLS:\n"
             << "\trank\t" << std::setw(labelWidth) << std::left << "label" << "\ttotal #calls\n";
      for ( auto i=0u;  i<sortedByTotalRunCount.size();  ++i ) 
         output << "\t#" << i+1 /*TODO: pad*/
                << "\t"  << std::setw(labelWidth) << std::left << sortedByTotalRunCount[i].label
                << "\t"  << sortedByTotalRunCount[i].callCount << "\n";
   }


   void  Probe::outputTotalRuntimes( std::ostream &output ) {
      DataVector  sortedByTotalRuntime {};
      sortedByTotalRuntime.reserve( data.size() );
      for ( auto const &[_,data] : data )
         sortedByTotalRuntime.emplace_back( data );

      auto predicate = []( auto const &lhs, auto const &rhs ) {
                          return lhs.runtimeMicrosecsTotal < rhs.runtimeMicrosecsTotal;
                       };
      std::sort( sortedByTotalRuntime.begin(), sortedByTotalRuntime.end(), predicate );

      output << "\nTOTAL RUNTIME:\n"
             << "\trank\t" << std::setw(labelWidth) << std::left << "label" << "\ttotal runtime (in microseconds)\n";
      for ( auto i=0u;  i<sortedByTotalRuntime.size();  ++i ) 
         output << "\t#" << i+1 /*TODO: pad*/
                << "\t"  << std::setw(labelWidth) << std::left << sortedByTotalRuntime[i].label
                << "\t"  << sortedByTotalRuntime[i].runtimeMicrosecsTotal << "\n";
   }


   void  Probe::outputMinRuntimes( std::ostream &output ) {
      DataVector  sortedByMinRuntime {};
      sortedByMinRuntime.reserve( data.size() );
      for ( auto const &[_,data] : data )
         sortedByMinRuntime.emplace_back( data );

      auto predicate = []( auto const &lhs, auto const &rhs ) {
                          return lhs.runtimeMicrosecsMin < rhs.runtimeMicrosecsMin;
                       };
      std::sort( sortedByMinRuntime.begin(), sortedByMinRuntime.end(), predicate );

      output << "\nMIN RUNTIME:\n"
             << "\trank\t" << std::setw(labelWidth) << std::left << "label" << "\tmin runtime (in microseconds)\n";
      for ( auto i=0u;  i<sortedByMinRuntime.size();  ++i ) 
         output << "\t#" << i+1 /*TODO: pad*/
                << "\t"  << std::setw(labelWidth) << std::left << sortedByMinRuntime[i].label
                << "\t"  << sortedByMinRuntime[i].runtimeMicrosecsMin << "\n";
   }


   void  Probe::outputMaxRuntimes( std::ostream &output ) {
      DataVector  sortedByMaxRuntime {};
      sortedByMaxRuntime.reserve( data.size() );
      for ( auto const &[_,data] : data )
         sortedByMaxRuntime.emplace_back( data );

      auto predicate = []( auto const &lhs, auto const &rhs ) {
                          return lhs.runtimeMicrosecsMax < rhs.runtimeMicrosecsMax;
                       };
      std::sort( sortedByMaxRuntime.begin(), sortedByMaxRuntime.end(), predicate );

      output << "\nMAX RUNTIME:\n"
             << "\trank\t" << std::setw(labelWidth) << std::left << "label" << "\tmax runtime (in microseconds)\n";
      for ( auto i=0u;  i<sortedByMaxRuntime.size();  ++i ) 
         output << "\t#" << i+1 /*TODO: pad*/
                << "\t"  << std::setw(labelWidth) << std::left << sortedByMaxRuntime[i].label
                << "\t"  << sortedByMaxRuntime[i].runtimeMicrosecsMax << "\n";
   }


   void Probe::outputAvgRuntimes( std::ostream &output ) {
      DataVector  sortedByAverageRuntime {};
      sortedByAverageRuntime.reserve( data.size() );
      for ( auto const &[_,data] : data )
         sortedByAverageRuntime.emplace_back( data );

      auto averageRuntime = []( ProbeData const &e ) {
                               return (static_cast<F32>(e.runtimeMicrosecsTotal) / e.callCount);
                            };

      auto predicate = [&averageRuntime]( auto const &lhs, auto const &rhs ) {
                          return averageRuntime(lhs) < averageRuntime(rhs);
                       };

      std::sort( sortedByAverageRuntime.begin(), sortedByAverageRuntime.end(), predicate );

      output << "\nAVG RUNTIME:\n"
             << "\trank\t" << std::setw(labelWidth) << std::left << "label" << "\taverage runtime (in microseconds)\n";
      for ( auto i=0u;  i<sortedByAverageRuntime.size();  ++i ) {
         auto const &entry = sortedByAverageRuntime[i];
         output << "\t#" << i+1 /*TODO: pad*/
                << "\t" << std::setw(labelWidth) << std::left << entry.label
                << "\t" << averageRuntime(entry) << "\n";
      }
   }

   void Probe::outputAll( std::ostream &output ) {
      outputCounters(      output );
      outputTotalRuntimes( output );
      outputMinRuntimes(   output );
      outputMaxRuntimes(   output );
      outputAvgRuntimes(   output );
   }

   void Probe::reset() {
      data.clear();
   }

   Probe::DataMap  Probe::data {};
};
