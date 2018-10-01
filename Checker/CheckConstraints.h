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
	xxf::Output outputsolve;     //输出结果
	std::vector<int> Parent;   //记录父节点的一维数组
	std::vector<int> Degree;   //记录节点的度数组
	int obj;
	#pragma endregion Type

	#pragma region Constant
	#pragma endregion Constant

	#pragma region Constructor

public:
	CheckConstraints(const xxf::Output &output,const xxf::Input &input) {  //初始化算例和输出结果
		inputins = input;
		outputsolve = output;
		obj = 0;
	}
	void initSet();//初始化ParentAndDegree数组
	int findParent(int node); //递归查找父亲节点
	void UnionSet(int source, int target);//将新节点加入集合
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
