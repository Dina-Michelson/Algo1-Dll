
/*
 * SimpleAnomalyDetector.cpp
 *
 *  Created on: 8 срхїз 2020
 *      Author: Eli
 */
#define _CRT_SECURE_NO_WARNINGS
#include "pch.h"
#include "SimpleAnomalyDetector.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>


using namespace std;
SimpleAnomalyDetector::SimpleAnomalyDetector() {
	threshold = 0.9;

}

SimpleAnomalyDetector::~SimpleAnomalyDetector() {
	// TODO Auto-generated destructor stub
}

Point** SimpleAnomalyDetector::toPoints(vector<float> x, vector<float> y) {
	Point** ps = new Point * [x.size()];
	for (size_t i = 0; i < x.size(); i++) {
		ps[i] = new Point(x[i], y[i]);
	}
	return ps;
}

float SimpleAnomalyDetector::findThreshold(Point** ps, size_t len, Line rl) {
	float max = 0;
	for (size_t i = 0; i < len; i++) {
		float d = abs(ps[i]->y - rl.f(ps[i]->x));
		if (d > max)
			max = d;
	}
	return max;
}



void SimpleAnomalyDetector::learnNormal(const TimeSeries& ts) {
    vector<string> atts = ts.gettAttributes();
    size_t len = ts.getRowSize();
    
    for (size_t i = 0; i < atts.size(); i++) {
        string f1 = atts[i];
        float max = 0;
        size_t jmax = 0;
        for (size_t j = i + 1; j < atts.size(); j++) { 
            float p = getCorrelation(f1, atts[j],ts) ;
            if (p > max) {
                max = p;
                jmax = j;
            }
        }
        string f2 = atts[jmax];
        Point** ps = toPoints(ts.getAttributeData(f1), ts.getAttributeData(f2));

        learnHelper(ts, max, f1, f2, ps);

        // delete points
        for (size_t k = 0; k < len; k++)
            delete ps[k];
        delete[] ps;
    }
}

float SimpleAnomalyDetector::getCorrelation(string& f1, string& f2, const TimeSeries& ts) {
    // Get the vector that contains the data of the first feature
    vector<float> colvector1 = ts.getMap()[f1];
    float* f1vals = &colvector1[0]; // Access the internal array of the vector
    // Get the vector that contains the data of the second feature
    vector<float> colvector2 = ts.getMap()[f2];
    float* f2vals = &colvector2[0]; // Access the internal array of the vector
    // Get the length of the vectors containing the features' data
    int size = colvector1.size();
    // Compute the correlation of these two features
    return pearson(f1vals, f2vals, size);
}

Line SimpleAnomalyDetector::getRegressionLine(string& f1, string& f2, const TimeSeries& ts) {
    // Get the vector that contains the data of the first feature
    vector<float> colvector1 = ts.getMap()[f1];
    float* f1vals = &colvector1[0]; // Access the internal array of the vector
    // Get the vector that contains the data of the second feature
    vector<float> colvector2 = ts.getMap()[f2];
    float* f2vals = &colvector2[0]; // Access the internal array of the vector
    // Get the length of the vectors containing the features' data
    int size = colvector1.size();
    // Since we've gotten to here, we know that the correlation >= 0.9
    // We have an array of x-coordinates, and an array of y-coordinates.
    // Now we'll combine them into a vector of Points
    vector<Point*> points = floatsToPoints(colvector1, colvector2);
    // We'll use this vector of points to calculate the line of regression
    Line reg = linear_reg(&points[0], size);
    // Free memory in points vector
    for (int i = 0; i < points.size(); i++) {
        delete points[i];
    }
    return reg;
}

float SimpleAnomalyDetector::getMaxDistFromLine(Line linreg, string& f1, string& f2, const TimeSeries& ts) {
    // Get the vector that contains the data of the first feature
    vector<float> colvector1 = ts.getMap()[f1];
    // Get the vector that contains the data of the second feature
    vector<float> colvector2 = ts.getMap()[f2];
    // Get the length of the vectors containing the features' data
    int size = colvector1.size();
    float maxDist = 0;
    for (int k = 0; k < size; k++) {
        Point point(colvector1[k], colvector2[k]);
        maxDist = max(dev(point, linreg), maxDist);
    }
    return maxDist;
}

//function for accepting 2 vectors of floats and returning a vector of pointers to points
vector<Point*> SimpleAnomalyDetector::floatsToPoints(vector <float> x, vector <float> y) {
    vector<Point*> points_vector;
    //creating a vector of pointers to points to send to lin_reg
    int s = x.size();
    for (int i = 0; i < s; i++) {
        Point* p = new Point(x[i], y[i]);
        points_vector.push_back(p);
    }
    return points_vector;
}

void SimpleAnomalyDetector::learnHelper(const TimeSeries& ts, float p/*pearson*/, string f1, string f2, Point** ps) {
	
		size_t len = ts.getRowSize();
		correlatedFeatures c;
		c.feature1 = f1;
		c.feature2 = f2;
		c.corrlation = p;
		c.lin_reg = linear_reg(ps, len);
		c.threshold = findThreshold(ps, len, c.lin_reg) * 1.1; // 10% increase
		cf.push_back(c);
	
}

vector<AnomalyReport> SimpleAnomalyDetector::detect(const TimeSeries& ts) {
	vector<AnomalyReport> v;
	for_each(cf.begin(), cf.end(), [&v, &ts, this](correlatedFeatures c) {
		vector<float> x = ts.getAttributeData(c.feature1);
		vector<float> y = ts.getAttributeData(c.feature2);
		for (size_t i = 0; i < x.size(); i++) {
			if (isAnomalous(x[i], y[i], c)) {
				string d = c.feature1 + "-" + c.feature2;
				v.push_back(AnomalyReport(d, (i + 1)));
			}
		}
		});
	return v;
}


