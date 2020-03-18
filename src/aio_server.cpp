#include <errno.h>
#include <stdio.h>
#include <fcntl.h> 
#include <string.h> 
#include <inttypes.h>
#include <sys/eventfd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <string>
#include "aio_server.h"
#include "native_aio.h"

aio_context_t aioctx_;

context::context(aio_server& srv)
: srv_(srv)
{
}

int context::send_message()
{
	return 0;
}

int context::send_message_by_nowait(const std::string& data)
{
	if (data.size() == 0)
		return 0;

	write_msgs_.push_back(data);

	if (incomplete_write_ == 0)
		srv_.post_write(this);
	return 0;
}

int context::close()
{
	::close(clientfd_);
	if (incomplete_read_ == 0 && incomplete_write_ == 0)
	{
		delete this;
	}

	return 0;
}

aio_server::aio_server()
{
}

aio_server::~aio_server()
{

}

void aio_server::assert(int code)
{
	if (code < 0)
	{
		printf("errno[%d] errmsg[%s]\n", errno, strerror(errno));
	}
}

int aio_server::start(short port)
{
	{ //epoll
		epollfd_ = epoll_create(128);  
		if (epollfd_ == -1) {  
			return -1;
		} 
	} 

	{ //eventfd
		efd_ = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);  
		if (efd_ == -1) {  
			return -2; 
		}

		struct epoll_event epevent;
		epevent.events = EPOLLIN; //| EPOLLET;
		epevent.data.fd = efd_;
		//epevent.data.ptr = NULL; union epoll_data注意这里是union 
		if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, efd_, &epevent)) {  
			return -3;  
		}
	}

	{ // listen fd
		int one = 1;
		struct sockaddr_in serveraddr;
		bzero(&serveraddr, sizeof(serveraddr));
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_port = htons(port);
		serveraddr.sin_addr.s_addr = INADDR_ANY;
		listenfd_ = socket(AF_INET, SOCK_STREAM, 0);
		assert(setsockopt(listenfd_, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)));
		if (bind(listenfd_, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) != 0)
		{
			return -4;
		}
		assert(listen(listenfd_, 1024));
		struct epoll_event epevent;
		epevent.events = EPOLLIN; // | EPOLLET;
		epevent.data.fd = listenfd_;
		if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, listenfd_, &epevent)) 
		{  
			return -5; 
		}
	}

	assert(io_setup(128, &aioctx_));

	printf("event fd[%d]\n", efd_);
	printf("epoll fd[%d]\n", epollfd_);
	printf("listen fd[%d]\n", listenfd_);
	exit_ = false;
	return 0;
}

int aio_server::stop()
{
	return 0;
}

int aio_server::cycle()
{
	if (epollfd_ == -1) 
	{
		return -1;
	}

	struct epoll_event epevents[2];

	while(!exit_)
	{
		int nfds = epoll_wait(epollfd_, epevents, 2, 1000);
		for (int i=0; i<nfds; i++)
		{
			printf("event on fd[%d]\n", epevents[i].data.fd);
			if (epevents[i].data.fd==listenfd_)
			{
				process_accept();
			}

			if (epevents[i].data.fd==efd_)
			{
				process_aio();
			}

		}
	}

}

void aio_server::process_accept()
{
	context* ctx = new context(*this);
	socklen_t clilen;
	ctx->clientfd_ = accept(listenfd_,(sockaddr *)&ctx->clientaddr_, &clilen);
	printf("new client fd[%d]\n", ctx->clientfd_);
	if (ctx->clientfd_ != -1)
	{
		post_read(ctx);
	}
}

