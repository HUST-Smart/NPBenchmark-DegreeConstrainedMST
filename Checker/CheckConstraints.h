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
	xxf::Input inputins;      //输入算例
	std::vector<std::vector<xxf::Edge>> AdjList;   //输出结果图的邻接表
	std::vector<int> Degree;   //记录节点的度数组
	std::vector<bool> visited;   //标记节点是否被访问
	int obj;
	#pragma endregion Type

	#pragma region Constant
	#pragma endregion Constant

	#pragma region Constructor

public:
	CheckConstraints(const xxf::Output &output, const xxf::Input &input); //初始化算例和输出结果
	bool isCyclicUtil(int node, int parent);
	int GetObj();

	#pragma endregion Constructor

	#pragma region method

public:
	bool isCyclic();   //判断是否有环存在
	int checkNodeDegree();    //判断节点的度是否符合要求

	#pragma endregion method

};

}


#endif
