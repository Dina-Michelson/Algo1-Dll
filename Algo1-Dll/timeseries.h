/*
 * timeseries.h
 *
 *  Created on: 26 срхїз 2020
 *      Author: Eli
 */

#ifndef TIMESERIES_H_
#define TIMESERIES_H_
#include <iostream>
#include <string.h>
#include <fstream>
#include<map>
#include <vector>
#include <string.h>
#include <sstream>
//#include <bits/stdc++.h>
#include <algorithm>
#include "anomaly_detection_util.h"

using namespace std;

class TimeSeries {


	map<string, vector<float>> ts;
	vector<string> atts;
	size_t dataRowSize;
	vector<float> floatVector;
public:
	
	TimeSeries(){}
	TimeSeries(const char* CSVfileName, char** l, int size) {
		
		ifstream in(CSVfileName);
		string head;
		in >> head;
		string att;
		stringstream hss(head);
		if (!in.is_open())printf("opening failed");//changed
		int k = 0;
		while (k < size) {
			vector <float> v;
			ts[l[k]] = v;
			atts.push_back(l[k]);
			k++;
		}
		while (!in.eof()) {

			string line;
			in >> line;
			string val;
			stringstream lss(line);
			int i = 0;
			while (getline(lss, val, ',')) {
				ts[atts[i]].push_back(stof(val));
				i++;
			}
		}

		in.close();
		dataRowSize = ts[atts[0]].size();
		//cout<<"timeseries created"<<endl;
	}

	const vector<float>& getAttributeData(string name)const {
		return ts.at(name);
	}



	const vector<string>& gettAttributes()const {
		return atts;
	}

	size_t getRowSize()const {
		return dataRowSize;
	}

	map<string, vector<float>> getMap() const {
		return map<string, vector<float>>(ts);
	}

	vector<string> getFeatures() const {
		return this->atts;
	}

	void find_lin_reg(const char* attA, const char* attB, char* f) {
		vector<float> v1 = getAttributeData(attA);
		vector<float> v2 = getAttributeData(attB);
		vector<int> floatVector;
		vector<Point*> pointv = floatsToPoints(v1, v2);
		Line l = linear_reg(pointv.data(), v1.size());
		int arSize = 4;
		string temperary;
		floatVector.push_back(0);
		floatVector.push_back(l.b);
		if (l.a == 0) {
			floatVector.push_back(1);
			floatVector.push_back(l.a + l.b);
		}
		else {
			floatVector.push_back((-l.b) / l.a);
			floatVector.push_back(0);
		}
		
		if (floatVector.size() != 0) {
			int k = 0;
			for (int j = 0; j < 4; j++) {
				int temp1 = floatVector[j];
				string temp(to_string(temp1));
				temperary += temp;
				temperary += ' ';
			}
			std::copy(temperary.begin(), temperary.end(), f);
			f[temperary.size() - 1] = '\0';
			return;
		}
		std::copy(temperary.begin(), temperary.end(), f);
		//f = &temperary[0];
		f[temperary.size() - 1] = '\0';
		return;
	}

	int getAPointsSize(const char* attA, const char* attB) {
		int twenty1 = 21;
		return twenty1;
	}

	/*void find_lin_reg(int &size,  const char* attA, const char* attB) {
			vector<float> v1 = getAttributeData(attA);
			vector<float> v2 = getAttributeData(attB);
			vector<Point*> pointv = floatsToPoints(v1, v2);
			//vector<float> floatVector;
			Line l = linear_reg(pointv.data(), v1.size());
			cout << "got here" << endl;
			float zero = 0;
			float lb = l.b;
			float one = 1;
			float lalb = l.a + l.b;
			float sub = (-l.b) / l.a;
			cout << zero << endl;
			floatVector.push_back(zero);
			cout << "got here too" << endl;
			floatVector.push_back(lb);
			if (l.a == 0) {
				floatVector.push_back(1);
				floatVector.push_back(lalb);
			}
			else {
				floatVector.push_back(sub);
				floatVector.push_back(0);
			}
			for (int i = 0; i < 4; i++) {
				cout << floatVector[i] << endl;;
			}
			size = 4;
		}*/

	vector<Point*> floatsToPoints(vector <float> x, vector <float> y) {
		vector<Point*> points_vector;
		//creating a vector of pointers to points to send to lin_reg
		int s = x.size();
		for (int i = 0; i < s; i++) {
			Point* p = new Point(x[i], y[i]);
			points_vector.push_back(p);
		}
		return points_vector;
	}

	const vector<float>& getVector()const {
		return floatVector;
	}




	~TimeSeries() {

	}
};

/*extern "C" __declspec(dllexport) void findLinReg(TimeSeries * ts, const char* attA, const char* attB,char*f, char* f2) {
	return ts->find_lin_reg(attA, attB, f,f2);
}*/

extern "C" __declspec(dllexport) void findLinReg(TimeSeries * ts,  const char* attA, const char* attB, char* f) {
	return ts->find_lin_reg( attA, attB,f);
}


extern "C" __declspec(dllexport) int getAPointsSize(TimeSeries * ts, const char* attA, const char* attB) {
	return ts->getAPointsSize(attA, attB);
}


#endif /* TIMESERIES_H_ */
