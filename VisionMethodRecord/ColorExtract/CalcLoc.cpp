#include "stdafx.h"
#include "CalcLoc.h"
CalcLoc::CalcLoc()
{

}

CalcLoc::~CalcLoc()
{

}

void CalcLoc::getHUMoment()
{
	vecmu.resize(vecpoly.size());
	for (int i = 0; i < vecmu.size(); i++)
		vecmu[i] = moments(vecpoly[i], false);
}

void CalcLoc::getCenter()
{
	this->getHUMoment();
	m_Pcenter.resize(vecpoly.size());
	m_Ecenter.resize(vececllipse.size());
	m_center = Point2f(0, 0);
	for (int i = 0; i < vecmu.size(); i++)
	{
		m_Pcenter[i] = Point2f(vecmu[i].m10 / vecmu[i].m00, vecmu[i].m01 / vecmu[i].m00);
		m_center += m_Pcenter[i];
	}
	for (int i = 0; i < vececllipse.size(); i++)
	{
		m_Ecenter[i] = vececllipse[i].center;
		m_center += m_Ecenter[i];
	}
	m_center /= (float)(m_Pcenter.size() + m_Ecenter.size());
}

void CalcLoc::printResult(CalcLoc &dst)
{
	ofstream fresult;
	fresult.open("result.txt", ios::in | ios::ate);
	if (fresult.is_open())
		fresult << dst.m_center << endl;
	
}

void CalcLoc::printResult(time_t nowtime)
{
	ofstream fresult;
	fresult.open("result.txt", ios::in | ios::ate);
	if (fresult.is_open())
		fresult << ctime(&nowtime) << endl;
}