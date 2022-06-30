#include <msp430.h> 
#include "oledfont.h"
#include "oled.h"       //������SDA��SCL���Ŷ��壬��ֲʱ��ע���޸�


//��ʱ������IAR�Դ�������ʹ�õ�
#define CPU_F ((double)8000000)   //�ⲿ��Ƶ����8MHZ
#define delay_us(x) __delay_cycles((long)(CPU_F*(double)x/1000000.0))
#define delay_ms(x) __delay_cycles((long)(CPU_F*(double)x/1000.0))


#define hy1 (P1IN&BIT4) //Ѱ�����
#define hy2 (P1IN&BIT5) //ֹͣ���

#define key3 (P1IN&BIT3) //������⣨���������ã������µ͵�ƽ��̧��ߵ�ƽ��
#define key2 (P1IN&BIT7) //������⣨���µ͵�ƽ��̧��ߵ�ƽ��
#define key1 (P2IN&BIT5) //������⣨���µ͵�ƽ��̧��ߵ�ƽ��

#define FM_L            P2OUT &= ~BIT0            //�������õ͵�ƽ
#define FM_H            P2OUT |= BIT0             //�������øߵ�ƽ



//***********************************************************************
//             TIMERA��ʼ��������ΪUPģʽ����
//***********************************************************************
int t_miao=0,count,setmode=15;
void TIMERA_Init(void)
{   //��TA0��׽/�Ƚ��ж�ʹ��
    TA0CCTL0 = CCIE;
    //TA0ʱ������      SMCLK��ʱ��Դ        8��Ƶ            ��������ģʽ
    TA0CTL |= (TASSEL1 + ID0 + ID1 + MC_1);
    //����50000�ο��ж�
    TA0CCR0 = 50000;
}

//***********************************************************************
//             TIMERA�жϷ��������Ҫ�ж��ж�����
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

      if(count>=20)     //1��
      {
         count=0;
         t_miao++;
         P1OUT^=BIT6;//LED��P1.6��˸��ʾ
      }
}
//***********************************************************************
//             TIMERA��ʼ��������
//***********************************************************************


