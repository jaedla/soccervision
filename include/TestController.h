#ifndef TESTCONTROLLER_H
#define TESTCONTROLLER_H

#include "BaseAI.h"
#include "Vision.h"
#include "DebouncedButton.h"
#include "Util.h"
#include "Config.h"

#include <map>
#include <string>

class TestController : public BaseAI {

public:
	enum TargetMode { LEFT = -1, INLINE = 0, RIGHT = 1, UNDECIDED = 2 };

	typedef std::map<std::string, std::string> Messages;
	typedef Messages::iterator MessagesIt;
	typedef std::map<int, std::string> Params;

	class State : public BaseAI::State {

	public:
		State(TestController* ai) : BaseAI::State(ai), ai(ai) {}

	protected:
		TestController* ai;

	};

	class ManualControlState : public State {

	public:
		ManualControlState(TestController* ai) : State(ai) {}
		void step(float dt, Vision::Results* visionResults, Robot* robot, float totalDuration, float stateDuration, float combinedDuration);

	};

	class WatchBallState : public State {

	public:
		WatchBallState(TestController* ai) : State(ai) {}
		void step(float dt, Vision::Results* visionResults, Robot* robot, float totalDuration, float stateDuration, float combinedDuration);

	};

	class WatchGoalState : public State {

	public:
		WatchGoalState(TestController* ai) : State(ai) {}
		void step(float dt, Vision::Results* visionResults, Robot* robot, float totalDuration, float stateDuration, float combinedDuration);

	};

	class WatchGoalBehindState : public State {

	public:
		WatchGoalBehindState(TestController* ai) : State(ai) {}
		void step(float dt, Vision::Results* visionResults, Robot* robot, float totalDuration, float stateDuration, float combinedDuration);

	};

	class SpinAroundDribblerState : public State {

	public:
		SpinAroundDribblerState(TestController* ai) : State(ai) {}
		void step(float dt, Vision::Results* visionResults, Robot* robot, float totalDuration, float stateDuration, float combinedDuration);

	};

	class DriveToState : public State {

	public:
		DriveToState(TestController* ai) : State(ai), x(Config::fieldWidth / 2.0f), y(Config::fieldHeight / 2.0f), orientation(0.0f) {}
		void onEnter(Robot* robot, Parameters parameters);
		void step(float dt, Vision::Results* visionResults, Robot* robot, float totalDuration, float stateDuration, float combinedDuration);

		float x;
		float y;
		float orientation;

	};

	class TurnByState : public State {

	public:
		TurnByState(TestController* ai) : State(ai), angle(0.0f) {}
		void onEnter(Robot* robot, Parameters parameters);
		void step(float dt, Vision::Results* visionResults, Robot* robot, float totalDuration, float stateDuration, float combinedDuration);

		float angle;

	};

	class FindBallState : public State {

	public:
		void onEnter(Robot* robot, Parameters parameters);
		FindBallState(TestController* ai) : State(ai), searchDir(1.0f), lastSearchTime(-1.0), lastTurnTime(-1.0), timeSinceLastSearch(-1.0) {}
		void step(float dt, Vision::Results* visionResults, Robot* robot, float totalDuration, float stateDuration, float combinedDuration);

	private:
		float searchDir;
		double lastSearchTime;
		double lastTurnTime;
		double timeSinceLastSearch;

	};

	class FetchBallFrontState : public State {

	public:
		FetchBallFrontState(TestController* ai) : State(ai), forwardSpeed(0.0f), startBrakingDistance(-1.0f), startBrakingVelocity(-1.0f), lastBallDistance(-1.0f) {}
		void onEnter(Robot* robot, Parameters parameters);
		void step(float dt, Vision::Results* visionResults, Robot* robot, float totalDuration, float stateDuration, float combinedDuration);

	private:
		void reset(Robot* robot);

		float forwardSpeed;
		float startBrakingDistance;
		float startBrakingVelocity;
		float lastBallDistance;

	};

	class FetchBallDirectState : public State {

	public:
		FetchBallDirectState(TestController* ai) : State(ai), forwardSpeed(0.0f), nearLine(false) {}
		void onEnter(Robot* robot, Parameters parameters);
		void step(float dt, Vision::Results* visionResults, Robot* robot, float totalDuration, float stateDuration, float combinedDuration);

	private:
		float forwardSpeed;
		bool nearLine;

	};

	class FetchBallBehindState : public State {

	public:
		FetchBallBehindState(TestController* ai) : State(ai), hadBall(false), lastBallWasGhost(false), forwardSpeed(0.0f), lastTargetAngle(0.0f), lostBallTime(-1.0), timeSinceLostBall(0.0), lostBallVelocity(0.0f), startBallDistance(-1.0f), lastBallDistance(-1.0f), targetMode(TargetMode::UNDECIDED), avgBallGoalDistance(10) {}
		void onEnter(Robot* robot, Parameters parameters);
		void step(float dt, Vision::Results* visionResults, Robot* robot, float totalDuration, float stateDuration, float combinedDuration);

