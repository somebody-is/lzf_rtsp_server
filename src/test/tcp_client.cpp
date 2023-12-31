#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    // 创建 epoll 实例
    int epoll_fd = epoll_create1(0);
    
    // 创建定时器
    int timer_fd = timerfd_create(CLOCK_MONOTONIC, 0);
    
    // 将定时器文件描述符添加到 epoll 实例中
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = timer_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, timer_fd, &event);

    // 设置定时器的超时时间（例如，1秒）
    struct itimerspec timer_spec;
    timer_spec.it_value.tv_sec = 1;
    timer_spec.it_value.tv_nsec = 0;
    timer_spec.it_interval.tv_sec = 1;
    timer_spec.it_interval.tv_nsec = 0;
    timerfd_settime(timer_fd, 0, &timer_spec, NULL);

    while (1) {
        // 等待事件发生
        struct epoll_event events[10];
        int num_events = epoll_wait(epoll_fd, events, 10, -1);
        printf("%d\n",num_events);
        // 处理事件
        for (int i = 0; i < num_events; ++i) {
            if (events[i].data.fd == timer_fd) {
                // 处理定时器事件，执行定时任务
                printf("Timer event occurred\n");
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, timer_fd, &event);
                timerfd_settime(timer_fd, 0, &timer_spec, NULL);
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, timer_fd, &event);
            }
        }
    }

    // 关闭文件描述符
    close(timer_fd);
    close(epoll_fd);

    return 0;
}
