#ifndef __CLUSTER_H__
#define __CLUSTER_H__

#include "common.h"
#include "faceboxlist.h"
#include <fstream>
#include <math.h>
#include <list>
#include <vector>

class ActData{
public:
	int mode;
	double val;
	ActData() { mode = -1; val = 0.0; };
	~ActData(){};
};

using namespace std;

class ModeData{

private:
	int mode;
	double mean;
	double varience;
	double meanSquare;
	std::vector< double > List;
	std::vector< double > squareList;
	double m_max; 
	double m_min;

public:
	ModeData() {
		mode = -1;
		mean = 0;
		varience = 0;
		meanSquare = 0;
		m_max = -5000.0;
		m_min = 5000.0;
	};
	ModeData(std::vector< ActData > data, int _mode) {
		mode = _mode;
		m_max = -5000.0;
		m_min = 5000.0;
		int dataSize = data.size();
		for (int i = 0; i < dataSize; i++) {
			if (data[i].mode == mode) {
				double val = data[i].val;
				mean += val;
				meanSquare += val*val;
				squareList.push_back(val*val);
				List.push_back(val);
				m_max = max((double)m_max, val);
				m_min = min((double)m_min, val);
			}
		}
		mean /= List.size();
		meanSquare /= squareList.size();
		setVarience();
	};
	//Copy Constructor
	ModeData(const ModeData &_data) {
		mode = _data.mode;
		mean = _data.mean;
		m_max = _data.m_max;
		m_min = _data.m_min;
		meanSquare = _data.meanSquare;
		varience = _data.varience;
		List = _data.List;
		squareList = _data.squareList;
	};



	~ModeData(){squareList.clear(); List.clear();};

	inline double getMean() {return mean;};
	inline double getMin() {return m_min;};
	inline double getMax() {return m_max;};
	inline double getVarience() { return varience; };
	inline void setVarience() { varience = meanSquare - (mean*mean); };
	inline int getSize() { return List.size(); };

// ================================================================

	inline int addData(ActData data) { 
		if (mode == data.mode) {
			double SquareListVal = meanSquare*squareList.size();
			double ListVal = mean*List.size();
			squareList.push_back(data.val*data.val);
			List.push_back(data.val);
			meanSquare = (SquareListVal + (data.val*data.val))/squareList.size();
			mean = (ListVal + data.val)/List.size();
			setVarience();
			m_max = max((double)m_max, data.val);
			m_min = min((double)m_min, data.val);
			return 1;
		}
		return 0;
	};

// ================================================================

	inline int subData(ActData data) {
		if(data.mode == mode) {
			double val = data.val;
			double squareVal = data.val*data.val;
			double squareListVal = meanSquare * squareList.size();
			double ListVal = mean*List.size();
			std::vector<double>::iterator it = List.begin();
			for (; it != List.end(); ++it) {
				if(*it == val) {
					List.erase(it);
					break;
				}
			}
			it = squareList.begin();
			for (; it != List.end(); ++it) {
				if(*it == squareVal) {
					squareList.erase(it);
					break;
				}
			}
			meanSquare = (squareListVal - squareVal)/squareList.size();
			mean = (ListVal - val)/List.size();
			setVarience();
			m_max = max((double)m_max, data.val);
			m_min = min((double)m_min, data.val);
			return 1;
		}
		return 0;
	};

// ================================================================

	inline void printData(ofstream &os)
	{
		os << "mode = " << mode << endl;
		for(unsigned int i = 0; i < List.size(); i++) {
			os << List[i] << "\t";
		}
		os << endl << endl;

	}


}; //ModeData

// ================================================================
// class Cluster
// ================================================================

class Cluster
{
public:

	Cluster(int _modes = 2) {
		m_modeSize = _modes;
		m_modesSet = 0;
		m_contData = 0;
		m_suggModeMean.reserve(m_modeSize);
		m_modes.reserve(m_modeSize);


	} // Constructor
	~Cluster() {
		m_dataList.clear(); 
		m_modes.clear(); 
}	// Destructor

	// ================================================================

