#ifndef CHECKCONSTRINTS_H
#define CHECKCONSTRAINTS_H

#include <vector>
#include <iostream>
#include"../Solver/DegreeConstrainedMST.pb.h"

namespace xxf {

class CheckConstraints
{
	#pragma region Type
public:
	xxf::Input inputins;      //��������
	xxf::Output outputsolve;     //������
	std::vector<int> Parent;   //��¼���ڵ��һά����
	std::vector<int> Degree;   //��¼�ڵ�Ķ�����
	int obj;
	#pragma endregion Type

	#pragma region Constant
	#pragma endregion Constant

	#pragma region Constructor

public:
	CheckConstraints(const xxf::Output &output,const xxf::Input &input) {  //��ʼ��������������
		inputins = input;
		outputsolve = output;
		obj = 0;
	}
	void initSet();//��ʼ��ParentAndDegree����
	int findParent(int node); //�ݹ���Ҹ��׽ڵ�
	void UnionSet(int source, int target);//���½ڵ���뼯��
	int GetObj();

	#pragma endregion Constructor

	#pragma region method

public:
	bool checkLoop();
	int checkNodeDegree();

	#pragma endregion method

};

}


#endif