/*********************С����������**************************/
int delaytime;
void go()
{
  P2DIR |= BIT1 + BIT4;  // P2.1��P2.4  ����Ϊ���
  P2SEL |= BIT1 + BIT4;  // P2.1��P2.4  ����ΪTimerA1��Out1��Out2���
  P2OUT &= ~BIT1;           // �õ�
  P2OUT &= ~BIT4;           // �õ�

  TA1CCR0 = 20000; // PWM ���� 20MS //��ȷ��
  TA1CCTL1 = OUTMOD_7;
  TA1CCR1 = 0; //PWMΪ0ֹͣ״̬

  TA1CCTL2 = OUTMOD_7;
  TA1CCR2 = 0;//PWMΪ0ֹͣ״̬

  //TA1ʱ������      SMCLK��ʱ��Դ        8��Ƶ       ���ϼ���ģʽ
  TA1CTL = (TASSEL1 + ID0 + ID1 + MC0);

     while(1)
     {
        //OLED��ʱ��������ʾ
         OLED_ShowChar(80,0,t_miao/100+'0');
         OLED_ShowChar(88,0,t_miao%100/10+'0');
         OLED_ShowChar(96,0,t_miao%10+'0');


         if(hy1!=0 && hy2!=0)//�յ��⵽
         {
            TA1CCR1=0;  //�ٶ�Ϊ0 //P2.1
            TA1CCR2=0;          //P2.4

            //OLED��ֹͣ��ʾ
            OLED_CLS();
            OLED_ShowChar(0,0,'E');
            OLED_ShowChar(8,0,'n');
            OLED_ShowChar(16,0,'d');

            //��ǰʱ����ʾ
            OLED_ShowChar(80,0,t_miao/100+'0');
            OLED_ShowChar(88,0,t_miao%100/10+'0');
            OLED_ShowChar(96,0,t_miao%10+'0');



            int i;
            for(i=0 ; i<3; i++)
            {
              FM_L;//����������
              delay_ms(100);
              FM_H;//�رշ�����
              delay_ms(100);
            }
            while(1)
            {

            }

         }
         else//û�е����յ�
         {
             if(hy1==0 && hy2==0) //ֱ��
             {
                 //���ֱ��
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


                 //ֱ��
                 TA1CCR1=(int)30.5*(100+60*(setmode-10));
                 TA1CCR2=(int)30.5*(100+60*(setmode-10));

             }
             else
             {
                 if(hy1!=0) //��ת
                 {
                     //�����ת
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
                 else //��ת
                 {
                     //�����ת
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
/*********************С�����н���**************************/


//
void main(void)//������
{
    WDTCTL = WDTPW + WDTHOLD;     //�رտ��Ź�����

     BCSCTL1 = CALBC1_8MHZ;          //ϵͳʱ������Ϊ8MHz
     DCOCTL = CALDCO_8MHZ;

     TIMERA_Init();               //����TIMERA
     _EINT();

     OLED_Init(); //OLED��ʼ��
     delay_ms(200);

     //
     P1SEL &=~(BIT3+BIT4+BIT5+BIT7);       // �������� 0101 1100
     P1DIR &=~(BIT3+BIT4+BIT5+BIT7);       //����ģʽ���������� ����Ѱ��0101 1100
     P1REN |= BIT3;                        //P1.3���Ž���������   //��ȷ��
     P1OUT |= (BIT3+BIT4+BIT5+BIT7); //������������

     P2SEL &= ~BIT5;       // P2.5��������
     P2DIR &= ~BIT5;       // P2.5����ģʽ
     P2OUT |= BIT5;      //������������


     P2DIR |= BIT0;//�������������
     P2OUT |= BIT0;//�����������

     P1DIR|=(BIT0+BIT6);//LED�������������
     P1OUT|=(BIT0+BIT6);//LED�����������

     while(1)
     {
        //ֹͣ��ʾ
         OLED_ShowChar(0,0,'S');
         OLED_ShowChar(8,0,'t');
         OLED_ShowChar(16,0,'o');
         OLED_ShowChar(24,0,'p');


        //��ǰ�ٶȵ�λ
         OLED_ShowChar(0,2,'M');
         OLED_ShowChar(8,2,'=');
         OLED_ShowChar(16,2,(30-setmode)%100/10+'0');//����ģʽ��ʾ
         OLED_ShowChar(24,2,(30-setmode)%10+'0');


        //��ʱ��ʾ
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





        if(key2==0)//����2 �趨+1
        {
          if(setmode!=20)setmode++;
          delay_ms(300);
        }else if(key1==0)//����1 �趨-1
        {
          if(setmode!=10)setmode--;
          delay_ms(300);
        }else if(key3==0)//��������
        {
            //������ʾ
            OLED_CLS();
            OLED_ShowChar(0,0,'S');
            OLED_ShowChar(8,0,'t');
            OLED_ShowChar(16,0,'a');
            OLED_ShowChar(24,0,'r');
            OLED_ShowChar(32,0,'t');

            while(key3==0);//�ȴ�����
            delay_ms(1000);//�ȵ�3��
            delay_ms(1000);
            delay_ms(1000);
            t_miao=0;
            go();//С������
        }


       if(hy1==0)//����1���
       {
          //OLED�Ϻ���1Ѱ����ʾ
           OLED_ShowChar(0,6,'h');
           OLED_ShowChar(8,6,'y');
           OLED_ShowChar(16,6,'1');
           OLED_ShowChar(24,6,'=');
           OLED_ShowChar(32,6,'N');
       }else
       {
          //OLED�Ϻ���1Ѱ����ʾ
           OLED_ShowChar(0,6,'h');
           OLED_ShowChar(8,6,'y');
           OLED_ShowChar(16,6,'1');
           OLED_ShowChar(24,6,'=');
           OLED_ShowChar(32,6,'Y');
       }

       if(hy2==0)//����2���
       {
           //OLED�Ϻ���2��ʾ
           OLED_ShowChar(48,6,'h');
           OLED_ShowChar(56,6,'y');
           OLED_ShowChar(64,6,'2');
           OLED_ShowChar(72,6,'=');
           OLED_ShowChar(80,6,'N');

       }else
       {
          //OLED�Ϻ���2��ʾ
           OLED_ShowChar(48,6,'h');
           OLED_ShowChar(56,6,'y');
           OLED_ShowChar(64,6,'2');
           OLED_ShowChar(72,6,'=');
           OLED_ShowChar(80,6,'Y');
       }

      P1OUT^=BIT0;//LED1.0��˸��ʾ
      delay_ms(100);

    }
}
