#include "ManualController.h"

#include "Robot.h"
#include "Command.h"
#include "Dribbler.h"
#include "Coilgun.h"
#include "Util.h"

ManualController::ManualController(Robot* robot, Communication* com) : Controller(robot, com) {
	targetSide = Side::UNKNOWN;
	dir = 1;
};

void ManualController::step(float dt, Vision::Results* visionResults) {
	robot->setAutostop(false);

	speed += dir;

	if (Math::abs((float)speed) > 255) {
		dir *= -1;
	}

	robot->getWheelFL()->setTargetOmega(Wheel::speedToOmega(speed));
}

bool ManualController::handleRequest(std::string request) {
    return false;
}

bool ManualController::handleCommand(const Command& cmd) {
    if (cmd.name == "toggle-side") {
        handleToggleSideCommand();
    } else if (cmd.name == "target-vector" && cmd.parameters.size() == 3) {
        handleTargetVectorCommand(cmd);
    } else if (cmd.name == "target-dir" && cmd.parameters.size() == 3) {
        handleTargetDirCommand(cmd);
    } else if (cmd.name == "set-dribbler" && cmd.parameters.size() == 1) {
        handleSetDribblerCommand(cmd);
    } else if (cmd.name == "kick" && cmd.parameters.size() == 1) {
        handleKickCommand(cmd);
    } else {
        return false;
    }

    return true;
}

void ManualController::handleCommunicationMessage(std::string message) {
	if (Command::isValid(message)) {
        Command command = Command::parse(message);

		handleCommand(command);
	}
}

void ManualController::handleToggleSideCommand() {
	if (targetSide == Side::BLUE) {
		targetSide = Side::YELLOW;
	} else {
		targetSide = Side::BLUE;
	}

	com->send("target:" + Util::toString(targetSide));
}

void ManualController::handleTargetVectorCommand(const Command& cmd) {
    float x = Util::toFloat(cmd.parameters[0]);
    float y = Util::toFloat(cmd.parameters[1]);
    float omega = Util::toFloat(cmd.parameters[2]);

    robot->setTargetDir(x, y, omega);
}

void ManualController::handleTargetDirCommand(const Command& cmd) {
    Math::Deg dir = Math::Deg(Util::toFloat(cmd.parameters[0]));
    float speed = Util::toFloat(cmd.parameters[1]);
    float omega = Util::toFloat(cmd.parameters[2]);

    robot->setTargetDir(dir, speed, omega);
}

void ManualController::handleSetDribblerCommand(const Command& cmd) {
    float targetOmega = Util::toFloat(cmd.parameters[0]);

	robot->getDribbler()->setTargetOmega(targetOmega);
}

void ManualController::handleKickCommand(const Command& cmd) {
    int strength = Util::toInt(cmd.parameters[0]);

	robot->getCoilgun()->kick(strength);
}

std::string ManualController::getJSON() {
	std::stringstream stream;

    stream << "null";

    return stream.str();
}
