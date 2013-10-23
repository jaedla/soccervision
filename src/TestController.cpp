#include "TestController.h"

#include "Robot.h"
#include "Dribbler.h"
#include "Command.h"

/**
 * TODO
 * - fetch ball straight and search for goal if lost goal at large angle
 * - search for goal state
 * - search for ball state
 * - avoid kicking through another ball
 */

TestController::TestController(Robot* robot, Communication* com) : BaseAI(robot, com), targetSide(Side::BLUE), manualSpeedX(0.0f), manualSpeedY(0.0f), manualOmega(0.0f), manualDribblerSpeed(0), manualKickStrength(0), blueGoalDistance(0.0f), yellowGoalDistance(0.0f), lastCommandTime(0.0), lastTargetGoalAngle(0.0f) {
	setupStates();
};

TestController::~TestController() {
	
}

void TestController::setupStates() {
	states["manual-control"] = new ManualControlState(this);
	states["watch-ball"] = new WatchBallState(this);
	states["watch-goal"] = new WatchGoalState(this);
	states["watch-goal-behind"] = new WatchGoalBehindState(this);
	states["spin-around-dribbler"] = new SpinAroundDribblerState(this);
	states["drive-to"] = new DriveToState(this);
	states["turn-by"] = new TurnByState(this);
	states["find-ball"] = new FindBallState(this);
	states["fetch-ball-front"] = new FetchBallFrontState(this);
	states["fetch-ball-direct"] = new FetchBallDirectState(this);
	states["fetch-ball-behind"] = new FetchBallBehindState(this);
	states["fetch-ball-near"] = new FetchBallNearState(this);
	states["aim"] = new AimState(this);
	states["drive-circle"] = new DriveCircleState(this);
}

void TestController::step(float dt, Vision::Results* visionResults) {
	updateGoalDistances(visionResults);
	
	if (currentState == NULL) {
		setState("manual-control");
	}

	currentStateDuration += dt;
	totalDuration += dt;

	if (currentState != NULL) {
		currentState->step(dt, visionResults, robot, totalDuration, currentStateDuration);
	}
}

bool TestController::handleCommand(const Command& cmd) {
	if (cmd.name == "target-vector" && cmd.parameters.size() == 3) {
        handleTargetVectorCommand(cmd);
    } else if (cmd.name == "set-dribbler" && cmd.parameters.size() == 1) {
        handleDribblerCommand(cmd);
    } else if (cmd.name == "kick" && cmd.parameters.size() == 1) {
        handleKickCommand(cmd);
    } else if (cmd.name == "reset-position") {
		robot->setPosition(Config::fieldWidth / 2.0f, Config::fieldHeight / 2.0f, 0.0f);
    } else if (cmd.name == "stop") {
        handleResetCommand();
		setState("manual-control");
    } else if (cmd.name == "reset") {
        handleResetCommand();
    } else if (cmd.name == "toggle-side") {
        handleToggleSideCommand();
    } else if (cmd.name == "drive-to" && cmd.parameters.size() == 3) {
        handleDriveToCommand(cmd);
    } else if (cmd.name == "turn-by" && cmd.parameters.size() == 1) {
        handleTurnByCommand(cmd);
    } else if (cmd.name.substr(0, 4) == "run-") {
        setState(cmd.name.substr(4));
    } else if (cmd.name == "parameter" && cmd.parameters.size() == 2) {
		handleParameterCommand(cmd);
	} else {
		return false;
	}

    return true;
}

void TestController::handleTargetVectorCommand(const Command& cmd) {
    manualSpeedX = Util::toFloat(cmd.parameters[0]);
    manualSpeedY = Util::toFloat(cmd.parameters[1]);
    manualOmega = Util::toFloat(cmd.parameters[2]);

	lastCommandTime = Util::millitime();
}

void TestController::handleDribblerCommand(const Command& cmd) {
    manualDribblerSpeed = Util::toInt(cmd.parameters[0]);

	lastCommandTime = Util::millitime();
}

void TestController::handleKickCommand(const Command& cmd) {
    manualKickStrength = Util::toInt(cmd.parameters[0]);

	lastCommandTime = Util::millitime();
}

void TestController::handleResetCommand() {
	if (!resetBtn.toggle()) {
		return;
	}

	std::cout << "! Resetting test controller" << std::endl;

	totalDuration = 0.0f;
	currentStateDuration = 0.0f;

	setState(currentStateName);
}

void TestController::handleToggleSideCommand() {
	if (!toggleSideBtn.toggle()) {
		return;
	}

	if (targetSide == Side::BLUE) {
		targetSide = Side::YELLOW;
	} else {
		targetSide = Side::BLUE;
	}

	std::cout << "! Now targeting " << (targetSide == Side::BLUE ? "blue" : "yellow") << " side" << std::endl;

	com->send("target:" + Util::toString(targetSide));
}

void TestController::handleParameterCommand(const Command& cmd) {
	int index = Util::toInt(cmd.parameters[0]);
	std::string value = cmd.parameters[1];

	parameters[index] = value;

	std::cout << "! Received parameter #" << index << ": " << value << std::endl;
}

void TestController::handleDriveToCommand(const Command& cmd) {
	DriveToState* state = (DriveToState*)states["drive-to"];

	state->x = Util::toFloat(cmd.parameters[0]);
	state->y = Util::toFloat(cmd.parameters[1]);
	state->orientation = Util::toFloat(cmd.parameters[2]);

	setState("drive-to");
}

