#include "general_functions.h"
//#include <sys/time.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>

#include "rtc.h"
#include <time.h>

General_Functions::General_Functions()
{
}

vector<string> General_Functions::splitString(string str,string pattern)
{
    string::size_type pos;
    vector<string> result;
    str+=pattern;//扩展字符串以方便操作
    int size=str.size();

    for(int i=0; i<size; i++)
    {
        pos=str.find(pattern,i);
        if(pos<size)
        {
            string s=str.substr(i,pos-i);
            result.push_back(s);
            i=pos+pattern.size()-1;
        }
    }
    return result;
}

/**
 * @brief General_Functions::getCurrentUTC
 * @return
 * 返回当前UTC时间。单位是微秒
 */
unsigned long long General_Functions::getCurrentUTC()
{
    /*struct timeval nowTime;
    gettimeofday(&nowTime, NULL);
    unsigned long long utc = (unsigned long long)nowTime.tv_sec * 1000000 + (unsigned long long)nowTime.tv_usec;
    return utc;*/
	
	RTC_TimeTypeDef time_struct;
	RTC_GetTime(RTC_Format_BIN, &time_struct);
	RTC_DateTypeDef date_struct;
	RTC_GetDate(RTC_Format_BIN, &date_struct);
	
	//printf("y=%d,m=%d,d=%d,h=%d,m=%d,s=%d\n",date_struct.RTC_Year,date_struct.RTC_Month,date_struct.RTC_Date,
	//time_struct.RTC_Hours,time_struct.RTC_Minutes,time_struct.RTC_Seconds);
	
	struct tm *gm_date;
	time_t utc =0;
	gm_date = localtime(&utc);//1970 0 1 000,此句必须有，否则会死机
	gm_date->tm_year = date_struct.RTC_Year + 100;
	gm_date->tm_mon = date_struct.RTC_Month - 1;
	gm_date->tm_mday = date_struct.RTC_Date;
	gm_date->tm_wday = date_struct.RTC_WeekDay;
	gm_date->tm_hour = time_struct.RTC_Hours;
	gm_date->tm_min = time_struct.RTC_Minutes;
	gm_date->tm_sec = time_struct.RTC_Seconds;
	
	utc = mktime(gm_date);
	//printf("unix timestamp:%d \r\n",utc);
	unsigned long long lutc = (unsigned long long)utc*1000; //llrp中的UTC是精确到毫秒的
	return lutc;

}

char *General_Functions::IPV4ToChar(llrp_u32_t ip){
    /*struct in_addr t; //注意，这里要求输入的IP是大端模式的。所以要将IP颠倒字节序

    unsigned char tmp;
    tmp = *(((unsigned char *)&ip)+0);
    *(((unsigned char *)&ip)+0) = *(((unsigned char *)&ip)+3);
    *(((unsigned char *)&ip)+3) = tmp;
    tmp = *(((unsigned char *)&ip)+1);
    *(((unsigned char *)&ip)+1) = *(((unsigned char *)&ip)+2);
    *(((unsigned char *)&ip)+2) = tmp;

    t.s_addr = ip;

    return inet_ntoa(t); //注意，这里返回的指针是静态内存中的指针，超过其作用范围，即会失效。
		*/
}

string General_Functions::snBytesToString(unsigned char* sn)
{
    string str;
    char b = sn[0];
    str += b;   // B
    char l = sn[1];
    str += l;   // L
    char year = sn[2];
    int iyear = (int)year;
    char chyear[10] = {0};
    sprintf(chyear,"%02d",iyear);
    str += chyear;

    char month = sn[3];
    int imonth = (int)month;
    char chmonth[10] = {0};
    sprintf(chmonth,"%02d",imonth);
    str += chmonth;


    int devicenum = (sn[4] << 24)
            + (sn[5] << 16)
            + (sn[6] << 8)
            + (sn[7]);

    char chdevicenum[20] = {0};
    sprintf(chdevicenum,"%06d",devicenum);
    str += chdevicenum;

    return str;
}



