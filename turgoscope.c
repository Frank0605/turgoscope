#include<reg52.h>				           //头文件
#define uchar unsigned char		     //宏定义
#define uint unsigned int		       // 6T 工作模式
#define LCD1602_dat P0			       //9V供电

sbit LCD1602_rs = P1^0;					   //I/O 定义
//sbit LCD1602_rw = P2^6;					 //I/O 定义
sbit LCD1602_e = P1^1;
sbit led_1 = P1^2;		             //指示灯IO口
sbit chuqi = P2^0;		             //出气电磁阀
sbit jinqi = P2^1;		             //气泵
sbit beep  = P1^3;		             //蜂鸣器
sbit key_1 = P3^2;		             //独立按键

uchar cs_1;                        // 测试变量
uint data_1,data_2,ys,ms,cs,shu,PD,time_1,time_xl;
uchar AP_H,AP_L,jc_1,zc_1,xl,xl_1,state,pjs[10],pj_s,pj_t,timer,zhi;
unsigned long shu1,cc;
bit jc,jc1,wc,PD1,wc_1,wc_2,GO,pj_1;

void delay(uint T)							   //延时程序
{
	while(T--);
}

void LCD1602_write(uchar order,dat)				  //1602一个字节处理
{
    LCD1602_e=0;
    LCD1602_rs=order;
    LCD1602_dat=dat;
    LCD1602_e=1;
    delay(1000);
    LCD1602_e=0;																								     
}

void LCD1602_writebyte(uchar *prointer)				   //1602 字符串处理
{
    while(*prointer!='\0')
    {
        LCD1602_write(1,*prointer);
        prointer++;
    }
}

void LCD1602_cls()									 //1602初始化
{
	LCD1602_write(0,0x01);     //1602 清屏指令
	delay(1500);
	LCD1602_write(0,0x38);     // 功能设置8位、5*7点阵
	delay(1500);
	LCD1602_write(0,0x0c);     //设置光标不显示开关、不显示光标、字符不闪烁
	LCD1602_write(0,0x06);
	LCD1602_write(0,0xd0);
	delay(1500);
}

void show()	    //显示程序
{
	if(state==0)	    //初始化显示界面
	{
		LCD1602_write(0,0x80);
		LCD1602_writebyte("Begin:          ");
		LCD1602_write(0,0xC0);
		LCD1602_write(1,0x30+shu1/1000000%10);	 //显示压力传感器模块频率
		LCD1602_write(1,0x30+shu1/100000%10);
		LCD1602_write(1,0x30+shu1/10000%10);
		LCD1602_write(1,0x30+shu1/1000%10);
		LCD1602_write(1,0x30+shu1/100%10);
		LCD1602_write(1,0x30+shu1/10%10);
		LCD1602_write(1,0x30+shu1%10);   
		LCD1602_writebyte("  ");   
		LCD1602_write(1,0x30+0);	//气压转换后的数据
		LCD1602_write(1,0x30+0);
		LCD1602_write(1,0x30+0);
		LCD1602_writebyte("mmHg");
	}else if(state==1)		 //充气血压检测界面
	{
		LCD1602_write(0,0x80);
		LCD1602_writebyte("Inflating:      ");
		LCD1602_write(0,0xC0);
		LCD1602_write(1,0x30+shu1/1000000%10);	 //显示压力传感器模块频率
		LCD1602_write(1,0x30+shu1/100000%10);
		LCD1602_write(1,0x30+shu1/10000%10);
		LCD1602_write(1,0x30+shu1/1000%10);
		LCD1602_write(1,0x30+shu1/100%10);
		LCD1602_write(1,0x30+shu1/10%10);
		LCD1602_write(1,0x30+shu1%10); 
		LCD1602_writebyte("  "); 
		LCD1602_write(1,0x30+shu/100%10); //显示当前压力
		LCD1602_write(1,0x30+shu/10%10);
		LCD1602_write(1,0x30+shu%10);
		LCD1602_writebyte("mmHg"); //单位 mmHg
	}else if(state==2)	 //放气检测血压界面   显示压力数据
	{
		LCD1602_write(0,0x80);
		LCD1602_writebyte("Measure:        ");
		LCD1602_write(0,0xC0);
		LCD1602_writebyte("         ");
		LCD1602_write(1,0x30+shu/100%10); //显示当前压力
		LCD1602_write(1,0x30+shu/10%10); //显示当前压力
		LCD1602_write(1,0x30+shu%10);
		LCD1602_writebyte("mmHg");	//单位 mmHg
	}else		 //血压检测结果显示界面
	{
				
		LCD1602_write(0,0x80);
		LCD1602_writebyte("SBP:");
		LCD1602_write(1,0x30+AP_H/100%10);	   //收缩压（高压）显示数据
		LCD1602_write(1,0x30+AP_H/10%10);
		LCD1602_write(1,0x30+AP_H%10);
		LCD1602_writebyte("    Pulse");	
		LCD1602_write(0,0xC0);
		LCD1602_writebyte("DBP:");
		LCD1602_write(1,0x30+AP_L/100%10);	 //舒张压（低压）显示数据
		LCD1602_write(1,0x30+AP_L/10%10);
		LCD1602_write(1,0x30+AP_L%10);
		LCD1602_writebyte("mmHg "); //单位 mmHg
		LCD1602_write(1,0x30+xl_1/100%10); //显示脉搏 数据
		LCD1602_write(1,0x30+xl_1/10%10);
		LCD1602_write(1,0x30+xl_1%10);
		LCD1602_writebyte(" ");	
	}
}

