/*****************************************************************************
 * Name: v2.0_7_segment.ino
 * Created: 2018/1/9 04:40 AM (GMT+8) yyyy/mm/dd
 * Author: Jiamingla
******************************************************************************/

/*七段顯示器2.0版 2018/1/10
使用 Arduino Mega 2560
版本更新: 
很多突破，無法一言以概之
顯示8888
可以連手機藍牙
當前時間
0000~9999
9999~0000
輸入四位數字並顯示

用上TIMER函式庫的倒數計時器、碼表

既有功能:
當前時間
0000~9999
9999~0000
輸入四位數字並顯示
倒數計時器
碼表
比賽
*/

/*四個七段 + 冒號(由左至右排列)
8/8/:/8/8
每個腳位前綴
0/1/4/2/3
0用上的腳位定義為:22,24,26,28,30,32,34
1用上的腳位定義為:23,25,27,29,31,33,35
2用上的腳位定義為:36,38,40,42,44,46,48
3用上的腳位定義為:37,39,41,43,45,47,49
4用上的腳位定義為:50,51
*/
//定義腳位
// Mega Pin I/O definitions
  //0的PIN
  #define pin_0_a 22
  #define pin_0_b 24
  #define pin_0_c 26
  #define pin_0_d 28
  #define pin_0_e 30
  #define pin_0_f 32
  #define pin_0_g 34
  
  //1的PIN
  #define pin_1_a 23
  #define pin_1_b 25
  #define pin_1_c 27
  #define pin_1_d 29
  #define pin_1_e 31
  #define pin_1_f 33
  #define pin_1_g 35
  
  //2的PIN
  #define pin_2_a 36
  #define pin_2_b 38
  #define pin_2_c 40
  #define pin_2_d 42
  #define pin_2_e 44
  #define pin_2_f 46
  #define pin_2_g 48
  
  //3的PIN
  #define pin_3_a 37
  #define pin_3_b 39
  #define pin_3_c 41
  #define pin_3_d 43
  #define pin_3_e 45
  #define pin_3_f 47
  #define pin_3_g 49

  //4的PIN
  #define pin_4_a 50
  #define pin_4_b 51
//定義七段顯示器位數


byte i;
#include <SoftwareSerial.h> //引用「軟體序列埠程式庫」
SoftwareSerial BT(10,9); //自訂的程式物件名稱 接收腳 傳送腳
#include <Timer.h>
Timer timerclockhourmin;
Timer timerclockminsec;
Timer countdown;
Timer countup;

// 宣告時間計數器   Timers
unsigned long time_previous;

//宣告一個數字作輸入用
int number = 0;

//宣告一個秒單位有多長(預設一秒)(用於改速度) Setup second setting
int ms = 1000;

//宣告冒號明暗 預設true恆亮
boolean colon = true;

//宣告小時數、分鐘數、秒數
byte Chour;
byte Cminute;
byte Csecond;
byte Chourtens;
byte Cminutetens;
byte Csecondtens;
byte Chourones;
byte Cminuteones;
byte Csecondones;

byte T1point =0;
byte T2point =0;
boolean t1t2display = HIGH;
//宣告功能轉換
byte mode;

//因為Serial.read一次只能讀一個值，偶爾也需要讀取字串
String s = "";
char gpr;
byte Ccs = 0; //厘秒

const byte seg4[4][7] = {
  {pin_0_a,pin_0_b,pin_0_c,pin_0_d,pin_0_e,pin_0_f,pin_0_g},
  {pin_1_a,pin_1_b,pin_1_c,pin_1_d,pin_1_e,pin_1_f,pin_1_g},
  {pin_2_a,pin_2_b,pin_2_c,pin_2_d,pin_2_e,pin_2_f,pin_2_g},
  {pin_3_a,pin_3_b,pin_3_c,pin_3_d,pin_3_e,pin_3_f,pin_3_g}
};
//七段顯示器零到十
const byte seven_seg_digits[10][7] = {
  { 1,1,1,1,1,1,0 },  // = 0
  { 0,1,1,0,0,0,0 },  // = 1
  { 1,1,0,1,1,0,1 },  // = 2
  { 1,1,1,1,0,0,1 },  // = 3
  { 0,1,1,0,0,1,1 },  // = 4
  { 1,0,1,1,0,1,1 },  // = 5
  { 1,0,1,1,1,1,1 },  // = 6
  { 1,1,1,0,0,0,0 },  // = 7
  { 1,1,1,1,1,1,1 },  // = 8
  { 1,1,1,0,0,1,1 }   // = 9
};



