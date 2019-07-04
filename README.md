# Timestamp
  c语言时间戳与时间互转，适用于嵌入式平台，需要10位时间戳数据，转换结果精确到秒。
  #### 导出函数
```
/*
解析时间戳信息到年月日时分秒
@param ts 10位时间戳信息,4字节
@param timeZone 时区,相对格林威治时间的偏移
@param *buffer 用于存储转换结果的缓冲区
*/
void praseTimestamp(long ts,int timeZone,int *buffer)
```

```
/*
将时间信息转换成时间戳
@param timeZone 时区,相对格林威治时间的偏移
@param *buffer 本地时间记录缓冲区
*/
long makeTimestamp(int timeZone,int *buffer)
```