#include "Dribbler.h"
#include "Command.h"
#include "Util.h"

#include <iostream>

Dribbler::Dribbler() : speed(0), ballDetected(false), ballInDribblerTime(0.0), ballLostTime(-1.0) {

};

Dribbler::~Dribbler() {

}

void Dribbler::setSpeed(int newSpeed) {
	speed = newSpeed;
}

void Dribbler::stop() {
	speed = 0;
}

void Dribbler::step(float dt) {
	if (ballDetected) {
		ballInDribblerTime += dt;

		if (ballInDribblerTime >= Config::ballInDribblerThreshold) {
			ballLostTime = 0.0;
		}
	} else {
		ballLostTime += dt;

		if (ballLostTime >= Config::dribblerBallLostThreshold) {
			ballInDribblerTime = 0.0;
		}
	}

	//std::cout << "ballInDribblerTime: " << ballInDribblerTime << ", ballLostTime: " << ballLostTime << ", got ball: " << (gotBall() ? "yes" : "no") << std::endl;
}

bool Dribbler::gotBall() const {
	return ballDetected;

	/*if (ballDetected && ballInDribblerTime >= Config::ballInDribblerThreshold) {
		return true;
	}

	if (!ballDetected && ballLostTime <= Config::dribblerBallLostThreshold) {
		return true;
	}

	return false;*/
}

bool Dribbler::handleCommand(const Command& cmd) {
	if (cmd.name == "ball") {
		if (cmd.parameters[0] == "1") {
			ballDetected = true;
		} else {
			ballDetected = false;
		}

		//std::cout << "@ BALL DETECTED: " << ballDetected << std::endl;

		return true;
	}

	return false;
}
