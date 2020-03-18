// #include<libaio.h>
// //#include<linux/aio_abi.h>
// #include<stdio.h>
// #include<sys/socket.h>
// #include<sys/eventfd.h>
// #include<sys/epoll.h>
// #include<netinet/in.h>
// #include<arpa/inet.h>
// #include<assert.h>
// #include<unistd.h>
// #include<stdlib.h>
// #include<errno.h>
// #include<string.h>
// #include<sys/types.h>
// #include<fcntl.h>
//#include<aio.h>

// #define NUM_EVENTS 16
// #define TEST_FILE "/home/server/service_mesh_master/log/combase.service_mesh.master.log.20191230"
// #define TEST_FILE_SIZE 1024
// #define ALIGN_SIZE 64
// #define RD_WR_SIZE 1024

// struct custom_iocb  
// {  
//     struct iocb iocb;  
//     int nth_request;  
// };
  
// void aio_callback(io_context_t ctx, struct iocb *iocb, long res, long res2)  
// {  
//     struct custom_iocb *iocbp = (struct custom_iocb *)iocb;  
//     printf("nth_request: %d, request_type: %s, offset: %lld, length: %lu, res: %ld, res2: %ld\n",   
//             iocbp->nth_request, (iocb->aio_lio_opcode == IO_CMD_PREAD) ? "READ" : "WRITE",  
//             iocb->u.c.offset, iocb->u.c.nbytes, res, res2);  
// }  
  
// int main(int argc, char *argv[])  
// {  
//     int efd, fd, epfd;  
//     io_context_t ctx;  
//     struct timespec tms;  
//     struct io_event events[NUM_EVENTS];  
//     struct custom_iocb iocbs[NUM_EVENTS];  
//     struct iocb *iocbps[NUM_EVENTS];  
//     struct custom_iocb *iocbp;  
//     int i, j, r;  
//     void *buf;  
//     struct epoll_event epevent;  
  
//     efd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);  
//     if (efd == -1) {  
//         perror("eventfd");  
//         return 2;  
//     }  
  
//     fd = open(TEST_FILE, O_RDWR | O_CREAT | O_DIRECT, 0644);  
//     if (fd == -1) {  
//         perror("open");  
//         return 3;  
//     }  
//     ftruncate(fd, TEST_FILE_SIZE);  
      
//     ctx = 0;  
//     if (io_setup(8192, &ctx)) {  
//         perror("io_setup");  
//         return 4;  
//     }  
  
//     if (posix_memalign(&buf, ALIGN_SIZE, RD_WR_SIZE)) {  
//         perror("posix_memalign");  
//         return 5;  
//     }  
//     printf("buf: %p\n", buf);  
  
//     for (i = 0, iocbp = iocbs; i < NUM_EVENTS; ++i, ++iocbp) {  
//         iocbps[i] = &iocbp->iocb;  
//         io_prep_pread(&iocbp->iocb, fd, buf, RD_WR_SIZE, i * RD_WR_SIZE);  
//         io_set_eventfd(&iocbp->iocb, efd);  
//         io_set_callback(&iocbp->iocb, aio_callback);  
//         iocbp->nth_request = i + 1;  
//     }  
  
//     if (io_submit(ctx, NUM_EVENTS, iocbps) != NUM_EVENTS) {  
//         perror("io_submit");  
//         return 6;  
//     }  
  
//     epfd = epoll_create(1);  
//     if (epfd == -1) {  
//         perror("epoll_create");  
//         return 7;  
//     }  
  
//     epevent.events = EPOLLIN | EPOLLET;  
//     epevent.data.ptr = NULL;  
//     if (epoll_ctl(epfd, EPOLL_CTL_ADD, efd, &epevent)) {  
//         perror("epoll_ctl");  
//         return 8;  
//     }  
  
//     i = 0;  
//     while (i < NUM_EVENTS) {  
//         uint64_t finished_aio;  
  