void TestController::handleTurnByCommand(const Command& cmd) {
	TurnByState* state = (TurnByState*)states["turn-by"];

	state->angle = Math::degToRad(Util::toFloat(cmd.parameters[0]));

	setState("turn-by");
}

void TestController::updateGoalDistances(Vision::Results* visionResults) {
	Object* blueGoal = visionResults->getLargestGoal(Side::BLUE);
	Object* yellowGoal = visionResults->getLargestGoal(Side::YELLOW);

	blueGoalDistance = blueGoal != NULL ? blueGoal->distance : 0.0f;
	yellowGoalDistance = yellowGoal != NULL ? yellowGoal->distance : 0.0f;

	if (targetSide == Side::BLUE && blueGoal != NULL) {
		lastTargetGoalAngle = blueGoal->angle;
	} else if (targetSide == Side::YELLOW && yellowGoal != NULL) {
		lastTargetGoalAngle = yellowGoal->angle;
	}
}

std::string TestController::getJSON() {
	std::stringstream stream;

	stream << "{";
	
	for (MessagesIt it = messages.begin(); it != messages.end(); it++) {
		stream << "\"" << (it->first) << "\": \"" << (it->second) << "\",";
	}

	messages.clear();

	stream << "\"currentState\": \"" << currentStateName << "\",";
	stream << "\"stateDuration\": \"" << currentStateDuration << "\",";
	stream << "\"totalDuration\": \"" << totalDuration << "\",";
	stream << "\"targetSide\": \"" << (targetSide == Side::BLUE ? "blue" : targetSide == Side::YELLOW ? "yellow" : "not chosen") << "\",";
	stream << "\"blueGoalDistance\": " << blueGoalDistance << ",";
	stream << "\"yellowGoalDistance\": " << yellowGoalDistance << ",";
	stream << "\"lastTargetGoalAngle\": " << Math::radToDeg(lastTargetGoalAngle);

	stream << "}";

	return stream.str();
}

float TestController::getTargetAngle(float goalX, float goalY, float ballX, float ballY, float D, TestController::TargetMode targetMode) {
	float targetX1;
	float targetX2;
	float targetY1;
	float targetY2;

	if (Math::abs(ballX - goalX) < 0.001f){
		// special case of small x difference
		if (targetMode == TargetMode::INLINE){
			// let's not divide by zero
			targetX1 = ballX;
			targetX2 = ballX;
			targetY1 = ballY + D;
			targetY2 = ballY - D;
		} else if (targetMode == TargetMode::LEFT || targetMode == TargetMode::RIGHT){
			targetX1 = ballX + D;
			targetX2 = ballX - D;
			targetY1 = ballY;
			targetY2 = ballY;
		}
	} else if(Math::abs(ballY - goalY) < 0.001f){
		// special case of small y difference
		if (targetMode == TargetMode::INLINE){
			targetX1 = ballX + D;
			targetX2 = ballX - D;
			targetY1 = ballY;
			targetY2 = ballY;
		} else if (targetMode == TargetMode::LEFT || targetMode == TargetMode::RIGHT) {
			targetX1 = ballX;
			targetX2 = ballX;
			targetY1 = ballY + D;
			targetY2 = ballY - D;
		}
	} else {
		// normal case
		float a = (ballY - goalY) / (ballX - goalX);

		if (targetMode == TargetMode::LEFT || targetMode == TargetMode::RIGHT){
			// perpendicular to the line from goal to ball
			a = -(1.0f / a);
		}

		float b = ballY - a * ballX;
		float underSqrt = sqrt(
			- pow(a, 2) * pow(ballX, 2)
			+ pow(a, 2) * pow(D, 2)
			- 2.0f * a * b * ballX
			+ 2.0f * a * ballX * ballY
			- pow(b, 2)
			+ 2.0f * b * ballY
			+ pow(D, 2)
			- pow(ballY, 2)
		);
		float rest = - a * b + a * ballY + ballX;
		float divisor = pow(a, 2) + 1.0f;

		targetX1 = ( + underSqrt + rest) / divisor;
		targetX2 = ( - underSqrt + rest) / divisor;
		targetY1 = a * targetX1 + b;
		targetY2 = a * targetX2 + b;
	}

	// target's distance from goal (squared)
	float target1Dist = pow(goalX - targetX1, 2) + pow(goalY - targetY1, 2);
	float target2Dist = pow(goalX - targetX2, 2) + pow(goalY - targetY2, 2);

	float targetX;
	float targetY;

	if (targetMode == TargetMode::INLINE) {
		// choose target which is farther away from goal
		if (target1Dist > target2Dist){
			targetX = targetX1;
			targetY = targetY1;
		} else{
			targetX = targetX2;
			targetY = targetY2;
		}
	} else if (targetMode == TargetMode::LEFT) {
		// choose one on left
		targetX = targetX2;
		targetY = targetY2;
	} else if (targetMode == TargetMode::RIGHT) {
		// choose one on right
		targetX = targetX1;
		targetY = targetY1;
	}

	float targetAngle = atan2(targetX, targetY);

	return targetAngle;
}

void TestController::ManualControlState::step(float dt, Vision::Results* visionResults, Robot* robot, float totalDuration, float stateDuration) {
	double time = Util::millitime();

	if (time - ai->lastCommandTime < 0.5) {
		robot->setTargetDir(ai->manualSpeedX, ai->manualSpeedY, ai->manualOmega);
		robot->dribbler->setTargetSpeed(-ai->manualDribblerSpeed);

		if (ai->manualKickStrength != 0.0f) {
			robot->kick(ai->manualKickStrength);

			ai->manualKickStrength = 0;
		}
	} else {
		robot->stop();
		robot->dribbler->setTargetSpeed(0);
	}
}

