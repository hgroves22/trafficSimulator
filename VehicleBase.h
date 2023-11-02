#ifndef __VEHICLE_BASE_H__
#define __VEHICLE_BASE_H__

#include <vector>

using namespace std;

// enum: see http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-enum
enum class Direction   {north, south, east, west};
enum class VehicleType {car, suv, truck};
enum class LightColor  {green, yellow, red};
enum class WillTurn {yes, no};
enum class Finished {yes, no};

class VehicleBase
{
   public:
      static int vehicleCount;

   private:
      int         vehicleID;
      VehicleType vehicleType;
      Direction   vehicleDirection;
      WillTurn    vehicleTurn;
      Finished    vehicleFinished;

   public:
      VehicleBase(VehicleType type, Direction originalDirection, WillTurn turn, Finished finished);
      VehicleBase(const VehicleBase& other);
      ~VehicleBase();
      VehicleBase(VehicleBase&& other)noexcept;
      VehicleBase& operator=(const VehicleBase& other);
      VehicleBase& operator=(VehicleBase&& other)noexcept;


      inline int getVehicleID() const { return this->vehicleID; }

      inline VehicleType getVehicleType() const { return this->vehicleType; }
      inline Direction   getVehicleOriginalDirection() const { return this->vehicleDirection; }

      bool getTurn();
      void changeTurn();
      int getSize();
      bool isFinished();
      void nowFinished();
      vector<VehicleBase*> moveStraight(vector<VehicleBase*>, Direction, LightColor, LightColor, int);
      vector<VehicleBase*> moveTurn(vector<VehicleBase*>, Direction, LightColor, LightColor, int);
};

#endif
