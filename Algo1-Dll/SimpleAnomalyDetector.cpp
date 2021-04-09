
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

using namespace std;
SimpleAnomalyDetector::SimpleAnomalyDetector() {
    cout << "SimpleAnomalyDetector created" << endl;
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

/*void SimpleAnomalyDetector::learnNormal(const TimeSeries& ts) {
    TimeSeries timeSeries = ts;
    map<string, vector<float>> tmpMap = timeSeries.getMap();
    float tmp, tmpMax;
    int size = tmpMap.begin()->second.size();
    /* We run a loop on the categories, and then another loop
     * on the categories to the right of the first one.
     */
    /*for (auto itr = tmpMap.begin(); itr != tmpMap.end(); itr++) {//outer loop
        tmpMax = 0;
        string f2;
        for (auto itr2 = itr; itr2 != tmpMap.end(); itr2++) {//inner loop
            if (itr2 == itr) {
                continue;
            }
            tmp = abs(pearson(itr->second.data(), itr2->second.data(), size));
            if (tmp > tmpMax) {//to get maximum correlation
                tmpMax = tmp;
                f2 = itr2->first;
            }
        }
        if (tmpMax >= 0.9) {//check if max correlation > threshold (0.5) for circle
            correlatedFeatures tmpCf;
            tmpCf.feature1 = itr->first;
            tmpCf.feature2 = f2;
            tmpCf.corrlation = tmpMax;
            vector<Point*> points;
            for (int i = 0; i < size; i++) {
                points.push_back(new Point(tmpMap[tmpCf.feature1][i], tmpMap[tmpCf.feature2][i]));
            }
            Point** pointsArr = points.data();
        
            tmpCf.lin_reg = linear_reg(pointsArr, size);
            tmpMax = 0;
            for (int i = 0; i < size; i++) {
                float tmpDev = dev(pointsArr[i][0], tmpCf.lin_reg);
                if (tmpDev > tmpMax) {//getting max threshold for dev of individual points in the vector
                    tmpMax = tmpDev;
                }
         
            }
            tmpCf.threshold = tmpMax;
            cf.push_back(tmpCf);
            for (int i = 0; i < size; i++) {
                points.push_back(new Point(tmpMap[tmpCf.feature1][i], tmpMap[tmpCf.feature2][i]));
            }
        }
    }
}*/

void SimpleAnomalyDetector::learnNormal(const TimeSeries& ts) {
    vector<string> keys = ts.gettAttributes();
    for (int i = 0; i < keys.size() - 1; i++) {
        for (int j = i + 1; j < keys.size(); j++) {
            int size = ts.getMap()[keys[0]].size();
            vector<float> xVector = ts.getMap()[keys[i]];
            vector<float> yVector = ts.getMap()[keys[j]];
            float* xCoords = xVector.data();
            float* yCoords = yVector.data();
            correlatedFeatures c_f;
            //build the struct of the correlatedFeature
            c_f.feature1 = keys[i];
            c_f.feature2 = keys[j];
            c_f.corrlation = pearson(xCoords, yCoords, size);
            // Discard correlations below 90%
            if (abs(c_f.corrlation) < 0.9) {
                continue;
            }
            vector<Point*> graph = floatsToPoints(xVector, yVector);
            c_f.lin_reg = linear_reg(graph.data(), size);
            c_f.threshold = 0;
            for (int k = 0; k < graph.size(); k++) {
                float newThresh = abs(c_f.lin_reg.f(xCoords[k]) - yCoords[k]);
                c_f.threshold = max(c_f.threshold, newThresh);
                delete graph[k];
            }
            cf.push_back(c_f);
        }
    }
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
	if (p > threshold) {
		size_t len = ts.getRowSize();
		correlatedFeatures c;
		c.feature1 = f1;
		c.feature2 = f2;
		c.corrlation = p;
		c.lin_reg = linear_reg(ps, len);
		c.threshold = findThreshold(ps, len, c.lin_reg) * 1.1; // 10% increase
		cf.push_back(c);
	}
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
    cout << "went in 1" << endl;
    TimeSeries ts(CSVfileName, l, size);
    learnNormal(ts);
    int sizeOfcf = getSize();
    char c[] = "no feature was found";
    for (int i = 0; i < sizeOfcf; i++) {
        char* temp;
        temp = &cf[i].feature1[0];
        if (!strcmp(temp, att)) {
            std::copy(cf[i].feature2.begin(), cf[i].feature2.end(), s);
            s[cf[i].feature2.size()] = '\0';
            return;
        }
        char* temp2;
        temp2 = &cf[i].feature2[0];
        cout << temp2 << endl;
        if (!strcmp(temp2, att)) {
            cout << "went in 6" << endl;
            //word = &cf[i].feature1[0];
            std::copy(cf[i].feature1.begin(), cf[i].feature1.end(), s);
            s[cf[i].feature1.size()] = '\0';
            return;
        }
        else {
            cout << "went in 7" << endl;
            continue;
        }
    }
    cout << "no most correlated feature was found" << endl;
}

void SimpleAnomalyDetector::trying(char* buffer) {
    std::string str = "linear_reg";
    std::copy(str.begin(), str.end(), buffer);
    buffer[str.size()] = '\0';
    cout << buffer << " is the most correlated feature" << endl;
}

int returnNum() {
    int x = 6;
    return x;
}

extern "C" _declspec(dllexport) void* CreateSAD() {
    return (void*) new SimpleAnomalyDetector();
}

extern "C" __declspec(dllexport) void MostCorrelatedFeature(SimpleAnomalyDetector* sad, const char* CSVfileName,  char** l, int size, const char* att, char* s) {
    return sad->mostCorrelatedFeature(CSVfileName,l,size, att, s);
}

extern "C" __declspec(dllexport) int returnANum() {
    return returnNum();
}

extern "C" _declspec(dllexport) void Analysis_AlgorithmType(SimpleAnomalyDetector* sad, char* buffer) {
    return sad->trying(buffer);
}
