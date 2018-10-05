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
	std::vector<std::vector<xxf::Edge>> AdjList;   //������ͼ���ڽӱ�
	std::vector<int> Degree;   //��¼�ڵ�Ķ�����
	std::vector<bool> visited;   //��ǽڵ��Ƿ񱻷���
	int obj;
	#pragma endregion Type

	#pragma region Constant
	#pragma endregion Constant

	#pragma region Constructor

public:
	CheckConstraints(const xxf::Output &output, const xxf::Input &input); //��ʼ��������������
	bool isCyclicUtil(int node, int parent);
	int GetObj();

	#pragma endregion Constructor

	#pragma region method

public:
	bool isCyclic();   //�ж��Ƿ��л�����
	int checkNodeDegree();    //�жϽڵ�Ķ��Ƿ����Ҫ��

	#pragma endregion method

};

}


#endif
