#ifndef CAMERATRANSLATOR_H
#define CAMERATRANSLATOR_H

#include <math.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

class CameraTranslator {

public:
	typedef std::vector <int> CameraMapRow;
	typedef std::vector <CameraMapRow> CameraMap;

	struct WorldPosition {
		WorldPosition() : dx(0.0f), dy(0.0f), distance(0.0f), angle(0.0f) {}
		WorldPosition(float dx, float dy, float distance, float angle) : dx(dx), dy(dy), distance(distance), angle(angle) {}

		float dx;
		float dy;
		float distance;
		float angle;
	};

	struct CameraPosition {
		CameraPosition() : x(0), y(0) {}
		CameraPosition(int x, int y) : x(x), y(y) {}

		int x;
		int y;
	};

	CameraTranslator() : A(0.0f), B(0.0f), C(0.0f), k1(0.0f), k2(0.0f), k3(0.0f), horizon(0.0f), cameraWidth(0), cameraHeight(0) {}

	void setConstants(
		float A, float B, float C,
		float k1, float k2, float k3,
		float horizon, float distortionFocus,
		int cameraWidth, int cameraHeight);

	bool loadUndistortionMapping(std::string xFilename, std::string yFilename);
	WorldPosition getWorldPosition(int cameraX, int cameraY);
	CameraPosition getCameraPosition(float dx, float dy);
	CameraTranslator::CameraPosition undistort(int x, int y);
	CameraTranslator::CameraPosition distort(int x, int y);

private:
	friend std::istream& operator >> (std::istream& inputStream, CameraMap& map);

	float A;
	float B;
	float C;
	float k1;
	float k2;
	float k3;
	float horizon;
	float distortionFocus;
	int cameraWidth;
	int cameraHeight;
	CameraMap xMap;
	CameraMap yMap;

};

std::istream& operator >> (std::istream&, CameraTranslator::CameraMap&);

#endif
