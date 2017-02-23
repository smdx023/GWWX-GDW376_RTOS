/******************************************************************************
    版权所有：
    版 本 号: 1.0
    文 件 名: dlt_104_upgrad.h
    生成日期: 
    作    者：李科
    功能说明：实现《配电自动化系统应用DL/T634.5104-2009实施细则》的软件升级
    其他说明：基于104规约被控站的方式来与控制站进行软件升级
    修改记录：
*******************************************************************************/
#ifndef _DLT_104_UPGRAD_
#define _DLT_104_UPGRAD_


extern int dlt_104_process_upgrad(void);
extern int process_upgrad_execute(void);
extern int process_upgrad_cancel(void);
extern int process_upgrad_start(void);

extern int dlt_104_process_upgrad_init(void);

#endif /* _DLT_104_UPGRAD_ */