uint mean()		//数据求平均数（返回求出的平均数）  取出两个最大值和最小值
{
	uint mean_dat;
	uchar i,da=0,xiao=0;
	for(i=1;i<10;i++)
	{
		if(pjs[da]<pjs[i])
		{
			da=i;
		}
		if(pjs[xiao]>pjs[i])
		{
			xiao=i;
		}	
	}
	pjs[da]=pjs[xiao]=0;
	da=xiao=0;
	for(i=1;i<10;i++)
	{
		if(pjs[da]<pjs[i])
		{
			da=i;
		}
		if(pjs[xiao]>pjs[i])
		{
			xiao=i;
		}	
	}
	pjs[da]=pjs[xiao]=0;
	mean_dat=(pjs[0]+pjs[1]+pjs[2]+pjs[3]+pjs[4]+pjs[5]+pjs[6]+pjs[7]+pjs[8]+pjs[9])/6;
	return mean_dat;
}

void main()
{
	TMOD=0x51;
	TH1=0;
	TL1=0;
	TL0 = 0xE0;		//设置定时初值
	TH0 = 0xB1;		//设置定时初值
	TR0=1;
	TR1=1;
	EA=1;
	ET0=1;
	ET1=1;
	LCD1602_cls();
	chuqi=1;
	jinqi=1;
	while(1)
	{	
		show();			 //显示程序
		if(!key_1) //如果按键按下
		{
			delay(888); //延时
			chuqi=0;  //启动电磁阀门
			if(!key_1)  //如果key_1=0
			{
				zhi=TL0%4;
				state=1; //模式1 
				chuqi=0;  //启动电磁阀门
				jinqi=0; //启动气泵充气
				AP_H=AP_L=time_1=xl=0; //清零
				pj_1=pj_t=0;	 //清零
			}
			while(!key_1) chuqi=0; //等待按键释放
		}
	}
}


