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
	float sum = 0;
	for (int i = 0; i < size; i++) {
		sum += x[i] * y[i];
	}
	sum /= size;

	return sum - average(x, size) * average(y, size);
}

float pearson(float* x, float* y, int size) {
	return cov(x, y, size) / (sqrt(var(x, size)) * sqrt(var(y, size)));
}



Line linear_reg(Point** points, int size) {
	float* x = (float*)std::malloc(size * sizeof(float));
	assert(x != NULL);
	float* y = (float*)std::malloc(size * sizeof(float));
	assert(y != NULL);
	for (int i = 0; i < size; i++) {
		x[i] = points[i]->x;
		y[i] = points[i]->y;
	}
	float a;
	float b;
	float vari = var(x, size);
	if (abs(vari - 0)< 0.0000001) {
		a = 0;
		b = 0;
	}
	else {
		a = cov(x, y, size) / var(x, size);
		b = average(y, size) - a * (average(x, size));
	}
	free(x);
	free(y);
	return Line(a, b);
}

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





