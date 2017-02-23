/******************************************************************************
    版权所有：
    版 本 号: 1.0
    文 件 名: dlt_104_param.h
    生成日期: 
    作    者：李科
    功能说明：实现《配电自动化系统应用DL/T634.5104-2009实施细则》的装载参数
    其他说明：基于104规约被控站的方式来与控制站进行装载参数
    修改记录：
*******************************************************************************/
#ifndef _DLT_104_PARAM_
#define _DLT_104_PARAM_

int param_fixed_area_read_process(unsigned char port);

int param_read_process(unsigned char port);

int param_fixed_area_switch_process(unsigned char port);

extern int param_set_process(unsigned char port);

extern int param_process_init(unsigned char port);

#endif /* _DLT_104_PARAM_ */