void TestController::WatchBallState::step(float dt, Vision::Results* visionResults, Robot* robot, float totalDuration, float stateDuration) {
	Object* ball = visionResults->getClosestBall(Dir::FRONT);

	if (ball == NULL) {
		robot->setTargetDir(ai->manualSpeedX, ai->manualSpeedY, ai->manualOmega);

		return;
	}

	robot->setTargetDir(ai->manualSpeedX, ai->manualSpeedY);
	robot->lookAt(ball);
}

void TestController::WatchGoalState::step(float dt, Vision::Results* visionResults, Robot* robot, float totalDuration, float stateDuration) {
	Object* goal = visionResults->getLargestGoal(ai->targetSide, Dir::FRONT);

	if (goal == NULL) {
		robot->setTargetDir(ai->manualSpeedX, ai->manualSpeedY, ai->manualOmega);

		return;
	}

	robot->setTargetDir(ai->manualSpeedX, ai->manualSpeedY);
	robot->lookAt(goal);
}

void TestController::WatchGoalBehindState::step(float dt, Vision::Results* visionResults, Robot* robot, float totalDuration, float stateDuration) {
	Object* goal = visionResults->getLargestGoal(ai->targetSide, Dir::REAR);

	if (goal == NULL) {
		robot->setTargetDir(ai->manualSpeedX, ai->manualSpeedY, ai->manualOmega);

		return;
	}

	robot->setTargetDir(ai->manualSpeedX, ai->manualSpeedY);
	robot->lookAtBehind(goal);
}

void TestController::SpinAroundDribblerState::step(float dt, Vision::Results* visionResults, Robot* robot, float totalDuration, float stateDuration) {
	robot->spinAroundDribbler();
}

void TestController::DriveToState::onEnter(Robot* robot) {
	robot->driveTo(x, y, orientation);
}

void TestController::DriveToState::step(float dt, Vision::Results* visionResults, Robot* robot, float totalDuration, float stateDuration) {
	if (!robot->hasTasks()) {
		ai->setState("manual-control");
	}
}

void TestController::TurnByState::onEnter(Robot* robot) {
	robot->turnBy(angle, Math::PI);
}

void TestController::TurnByState::step(float dt, Vision::Results* visionResults, Robot* robot, float totalDuration, float stateDuration) {
	if (!robot->hasTasks()) {
		ai->setState("manual-control");
	}
}

void TestController::FindBallState::onEnter(Robot* robot) {
	if (ai->lastTargetGoalAngle > 0.0f) {
		searchDir = 1.0f;
	} else {
		searchDir = -1.0f;
	}

	lastTurnTime = -1.0;
}

void TestController::FindBallState::step(float dt, Vision::Results* visionResults, Robot* robot, float totalDuration, float stateDuration) {
	if (robot->dribbler->gotBall()) {
		ai->dbg("gotBall", true);
		ai->dbgs("action", "Switch to aim");

		robot->dribbler->start();

		ai->setState("aim");

		return;
	}

	ai->dbg("hasTasks", robot->hasTasks());

	robot->stop();
	
	Object* ball = visionResults->getClosestBall(Dir::FRONT);
	Object* goal = visionResults->getLargestGoal(ai->targetSide, Dir::FRONT);

	if (ball == NULL) {
		ball = visionResults->getClosestBall(Dir::ANY);
	}

	if (robot->hasTasks()) {
		if (ball != NULL && !ball->behind) {
			robot->clearTasks();
		} else {
			return;
		}
	}

	float searchOmega = Math::PI;
	double minTurnBreak = (double)(Math::TWO_PI / searchOmega);

	ai->dbg("ballVisible", ball != NULL);
	ai->dbg("goalVisible", goal != NULL);
	ai->dbg("searchDir", searchDir);
	ai->dbg("timeSinceLastTurn", lastTurnTime == -1.0 ? -1.0 : Util::duration(lastTurnTime));

	if (ball != NULL) {
		ai->dbg("ballBehind", ball->behind);
		ai->dbg("ballDistance", ball->getDribblerDistance());
		ai->dbg("ballAngle", Math::radToDeg(ball->angle));

		if (!ball->behind) {
			if (goal != NULL) {
				ai->setState("fetch-ball-front");
			} else {
				ai->setState("fetch-ball-direct");
			}
		} else if (lastTurnTime == -1.0 || Util::duration(lastTurnTime) >= minTurnBreak) {
			float turnAngle = ball->angle;
			//float underturnAngle = Math::degToRad(60.0f);
			float turnSpeed = Math::TWO_PI;

			if (turnAngle < 0.0f) {
				//turnAngle += underturnAngle;
				searchDir = -1.0f;
			} else {
				//turnAngle -= underturnAngle;
				searchDir = 1.0f;
			}

			ai->dbg("turnAngle", Math::radToDeg(turnAngle));
			ai->dbg("turnSpeed", Math::radToDeg(turnSpeed));
			ai->dbg("searchDir", searchDir);

			robot->turnBy(turnAngle, turnSpeed);

			lastTurnTime = Util::millitime();

			return;
		}
	}

	robot->setTargetOmega(searchOmega * searchDir);
}