//         if (epoll_wait(epfd, &epevent, 1, -1) != 1) {  
//             perror("epoll_wait");  
//             return 9;  
//         }  
  
//         if (read(efd, &finished_aio, sizeof(finished_aio)) != sizeof(finished_aio)) {  
//             perror("read");  
//             return 10;  
//         }  
  
//         printf("finished io number: %llu64\n", finished_aio);  
      
//         while (finished_aio > 0) {  
//             tms.tv_sec = 0;  
//             tms.tv_nsec = 0;  
//             r = io_getevents(ctx, 1, NUM_EVENTS, events, &tms);  
//             if (r > 0) {  
//                 for (j = 0; j < r; ++j) {  
//                     ((io_callback_t)(events[j].data))(ctx, events[j].obj, events[j].res, events[j].res2);  
//                 }  
//                 i += r;  
//                 finished_aio -= r;  
//             }  
//         }  
//     }  
      
//     close(epfd);  
//     free(buf);  
//     io_destroy(ctx);  
//     close(fd);  
//     close(efd);  
//     remove(TEST_FILE);  
  
//     return 0;  
// } 

// #include <stdio.h> /* for perror() */ 
// #include <fcntl.h> /* O_RDWR */ 
// #include <string.h> /* memset() */ 
// #include <inttypes.h> /* uint64_t */
// #include <sys/eventfd.h>
// #include "aio_server.h"


// int cycle()
// {
//     int efd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);  
//     if (efd == -1) {  
//         perror("eventfd");  
//         return 2;  
//     }

    
// }

// int main() 
// { 
// 	aio_context_t ctx; 
// 	struct iocb cb; struct iocb *cbs[1]; 
// 	char data[4096]; 
// 	struct io_event events[1]; 
// 	int ret; 
// 	int fd;
// 	int i ; 
// 	for(i=0;i<4096;i++) 
// 	{ 
// 		data[i]=i%50+60; 
// 	} 
// 	fd = open("./testfile", O_RDWR | O_CREAT,S_IRWXU);
// 	if (fd < 0) 
// 	{ 
// 		perror("open error"); 
// 		return -1; 
// 	} 
	
//     ctx = 0;
//     ret = io_setup(128, &ctx); 
//     printf("after io_setup ctx:%ld",ctx); 
//     if (ret < 0)
//     { 
//     	perror("io_setup error"); 
//     	return -1; 
//     } /* setup I/O control block */ 
//     memset(&cb, 0, sizeof(cb)); 
//     cb.aio_fildes = fd; 
//     cb.aio_lio_opcode = IOCB_CMD_PWRITE;/* command-specific options */ 
//     cb.aio_buf = (uint64_t)data; 
//     cb.aio_offset = 0; 
//     cb.aio_nbytes = 4096;
//     cb.aio_flags |= IOCB_FLAG_RESFD;
//     cb.aio_resfd = 0;
//     cbs[0] = &cb;
// 	ret = io_submit(ctx, 1, cbs);
// 	if (ret != 1) 
// 	{ 
// 		if (ret < 0) 
// 			perror("io_submit error"); 
// 		else
// 			fprintf(stderr, "could not sumbit IOs"); 
// 		return -1; 
// 	} /* get the reply */ 
    
//     ret = io_getevents(ctx, 1, 1, events, NULL); 
//     printf("%d\n", ret); 
//     struct iocb * result = (struct iocb *)events[0].obj; 
//     printf("reusult:%Ld",result->aio_buf); 
//     ret = io_destroy(ctx); 
//     if (ret < 0)
//     { 
//     	perror("io_destroy error"); 
//     	return -1; 
//     } 
//     return 0;
// }

#include <errno.h>
#include <stdio.h> /* for perror() */ 
#include <fcntl.h> /* O_RDWR */ 
#include <string.h> /* memset() */ 
#include <inttypes.h> /* uint64_t */
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