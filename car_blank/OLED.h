//
/*********************OLED��������**************************/
// ------------------------------------------------------------
// IO��ģ��I2Cͨ��
// SCL��P2^3
// SDA��P2^2
// ------------------------------------------------------------

#define SCL_1 P2OUT |= BIT3
#define SCL_0 P2OUT &= ~BIT3
#define SDA_1 P2OUT |= BIT2
#define SDA_0 P2OUT &= ~BIT2

#define Brightness  0xCF
#define X_WIDTH     128
#define Y_WIDTH     64
#define OLED_CMD  0 //д����
#define OLED_DATA 1 //д����

#define high 1
#define low 0

/*********************OLED�ӳٳ���**************************/
void delay(unsigned int z)
{
  unsigned int x,y;
  for(x=z;x>0;x--)
    for(y=100;y>0;y--);
}
/**********************************************
//IIC Start
**********************************************/
void IIC_Start()
{
   SCL_1;
   SDA_1;
   SDA_0;
   SCL_0;
}

/**********************************************
//IIC Stop
**********************************************/
void IIC_Stop()
{
   SCL_0;
   SDA_0;
   SCL_0;
   SDA_0;
}

/**********************************************
// ͨ��I2C����дһ���ֽ�
**********************************************/
void Write_IIC_Byte(unsigned char IIC_Byte)
{
    unsigned char i;
    for(i=0;i<8;i++)
    {
            if(IIC_Byte & 0x80)
              SDA_1;
            else
              SDA_0;
            SCL_1;
            SCL_0;
            IIC_Byte<<=1;
    }
    SDA_1;
    SCL_1;
    SCL_0;
}


/*********************OLEDд����************************************/
void OLED_WrDat(unsigned char IIC_Data)
{
    IIC_Start();
    Write_IIC_Byte(0x78);
    Write_IIC_Byte(0x40);           //write data
    Write_IIC_Byte(IIC_Data);
    IIC_Stop();
}
/*********************OLEDд����************************************/
void OLED_WrCmd(unsigned char IIC_Command)
{
    IIC_Start();
    Write_IIC_Byte(0x78);            //Slave address,SA0=0
    Write_IIC_Byte(0x00);           //write command
    Write_IIC_Byte(IIC_Command);
    IIC_Stop();
}
/*********************OLED ��������************************************/
void OLED_Set_Pos(unsigned char x, unsigned char y)
{
    OLED_WrCmd(0xb0+y);
    OLED_WrCmd(((x&0xf0)>>4)|0x10);
    OLED_WrCmd((x&0x0f)|0x01);
}
/*********************OLEDȫ��************************************/
void OLED_Fill(unsigned char bmp_dat)
{
    unsigned char y,x;
    for(y=0;y<8;y++)
    {
        OLED_WrCmd(0xb0+y);
        OLED_WrCmd(0x01);
        OLED_WrCmd(0x10);
        for(x=0;x<X_WIDTH;x++)
        OLED_WrDat(bmp_dat);
    }
}
/*********************OLED��λ************************************/
void OLED_CLS(void)
{
    unsigned char y,x;
    for(y=0;y<8;y++)
    {
        OLED_WrCmd(0xb0+y);
        OLED_WrCmd(0x01);
        OLED_WrCmd(0x10);
        for(x=0;x<X_WIDTH;x++)
        OLED_WrDat(0);
    }
}
//д�ֽ�
void OLED_WR_Byte(unsigned dat,unsigned cmd)
{
    if(cmd)
        {
          OLED_WrDat(dat);
        }
    else
        {
          OLED_WrCmd(dat);
    }


}
//��ʾ����
void OLED_ShowCHinese(unsigned char x,unsigned char y,unsigned char no)
{
    unsigned char t,adder=0;
    OLED_Set_Pos(x,y);
        for(t=0;t<16;t++)
        {
          OLED_WR_Byte(Hzk[2*no][t],OLED_DATA);
          adder+=1;
        }
        OLED_Set_Pos(x,y+1);
        for(t=0;t<16;t++)
        {
          OLED_WR_Byte(Hzk[2*no+1][t],OLED_DATA);
          adder+=1;
        }
}
//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
void OLED_ShowChar(unsigned char x,unsigned char y,unsigned char chr)
{
    unsigned char c=0,i=0;
        c=chr-' ';//�õ�ƫ�ƺ��ֵ
        if(x>128-1){x=0;y=y+2;}

        OLED_Set_Pos(x,y);
        for(i=0;i<8;i++)
          OLED_WR_Byte(F8X16[c*16+i],OLED_DATA);
        OLED_Set_Pos(x,y+1);
        for(i=0;i<8;i++)
          OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA);
}
/*********************OLED��ʼ��************************************/
void OLED_Init(void)
{
      P2DIR |= (BIT3+BIT2);                  //����IO�ڷ���Ϊ���0000 1100
      P2OUT |= (BIT3+BIT2);                     //��ʼ����Ϊ0000 1100
      delay(4000);//��ʼ��֮ǰ����ʱ����Ҫ��
      //delay(500);//��ʼ��֮ǰ����ʱ����Ҫ��
      OLED_WrCmd(0xae);//--turn off oled panel
      OLED_WrCmd(0x00);//---set low column address
      OLED_WrCmd(0x10);//---set high column address
      OLED_WrCmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
      OLED_WrCmd(0x81);//--set contrast control register
      OLED_WrCmd(Brightness); // Set SEG Output Current Brightness
      OLED_WrCmd(0xa1);//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
      OLED_WrCmd(0xc8);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
      OLED_WrCmd(0xa6);//--set normal display
      OLED_WrCmd(0xa8);//--set multiplex ratio(1 to 64)
      OLED_WrCmd(0x3f);//--1/64 duty
      OLED_WrCmd(0xd3);//-set display offset    Shift Mapping RAM Counter (0x00~0x3F)
      OLED_WrCmd(0x00);//-not offset
      OLED_WrCmd(0xd5);//--set display clock divide ratio/oscillator frequency
      OLED_WrCmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
      OLED_WrCmd(0xd9);//--set pre-charge period
      OLED_WrCmd(0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
      OLED_WrCmd(0xda);//--set com pins hardware configuration
      OLED_WrCmd(0x12);
      OLED_WrCmd(0xdb);//--set vcomh
      OLED_WrCmd(0x40);//Set VCOM Deselect Level
      OLED_WrCmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
      OLED_WrCmd(0x02);//
      OLED_WrCmd(0x8d);//--set Charge Pump enable/disable
      OLED_WrCmd(0x14);//--set(0x10) disable
      OLED_WrCmd(0xa4);// Disable Entire Display On (0xa4/0xa5)
      OLED_WrCmd(0xa6);// Disable Inverse Display On (0xa6/a7)
      OLED_WrCmd(0xaf);//--turn on oled panel
      OLED_Fill(0x00); //��ʼ����
      OLED_Set_Pos(0,0);
}
/*********************OLED�����������**************************/
//