void TestController::FetchBallFrontState::onEnter(Robot* robot) {
	reset();
}

void TestController::FetchBallFrontState::reset() {
	startBrakingDistance = -1.0f;
	startBrakingVelocity = -1.0f;
	lastBallDistance = -1.0f;
}

void TestController::FetchBallFrontState::step(float dt, Vision::Results* visionResults, Robot* robot, float totalDuration, float stateDuration) {
	robot->stop();
	
	if (robot->dribbler->gotBall()) {
		ai->dbg("gotBall", true);
		ai->dbgs("action", "Switch to aim");

		robot->dribbler->start();

		ai->setState("aim");

		return;
	}

	double minSearchFrontDuration = 1.0;
	
	// prefer balls in the front camera
	Object* ball = visionResults->getClosestBall(Dir::FRONT);
	Object* goal = visionResults->getLargestGoal(ai->targetSide, Dir::FRONT);

	if (ball == NULL && stateDuration > minSearchFrontDuration) {
		ball = visionResults->getClosestBall(Dir::ANY);
	}

	ai->dbg("ballVisible", ball != NULL);
	ai->dbg("goalVisible", goal != NULL);

	if (ball != NULL && ball->behind) {
		ai->dbgs("action", "Switch to fetch behind");

		ai->setState("fetch-ball-behind");

		return;
	}

	if (goal == NULL && ball != NULL) {
		ai->setState("fetch-ball-direct");

		return;
	}

	if (ball == NULL) {
		ai->setState("find-ball");

		return;
	}

	float approachSpeed = 2.0f;
	float maxNearSpeed = 1.0f;
	float startAccelerationDuration = 0.75f;
	float maxOffsetDistanceAngleDiff = 45.0f;
	float maxAngleDiffDistance = 0.6f;
	float focusBetweenBallGoalAngle = 15.0f;
	float maxAngleBrakingAngle = 40.0f;
	float maxBallBrakingAngle = 10.0f;
	float maxBrakingDistanceVelocity = 2.0f;
	float minVelocityBrakeDistance = 0.5f;
	float maxVelocityBrakingDistance = 1.5f;
	float minApproachSpeed = 0.5f;
	float nearDistance = 0.3f;

	float ballDistance = ball->getDribblerDistance();
	float ballAngle = ball->angle;
	float goalAngle = goal->angle;
	float angleDiff = Math::abs(goalAngle - ballAngle);
	float offsetDistance = Math::map(Math::radToDeg(angleDiff), 0.0f, maxOffsetDistanceAngleDiff, nearDistance, maxAngleDiffDistance);

	if (ai->parameters[0].length() > 0) approachSpeed = Util::toFloat(ai->parameters[0]);
	if (ai->parameters[1].length() > 0) offsetDistance = Util::toFloat(ai->parameters[1]);
	if (ai->parameters[2].length() > 0) startAccelerationDuration = Util::toFloat(ai->parameters[2]);

	// reset if we probably started to watch a ball we just kicked
	if (lastBallDistance != -1.0f && lastBallDistance - ballDistance < -0.2f) {
		reset();
	}

	//if (ballDistance < offsetDistance) {
	if (ballDistance < nearDistance) {
		ai->dbgs("action", "Switch to fetch ball near");
		ai->setState("fetch-ball-near");
		
		return;
	}
	
	float adaptiveBrakingDistance = Math::map(robot->getVelocity(), 0.0f, maxBrakingDistanceVelocity, minVelocityBrakeDistance, maxVelocityBrakingDistance);
	float targetAngle = ai->getTargetAngle(goal->distanceX, goal->distanceY, ball->distanceX, ball->distanceY, offsetDistance);

	// accelerate in the beginning
	float acceleratedSpeed = approachSpeed * Math::map(stateDuration, 0.0f, startAccelerationDuration, 0.0f, 1.0f);

	// only choose the braking distance once
	if (startBrakingDistance == -1.0f && ballDistance < adaptiveBrakingDistance) {
		startBrakingDistance = adaptiveBrakingDistance;
		startBrakingVelocity = robot->getVelocity();
	}

	if (startBrakingDistance != -1.0f) {
		if (ball->distance < startBrakingDistance) {
			// brake as getting close and large target angle
			float distanceBraking = Math::map(ballDistance, nearDistance, startBrakingDistance, 1.0, 0.0f);
			float targetAngleBreaking = Math::map(Math::abs(targetAngle), 0.0f, Math::degToRad(maxAngleBrakingAngle), 0.0f, 1.0f);
			float ballAngleBreaking = Math::map(Math::abs(ballAngle), 0.0f, Math::degToRad(maxBallBrakingAngle), 0.0f, 1.0f);
			float combinedBrakeFactor = distanceBraking * (targetAngleBreaking + ballAngleBreaking);
			//float combinedBrakeFactor = brakeP * (distanceBraking + angleBreaking);
		
			// limit max speed near the ball
			float maxSpeed = Math::map(ballDistance, nearDistance, startBrakingDistance, maxNearSpeed, startBrakingVelocity);
		
			acceleratedSpeed = acceleratedSpeed * (1.0f - combinedBrakeFactor);
			acceleratedSpeed = Math::min(acceleratedSpeed, maxSpeed);
			acceleratedSpeed = Math::max(acceleratedSpeed, minApproachSpeed);

			ai->dbg("distanceBraking", distanceBraking);
			ai->dbg("targetAngleBreaking", targetAngleBreaking);
			ai->dbg("ballAngleBreaking", ballAngleBreaking);
			ai->dbg("combinedBrakeFactor", combinedBrakeFactor);
			ai->dbg("maxSpeed", maxSpeed);
		} else if (ballDistance - startBrakingDistance > 0.2f) {
			reset(); // the ball has gone further than when started to brake, may have seen kicked ball, reset
		}
	}
	
	//float lookAngle = Math::map(angleDiff, 0.0f, Math::degToRad(focusBetweenBallGoalAngle), goal->angle, (goal->angle + ball->angle) / 2.0f);
	float lookAngle = Math::map(ballDistance, nearDistance, maxAngleDiffDistance, goal->angle, (goal->angle + ball->angle) / 2.0f);

	robot->setTargetDir(Math::Rad(targetAngle), acceleratedSpeed);
	robot->lookAt(Math::Rad(lookAngle));

	lastBallDistance = ballDistance;

	ai->dbg("acceleratedSpeed", acceleratedSpeed);
	ai->dbg("startBrakingDistance", startBrakingDistance);
	ai->dbg("startBrakingVelocity", startBrakingVelocity);
	ai->dbg("ballDistance", ballDistance);
	ai->dbg("ballAngle", Math::radToDeg(ballAngle));
	ai->dbg("goalAngle", Math::radToDeg(goalAngle));
	ai->dbg("targetAngle", Math::radToDeg(targetAngle));
	ai->dbg("angleDiff", Math::radToDeg(angleDiff));
	ai->dbg("offsetDistance", offsetDistance);
	ai->dbg("nearDistance", nearDistance);
	ai->dbg("lookAngle", Math::radToDeg(lookAngle));
}

