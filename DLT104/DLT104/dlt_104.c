/******************************************************************************
    版权所有：
    版 本 号: 1.0
    文 件 名: dlt_104.c
    生成日期: 
    作	 者：李科
    功能说明：实现《配电自动化系统应用DL/T634.5104-2009实施细则》
    其他说明：
    修改记录：
*******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "dlt_104_file.h"
#include "dlt_104_param.h"
#include "dlt_104_upgrad.h"
#include "dlt_104_port_com.h"
#include "dlt_104_init.h"
#include "dlt_104_call.h"
#include "dlt_104_time.h"
#include "dlt_104_soe.h"
#include "dlt_104_misc.h"
#include "dlt_104_lib.h"
#include "dlt_104_elect.h"


int dlt_104(char port)
{
	dlt_104_receive(port);

	init_process(port);

	call_process(port);
//	
	time_syn_process(port);

	time_read_process(port);
	
//	soe_YX_process(port);

//	soe_YC_process(port);

//	param_fixed_area_read_process(port);

//	param_read_process(port);
	
//	param_fixed_area_switch_process(port);
  	
//	param_set_process(port);
	
//	elect_process(port);

	test_link_process(port);

	S_frame_ack_process(port);

	return 0;
}



/******************************************************************************
    功能说明：DLT101进程初始化
    输入参数：无
    输出参数：无
    返 回 值：无
*******************************************************************************/
void dlt_104_init(char port)
{
	dlt_104_com_init(port);
	
	init_process_init(port);
	
	call_process_init(port);
	
	dlt_104_lib_init(port);
	
	time_process_init(port);

	soe_process_init(port);
	
	param_process_init(port);

	elect_process_init(port);

	test_init_process_init(port);
}