	inline double UpdateCluster(double curAct)
	{
		float lamda = 0.1f;
		ActData data;
		m_contData = curAct*lamda + (1-lamda)*m_contData;
		data.val = m_contData;
		static int curI = 0;
		int curSize = m_dataList.size();
		if (curSize > 2000) {
			subVal();
			m_dataList.erase(m_dataList.begin());
			curSize--;
		}
		else m_dataList.push_back(data);

		if (curSize > 150 && !m_modesSet){
			setModes();
			for(int m = 0; m < m_modeSize; m++) {
				ModeData curMode(m_dataList, m);
				m_modes[m] = curMode;
			}
			m_modesSet = 1;
		}

		else if (curI > 500) {
			curI = 0;
			setModes();
			m_modes.clear();
			m_modesSet = 0;
			for(int m = 0; m < m_modeSize; m++) {
				ModeData curMode(m_dataList, m);
				m_modes[m] = curMode;
			}
			printData();
			m_modesSet = 2;
		}
		else {
			int closestMode = 0;
			double dist = 100000.0;
			for (int m = 0; m < m_modeSize; m++) {
				double curDist = data.val - m_suggModeMean[m];
				if (fabs(curDist) < dist) {
					dist = fabs(curDist);
					closestMode = m;
				}
			}
			data.mode = closestMode;
			m_modes[data.mode].addData(data);
		}


		curI++;
		//only works with 2 modes, change later
		if (m_modes.size() > 1)
			m_currThresh = min(m_modes[0].getMax(), m_modes[1].getMax()) - max(m_modes[0].getMin(), m_modes[1].getMin());
		else
			m_currThresh = 0.0;
		return m_currThresh;
		//return (find optimal position);
	}

// ================================================================

private:

	std::vector < ActData > m_dataList;
	std::vector < ModeData > m_modes;
	std::vector < double > m_suggModeMean;
	double m_min, m_max, m_mean, m_contData;
	double m_currThresh;
	int m_modesSet, m_modeSize;

// ================================================================


	inline int subVal() {
		int mode = m_dataList[0].mode;
		int rtn = m_modes[mode].subData(m_dataList[0]);
		m_dataList.erase(m_dataList.begin());
		return rtn;
	}

// ================================================================

	inline void setModes(int maxTrials = 100) {
		int dataSize = m_dataList.size();
		int i, m, trial = 0;
		int notStable = 1;
		std::vector < double > curModeMean;
		curModeMean.reserve(m_modeSize);
		std::vector < int > curModeSize;
		curModeSize.reserve(m_modeSize);
		while (notStable && trial < maxTrials) {
			for (i = 0; i < dataSize; i++) {
				int closestMode = 0;
				double dist = 100000.0;
				for (m = 0; m < m_modeSize; m++) {
					double curDist = m_dataList[i].val - m_suggModeMean[m];
					if (fabs(curDist) < dist) {
						dist = fabs(curDist);
						closestMode = m;
						curModeMean[m] += m_dataList[i].val;
						curModeSize[m]++;
					}
				}
				m_dataList[i].mode = closestMode;
			}
			notStable = 0;
			for (m = 0; m < m_modeSize; m++) {
				double curMean = m_suggModeMean[m];
				m_suggModeMean[m] = curModeMean[m]/curModeSize[m];
				if (m_suggModeMean[m] != curMean) notStable++;
			}
			trial++;
		}
	}


// ================================================================

	inline void printData()
	{
		ofstream os;
		os.open("c:\\timedata.out", ios::trunc);
		os << "data size = " << m_dataList.size() << "; Threshold = " << m_currThresh << endl;
		for(unsigned int i = 0; i < m_dataList.size(); i++) 
			os << m_dataList[i].val << "\t";
		for(int m = 0; m < m_modeSize; m++) {
			os << "mode = " << m << endl;
			os << "mean = " << m_modes[m].getMean() << "; varience = " << m_modes[m].getVarience() << "; size = " 
			   << m_modes[m].getSize() << endl;

			m_modes[m].printData(os);
		}
		os << endl << endl;
		os.close();



	}
	///////////////////////////////////////////////////////////


}; // Cluster

#endif //__CLUSTER_H__