void TestController::FetchBallDirectState::onEnter(Robot* robot) {
	float minAllowedApproachSpeed = 0.5f;

	enterVelocity = Math::max(robot->getVelocity(), minAllowedApproachSpeed);
}

void TestController::FetchBallDirectState::step(float dt, Vision::Results* visionResults, Robot* robot, float totalDuration, float stateDuration) {
	robot->stop();
	
	if (robot->dribbler->gotBall()) {
		ai->dbg("gotBall", true);
		ai->dbgs("action", "Switch to aim");

		robot->dribbler->start();

		ai->setState("aim");

		return;
	}

	double minSearchFrontDuration = 1.0;
	
	Object* ball = visionResults->getClosestBall(Dir::FRONT);
	Object* goal = visionResults->getLargestGoal(ai->targetSide, Dir::FRONT);

	if (ball == NULL && stateDuration > minSearchFrontDuration) {
		ball = visionResults->getClosestBall(Dir::ANY);
	}

	ai->dbg("ballVisible", ball != NULL);
	ai->dbg("goalVisible", goal != NULL);

	if (ball != NULL && goal != NULL && !ball->behind) {
		ai->dbgs("action", "Switch to fetch front");

		ai->setState("fetch-ball-front");

		return;
	}

	if (ball != NULL && ball->behind) {
		ai->dbgs("action", "Switch to fetch behind");

		ai->setState("fetch-ball-behind");

		return;
	}

	if (ball == NULL) {
		ai->setState("find-ball");

		return;
	}

	float approachSpeed = 2.0f;
	float ballDistance = ball->getDribblerDistance();
	float forwardSpeed = Math::map(ballDistance, 0.0f, 1.0f, 0.3f, approachSpeed);

	ai->dbg("ballDistance", ballDistance);
	ai->dbg("forwardSpeed", forwardSpeed);
	ai->dbg("enterVelocity", enterVelocity);

	robot->setTargetDir(forwardSpeed, 0.0f);
	robot->lookAt(ball);
}

void TestController::FetchBallBehindState::onEnter(Robot* robot) {
	hadBall = false;
	lastTargetAngle = 0.0f;
	lostBallTime = -1.0;
	timeSinceLostBall = 0.0;
	lostBallVelocity = 0.0f;
	startBallDistance = -1.0f;
	targetMode = TargetMode::UNDECIDED;
}

