/*
 * SimpleAnomalyDetector.h
 *
 *  Created on: 8 срхїз 2020
 *      Author: Eli
 */

#ifndef SIMPLEANOMALYDETECTOR_H_
#define SIMPLEANOMALYDETECTOR_H_

#include "anomaly_detection_util.h"
#include "AnomalyDetector.h"
#include <vector>
#include <algorithm>
#include <string.h>
#include <math.h>

struct correlatedFeatures {
	string feature1, feature2;  // names of the correlated features
	float corrlation;
	Line lin_reg;
	float threshold;
	float cx, cy;
};


class SimpleAnomalyDetector :public TimeSeriesAnomalyDetector {
protected:
	vector<correlatedFeatures> cf;
	float threshold;
public:
	SimpleAnomalyDetector();
	virtual ~SimpleAnomalyDetector();

	virtual void learnNormal(const TimeSeries& ts);
	virtual vector<AnomalyReport> detect(const TimeSeries& ts);
	vector<correlatedFeatures> getNormalModel() {
		return cf;
	}
	void setCorrelationThreshold(float threshold) {
		this->threshold = threshold;
	}
	void mostCorrelatedFeature(const char* CSVfileName, char** l, int size, const char* att, char* s);
	void trying(char* buffer);
	void getAnomalyTimeSteps(const char* CSVfileName, char** l, int size/*, const char* oneWay, const char* otherWay*/, float* f);
	// helper methods
protected:
	vector<Point*> floatsToPoints(vector <float> x, vector <float> y);
	virtual void learnHelper(const TimeSeries& ts, float p/*pearson*/, string f1, string f2, Point** ps);
	virtual bool isAnomalous(float x, float y, correlatedFeatures c);
	Point** toPoints(vector<float> x, vector<float> y);
	float findThreshold(Point** ps, size_t len, Line rl);
	//char* mostCorrelatedFeature(const char* CSVfileName, const char* att);
	//ronia helper methods
	Line getRegressionLine(string& f1, string& f2, const TimeSeries& ts);
	float getMaxDistFromLine(Line linreg, string& f1, string& f2, const TimeSeries& ts);
	float getCorrelation(string& f1, string& f2, const TimeSeries& ts);
	int getSize();
};



#endif /* SIMPLEANOMALYDETECTOR_H_ */