	private:
		bool hadBall;
		bool reversePerformed;
		bool turnAroundPerformed;
		bool lastBallWasGhost;
		float forwardSpeed;
		float lastTargetAngle;
		float searchDir;
		double lostBallTime;
		double timeSinceLostBall;
		float lostBallVelocity;
		float startBallDistance;
		float lastBallDistance;
		TargetMode targetMode;
		Math::Avg avgBallGoalDistance;

	};

	class FetchBallNearState : public State {

	public:
		FetchBallNearState(TestController* ai) : State(ai), enterDistance(-1.0f), enterVelocity(0.0f), enterBallDistance(-1.0f), smallestForwardSpeed(-1.0f) {}
		void onEnter(Robot* robot, Parameters parameters);
		void step(float dt, Vision::Results* visionResults, Robot* robot, float totalDuration, float stateDuration, float combinedDuration);

	private:
		float enterDistance;
		float enterVelocity;
		float enterBallDistance;
		float smallestForwardSpeed;

	};

	class AimState : public State {

	public:
		AimState(TestController* ai) : State(ai), lastKickTime(-1.0), foundOwnGoalTime(-1.0), avoidBallSide(TargetMode::UNDECIDED), searchGoalDir(0.0f), performReverse(Decision::UNDECIDED), reverseTime(0.0f), avoidBallDuration(0.0f), nearLine(false) {}
		void onEnter(Robot* robot, Parameters parameters);
		void step(float dt, Vision::Results* visionResults, Robot* robot, float totalDuration, float stateDuration, float combinedDuration);

	private:
		double lastKickTime;
		double foundOwnGoalTime;
		float searchGoalDir;
		TargetMode avoidBallSide;
		Decision performReverse;
		float reverseTime;
		float avoidBallDuration;
		bool nearLine;
	};

	class DriveCircleState : public State {

	public:
		DriveCircleState(TestController* ai) : State(ai) {}
		void step(float dt, Vision::Results* visionResults, Robot* robot, float totalDuration, float stateDuration, float combinedDuration);

	private:
		static float getCircleTargetAngle(float start, float time, float period);

	};

	class AccelerateState : public State {

	public:
		AccelerateState(TestController* ai) : State(ai), forwardSpeed(0.0f) {}
		void onEnter(Robot* robot, Parameters parameters);
		void step(float dt, Vision::Results* visionResults, Robot* robot, float totalDuration, float stateDuration, float combinedDuration);

	private:
		float forwardSpeed;

	};

	class ReturnFieldState : public State {

	public:
		ReturnFieldState(TestController* ai) : State(ai) {}
		void step(float dt, Vision::Results* visionResults, Robot* robot, float totalDuration, float stateDuration, float combinedDuration);

	private:
		float driveTowardsGoalTime;

	};
	
	TestController(Robot* robot, Communication* com);
	~TestController();

	void onEnter();
	void onExit();
	void reset();

    bool handleCommand(const Command& cmd);
	void handleTargetVectorCommand(const Command& cmd);
	void handleDribblerCommand(const Command& cmd);
	void handleKickCommand(const Command& cmd);
	void handleResetCommand();
	void handleToggleGoCommand();
	void handleToggleSideCommand();
	void handleDriveToCommand(const Command& cmd);
	void handleTurnByCommand(const Command& cmd);
	void handleParameterCommand(const Command& cmd);

	float getTargetAngle(float goalX, float goalY, float ballX, float ballY, float D, TargetMode targetMode = TargetMode::INLINE);

    void step(float dt, Vision::Results* visionResults);
	Side getTargetSide() { return targetSide; }
	void dbgs(std::string key, std::string value) { messages[key] = value; }
	void dbg(std::string key, int value) { messages[key] = Util::toString(value); }
	void dbg(std::string key, float value) { messages[key] = Util::toString(value); }
	void dbg(std::string key, double value) { messages[key] = Util::toString(value); }
	void dbg(std::string key, bool value) { messages[key] = value ? "true" : "false"; }
	std::string getJSON();

private:
	void setupStates();
	void updateVisionDebugInfo(Vision::Results* visionResults);
	void resetLastBall();
	void setLastBall(Object* ball);
	Object* getLastBall(Dir dir = Dir::ANY);

	DebouncedButton toggleGoBtn;
	DebouncedButton toggleSideBtn;
	DebouncedButton resetBtn;

	Side targetSide;
	float manualSpeedX;
	float manualSpeedY;
	float manualOmega;
	int manualDribblerSpeed;
	int manualKickStrength;
	float blueGoalDistance;
	float yellowGoalDistance;
	float lastTargetGoalAngle;
	Vision::ColorDistance whiteDistance;
	Vision::ColorDistance blackDistance;
	double lastTurnAroundTime;
	bool isRobotOutFront;
	bool isRobotOutRear;

	double lastCommandTime;
	double lastBallTime;

	Object* lastBall;

	Params parameters;
	Messages messages;

};

#endif // TESTCONTROLLER_H
