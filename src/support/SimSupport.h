// Units, constants and small types shared by the simulation.  Everything
// here is unit conversion or a plain value type; the model code itself
// lives in src/sim.

#pragma once

#include <cmath>
#include <cstdio>
#include <string>

#include "Vector.h"
#include "Date.h"
#include "SimClock.h"
#include "Log.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define DEGtoRAD(x) ((x) * M_PI / 180.0)
#define RADtoDEG(x) ((x) * 180.0 / M_PI)

#define FEETtoMETERS(v) ((v) * 0.3048)
#define METERStoFEET(v) ((v) * 3.28084)

namespace sim {

// Geodetic position (WGS-84): latitude and longitude in radians, altitude
// in meters above the ellipsoid.
struct GeodeticPosition {
    double m_latitude_rad = 0.0;
    double m_longitude_rad = 0.0;
    double m_altitude_meters = 0.0;
};

}  // namespace sim