void TestController::FetchBallBehindState::step(float dt, Vision::Results* visionResults, Robot* robot, float totalDuration, float stateDuration) {
	robot->stop();

	if (robot->dribbler->gotBall()) {
		ai->dbg("gotBall", true);

		robot->dribbler->start();

		ai->setState("aim");

		return;
	}

	double maxBlindReverseDuration = 1.5;
	double minSearchBehindDuration = 1.0;

	Object* ball = visionResults->getClosestBall(Dir::REAR);
	Object* goal = visionResults->getLargestGoal(ai->targetSide, Dir::FRONT);

	if (ball == NULL && stateDuration > minSearchBehindDuration) {
		ball = visionResults->getClosestBall(Dir::ANY);
	}

	ai->dbg("ballVisible", ball != NULL);
	ai->dbg("goalVisible", goal != NULL);
	ai->dbg("timeSinceLostBall", timeSinceLostBall);

	if (goal == NULL) {
		if (ball != NULL && !ball->behind) {
			ai->setState("fetch-ball-direct");
		} else {
			ai->setState("find-ball");
		}

		return;
	}

	if (ball != NULL) {
		ai->dbg("ballDistance", ball->getDribblerDistance());
		ai->dbg("ball->behind", ball->behind);
	}

	ai->dbg("startBallDistance", startBallDistance);
	ai->dbg("hadBall", hadBall);

	// only revert to fetch front if not fetching behind blind
	if (
		ball != NULL
		&& !ball->behind
		&& (
			ball->getDribblerDistance() <= startBallDistance
			|| timeSinceLostBall >= maxBlindReverseDuration
			|| !hadBall
		)
	) {
		ai->setState("fetch-ball-front");

		return;
	}

	if (ball == NULL || !ball->behind) {
		if (!hadBall) {
			return; // TODO Never had the ball, what now?
		}

		if (lostBallTime == -1.0) {
			lostBallTime = Util::millitime();
			lostBallVelocity = robot->getVelocity();
		}
		
		timeSinceLostBall = Util::duration(lostBallTime);

		if (timeSinceLostBall > maxBlindReverseDuration) {
			if (ball != NULL) {
				ai->setState("fetch-ball-front");
			}

			return; // TODO Start searching for new ball
		}

		float fetchBlindSpeed = 0.5f;
		//float sideP = 0.4f;
		float sideAccelerationDuration = 0.5f;
		float deaccelerationDuration = 0.5f;
		double sideSpeedDelay = 0.5;
		float deacceleratedSpeed = Math::map((float)timeSinceLostBall, 0.0f, deaccelerationDuration, lostBallVelocity, fetchBlindSpeed);
		float targetModeSide = targetMode == TargetMode::LEFT ? 1.0f : -1.0f;
		float sideSpeed = targetModeSide * Math::map((float)(timeSinceLostBall - sideSpeedDelay), 0.0f, sideAccelerationDuration, 0.0f, deacceleratedSpeed);

		Math::Vector dirVector = Math::Vector::createForwardVec(lastTargetAngle, deacceleratedSpeed);

		dirVector.y += sideSpeed;

		robot->setTargetDir(dirVector.x, dirVector.y);
		robot->lookAt(goal);

		ai->dbgs("mode", "blind");
		ai->dbg("sideSpeed", sideSpeed);
		ai->dbg("deacceleratedSpeed", deacceleratedSpeed);

		return;
	}

	float ballDistance = ball->getDribblerDistance();

	hadBall = true;
	timeSinceLostBall = 0.0;
	lostBallTime = -1.0;

	if (startBallDistance == -1.0f) {
		startBallDistance = ballDistance;
	}

	ai->dbgs("mode", "visible");

	float offsetDistance = 0.2f;
	float approachP = 2.0f;
	float startAccelerationDuration = 0.5f;

	if (targetMode == TargetMode::UNDECIDED) {
		if (ball->angle + goal->angle > 0.0f) {
			targetMode = TargetMode::LEFT;
		} else {
			targetMode = TargetMode::RIGHT;
		}
	}

	if (ai->parameters[0].length() > 0) offsetDistance = Util::toFloat(ai->parameters[0]);
	if (ai->parameters[1].length() > 0) approachP = Util::toFloat(ai->parameters[1]);
	if (ai->parameters[2].length() > 0) {
		int targetModeVal = Util::toInt(ai->parameters[2]);

		if (targetModeVal == -1) {
			targetMode = TargetMode::LEFT;
		} else if (targetModeVal == 1) {
			targetMode = TargetMode::RIGHT;
		} else {
			targetMode = TargetMode::INLINE;
		}
	}

	//float targetAngle = ai->getTargetAngle(goal->distanceX, goal->distanceY * (goal->behind ? -1.0f : 1.0f), ball->distanceX, ball->distanceY * (ball->behind ? -1.0f : 1.0f), offsetDistance, TargetMode::RIGHT);
	float targetAngle = ai->getTargetAngle(goal->distanceX * (goal->behind ? -1.0f : 1.0f), goal->distanceY * (goal->behind ? -1.0f : 1.0f), ball->distanceX * (ball->behind ? -1.0f : 1.0f), ball->distanceY * (ball->behind ? -1.0f : 1.0f), offsetDistance, targetMode);
	float approachSpeed = approachP * Math::map(stateDuration, 0.0f, startAccelerationDuration, 0.0f, 1.0f);
	float deacceleratedSpeed = Math::map(ballDistance, 0.3f, 1.0f, 0.5f, approachSpeed);

	ai->dbg("offsetDistance", offsetDistance);
	ai->dbg("approachSpeed", approachSpeed);
	ai->dbg("deacceleratedSpeed", deacceleratedSpeed);
	ai->dbg("targetAngle", Math::radToDeg(targetAngle));
	/*ai->dbg("goal->distanceX", goal->distanceX * (goal->behind ? -1.0f : 1.0f));
	ai->dbg("goal->distanceY", goal->distanceY * (goal->behind ? -1.0f : 1.0f));
	ai->dbg("ball->distanceX", ball->distanceX * (ball->behind ? -1.0f : 1.0f));
	ai->dbg("ball->distanceY", ball->distanceY * (ball->behind ? -1.0f : 1.0f));*/

	robot->setTargetDir(Math::Rad(targetAngle), deacceleratedSpeed);
	robot->lookAt(goal);

	lastTargetAngle = targetAngle;
}

void TestController::FetchBallNearState::onEnter(Robot* robot) {
	enterVelocity = robot->getVelocity();
	enterDistance = -1.0f;
	enterBallDistance = -1.0f;
	smallestForwardSpeed = -1.0f;
}

