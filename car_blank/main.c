#include <msp430.h> 
#include "oledfont.h"
#include "oled.h"       //里面有SDA和SCL引脚定义，移植时需注意修改


//延时函数，IAR自带，经常使用到
#define CPU_F ((double)8000000)   //外部高频晶振8MHZ
#define delay_us(x) __delay_cycles((long)(CPU_F*(double)x/1000000.0))
#define delay_ms(x) __delay_cycles((long)(CPU_F*(double)x/1000.0))


#define hy1 (P1IN&BIT4) //寻迹检测
#define hy2 (P1IN&BIT5) //停止检测

#define key3 (P1IN&BIT3) //按键检测（开发板内置）（按下低电平，抬起高电平）
#define key2 (P1IN&BIT7) //按键检测（按下低电平，抬起高电平）
#define key1 (P2IN&BIT5) //按键检测（按下低电平，抬起高电平）

#define FM_L            P2OUT &= ~BIT0            //蜂鸣器置低电平
#define FM_H            P2OUT |= BIT0             //蜂鸣器置高电平



//***********************************************************************
//             TIMERA初始化，设置为UP模式计数
//***********************************************************************
int t_miao=0,count,setmode=15;
void TIMERA_Init(void)
{   //打开TA0捕捉/比较中断使能
    TA0CCTL0 = CCIE;
    //TA0时钟设置      SMCLK做时钟源        8分频            连续计数模式
    TA0CTL |= (TASSEL1 + ID0 + ID1 + MC_1);
    //计数50000次开中断
    TA0CCR0 = 50000;
}

//***********************************************************************
//             TIMERA中断服务程序，需要判断中断类型
//***********************************************************************
// Timer A0 interrupt service routine



#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer_A (void)
#else
#error Compiler not supported!
#endif
{
    TA0CCR0 += 50000;                            // Add Offset to CCR0
    count++;

      if(count>=20)     //1秒
      {
         count=0;
         t_miao++;
         P1OUT^=BIT6;//LED灯P1.6闪烁提示
      }
}
//***********************************************************************
//             TIMERA初始化，结束
//***********************************************************************


/*********************小车启动程序**************************/
int delaytime;
void go()
{
  P2DIR |= BIT1 + BIT4;  // P2.1和P2.4  配置为输出
  P2SEL |= BIT1 + BIT4;  // P2.1和P2.4  配置为TimerA1的Out1和Out2输出
  P2OUT &= ~BIT1;           // 置低
  P2OUT &= ~BIT4;           // 置低

  TA1CCR0 = 20000; // PWM 周期 20MS //不确定
  TA1CCTL1 = OUTMOD_7;
  TA1CCR1 = 0; //PWM为0停止状态

  TA1CCTL2 = OUTMOD_7;
  TA1CCR2 = 0;//PWM为0停止状态

  //TA1时钟设置      SMCLK做时钟源        8分频       向上计数模式
  TA1CTL = (TASSEL1 + ID0 + ID1 + MC0);

     while(1)
     {
        //OLED上时钟运行提示
         OLED_ShowChar(80,0,t_miao/100+'0');
         OLED_ShowChar(88,0,t_miao%100/10+'0');
         OLED_ShowChar(96,0,t_miao%10+'0');


         if(hy1!=0 && hy2!=0)//终点检测到
         {
            TA1CCR1=0;  //速度为0 //P2.1
            TA1CCR2=0;          //P2.4

            //OLED上停止提示
            OLED_CLS();
            OLED_ShowChar(0,0,'E');
            OLED_ShowChar(8,0,'n');
            OLED_ShowChar(16,0,'d');

            //当前时间提示
            OLED_ShowChar(80,0,t_miao/100+'0');
            OLED_ShowChar(88,0,t_miao%100/10+'0');
            OLED_ShowChar(96,0,t_miao%10+'0');



            int i;
            for(i=0 ; i<3; i++)
            {
              FM_L;//启动蜂鸣器
              delay_ms(100);
              FM_H;//关闭蜂鸣器
              delay_ms(100);
            }
            while(1)
            {

            }

         }
         else//没有到达终点
         {
             if(hy1==0 && hy2==0) //直行
             {
                 //输出直行
                 OLED_ShowChar(0,0,'S');
                 OLED_ShowChar(8,0,'t');
                 OLED_ShowChar(16,0,'r');
                 OLED_ShowChar(24,0,'a');
                 OLED_ShowChar(32,0,'i');
                 OLED_ShowChar(40,0,'g');
                 OLED_ShowChar(48,0,'h');
                 OLED_ShowChar(56,0,'t');
                 OLED_ShowChar(64,0,' ');
                 OLED_ShowChar(72,0,' ');

                 OLED_ShowChar(80,0,t_miao/100+'0');
                 OLED_ShowChar(88,0,t_miao%100/10+'0');
                 OLED_ShowChar(96,0,t_miao%10+'0');


                 //直行
                 TA1CCR1=(int)30.5*(100+60*(setmode-10));
                 TA1CCR2=(int)30.5*(100+60*(setmode-10));

             }
             else
             {
                 if(hy1!=0) //右转
                 {
                     //输出右转
                     OLED_ShowChar(0,0,'T');
                     OLED_ShowChar(8,0,'r');
                     OLED_ShowChar(16,0,'u');
                     OLED_ShowChar(24,0,'n');
                     OLED_ShowChar(32,0,' ');
                     OLED_ShowChar(40,0,'r');
                     OLED_ShowChar(48,0,'i');
                     OLED_ShowChar(56,0,'g');
                     OLED_ShowChar(64,0,'h');
                     OLED_ShowChar(72,0,'t');

                     OLED_ShowChar(80,0,t_miao/100+'0');
                     OLED_ShowChar(88,0,t_miao%100/10+'0');
                     OLED_ShowChar(96,0,t_miao%10+'0');

                     TA1CCR1=(int)30.5*(100+2000*(setmode));
                     TA1CCR2=20;
                     delay_ms(125);
                 }
                 else //左转
                 {
                     //输出右转
                     OLED_ShowChar(0,0,'T');
                     OLED_ShowChar(8,0,'r');
                     OLED_ShowChar(16,0,'u');
                     OLED_ShowChar(24,0,'n');
                     OLED_ShowChar(32,0,' ');
                     OLED_ShowChar(40,0,'l');
                     OLED_ShowChar(48,0,'e');
                     OLED_ShowChar(56,0,'f');
                     OLED_ShowChar(64,0,'t');
                     OLED_ShowChar(72,0,' ');

                     OLED_ShowChar(80,0,t_miao/100+'0');
                     OLED_ShowChar(88,0,t_miao%100/10+'0');
                     OLED_ShowChar(96,0,t_miao%10+'0');

                     TA1CCR1=20;
                     TA1CCR2=(int)30.5*(100+2000*(setmode));
                     delay_ms(125);
                 }

             }

         }
     }
}
/*********************小车运行结束**************************/


