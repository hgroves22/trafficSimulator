#ifndef __VEHICLE_BASE_CPP__
#define __VEHICLE_BASE_CPP__

#include "VehicleBase.h"
#include <iostream>
#include <vector>



int VehicleBase::vehicleCount = 0;

VehicleBase::VehicleBase(VehicleType type, Direction direction, WillTurn turn, Finished finished)
    : vehicleID(VehicleBase::vehicleCount++), 
      vehicleType(type),
      vehicleDirection(direction),
      vehicleTurn(turn),
      vehicleFinished(finished)
{}

VehicleBase::VehicleBase(const VehicleBase& other)
    : vehicleID(other.vehicleID),
      vehicleType(other.vehicleType),
      vehicleDirection(other.vehicleDirection),
      vehicleTurn(other.vehicleTurn),
      vehicleFinished(other.vehicleFinished)
{}

VehicleBase::~VehicleBase() {}

VehicleBase& VehicleBase::operator=(const VehicleBase& other) {
    if (this == &other) {
        return *this;
    }
    vehicleID = other.vehicleID;
    vehicleType = other.vehicleType;
    vehicleDirection = other.vehicleDirection;
    vehicleTurn = other.vehicleTurn;
    vehicleFinished = other.vehicleFinished;
    return *this;
}

VehicleBase::VehicleBase(VehicleBase&& other)noexcept {
    vehicleID = other.vehicleID;
    vehicleType = other.vehicleType;
    vehicleDirection = other.vehicleDirection;
    vehicleTurn = other.vehicleTurn;
    vehicleFinished = other.vehicleFinished;

    other.vehicleID = 0;
    other.vehicleType = VehicleType::car; 
    other.vehicleDirection = Direction::north;
    other.vehicleTurn = WillTurn::no; 
    other.vehicleFinished = Finished::no;
}

VehicleBase& VehicleBase::operator=(VehicleBase&& other)noexcept {
    if(this == &other) {
        return *this;
    }
    vehicleID = move(other.vehicleID);
    vehicleType = move(other.vehicleType);
    vehicleDirection = move(other.vehicleDirection);
    vehicleTurn = move(other.vehicleTurn);
    vehicleFinished = move(other.vehicleFinished);

    other.vehicleID = 0;
    other.vehicleType = VehicleType::car;
    other.vehicleDirection = Direction::north;
    other.vehicleTurn = WillTurn::no;
    other.vehicleFinished = Finished::no;

    return *this;
}

void VehicleBase::changeTurn() {
  if(vehicleTurn == WillTurn::no) {
    vehicleTurn = WillTurn::yes;
  }
  else {
    vehicleTurn = WillTurn::no;
  }
}

bool VehicleBase::getTurn() {
  if(vehicleTurn == WillTurn::yes) {
    return true;
  }
  else {
    return false;
  }
}

int VehicleBase::getSize() {
  if(vehicleType == VehicleType::car) {
    return 2;
  }
  else if(vehicleType == VehicleType::suv) {
    return 3;
  }
  else {
    return 4;
  }
}
bool VehicleBase::isFinished() {
  if(vehicleFinished == Finished::yes) {
    return true;
  }
  else {
    return false;
  }
}

void VehicleBase::nowFinished() {
  vehicleFinished = Finished::yes;
}

vector<VehicleBase*> VehicleBase::moveTurn(vector<VehicleBase*> directionBound, Direction direction, LightColor EW, LightColor NS, int yellowTime) {
  int size = directionBound.size();

  if(this->getSize() + 1 > yellowTime and 
  (((EW == LightColor::yellow) and (direction == Direction::north or direction == Direction::south)) or
  ((NS == LightColor::yellow) and (direction == Direction::east or direction == Direction::west))) and
  (directionBound[size/2] != this)) {
    return directionBound;
  }

  if((((EW == LightColor::green) and (direction == Direction::north or direction == Direction::south)) or
  ((NS == LightColor::green) and (direction == Direction::east or direction == Direction::west))) or
  (directionBound[size/2] == this)) {
    if(this->getSize() == 4) {
      if(directionBound[size/2] != this) {
        directionBound[size/2] = this;
      }
      else if(directionBound[size/2 + 1] != this) {
        directionBound[size/2] = this;
        directionBound[size/2 + 1] = this;
      }
      else if(directionBound[size/2 + 2] != this) {
        directionBound[size/2] = this;
        directionBound[size/2 + 1] = this;
        directionBound[size/2 + 2] = this;
      }
      else {
        directionBound[size/2] = this;
        directionBound[size/2 + 1] = this;
        directionBound[size/2 + 2] = this;
        directionBound[size/2 + 3] = this;
      }
    }
    else if(this->getSize() == 3) {
      if(directionBound[size/2] != this) {
        directionBound[size/2] = this;
      }
      else if(directionBound[size/2 + 1] != this) {
        directionBound[size/2] = this;
        directionBound[size/2 + 1] = this;
      }
      else {
        directionBound[size/2] = this;
        directionBound[size/2 + 1] = this;
        directionBound[size/2 + 2] = this;
      }
    }
    else if(this->getSize() == 2) {
      if(directionBound[size/2] != this) {
        directionBound[size/2] = this;
      }
      else {
        directionBound[size/2] = this;
        directionBound[size/2 + 1] = this;
      }
    }
  }
  return directionBound;
}