void TestController::FetchBallNearState::step(float dt, Vision::Results* visionResults, Robot* robot, float totalDuration, float stateDuration) {
	robot->stop();
	
	if (robot->dribbler->gotBall()) {
		ai->dbg("gotBall", true);

		robot->dribbler->start();

		ai->setState("aim");

		return;
	}
	
	Object* ball = visionResults->getClosestBall();
	Object* goal = visionResults->getLargestGoal(ai->targetSide, Dir::FRONT);

	ai->dbg("ballVisible", ball != NULL);
	ai->dbg("goalVisible", goal != NULL);
	ai->dbg("enterBallDistance", enterBallDistance);

	if (goal == NULL) {
		if (ball != NULL) {
			ai->setState("fetch-ball-direct");
		} else {
			ai->setState("find-ball");
		}

		return;
	}

	if (ball != NULL && ball->behind) {
		ai->setState("fetch-ball-behind");

		return;
	}

	if (ball == NULL) {
		ai->setState("find-ball");

		return;
	}

	float ballDistance = ball->getDribblerDistance();

	if (enterDistance == -1.0f) {
		enterDistance = ballDistance;
	}

	
	
	//float approachP = 1.5f;
	//float sideP = 0.5f;
	//float sideP = 0.5f;
	//float nearZeroSpeedAngle = 10.0f;
	float minAllowedApproachSpeed = 0.5f;
	//float nearZeroSpeedAngle = Math::map(ballDistance, 0.0f, 0.75f, 5.0f, 20.0f);
	//float nearMaxSideSpeedAngle = 35.0f;
	float nearDistance = 0.35f;
	//float maxSideSpeedDistance = 0.2f;
	//float nearMaxSideSpeedAngle = nearZeroSpeedAngle * 2.0f;

	if (enterBallDistance == -1.0f) {
		enterBallDistance = ballDistance;
	} else if (ballDistance > enterBallDistance + 0.1f || ballDistance > nearDistance) {
		// ball has gotten further than when started, probably messed it up, switch to faster fetch
		ai->setState("fetch-ball-front");

		return;
	}

	//if (ai->parameters[0].length() > 0) approachP = Util::toFloat(ai->parameters[0]);
	//if (ai->parameters[1].length() > 0) sideP = Util::toFloat(ai->parameters[1]);
	//if (ai->parameters[2].length() > 0) nearZeroSpeedAngle = Util::toFloat(ai->parameters[2]);

	//float forwardSpeed = Math::min(approachP * Math::map(Math::abs(Math::radToDeg(ball->angle)), 0.0f, nearZeroSpeedAngle, 1.0f, 0.0f), enterVelocity);
	//float forwardSpeed = 0.0f;
	//float sideSpeed = sideP * Math::sign(ball->distanceX) * Math::map(Math::abs(Math::radToDeg(ball->angle)), 0.0f, nearMaxSideSpeedAngle, 0.0f, 1.0f);
	float maxSideSpeedDistance = Math::map(ballDistance, 0.1f, 0.5f, 0.05f, 0.2f);
	float sideP = Math::map(ballDistance, 0.1f, enterDistance, 0.2f, 0.5f);
	float approachP = Math::map(ballDistance, 0.1f, enterDistance, 0.5f, enterVelocity);
	float sidePower = Math::map(Math::abs(ball->distanceX), 0.0f, maxSideSpeedDistance, 0.0f, 1.0f);
	float sideSpeed = sideP * Math::sign(ball->distanceX) * sidePower;
	float forwardSpeed = approachP * (1.0f - sidePower);
	float limitedForwardSpeed = Math::min(forwardSpeed, Math::max(enterVelocity, minAllowedApproachSpeed));
	
	/*if (smallestForwardSpeed == -1.0f || forwardSpeed < smallestForwardSpeed) {
		smallestForwardSpeed = forwardSpeed;
	}

	float limitedForwardSpeed = Math::map(smallestForwardSpeed, 0.0f, enterVelocity, 0.5f, enterVelocity);*/

	ai->dbg("approachP", approachP);
	ai->dbg("sideP", sideP);
	ai->dbg("ballDistance", ballDistance);
	ai->dbg("forwardSpeed", forwardSpeed);
	ai->dbg("limitedForwardSpeed", limitedForwardSpeed);
	ai->dbg("sidePower", sidePower);
	ai->dbg("sideSpeed", sideSpeed);
	ai->dbg("enterVelocity", enterVelocity);
	ai->dbg("enterDistance", enterDistance);
	ai->dbg("ballAngle", (Math::radToDeg(ball->angle)));
	ai->dbg("goalAngle", (Math::radToDeg(goal->angle)));
	//ai->dbg("nearZeroSpeedAngle", nearZeroSpeedAngle);
	ai->dbg("ball->distanceX", ball->distanceX);

	robot->dribbler->start();
	robot->setTargetDir(limitedForwardSpeed, sideSpeed);
	robot->lookAt(goal);
}

void TestController::AimState::onEnter(Robot* robot) {
	avoidBallSide = TargetMode::UNDECIDED;
	searchGoalDir = 0.0f;
}