//
void main(void)//主函数
{
    WDTCTL = WDTPW + WDTHOLD;     //关闭看门狗设置

     BCSCTL1 = CALBC1_8MHZ;          //系统时钟设置为8MHz
     DCOCTL = CALDCO_8MHZ;

     TIMERA_Init();               //设置TIMERA
     _EINT();

     OLED_Init(); //OLED初始化
     delay_ms(200);

     //
     P1SEL &=~(BIT3+BIT4+BIT5+BIT7);       // 引脚配置 0101 1100
     P1DIR &=~(BIT3+BIT4+BIT5+BIT7);       //输入模式，按键引脚 红外寻迹0101 1100
     P1REN |= BIT3;                        //P1.3引脚接上拉电阻   //不确定
     P1OUT |= (BIT3+BIT4+BIT5+BIT7); //按键引脚上拉

     P2SEL &= ~BIT5;       // P2.5按键引脚
     P2DIR &= ~BIT5;       // P2.5输入模式
     P2OUT |= BIT5;      //按键引脚上拉


     P2DIR |= BIT0;//蜂鸣器输出配置
     P2OUT |= BIT0;//蜂鸣器输出高

     P1DIR|=(BIT0+BIT6);//LED灯引脚输出配置
     P1OUT|=(BIT0+BIT6);//LED灯引脚输出高

     while(1)
     {
        //停止提示
         OLED_ShowChar(0,0,'S');
         OLED_ShowChar(8,0,'t');
         OLED_ShowChar(16,0,'o');
         OLED_ShowChar(24,0,'p');


        //当前速度档位
         OLED_ShowChar(0,2,'M');
         OLED_ShowChar(8,2,'=');
         OLED_ShowChar(16,2,(30-setmode)%100/10+'0');//设置模式提示
         OLED_ShowChar(24,2,(30-setmode)%10+'0');


        //计时提示
         OLED_ShowChar(0,4,'T');
         OLED_ShowChar(8,4,'i');
         OLED_ShowChar(16,4,'m');
         OLED_ShowChar(24,4,'e');
         OLED_ShowChar(32,4,' ');
         OLED_ShowChar(48,4,'s');
         OLED_ShowChar(56,4,'t');
         OLED_ShowChar(64,4,'a');
         OLED_ShowChar(72,4,'r');
         OLED_ShowChar(80,4,'t');
         OLED_ShowChar(88,4,'s');





        if(key2==0)//按键2 设定+1
        {
          if(setmode!=20)setmode++;
          delay_ms(300);
        }else if(key1==0)//按键1 设定-1
        {
          if(setmode!=10)setmode--;
          delay_ms(300);
        }else if(key3==0)//启动按键
        {
            //启动提示
            OLED_CLS();
            OLED_ShowChar(0,0,'S');
            OLED_ShowChar(8,0,'t');
            OLED_ShowChar(16,0,'a');
            OLED_ShowChar(24,0,'r');
            OLED_ShowChar(32,0,'t');

            while(key3==0);//等待松手
            delay_ms(1000);//等等3秒
            delay_ms(1000);
            delay_ms(1000);
            t_miao=0;
            go();//小车启动
        }


       if(hy1==0)//红外1检测
       {
          //OLED上红外1寻迹提示
           OLED_ShowChar(0,6,'h');
           OLED_ShowChar(8,6,'y');
           OLED_ShowChar(16,6,'1');
           OLED_ShowChar(24,6,'=');
           OLED_ShowChar(32,6,'N');
       }else
       {
          //OLED上红外1寻迹提示
           OLED_ShowChar(0,6,'h');
           OLED_ShowChar(8,6,'y');
           OLED_ShowChar(16,6,'1');
           OLED_ShowChar(24,6,'=');
           OLED_ShowChar(32,6,'Y');
       }

       if(hy2==0)//红外2检测
       {
           //OLED上红外2提示
           OLED_ShowChar(48,6,'h');
           OLED_ShowChar(56,6,'y');
           OLED_ShowChar(64,6,'2');
           OLED_ShowChar(72,6,'=');
           OLED_ShowChar(80,6,'N');

       }else
       {
          //OLED上红外2提示
           OLED_ShowChar(48,6,'h');
           OLED_ShowChar(56,6,'y');
           OLED_ShowChar(64,6,'2');
           OLED_ShowChar(72,6,'=');
           OLED_ShowChar(80,6,'Y');
       }

      P1OUT^=BIT0;//LED1.0闪烁提示
      delay_ms(100);

    }
}
