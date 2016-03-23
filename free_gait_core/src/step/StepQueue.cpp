/*
 * StepQueue.cpp
 *
 *  Created on: Jan 20, 2015
 *      Author: Péter Fankhauser
 *   Institute: ETH Zurich, Autonomous Systems Lab
 */

#include "free_gait_core/step/StepQueue.hpp"

// STD
#include <stdexcept>

// Roco
#include <roco/log/log_messages.hpp>

namespace free_gait {

StepQueue::StepQueue()
    : active_(false)
{
  previousStep_.reset();
}

StepQueue::~StepQueue()
{
}

StepQueue::StepQueue(const StepQueue& other)
    : queue_(other.queue_),
      active_(other.active_)
{
  if (other.previousStep_) previousStep_ = std::move(other.previousStep_->clone());
}

void StepQueue::add(const Step& step)
{
  queue_.push_back(step);
}

bool StepQueue::advance(double dt, bool& hasSwitchedStep, bool& hasStartedStep)
{
  // Check if empty.
  hasSwitchedStep = false;
  hasStartedStep = false;
  if (queue_.empty()) {
    active_ = false;
    return true;
  }

  // Special treatment of first step of queue.
  if (!active_) {
    active_ = true;
    hasSwitchedStep = true;
    return true;
  }

  // Check if step is updated (multi-threading).
  if (!queue_.front().isUpdated()) {
    if (queue_.front().update()) {
      hasStartedStep = true;
    } else {
      return true;
    }
  }

  // Advance current step.
  if (!queue_.front().advance(dt)) {
    // Step finished.
    previousStep_ = std::move(std::unique_ptr<Step>(new Step(queue_.front())));
    queue_.pop_front();
    if (queue_.empty()) {
      // End reached.
      active_ = false;
      return true;
    }
    hasSwitchedStep = true;
  }

  return true;
}

bool StepQueue::active() const
{
  if (empty()) return false;
  return queue_.front().isUpdated();
}

bool StepQueue::empty() const
{
  return queue_.empty();
}

void StepQueue::clearNextSteps()
{
  if (empty()) return;
  queue_.erase(queue_.begin() + 1, queue_.end());
}

void StepQueue::clear()
{
  previousStep_.reset();
  queue_.clear();
}

const Step& StepQueue::getCurrentStep() const
{
  if (empty()) throw std::out_of_range("StepQueue::getCurrentStep(): No steps in queue!");
  return queue_.front();
}

Step& StepQueue::getCurrentStep()
{
  if (empty()) throw std::out_of_range("StepQueue::getCurrentStep(): No steps in queue!");
  return queue_.front();
}

void StepQueue::replaceCurrentStep(const Step& step)
{
  queue_.pop_front();
  queue_.push_front(step);
}

const Step& StepQueue::getNextStep() const
{
  if (size() <= 1) throw std::out_of_range("StepQueue::getNextStep(): No next step in queue!");
  auto iterator = queue_.begin() + 1;
  return *iterator;
}

bool StepQueue::previousStepExists() const
{
 return (bool)previousStep_;
}

const Step& StepQueue::getPreviousStep() const
{
  if (!previousStep_) throw std::out_of_range("StepQueue::getPreviousStep(): No previous step available!");
  return *previousStep_;
}

std::deque<Step>::size_type StepQueue::size() const
{
  return queue_.size();
}

} /* namespace */