// 設定某個七段顯示器所顯示的數字，
// 參數pos為0~3，指出想要更新哪一個七段顯示器，
// 參數n為0~9，顯示數字

void setup() {
  
// 開啟序列埠, 通訊速率為 9600 bps
// Setup Serial communications for Printing to console and debugging  
Serial.begin(9600);
BT.begin(9600);
// 宣告4顆七段顯示器的腳位為輸出
// Setup pins
for(int i = 0;i < 4;i++)
  for(int j = 0;j < 7;j++)
    pinMode(seg4[i][j],OUTPUT);
pinMode(pin_4_a,OUTPUT);    
pinMode(pin_4_b,OUTPUT); 

timerclockhourmin.every(1000,nowhourcolonmin); //配對四個功能的計時器
timerclockminsec.every(1000,nowmincolonsec);
countdown.every(1000,countDowntimer);
countup.every(1000,countUptimer);
}

void loop() {
// 檢查是否有資料可供讀取
if (Serial.available() > 0|| BT.available() > 0) 
{
  char c = BT.read();
  mode = c;
  if(mode = '@')
    mode = BT.read();
  char mode2;
  switch(mode)
  {
    
    // 當前時分 時：分
    case 'a':
    // 從手機端取得三個時間數值
    phone3valuenow();    
    BTpf("Time = %d : %d : %d ",Chour,Cminute,Csecond);
    pf("Time = %d : %d : %d ",Chour,Cminute,Csecond);
    timerclockhourmin.update();
    if(BT.available() > 0) //有值才讀，確認下一步
      mode2 = BT.read();
    if(mode2 == '@')
      timerclockhourmin.stop(nowhourcolonmin);
    break;
    
    // 當前時分 分：秒
    case 'b':
    // 從手機端取得三個時間數值
    phone3valuenow();
    BTpf("Time = %d : %d : %d ",Chour,Cminute,Csecond);
    pf("Time = %d : %d : %d ",Chour,Cminute,Csecond);
    timerclockminsec.update();
    if(BT.available() > 0) //有值才讀，確認下一步
      mode2 = BT.read();
    if(mode2 == '@')
      timerclockminsec.stop(nowmincolonsec);
    break;

    // 倒數計時器，開始結束問題很大
    case 'c':
    if(BT.available() > 0) //有值才讀，確認下一步
      mode2 = BT.read();
    if(mode == 's') //代表正在設定時間
      phone3valuenow(); //拿值
    if(BT.available() > 0) //有值才讀，確認下一步
      mode2 = BT.read();
    switch (mode2)
    {
      case '@':
        break;
      case 'g':
        countdown.update();
        break;
      case 'p':
        countdown.stop(countDowntimer);
        break;
      case 'r':  
        countdown.stop(countDowntimer);
        break;
    }
    /*while(BT.available() == 0)
    {
      gpr = BT.read();
      while(gpr == 'g')
      {
        // 從手機端取得三個時間數值
        phone3valuenow();
        if(BT.available() > 0) //有值才讀，確認下一步
          gpr = BT.read(); 
        while(gpr != 'p' || 'r')
        {
          if(BT.available() > 0) //有值才讀，確認下一步
          gpr = BT.read();
          if(gpr == '@')
             break;
          countDowntimer();    //倒數一次  
        }
        if(gpr == 'p')
        {
          continue;
        }
      }
      break;
    }
    break;
    
    while(gpr == 'g' || 'p') //如果按下開始暫停就進入迴圈
    {

      while(gpr != 'p' || 'r') //如果仍舊繼續開始的話就跑迴圈
      { 
        if(BT.available() > 0)
          gpr = BT.read();   //有值才讀，確認下一步
        countDowntimer();    //倒數一次  
      }
      if(gpr == 'r') //重置則跳出迴圈
        break;
    }
    if(gpr == 'r') //重置則再次讀取
      break;
    while(BT.available() == 0) //數完了
    {
      gpr = BT.read();
      if(gpr != "" || 'g' || 'p' || 'r')
      {
        mode = gpr;
        break;
      }     
      setNumber(0);
      colonswitch();
    }*/
    break;

    // 碼表
    case 'd':
    if(BT.available() > 0) //有值才讀，確認下一步
      mode2 = BT.read();
    if(mode2 == 's') //代表正在設定時間
      phone3valuenow(); //拿值
    if(BT.available() > 0) //有值才讀，確認下一步
      mode2 = BT.read();
    switch (mode2)
    {
      case '@':
        break;
      case 'g':
        countup.update();
        break;
      case 'p':
        countup.stop(countUptimer);
        break;
      case 'r':  
        countup.stop(countUptimer);
        break;
    }
    /*phone3valuenow();  
    gpr = BT.read();
    while(gpr == 'g' || 'p')
    {
      if(BT.available() > 0)
          gpr = BT.read();
      while(gpr != 'p' || 'r')
      { 
        if(BT.available() > 0)
          gpr = BT.read();   
        countUptimer();      
      }
    }
    while(BT.available() == 0 || gpr == 'r') //結束
    {
      gpr = BT.read();
      if(gpr != "" || 'g' || 'p' || 's')
      {
        mode = gpr;
        break;
      }     
      setNumber(0);
      colonswitch();
    }*/
    break;
    
    //比賽計分
    case 'e':
    if(BT.available() > 0) //有值才讀，確認下一步
      mode2 = BT.read();
    switch (mode2)
    {
      case '@':
        break;
      case 'j':
        T1point +=1;
        if(t1t2display = HIGH)
          setNumber(T1point*100+T2point);
        else
          setNumber(T2point*100+T1point);
        break;
      case 'k':
        T1point +=2;
        if(t1t2display = HIGH)
          setNumber(T1point*100+T2point);
        else
          setNumber(T2point*100+T1point);
        break;
      case 'l':  
        T1point +=3;
        if(t1t2display = HIGH)
          setNumber(T1point*100+T2point);
        else
          setNumber(T2point*100+T1point);
        break;
      case 'm':
        T2point +=1;
        if(t1t2display = HIGH)
          setNumber(T1point*100+T2point);
        else
          setNumber(T2point*100+T1point);
        break;
      case 'n':
        T2point +=2;
        if(t1t2display = HIGH)
          setNumber(T1point*100+T2point);
        else
          setNumber(T2point*100+T1point);
        break;
      case 'o':
        T2point +=3;
        if(t1t2display = HIGH)
          setNumber(T1point*100+T2point);
        else
          setNumber(T2point*100+T1point);
        break;
      case 'p':
        t1t2display = HIGH;
        setNumber(T1point*100+T2point);
        break;
      case 'q':
        t1t2display = LOW;
        setNumber(T2point*100+T1point);
        break;
      case 't':
        T1point = 0;
        T2point = 0;
        if(t1t2display = HIGH)
          setNumber(T1point*100+T2point);
        else
          setNumber(T2point*100+T1point);
        break;  
    }
    break;
    
    
     //從0000上數到9999
    case 'f':
    if(BT.available() > 0) //有值才讀，確認下一步
      mode2 = BT.read();
    if(mode2 != '@') 
      from0000to9999();
    break;
    // 從9999下數到0000
    case 'g':
    if(BT.available() > 0) //有值才讀，確認下一步
      mode2 = BT.read();
    if(mode2 != '@') 
      from9999to0000();
    break;
    // 輸入一個數字並顯示
    case 'h':
    if(BT.available() > 0) //有值才讀，確認下一步
      mode2 = BT.read();
    if(mode2 != '@') 
      setNumber(8888);
      colonswitch();
    /*if(number > 9999 || number < 0) // 如果數字輸入錯誤則退出
      break;
    while(Serial.available() == 0)
    {
      setNumber(number);
      colonswitch();
    }*/
    break;
  }
  
}
else
{
  // 測試時全亮
  setNumber(8888);
  colonswitch();
}
}
