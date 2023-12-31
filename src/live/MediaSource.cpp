#include "MediaSource.h"
#include"stdio.h"

MediaSource::MediaSource(std::string filename)
{
    _filename = filename;
    _fp = fopen(filename.c_str(), "rb");
    if (!_fp)
    {
        LOGE("读取文件%s失败\n", filename.c_str());
        return;
    }
}

MediaSource::~MediaSource()
{
    fclose(_fp);
}
