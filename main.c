#define yyyy 0
#define MM 1
#define dd 2
#define HH 3
#define mm 4
#define ss 5

/*
本代码为8位平台单片机使用
其中int占2字节，long占4字节
请根据自己硬件平台修改
*/

//时钟缓冲区
xdata int g_time[6];

/*
解析时间戳信息到年月日时分秒
@param ts 10位时间戳信息,4字节
@param timeZone 时区,相对格林威治时间的偏移
@param *buffer 用于存储转换结果的缓冲区,转换后的结果每位占2字节
*/
void praseTimestamp(long ts,int timeZone,int *buffer) {
    //闰年标记
    bit leapYear = 0;
    unsigned char i = 0;
    
    //中国区需要补28800秒
    //计算时间戳的天数
    long days = (ts + 3600 * timeZone) / 86400 + 1;
    //四年周期数
    int year4 = (int)(days / 1461);
    //不足四年周期剩下的天数
    int remain = (int)(days % 1461);
    
    //得到时间戳范围内满足四年周期的年数
    buffer[yyyy] = 1970 + year4 * 4;
    
    //剩下的天数小于一年@365不处理
    if(remain < 365){
        //@占位保留
    //第二年@365+365
    }else if(remain < 730){
        buffer[yyyy]++;
        remain -= 365;
    //第三年@365+365+365
    }else if(remain < 1095){
        buffer[yyyy] += 2;
        remain -= 730;
    //第四年
    }else{
        buffer[yyyy] += 3;
        remain -= 1095;
        leapYear = 1;
    }
    
    //剩下一年内的天数 用12个月天数循环减
    //直到剩下天数<=0 则找到对应月份
    //剩下天数则为日期
    for(i=0;i<12;i++){
        
        //year4复用于暂存每月天数
        //闰年2月补一天
        year4 = (i == 1 && leapYear) ? (dayOfMonth[i] + 1) : dayOfMonth[i];
        
        //days复用于暂存减去当前月份天数的剩余天数
        days = remain - year4;
       
        if(days <= 0){
            
            buffer[MM] = i + 1;
            if(days == 0){
                buffer[dd] = year4;
               
            }else{
                    buffer[dd] = remain;
            }
            break;
        }
        remain = (int)days;
    }
    
    buffer[ss] = (int)(ts % 60);

    buffer[mm] = (int)(ts /60 % 60);
    
    while(ts > 86400){
        ts -= 86400;
    }
    
    if(ts == 86400){
        buffer[HH] = 0;
    }else{
        //中国区需要+8h
        buffer[HH] =(int)(ts / 3600) + timeZone;
    }
    
    if(buffer[HH] >= 24){
         buffer[HH] -= 24;
    }
}

/*
将时间信息转换成时间戳
@param timeZone 时区,相对格林威治时间的偏移
@param *buffer 本地时间记录缓冲区
*/
long makeTimestamp(int timeZone,int *buffer) {
    xdata long ts;
    xdata int remain;
    xdata int circle;
    bit leapYear = 0;
    int i = 0;

    //1970平,1971平,1972闰,1973平,
    
    leapYear = ((buffer[yyyy] & 3) == 0) ? 1 : 0;
    
    circle = buffer[yyyy] - 1970;
    ts = 126230400L * (circle / 4);
    
    remain = circle % 4;
    
    //0年(当前年)不处理
    
    if(remain == 1){
        //+365*24*3600
        ts += 31536000L;
    }else if(remain == 2){
        //+365*2*24*3600
        ts += 63072000L;
    }else if(remain == 3){
        //+365*2*24*3600+366*24*3600
        ts += 94694400L;
    }
    
    //处理当前月之前月份的天数的秒数
    for(i=0;i<buffer[MM]-1;i++){
        if(i==1 && leapYear){
            ts += (dayOfMonth[i] + 1) * 86400L;
        }else{
            ts += dayOfMonth[i] * 86400L;
        }
    }
    
    //处理当前日期之前的天数的秒数
    ts += 86400L * (buffer[dd] - 1);
    
    //处理当前天小时的秒数
    ts += 3600L * (buffer[HH]);
    
    //处理当前天分钟的秒数
    ts += 60L * (buffer[mm]);
    
    //加上当前天的秒数
    ts += buffer[ss];
    
    //减去时区差
    ts -= (timeZone * 3600);
    
    return ts;
}