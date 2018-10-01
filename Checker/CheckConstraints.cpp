#include<vector>
#include<iostream>

#include "CheckConstraints.h"

namespace xxf {
	
	void CheckConstraints::initSet() {
		for (int n = 0; n < inputins.graph().nodes().size(); n++)
		{
			Parent.push_back(-1); //��ʼ��ÿ���ڵ�ĸ��ڵ��Ϊ-1��ÿ���ڵ�Ķ�Ϊ0
			Degree.push_back(0);
		}
	}

	int CheckConstraints::findParent(int node) {
		
		if (Parent[node] == -1)return node;
		return findParent(Parent[node]);
	}

	void CheckConstraints::UnionSet(int source, int target) {
		
		int sourceParent = findParent(source);
		int targetParent = findParent(target);
		Parent[sourceParent] = targetParent;
	
	}

	bool CheckConstraints::checkLoop() {
	
		bool flag = true;   //��¼�Ƿ��л��ı��
		initSet();
		for (auto edgeOnTree = outputsolve.edges().begin(); edgeOnTree != outputsolve.edges().end(); ++edgeOnTree)
		{
			Degree[edgeOnTree->source()]++;  //�ڵ�ȼ�1
			Degree[edgeOnTree->target()]++;

			obj += edgeOnTree->length();

			int sp = findParent(edgeOnTree->source());   //�ж��Ƿ��л�
			int tp = findParent(edgeOnTree->target());
			if (sp == tp) {
				std::cout << "Exist loop." << std::endl;
				flag = false;
			}
			UnionSet(edgeOnTree->source(), edgeOnTree->target());
		}
		return flag;
	}

	int CheckConstraints::checkNodeDegree() {
	 
		bool nodecoverFlag = true;   //�ڵ��Ƿ�ȫ�������Ǳ��
		bool degreeconsFlag = true;   //�ڵ�Ķ�Լ�����
		for (int i = 0; i < inputins.graph().nodes().size(); i++)
		{
			if (Degree[i] == 0) {
				std::cout << "The node "<<i <<" is not covered."<< std::endl;
				nodecoverFlag = false;
			}
			else if (Degree[i] > inputins.maxdegree())
			{
				std::cout << "The degree of node " << i << " dissatisfies constraint ." << std::endl;
				degreeconsFlag = false;
			}
			else;
		}
		if (nodecoverFlag) {
			if (degreeconsFlag)return 0;   //ͬʱ��������Լ��
			else return 1;   //����ڵ㸲��Լ�����������Լ��
		}
		else {
			if (degreeconsFlag)return 2;   //�����Լ����������ڵ㸲��Լ��
			else return 3;    //����Լ����������
		}
	}

	int CheckConstraints::GetObj() {
		return obj;
	}
}