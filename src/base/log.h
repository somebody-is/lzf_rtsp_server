#ifndef RTSPSERVER_LOG_H
#define RTSPSERVER_LOG_H
#include <time.h>
#include <string>
#include <vector>
#include <cstring>
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

static std::string getTime() {
    const char* time_fmt = "%Y-%m-%d %H:%M:%S";
    time_t t = time(nullptr);
    char time_str[64];
    strftime(time_str, sizeof(time_str), time_fmt, localtime(&t));
    
    return time_str;
}

static int level = 1;

#define LOGD(format, ...) if(0>=level) fprintf(stderr,"\033[34m[DEBUG] [%s] [%s %s:%d]: " format "\033[0m\n", getTime().data(),__FILENAME__,__FUNCTION__,__LINE__,##__VA_ARGS__)
#define LOGW(format, ...) if(1>=level) fprintf(stderr,"\033[33m[WARNING] [%s] [%s %s:%d]: " format "\033[0m\n", getTime().data(),__FILENAME__,__FUNCTION__,__LINE__,##__VA_ARGS__)
#define LOGI(format, ...) if(2>=level) fprintf(stderr,"\033[32m[INFO] [%s] [%s %s:%d]: " format "\033[0m\n", getTime().data(),__FILENAME__,__FUNCTION__,__LINE__,##__VA_ARGS__)
#define LOGE(format, ...) if(3>=level) fprintf(stderr,"\033[31m[ERROR] [%s] [%s %s:%d]: " format "\033[0m\n",getTime().data(),__FILENAME__,__FUNCTION__,__LINE__,##__VA_ARGS__)
#endif