vector<VehicleBase*> VehicleBase::moveStraight(vector<VehicleBase*> directionBound, Direction direction, LightColor EW, LightColor NS, int yellowTime) {
  int size = directionBound.size();
  bool done = false;

  for(int i = 0; i < size; i++) {
    if (directionBound[i] == this) {
      done = true;
      directionBound[i] = nullptr;

      if(this->getSize() == 4) { //movement for the trucks
        if(i == 0) {
          if(directionBound[i+this->getSize()-3] != this) {
            if(directionBound[i+this->getSize()-3] != nullptr) {
              directionBound[i] = this;
            }
            else {
              directionBound[i] = this;
              directionBound[i+1] = this;
            }
          }
          else if(directionBound[i+this->getSize()-2] != this) {
            if(directionBound[i+this->getSize()-2] != nullptr) {
              directionBound[i] = this;
              directionBound[i+1] = this;
            }
            else {
              directionBound[i] = this;
              directionBound[i+1] = this;
              directionBound[i+2] = this;
            }
          }
          else if(directionBound[i+this->getSize()-1] != this) {
            if(directionBound[i+this->getSize()-1] != nullptr) {
              directionBound[i] = this;
              directionBound[i+1] = this;
              directionBound[i+2] = this;
              directionBound[i+3] = this;

            }
            else {
              directionBound[i] = this;
              directionBound[i+1] = this;
              directionBound[i+2] = this;
              directionBound[i+3] = this;
            }
          }
          else {
            if(directionBound[i+4] != nullptr) { //vehicle in front
              directionBound[i+3] = this;
              directionBound[i+2] = this;
              directionBound[i+1] = this;
              directionBound[i] = this;
            }
            else { //no vehicle in front
              directionBound[i+4] = this;
              directionBound[i+3] = this;
              directionBound[i+2] = this;
              directionBound[i+1] = this;
            }
          }
        }
        else if(directionBound[i+4] != nullptr and i < directionBound.size() - 4) {
          directionBound[i+3] = this;
          directionBound[i+2] = this;
          directionBound[i+1] = this;
          directionBound[i] = this;
        }
        else if ((i == directionBound.size()/2 - 5) and 
        (((EW == LightColor::red) and (direction == Direction::east or direction == Direction::west)) or
        ((NS == LightColor::red) and (direction == Direction::north or direction == Direction::south)))) {
          directionBound[i+3] = this;
          directionBound[i+2] = this;
          directionBound[i+1] = this;
          directionBound[i] = this;
        }
        else if((i == directionBound.size()/2 - 5) and
        (((EW == LightColor::yellow) and (direction == Direction::east or direction == Direction::west)) or
        ((NS == LightColor::yellow) and (direction == Direction::north or direction == Direction::south))) and
        (this->getSize() + 2 > yellowTime)) {
          directionBound[i+3] = this;
          directionBound[i+2] = this;
          directionBound[i+1] = this;
          directionBound[i] = this;
        }
        else if((i >= directionBound.size()/2 - 5) and (i < directionBound.size()/2 -1) and
        (((EW == LightColor::green or EW == LightColor::yellow) and (direction == Direction::east or direction == Direction::west)) or
        ((NS == LightColor::green or NS == LightColor::yellow) and (direction == Direction::north or direction == Direction::south))) and
        (this->getTurn() == true)) {
          if(i == directionBound.size()/2 - 5) {
            if(this->getSize() + 1 > yellowTime and 
            (((EW == LightColor::yellow) and (direction == Direction::east or direction == Direction::west)) or
            ((NS == LightColor::yellow) and (direction == Direction::north or direction == Direction::south)))) {
              directionBound[i+3] = this;
              directionBound[i+2] = this;
              directionBound[i+1] = this;
              directionBound[i] = this;
            }
            else {
              directionBound[i+3] = this;
              directionBound[i+2] = this;
              directionBound[i+1] = this;
            }
          }
          else if(i == directionBound.size()/2 - 4) {
            directionBound[i+2] = this;
            directionBound[i+1] = this;
          }
          else if(i == directionBound.size()/2 - 3) {
            directionBound[i+1] = this;
          }
          else {
            continue;
          }
        }

        else if(i >= directionBound.size() - 4) {
          if(i == directionBound.size() - 4) {
            directionBound[i+3] = this;
            directionBound[i+2] = this;
            directionBound[i+1] = this;
          }
          else if(i == directionBound.size() - 3) {
            directionBound[i+2] = this;
            directionBound[i+1] = this;
          }
          else if(i == directionBound.size() - 2) {
            directionBound[i+1] = this;
          }
          else {
            vehicleFinished = Finished::yes;
            continue;
          }
        }

        else {
          if(directionBound[i+3] != this) {
            directionBound[i] = this;
            directionBound[i+1] = this;
            directionBound[i+2] = this;
            directionBound[i+3] = this;
          }
          else {
            directionBound[i+4] = this;
            directionBound[i+3] = this;
            directionBound[i+2] = this;
            directionBound[i+1] = this;
          }
        }
      }

      else if(this->getSize() == 3) { //this is when the vehicle is an suv
        if(i == 0) {
          if(directionBound[i+this->getSize()-2] != this and directionBound[i+this->getSize()-2] == nullptr) { //when suv is only one part in the lane and there is no vehicle in front
            directionBound[i] = this;
            directionBound[i+1] = this;
          }
          else if(directionBound[i+this->getSize()-1] != this and directionBound[i+this->getSize()-1] == nullptr) { //when suv is only two parts in the lane and there is no vehicle in front
            directionBound[i] = this;
            directionBound[i+1] = this;
            directionBound[i+2] = this;
          }
          else if(directionBound[i+this->getSize()-2] != this and directionBound[i+this->getSize()-2] != nullptr) { //when suv is only one part in the lane and there is a vehicle in front
            directionBound[i] = this;
          }
          else if(directionBound[i+this->getSize()-1] != this and directionBound[i+this->getSize()-1] != nullptr) { //when suv is only two parts in the lane and there is a vehicle in front
            directionBound[i] = this;
            directionBound[i+1] = this;
          }
          else { //when the suv is all three parts in the lane
            if(directionBound[i+3] != nullptr) { //vehicle in front
              directionBound[i+2] = this;
              directionBound[i+1] = this;
              directionBound[i] = this;
            }
            else { //no vehicle in front
              directionBound[i+3] = this;
              directionBound[i+2] = this;
              directionBound[i+1] = this;
            }
          }
        }

        else if (directionBound[i+3] != nullptr and i < directionBound.size() - 3) {
            directionBound[i+2] = this;
            directionBound[i+1] = this;
            directionBound[i] = this;
        }

        //when the light is red 
        else if ((i == directionBound.size()/2 - 4) and 
        (((EW == LightColor::red) and (direction == Direction::east or direction == Direction::west)) or
        ((NS == LightColor::red) and (direction == Direction::north or direction == Direction::south)))) {
          directionBound[i+2] = this;
          directionBound[i+1] = this;
          directionBound[i] = this;
        }

        else if((i == directionBound.size()/2 - 4) and
        (((EW == LightColor::yellow) and (direction == Direction::east or direction == Direction::west)) or
        ((NS == LightColor::yellow) and (direction == Direction::north or direction == Direction::south))) and
        (this->getSize() + 2 > yellowTime)) {
          directionBound[i+2] = this;
          directionBound[i+1] = this;
          directionBound[i] = this;
        }

        else if((i >= directionBound.size()/2 - 4) and (i < directionBound.size()/2 - 1) and
        (((EW == LightColor::green or EW == LightColor::yellow) and (direction == Direction::east or direction == Direction::west)) or
        ((NS == LightColor::green or NS == LightColor::yellow) and (direction == Direction::north or direction == Direction::south))) and
        (this->getTurn() == true)) {
          if(i == directionBound.size()/2 - 4) {
            if(this->getSize() + 1 > yellowTime and 
            (((EW == LightColor::yellow) and (direction == Direction::east or direction == Direction::west)) or
            ((NS == LightColor::yellow) and (direction == Direction::north or direction == Direction::south)))) {
              directionBound[i+2] = this;
              directionBound[i+1] = this;
              directionBound[i] = this;
            }
            else {
              directionBound[i+2] = this;
              directionBound[i+1] = this;
            }
          }
          else if(i == directionBound.size()/2 - 3) {
            directionBound[i+1] = this;
          }
          else {
            continue;
          }
        }

        else if(i >= directionBound.size() - 3) {
          if(i == directionBound.size() - 3) {
            directionBound[i+2] = this;
            directionBound[i+1] = this;
          }
          else if(i == directionBound.size() - 2) {
            directionBound[i+1] = this;
          }
          else {
            vehicleFinished = Finished::yes;
            continue;
          }
        }
        else {
          if(directionBound[i+2] != this) {
            directionBound[i] = this;
            directionBound[i+1] = this;
            directionBound[i+2] = this;
          }
          else {
            directionBound[i+3] = this;
            directionBound[i+2] = this;
            directionBound[i+1] = this;
          }
        }
      }

      //cars
      else if(this->getSize() == 2) {
        if(i == 0) {
          if(directionBound[i+this->getSize()-1] != this and directionBound[i+this->getSize()-1] == nullptr) {
            directionBound[i+1] = this;
            directionBound[i] = this;
          }
          else if(directionBound[i+this->getSize()-1] != this and directionBound[i+this->getSize()-1] != nullptr) {
            directionBound[i] = this;
          }
          else {
            if(directionBound[i+2] != nullptr) { //vehicle in front
              directionBound[i+1] = this;
              directionBound[i] = this;
            }
            else { //no vehicle in front
              directionBound[i+2] = this;
              directionBound[i+1] = this;
            }
          }
        }
        else if(directionBound[i+2] != nullptr and i < directionBound.size() - 2) {
          directionBound[i+1] = this;
          directionBound[i] = this;
        }

        //red light
        else if ((i == directionBound.size()/2 - 3) and 
        (((EW == LightColor::red) and (direction == Direction::east or direction == Direction::west)) or
        ((NS == LightColor::red) and (direction == Direction::north or direction == Direction::south)))) {
          directionBound[i+1] = this;
          directionBound[i] = this;
        }

        else if((i == directionBound.size()/2 - 3) and
        (((EW == LightColor::yellow) and (direction == Direction::east or direction == Direction::west)) or
        ((NS == LightColor::yellow) and (direction == Direction::north or direction == Direction::south))) and
        (this->getSize() + 2 > yellowTime)) {
          directionBound[i+1] = this;
          directionBound[i] = this;
        }

        else if((i >= directionBound.size()/2 - 3) and (i < directionBound.size()/2 - 1) and
        (((EW == LightColor::green or EW == LightColor::yellow) and (direction == Direction::east or direction == Direction::west)) or
        ((NS == LightColor::green or NS == LightColor::yellow) and (direction == Direction::north or direction == Direction::south))) and
        (this->getTurn() == true)) {
          if(i == directionBound.size()/2 - 3) {
            if(this->getSize() + 1 > yellowTime and 
            (((EW == LightColor::yellow) and (direction == Direction::east or direction == Direction::west)) or
            ((NS == LightColor::yellow) and (direction == Direction::north or direction == Direction::south)))) {
              directionBound[i+1] = this;
              directionBound[i] = this;
            }
            else {
              directionBound[i+1] = this;
            }
          }
          else {
            continue;
          }
        }
        else if(i >= directionBound.size() - 2) {
          if(i == directionBound.size() - 2) {
            directionBound[i+1] = this;
          }
          else {
            vehicleFinished = Finished::yes;
            continue;
          }
        }
        else {
          if(directionBound[i+1] != this) {
            directionBound[i] = this;
            directionBound[i+1] = this;
          }
          else {
            directionBound[i+2] = this;
            directionBound[i+1] = this;
          }
        }
      }
      break;
    }
  }
  if(this->isFinished() == true) { //checks if the vehicle has already gone through
    return directionBound;
  }

  if((directionBound[0] == nullptr) and (done == false)) {
    directionBound[0] = this;
  }

  return directionBound;
}

#endif
