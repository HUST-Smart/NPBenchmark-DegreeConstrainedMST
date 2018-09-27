1.示例图为4节点5条边的无向图，各边的长度已在图中标示，此图节点的最大度为3；
合法解保存在LegalSolu.jpg中，不满足给定节点度约束的非法解保存在IllegalSolu_Degree.jpg中，不满足最小生成树无环且覆盖所有节点约束的非法解保存在IllegalSolu_Cycle.jpg中。

2.输入数据：2
            限制节点的最大度为2，寻找满足约束的最小生成树。

3.输出数据：LegalSolu.jpg:                  路径：1-4,4-2,2-3；路径长度：20
	    IllegalSolu_Degree.jpg:         路径：1-4,4-2,4-3；路径长度：20
	    IllegalSolu_Cycle.jpg:          路径：1-4,1-3,4-3；路径长度：20