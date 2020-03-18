#ifndef _H_native_aio_H_
#define _H_native_aio_H_

#include <unistd.h> /* for syscall() */ 
#include <sys/syscall.h> /* for __NR_* definitions */ 
#include <linux/aio_abi.h> /* for AIO types and constants */ 

/**
 * 创建AIO上下文 
 * nr为最大同时驻留在里面的IO请求个数 
 * ctxp 指针,内核会填充这个值
 */
inline int io_setup(unsigned nr, aio_context_t *ctxp) 
{ 
    return syscall(__NR_io_setup, nr, ctxp); 
}

/**
 * 销毁AIO上下文
 */
inline int io_destroy(aio_context_t ctx) 
{ 
	return syscall(__NR_io_destroy, ctx); 
} 

/**
 * 提交一批AIO
 * ctx AIO上下文
 * nr AIO个数
 * iocbpp 数组指针
 * 返回值: 
 * ret = nr 表示所有成功接受 
 * 0 < ret < nr io_submit() 系统调用会从传入的cbs中一个一个处理iocb,如果提交的某个iocb失败,将停止并且返回iocb的索引号.没办法知晓错误的具体原因 
 * 0 > ret 在io_submit()开始之前发生了某种错误(e.g.比如AIO context非法) 或者提交 第一个失败
 */
inline int io_submit(aio_context_t ctx, long nr, struct iocb **iocbpp) 
{ 
	return syscall(__NR_io_submit, ctx, nr, iocbpp); 
}

/**
 * 获取一批完成的事件
 * ctx AIO上下文
 * min_nr events的最小个数 可以设置为1
 * max_nr 想要获取的events的最大个数
 * events 存储事件的指针
 * timeout 等待时间 timeout为NULL,表示永久等待 timeout为0,io_getevents()不阻塞
 */
inline int io_getevents(aio_context_t ctx, long min_nr, long max_nr, struct io_event *events, struct timespec *timeout) 
{ 
	return syscall(__NR_io_getevents, ctx, min_nr, max_nr, events, timeout);
}

/**
 * 取消 正常不需要调用 发现异常后调用close()让所有异步都失败
 */
inline int io_cancel(aio_context_t ctx, struct iocb *iocb, struct io_event *evt)
{ 
    return syscall(__NR_io_cancel, ctx, iocb, evt);
}

/**
 * 创建eventfd用来接收内核事件通知 
 * initval  counter初始值 一般为0
 * flags EFD_NONBLOCK | EFD_CLOEXEC
 */
inline int eventfd2(unsigned int initval, int flags) 
{
    return syscall(__NR_eventfd2, initval, flags);
}

extern aio_context_t aioctx_;

#endif //_H_native_aio_H_