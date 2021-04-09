/*
 *  Anomaly_Detection.cpp
 *  Created on: 20 10 2020
 *  Author: Dina michelson
 */

#include "pch.h" // use stdafx.h in Visual Studio 2017 and earlier
#include <iostream>
#include <cmath>
#include <cassert> 
#include <cstdlib>
#include "anomaly_detection_util.h"
using namespace std;

/*function for calculating the average I added for usefulness*/
float average(float* x, int size) {
	float sum = 0.0;
	for (int i = 0; i < size; i++) {
		sum += x[i];
	}
	return sum / size;
}

float var(float* x, int size) {
	float u = average(x, size);
	float sigma = 0.0;
	for (int i = size - 1; i > -1; i--) {
		sigma += x[i] * x[i];
	}
	float div = sigma / size;
	return div - (u * u);
}

float cov(float* x, float* y, int size) {
	float avX = average(x, size);
	float avY = average(y, size);
	float sigma = 0.0;
	for (int i = size - 1; i > -1; i--) {
		sigma += (x[i] - avX) * (y[i] - avY);
	}
	return sigma / size;
}

float pearson(float* x, float* y, int size) {
	float sqrtVarX = sqrt(var(x, size));
	float sqrtVarY = sqrt(var(y, size));
	float pear = cov(x, y, size) / (sqrtVarX * sqrtVarY);
	return pear;
}

Line linear_reg(Point** points, int size) {
	//creating 2 seperate arrays for the x and y coordinates
	float* x = (float*)std::malloc(size * sizeof(float));
	assert(x != NULL);
	float* y = (float*)std::malloc(size * sizeof(float));
	assert(y != NULL);
	//loop for creating seperate arrays
	for (int i = size - 1; i > -1; i--) {
		Point p = *points[i];
		x[i] = p.x;
		y[i] = p.y;
	}
	float a = cov(x, y, size) / var(x, size);
	float avX = average(x, size);
	float avY = average(y, size);
	float b = avY - (a * avX);
	Line l(a, b);

	//freeing the allocated memory to avoid memory leak
	free(x);
	free(y);
	return l;
}

/*Line linear_reg(Point** points, int size) {
	float x[size], y[size];
	for (int i = 0; i < size; i++) {
		x[i] = points[i]->x;
		y[i] = points[i]->y;
	}
	float a = cov(x, y, size) / var(x, size);
	float b = average(y, size) - a * (average(x, size));

	return Line(a, b);
}*/

float dev(Point p, Point** points, int size) {
	//calculating the difference in y's between the point and the line
	Line l = linear_reg(points, size);
	float y = l.f(p.x);
	return std::abs(p.y - y);
}

float dev(Point p, Line l) {
	//calculating the difference in y's between the point and the line
	float y = l.f(p.x);
	return std::abs(p.y - y);
}





