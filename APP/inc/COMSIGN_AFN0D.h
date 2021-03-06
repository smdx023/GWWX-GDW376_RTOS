//;===================================================================================     
//; 《国家电网公司企业标准--电力负荷管理系统数据传输规约》       Q/GDW 130-2005             
//;  用于通讯的表格,针对 AFN=0DH  
//;===================================================================================

#include "GDW130_CFG.h"

///{//;T2Fn_TCode用于指示各个Fn对应的类型
	//; 1--日冻结   2--抄表日冻结
	//; 3--月冻结   4--曲线
	//; 0--不支持的Fn
B1 T2Fn_TCode[]={\
0,\
1,1,1,1,1,1,1,1,\
2,2,2,2,0,0,0,0,\
3,3,3,3,3,3,3,3,\
1,1,1,1,1,1,1,0,\
3,3,3,3,3,3,0,0,\
1,1,1,3,0,0,0,0,\
1,1,3,3,0,0,0,0,\
1,1,1,3,3,3,0,0,\
3,3,0,0,0,0,0,0,\
4,4,4,4,0,0,0,0,\
4,4,4,4,4,4,4,4,\
4,4,4,4,4,4,4,0,\
4,4,4,4,4,4,4,4,\
4,4,4,4,0,0,0,0,\
1,1,1,1,1,1,0,0,\
1,1,1,0,0,0,0,0,\
1,3,0,0,0,0,0,0,\
0,4,0,0,0,0,0,0,\
};
//}

///{//;T2Fn_TCode用于指示各个Fn对应的数据的保存类型
	//; S--数据在页中的起始地址   L--数据的长度
  //; T--0Xab   a=1 包含终端抄表时间  b=1 包含费率数
#define LI(S,L,T)   S+L*0x10000+T*0x1000000
B4 T2Fn_SCode[139]={\
LI(  0, 0,0x00),\
LI(  9,85,0x11),\
LI( 94,85,0x11),\
LI(179,70,0x11),\
LI(249,70,0x11),\
LI(319,20,0x01),\
LI(339,20,0x01),\
LI(359,20,0x01),\
LI(379,20,0x01),\
LI(  9,85,0x11),\
LI( 94,85,0x11),\
LI(179,70,0x11),\
LI(249,70,0x11),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  8,85,0x11),\
LI( 93,85,0x11),\
LI(178,70,0x11),\
LI(248,70,0x11),\
LI(318,20,0x01),\
LI(338,20,0x01),\
LI(358,20,0x01),\
LI(378,20,0x01),\
LI(399,32,0x00),\
LI(431,24,0x00),\
LI(455,66,0x00),\
LI(521, 4,0x00),\
LI(  9,34,0x00),\
LI( 43, 4,0x00),\
LI( 47,52,0x10),\
LI(  0, 0,0x00),\
LI(398,32,0x00),\
LI(430,24,0x00),\
LI(454,66,0x00),\
LI(520, 4,0x00),\
LI(  8,34,0x00),\
LI( 42, 4,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  5, 3,0x00),\
LI(  8, 3,0x00),\
LI( 11, 3,0x00),\
LI( 14, 3,0x00),\
LI( 17, 3,0x00),\
LI( 20, 3,0x00),\
LI( 23, 3,0x00),\
LI( 26, 3,0x00),\
LI( 29, 2,0x00),\
LI( 31, 2,0x00),\
LI( 33, 2,0x00),\
LI( 35, 2,0x00),\
LI( 37, 2,0x00),\
LI( 39, 2,0x00),\
LI( 41, 2,0x00),\
LI(  0, 0,0x00),\
LI( 43, 4,0x00),\
LI( 47, 4,0x00),\
LI( 51, 4,0x00),\
LI( 55, 4,0x00),\
LI( 59, 4,0x00),\
LI( 63, 4,0x00),\
LI( 67, 4,0x00),\
LI( 71, 4,0x00),\
LI( 75, 2,0x00),\
LI( 77, 2,0x00),\
LI( 79, 2,0x00),\
LI( 81, 2,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00),\
LI(  0, 0,0x00)};
//}	           
                
