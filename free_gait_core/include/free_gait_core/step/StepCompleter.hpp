/*
 * StepCompleter.hpp
 *
 *  Created on: Mar 7, 2015
 *      Author: Péter Fankhauser
 *   Institute: ETH Zurich, Autonomous Systems Lab
 */

#pragma once

// Free Gait
#include "free_gait_core/TypeDefs.hpp"
#include "free_gait_core/executor/State.hpp"
#include "free_gait_core/executor/AdapterBase.hpp"
#include "free_gait_core/step/Step.hpp"
#include "free_gait_core/leg_motion/Footstep.hpp"
#include "free_gait_core/base_motion/BaseAuto.hpp"

namespace free_gait {

class StepCompleter
{
 public:
  StepCompleter(std::shared_ptr<free_gait::AdapterBase> adapter);
  virtual ~StepCompleter();
  bool complete(const State& state, Step& step) const;
  bool complete(const State& state, const Step& step, BaseMotionBase& baseMotion) const;
  void setParameters(Footstep& footTarget) const;
  void setParameters(BaseAuto& baseAuto) const;

 protected:

  struct FootstepParameters
  {
    std::string profileType = "triangle";
    Vector surfaceNormal = Vector::UnitZ();
    double profileHeight = 0.06;
    double averageVelocity = 2.0;
  } footTargetParameters_;

  struct BaseAutoParameters
  {
    double height = 0.46;
    double averageLinearVelocity = 0.05;
    double averageAngularVelocity = 0.1;
    double supportMargin = 0.04;
    PlanarStance nominalPlanarStanceInBaseFrame;

    BaseAutoParameters()
    {
      Position2 position;
      position << 0.2, 0.2;
      nominalPlanarStanceInBaseFrame.emplace(LimbEnum::LF_LEG, position);
      nominalPlanarStanceInBaseFrame.emplace(LimbEnum::RF_LEG, Position2(Eigen::Vector2d(position(0), -position(1))));
      nominalPlanarStanceInBaseFrame.emplace(LimbEnum::LH_LEG, Position2(Eigen::Vector2d(-position(0), position(1))));
      nominalPlanarStanceInBaseFrame.emplace(LimbEnum::RH_LEG, Position2(Eigen::Vector2d(-position(0), -position(1))));
    }

  } baseAutoParameters_;

 private:
  std::shared_ptr<free_gait::AdapterBase> adapter_;
};

} /* namespace */