void init_1() interrupt 1	
{
	TL0 = 0xE0;		//设置定时初值
	TH0 = 0xB1;		//设置定时初值
	ms++;   //变量ms++
	if(ms%10==0) 
	{
		if(timer!=0) //如果timer不为0，则启动蜂鸣器发声
		{
			timer--; //timer--,计时。减完停止发声
			beep=0; //开启蜂鸣器发声
		}else
		{
			beep=1; //关闭蜂鸣器
		}
	}
	
	if(ms>19)	//计时	        
	{
		ms=0;
		if(pj_1==1)
		{
			time_1++;	
		}
		shu1=(cs*65535+TH1*256+TL1)*10; //测出频率单位khz，*10得到MHZ
		if(jc==1) //如果jc=1，也就是shu<155 开启测量脉搏信号
		{	 
			if(wc_1==1&&wc_2==0)//如果wc_1=1且wc2=0，启动测量。wc2是充气180mmhg完毕后，wc1来脉搏后
			{
				if(PD1==0)//如果PD1==0,有脉搏pd1=1，无脉搏信号开始计时
				{
					PD++;
					if((PD>=10)||(shu<45))//如果PD>=10，说明无脉搏信号了，测量要结束，显示结果
					{
						PD=0; //计时清零
						AP_L=zc_1+24;//显示低压
						wc_1=0;//脉搏信号清零
						wc_2=1;//压力标志置1
						chuqi=1;//打开电磁阀阀门，快速排气
						pj_s=mean();
						xl_1=(520/pj_s)+zhi;
						time_1=xl=0;//计时清零
						state=3;//显示结果
						timer=30;//启动蜂鸣器滴一声提示
					}
				}else//否则有脉搏信号，PD计时清零
				{
					PD=0;//PD清零
				}
			}
			PD1=0; //PD1清零
			if(shu1>=cc)
			{
				if((shu1-cc)>600)//如果当前频率-前时刻的频率，关闭led脉搏指示灯，pd1置1
				{
					led_1=1;//关闭led脉搏指示灯
					PD1=1;//PD1置1
				}
			}else//否则有脉搏信号来了，打开led脉搏指示灯
			{
				if((cc-shu1)>600)//如果前时刻频率-当前的频率，打开led，脉搏信号处理
				{
					if(AP_H!=0&&led_1==1&&AP_L==0)//如果高压不等于0且led关闭且低压等于0时
					{
						xl++;	//开始计时
					}
					led_1=0;//打开脉搏指示灯
					if(pj_1==1) //如果pj1=1开始计时
					{
						pjs[pj_t]=time_1;//计时数据放入pjs【】中
						time_1=0;
						pj_t=(pj_t+1)%10;		//平均值变量+1，等待下一个脉搏，脉搏数据存入pjs【】中
					}
					pj_1=1;
					jc_1++;		    
					zc_1=shu;
					PD1=1;
					if(wc==0&&jc_1>1)//如果充气完后且jc1>1时 
					{			   
						jc_1=0;
						AP_H=shu;//保存当前压力值
						if(AP_H>=170) AP_H-=60;
						if(AP_H>=170) AP_H-=50;
						if(AP_H>=160) AP_H-=40;
						if(AP_H>=150) AP_H-=30;
						if(AP_H>=140) AP_H-=20;
						if(AP_H>=130) AP_H-=10;
						wc=1; //找出高压后，置1
						wc_1=1;//置1继续测量找低压
					}
				}
			}
		}else led_1=1;
		cc=shu1;
    if(shu1<1218000&&shu1!=0&&shu1>800000)		
		{					  
			shu=(1218000-shu1)/1290;
			if(shu>=180)	    //压力大于180mmHg  则停止充气 
			{
				wc=wc_2=0;
				jinqi=1;
				state=2;
			}
			if(shu>=163)		//如果当前压力值大于等于163，开始检测
			{
				jc1=1;
			}															    
			if(shu<155&&jc1==1) //如果当前压力值低于155且jc1=1，启动脉搏检测
			{
				jc=1;
				jc1=0;
			}
		}
		else   //不再正常值范围内数据数据处理
		{
			 shu=0;  
			 jc=0;
		 	 jc1=0;
			 wc=0;
			 PD1=1;
			 wc_2=0;
		}
		cs=TH1=TL1=0;
	}
}

void init_3() interrupt 3  //外部中断检测当前传感器频率
{
	cs++;
}