bool SimpleAnomalyDetector::isAnomalous(float x, float y, correlatedFeatures c) {
	return (abs(y - c.lin_reg.f(x)) > c.threshold);
}

int SimpleAnomalyDetector::getSize() {
    return SimpleAnomalyDetector::cf.size();
}

void SimpleAnomalyDetector::mostCorrelatedFeature(const char* CSVfileName, char** l, int size, const char* att, char* s) {
    TimeSeries ts(CSVfileName, l, size);
    learnNormal(ts);
    int sizeOfcf = getSize();
    for (int i = 0; i < sizeOfcf; i++) {
        char* temp;
        temp = &cf[i].feature1[0];
        if (!strcmp(temp, att)) {
            std::copy(cf[i].feature2.begin(), cf[i].feature2.end(), s);
            s[cf[i].feature2.size()] = '\0';
            return;
        }
        else {
            continue;
        }
    }
    s[0] = ' ';
    s[1] = '\0';
}


/*void SimpleAnomalyDetector::getAnomalyTimeSteps(const char* CSVfileName, char** l, int size, const char* oneWay, char* f, char* f2) {
    TimeSeries newTs(CSVfileName, l, size);
    vector< AnomalyReport> ar = detect(newTs);
    int size2 = ar.size();
    vector<int> floatVector;
    int arSize = ar.size();
    string temperary;
    string temperary2;
    for (int i = 0; i < arSize; i++) {
        char* temp;
        temp = &ar[i].description[0];
        if (!strcmp(temp, oneWay)) {
            floatVector.push_back(ar[i].timeStep);
        }
    }
    
    if (floatVector.size() != 0) {
        int k = 0;
        for (int j = 0; j < floatVector.size(); j++) {
            if (temperary.length() < 507) {
                int temp1 = floatVector[j];
                string temp(to_string(temp1));
                temperary += temp;
                temperary += ' ';
            }
            else {
                int temp2 = floatVector[j];
                string temp3(to_string(temp2));
                temperary2 += temp3;
                temperary2 += ' ';
            }
        }
        cout << temperary << endl;
        cout << temperary2 << endl;
    }
    
    else {
        string ns = "no timesteps";
        std::copy(ns.begin(), ns.end(), f);
        f[ns.size()] = '\0';
        std::copy(ns.begin(), ns.end(), f2);
        f2[ns.size()] = '\0';
        return;
    }
    
    std::copy(temperary.begin(), temperary.end(), f);
    f[temperary.size()-1] = '\0';
    std::copy(temperary2.begin(), temperary2.end(), f2);
    f2[temperary2.size() - 1] = '\0';
    
    return;
}*/

void SimpleAnomalyDetector::getAnomalyTimeSteps(const char* CSVfileName, char** l, int size, const char* oneWay, char* f) {
    TimeSeries newTs(CSVfileName, l, size);
    vector< AnomalyReport> ar = detect(newTs);
    int size2 = ar.size();
    vector<int> floatVector;
    int arSize = ar.size();
    string temperary;
    for (int i = 0; i < arSize; i++) {
        char* temp;
        temp = &ar[i].description[0];
        if (!strcmp(temp, oneWay)) {
            floatVector.push_back(ar[i].timeStep);
        }
    }

    if (floatVector.size() != 0) {
        for (int j = 0; j < floatVector.size(); j++) {
                int temp1 = floatVector[j];
                string temp(to_string(temp1));
                temperary += temp;
                temperary += ' ';
        } 
    }
    else {
        string ns = "no timesteps";
        std::copy(ns.begin(), ns.end(), f);
        f[ns.size()] = '\0';
    }
    std::copy(temperary.begin(), temperary.end(), f);
    f[temperary.size() - 1] = '\0';
    return;
}

int SimpleAnomalyDetector::getAnomalyTimeStepsSize(const char* CSVfileName, char** allAtts, int numAtts, const char* atts) {
    TimeSeries newTs(CSVfileName, allAtts, numAtts);
    vector< AnomalyReport> anomalies = detect(newTs);
    int numAnomalies = anomalies.size();
    vector<int> timesteps;
    for (int i = 0; i < numAnomalies; i++) {
        char* currAtts = &anomalies[i].description[0];
        if (!strcmp(currAtts, atts)) {
            timesteps.push_back(anomalies[i].timeStep);
        }
    }

    int numChars = 0;
    if (!timesteps.empty()) {
        for (int j = 0; j < timesteps.size(); j++) {
            int currTimestep = timesteps[j];
            numChars += currTimestep == 0 ? 1 : (int)(log10(currTimestep) + 1);
            numChars++; // for space
        }

    }

    return numChars;
}


extern "C" _declspec(dllexport) void* CreateSAD() {
    return (void*) new SimpleAnomalyDetector();
}

extern "C" __declspec(dllexport) void MostCorrelatedFeature(SimpleAnomalyDetector* sad, const char* CSVfileName,  char** l, int size, const char* att, char* s) {
    return sad->mostCorrelatedFeature(CSVfileName,l,size, att, s);
}

extern "C" __declspec(dllexport) void getTimeSteps(SimpleAnomalyDetector * sad, const char* CSVfileName, char** l, int size, const char* oneway, char* f) {
    return sad->getAnomalyTimeSteps(CSVfileName, l, size, oneway,  f);
}

extern "C" __declspec(dllexport) int getTimeStepsSize(SimpleAnomalyDetector * sad, const char* CSVfileName, char** allAtts, int numAtts, const char* atts) {
    return sad->getAnomalyTimeStepsSize(CSVfileName, allAtts, numAtts, atts);
}