void TestController::AimState::step(float dt, Vision::Results* visionResults, Robot* robot, float totalDuration, float stateDuration) {
	robot->stop();

	ai->dbg("gotBall", robot->dribbler->gotBall());
	
	if (!robot->dribbler->gotBall()) {
		ai->setState("fetch-ball-front");

		return;
	}

	robot->dribbler->start();
	
	Object* goal = visionResults->getLargestGoal(ai->targetSide, Dir::FRONT);

	ai->dbg("goalVisible", goal != NULL);

	if (goal == NULL) {
		float searchPeriod = Config::robotSpinAroundDribblerPeriod;

		if (searchGoalDir == 0.0f) {
			if (ai->lastTargetGoalAngle > 0.0f) {
				searchGoalDir = 1.0f;
			} else {
				searchGoalDir = -1.0f;
			}
		}

		robot->spinAroundDribbler(searchGoalDir == -1.0f, searchPeriod);

		if (stateDuration > searchPeriod) {
			float approachOwnGoalBackwardsSpeed = 1.0f;
			float approachOwnGoalSideSpeed = 0.5f;
			float accelerationPeriod = 0.5f;
			float reverseDuration = 1.5f;

			if (stateDuration > searchPeriod + reverseDuration) {
				ai->setState("aim");

				return;
			}

			// didn't find our goal in time, search for opponent goal and drive towards it instead
			Side ownSide = ai->targetSide == Side::YELLOW ? Side::BLUE : Side::YELLOW;

			Object* ownGoal = visionResults->getLargestGoal(ownSide, Dir::REAR);

			float accelerationMultiplier = Math::map(stateDuration, searchPeriod, searchPeriod + accelerationPeriod, 0.0f, 1.0f);

			if (ownGoal != NULL) {
				robot->setTargetDir(
					-approachOwnGoalBackwardsSpeed * accelerationMultiplier,
					//approachOwnGoalSideSpeed * (ownGoal->angle > 0.0f ? 1.0f : -1.0f) * accelerationMultiplier,
					0.0f,
					0.0f
				);
				robot->lookAtBehind(ownGoal);
			}
		}

		return;
	}

	ai->dbg("goalVisible", true);

	robot->setTargetDir(0.0f, 0.0f, 0.0f);
	robot->dribbler->start();

	float avoidBallSpeed = 0.5f;
	float minBallAvoidSideSpeed = 0.25f;
	int halfWidth = Config::cameraWidth / 2;
	int leftEdge = goal->x - goal->width / 2;
	int rightEdge = goal->x + goal->width / 2;
	int goalKickThresholdPixels = (int)((float)goal->width * Config::goalKickThreshold);
	double timeSinceLastKick = lastKickTime != 0.0 ? Util::duration(lastKickTime) : -1.0;
	bool isBallInWay = visionResults->isBallInWay(visionResults->front->balls, goal->y + goal->height / 2);
	bool shouldKick = false;
	float forwardSpeed = 0.0f;
	float sideSpeed = 0.0f;

	if (isBallInWay) {
		if (avoidBallSide == TargetMode::UNDECIDED) {
			if (robot->getPosition().y < Config::fieldHeight / 2.0f) {
				avoidBallSide = TargetMode::RIGHT;
			} else {
				avoidBallSide = TargetMode::LEFT;
			}
		}

		forwardSpeed = Math::map(goal->distance, 0.5f, 1.0f, 0.0f, avoidBallSpeed);
		sideSpeed = (avoidBallSide == TargetMode::LEFT ? -1.0f : 1.0f) * Math::max(forwardSpeed, minBallAvoidSideSpeed);
	}

	if (!goal->behind) {
		if (
			leftEdge + goalKickThresholdPixels < halfWidth
			&& rightEdge - goalKickThresholdPixels > halfWidth
		) {
			shouldKick = true;
		}
	}

	ai->dbg("shouldKick", shouldKick);
	ai->dbg("isBallInWay", isBallInWay);
	ai->dbg("avoidBallSide", avoidBallSide);
	ai->dbg("leftEdge", leftEdge);
	ai->dbg("rightEdge", rightEdge);
	ai->dbg("halfWidth", halfWidth);
	ai->dbg("leftValid", leftEdge + goalKickThresholdPixels < halfWidth);
	ai->dbg("rightValid", rightEdge - goalKickThresholdPixels > halfWidth);
	ai->dbg("goalKickThresholdPixels", goalKickThresholdPixels);
	ai->dbg("sinceLastKick", timeSinceLastKick);
	ai->dbg("forwardSpeed", forwardSpeed);
	ai->dbg("sideSpeed", sideSpeed);

	if (shouldKick && !isBallInWay && (lastKickTime == -1.0 || timeSinceLastKick >= 1.0)) {
		robot->kick();

		lastKickTime = Util::millitime();
	} else {
		robot->setTargetDir(forwardSpeed, sideSpeed);
		robot->lookAt(goal);
	}
}

void TestController::DriveCircleState::step(float dt, Vision::Results* visionResults, Robot* robot, float totalDuration, float stateDuration) {
	Object* goal = visionResults->getLargestGoal(ai->targetSide, Dir::FRONT);

	ai->dbg("goalVisible", goal != NULL);

	if (goal == NULL) {
		return;
	}

	float speed = 0.5f;
	float period = 5.0f;

	if (ai->parameters[0].length() > 0) speed = Util::toFloat(ai->parameters[0]);
	if (ai->parameters[1].length() > 0) period = Util::toFloat(ai->parameters[1]);

	float targetAngle = getCircleTargetAngle(0.0f, stateDuration, period);

	robot->setTargetDir(Math::Rad(targetAngle), speed);
	robot->lookAt(goal);
}

float TestController::DriveCircleState::getCircleTargetAngle(float start, float time, float period){
	float omega = 2.0f * Math::PI / period;
	float targetX = sin(omega * time + start);
	float targetY = cos(omega * time + start);
	float targetAngle = atan2(targetY, targetX);

	return targetAngle;
}