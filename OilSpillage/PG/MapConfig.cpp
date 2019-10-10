#include "MapConfig.hpp"

Str mapConfigToFilename( MapConfig const &config, Str extension ) {
   std::ostringstream filename;
   filename << "map_"
            <<  config.seed
            << '_'
            <<  config.dimensions.x
            << 'x'
            <<  config.dimensions.y
            << '_'
            <<  config.districtCellSide
            << (config.isUsingManhattanDistance? 'M':'E')
            << '_'
            <<  config.roadDepthMax
            << '_'
            <<  config.roadStepSize
            << '_'
            <<  config.roadMinTilesBeforeTurn
            << '_'
            <<  config.roadMinTilesBeforeBranch
            << '_'
            <<  config.roadMinTilesBeforeTurn
            << '_'
            <<  config.roadLengthMin
            << '-'
            <<  config.roadLengthMax
            << 'x'
            <<  config.roadLengthFactor
            << '_'
            <<  config.roadTurnProbability
            << 'x'
            <<  config.roadTurnProbabilityFactor
            << '_'
            <<  config.roadBranchProbability
            << 'x'
            <<  config.roadBranchProbabilityFactor
            <<  extension;
   return filename.str();
}