void aio_server::process_aio()
{
	uint64_t finished_aio;
	if (read(efd_, &finished_aio, sizeof(finished_aio)) != sizeof(finished_aio)) {  
		return;  
	}  

	printf("finished io number: %d\n", finished_aio);
	struct timespec tms;
	tms.tv_sec = 0;  
	tms.tv_nsec = 0;
	struct io_event events[128];
	int r,j;
	while (finished_aio > 0) 
	{ 
		r = io_getevents(aioctx_, 1, 128, events, &tms); 
		printf("io_getevents: %d\n", r);
		if (r > 0) 
		{  
			for (j = 0; j < r; ++j) 
			{  
				// 通知处理读|写异步完成
				context* ctx = (context*)(events[j].data);
				struct iocb *cb = (struct iocb *)(events[j].obj);

				printf("%s:%d %d\n", __FILE__, __LINE__,cb);
				printf("%s:%d %d %d %d\n", __FILE__, __LINE__, cb->aio_lio_opcode, IOCB_CMD_PREAD, IOCB_CMD_PWRITE);

				if (cb->aio_lio_opcode == IOCB_CMD_PREAD)
				{
					on_read(ctx, events[j].res);
				}
				else if(cb->aio_lio_opcode == IOCB_CMD_PWRITE)
				{
					on_write(ctx, events[j].res);
				}
				
			}  
			finished_aio -= r;
		}
	} 
}

int aio_server::post_read(context* ctx)
{
	printf("clientfd: %d efd:%d\n", ctx->clientfd_, efd_);
	struct iocb *cbs[1];
	memset(&ctx->iocb_read_, 0, sizeof(ctx->iocb_read_)); 
    ctx->iocb_read_.aio_fildes = ctx->clientfd_; 
    ctx->iocb_read_.aio_lio_opcode = IOCB_CMD_PREAD;/* command-specific options */ 
    ctx->iocb_read_.aio_buf = (uint64_t)ctx->read_buff;
    ctx->iocb_read_.aio_offset = 0; 
    ctx->iocb_read_.aio_nbytes = 4096;
	ctx->iocb_read_.aio_data = (uint64_t)ctx;
    ctx->iocb_read_.aio_flags |= IOCB_FLAG_RESFD;
    ctx->iocb_read_.aio_resfd = efd_;
    cbs[0] = &ctx->iocb_read_;
	assert(io_submit(aioctx_, 1, cbs));
	ctx->incomplete_read_++;

	printf("%s:%d %d\n", __FILE__, __LINE__,&ctx->iocb_read_);

	return 0;
}

int aio_server::on_read(context* ctx, int64_t res)
{
	ctx->incomplete_read_--;
	printf("on_read [%lld]\n", res);
	if (res > 0)
	{
		decoder_.append(ctx->read_buff, res);
		std::string msg;
		while(decoder_.fetch(msg))
		{
			process(*ctx, msg);
		}
		post_read(ctx);
	} 
	else
	{
		// 出错(或者对方关闭连接)应该关闭连接
		printf("error [%d] [%s]\n", res > 0, strerror(res));
		ctx->close();
	}
	return 0;
}

int aio_server::post_write(context* ctx)
{
	if (ctx->write_msgs_.empty())
	{
		return 0;
	}

	struct iocb *cbs[1];
	memset(&ctx->iocb_write_, 0, sizeof(ctx->iocb_write_)); 
    ctx->iocb_write_.aio_fildes = ctx->clientfd_; 
    ctx->iocb_write_.aio_lio_opcode = IOCB_CMD_PWRITE;/* command-specific options */ 
    ctx->iocb_write_.aio_buf = (uint64_t)ctx->write_msgs_.front().data();
    ctx->iocb_write_.aio_offset = 0; 
    ctx->iocb_write_.aio_nbytes = ctx->write_msgs_.front().size();
    ctx->iocb_write_.aio_data = (uint64_t)ctx;
    ctx->iocb_write_.aio_flags |= IOCB_FLAG_RESFD;
    ctx->iocb_write_.aio_resfd = efd_;
    cbs[0] = &ctx->iocb_write_;
	assert(io_submit(aioctx_, 1, cbs));
	ctx->incomplete_write_++;
	return 0;
}

int aio_server::on_write(context* ctx, int64_t res)
{
	ctx->incomplete_write_--;
	printf("on_write [%lld]\n", res);
	if (res > 0)
	{
		if (res == ctx->write_msgs_.front().size()) 
		{
			ctx->write_msgs_.pop_front();
			post_write(ctx);
		}
		else
		{
			printf("on_write error [%lld] != [%lld]\n", res, ctx->write_msgs_.front().size() );
			ctx->close();
		}
		
	} 
	else
	{
		// 出错(或者对方关闭连接)应该关闭连接
		printf("error [%d] [%s]\n", res > 0, strerror(res));
		ctx->close();
	}
	return 0;

	
}