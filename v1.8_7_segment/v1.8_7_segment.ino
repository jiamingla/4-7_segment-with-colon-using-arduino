/*****************************************************************************
 * Name: v1.8_7_segment.ino
 * Created: 2018/1/4 06:15 AM (GMT+8) yyyy/mm/dd
 * Author: Jiamingla
******************************************************************************/

/*七段顯示器1.8版 2018/1/4
使用 Arduino Mega 2560
版本更新: 
先顯示8888，輸入數字選擇模式再顯示功能並持續動作
冒號能動了
當前時間
0000~9999
9999~0000
輸入四位數字並顯示
不能暫停的倒數計時器
不能暫停的碼表

既有功能:
當前時間
0000~9999
9999~0000
輸入四位數字並顯示
不能暫停的倒數計時器
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

//宣告功能轉換
byte mode;

//因為Serial.read一次只能讀一個值，偶爾也需要讀取字串
String s = "";

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

// 一支方便的函式，以格式字串輸出到序列埠
void pf(const char *fmt, ... ){
    char tmp[128]; // max is 128 chars
    va_list args; //宣告一個指標，讓它指向引數串列。
    va_start (args, fmt ); //初始化這個指標，讓它真正指向正確的引數串列開頭。
    vsnprintf(tmp, 128, fmt, args);
    va_end (args); //來取得va_list中的資料。
    Serial.print(tmp);//清除這個指標，把它設為NULL。
}

// 設定某個七段顯示器所顯示的數字，
// 參數pos為0~3，指出想要更新哪一個七段顯示器，
// 參數n為0~9，顯示數字
void setDigit(byte pos,byte n)
{
  if(pos < 0 || 3 < pos)
  {
    printf("error pos=%d\n", pos);
    return;
  }
  if(0 <= n && n <= 9)
  {
    for(int j = 0;j < 7;j++)
    {
      digitalWrite(seg4[pos][j],seven_seg_digits[n][j]);
    }
  }
}  

// 設定整個四合一型七段顯示器想要顯示的數字
// 參數number的範圍應是0~9999
// 數字小於1000前面會因為剛好而補上0
void setNumber(int number)
{
  int n0, n1, n2, n3;
  n3 = number / 1000;
  number %= 1000;
  n2 = number / 100;
  number %= 100;
  n1 = number / 10;
  n0 = number % 10;

  // 求出每個位數的值後，分別更新
  // 注意，此處以delay(5)隔開每個位數的更新(葉難版本)
  //可嘗試使用無延遲版本(家名版本)
  setDigit(0, n3);
  setDigit(1, n2);
  setDigit(2, n1);
  setDigit(3, n0);
}
// 冒號（： colon），提起下文或總結上文的符號。
// 控制冒號亮暗
void colonswitch()
{
  delay(ms);
  colon = !colon;
  digitalWrite(pin_4_a,colon);
  digitalWrite(pin_4_b,colon);
}
// 從0000上數到9999
void from0000to9999()
{
  // 經過一秒單位後就讓number加1
  unsigned long time_now = millis();
  if(number == 9999)
    number = 0;
  if(time_now - time_previous > ms)
  {
    number++;
    time_previous += ms; //依次使時間計數器加一個秒單位
    pf("number=%d\n", number);//把數字也顯示在序列埠上
  }
  
  // 不斷地寫入數字
    setNumber(number);
  // 冒號動作  
    colonswitch();
}
// 從9999下數到0000
void from9999to0000()
{
  // 經過一秒單位後就讓number減1
  unsigned long time_now = millis();
  if(number == 0)
    number = 9999;
  if(time_now - time_previous > ms)
  {
    number--;
    time_previous += ms; //依次使時間計數器加一個秒單位
    pf("number=%d\n", number);//把數字也顯示在序列埠上
  }
  
  // 不斷地寫入數字
    setNumber(number);
  // 冒號動作  
    colonswitch();
}
// 設定當前時分
void setTimenow()
{
  setNumber(Chour*100 + Cminute); //設定當前時分
  colonswitch(); //冒號動作 裡頭有延遲一個秒單位
  Csecond++; //秒數加1
  if(Csecond == 60) //當59秒跳到60秒的時候
  {
    Csecond = 0;
    Cminute++;  
  }
  if(Cminute == 60) //當59分跳到60分的時候
  {
    Cminute = 0;
    Chour++;  
  }
  if(Chour == 24) //當23時跳到24時的時候
    Chour = 0;
}
// 倒數計時器
void countDowntimer()
{
  //byte Ccs = 100; // 厘秒(10^-2秒)
  if(Chour >= 1)// 如果倒數時間大於等於1小時
      while(Chour > 1)
        {
          setNumber(Chour*100 + Cminute); //設定當前時分
          colonswitch(); //冒號動作 裡頭有延遲一個秒單位
          Csecond--; //秒數減1
          if(Csecond == -1) // 0秒下數到59秒的時候
          {
            Csecond = 59;
            Cminute--;
          }
          if(Cminute == -1) // 0分下數到59分的時候
          {
            Cminute = 59;
            Chour--;
          }
        }
    else if(Cminute >= 1) // 如果倒數時間小於1小時又大於1分鐘
      while(Cminute > 1)
      {
        setNumber(Cminute*100 + Csecond); //設定當前分秒
        colonswitch(); //冒號動作 裡頭有延遲一個秒單位
        Csecond--; //秒數減1
        if(Csecond == -1) // 0秒下數到59秒的時候
        {
          Csecond = 59;
          Cminute--;
        }
      }
    else
    {
      time_previous = millis();
      while(Csecond > 0) // 如果倒數時間小於1分鐘
      {
        setNumber(Csecond*100+Ccs); //設定當前秒厘秒 (厘秒未測試)
        unsigned long time_now = millis();
        if(Ccs ==0)
          Ccs = 99;
        for(i=0;i<100;i++)
        {
          time_now = millis();
          if(time_now - time_previous > 10)
          {
            setNumber(Csecond*100+Ccs);
            Ccs--;
            time_previous += 10; //依次使時間計數器加10毫秒
          }
        }
        colon = !colon;
        digitalWrite(pin_4_a,colon);
        digitalWrite(pin_4_b,colon); //冒號動作 
        Csecond--; //秒數減1
      }
      /*while(Csecond > 0) // 如果倒數時間小於1分鐘
      {
        setNumber(Csecond*100); // 設定當前秒
        colonswitch(); // 冒號動作 有延遲1秒
        Csecond--; //秒數減1
      }
    }*/
}
}
//碼表
void countUptimer()
{
  //byte Ccs = 0; // 厘秒(10^-2秒)
  if(Chour >= 1)// 如果碼表時間大於等於1小時
        {
          setNumber(Chour*100 + Cminute); //設定當前時分
          colonswitch(); //冒號動作 裡頭有延遲一個秒單位
          Csecond++; //秒數加1
          if(Csecond == 60) // 59上數到60秒的時候
          {
            Csecond = 0;
            Cminute++;
          }
          if(Cminute == 60) // 59分上數到60分的時候
          {
            Cminute = 0;
            Chour++;
          }
        }
    else if(Cminute >= 1) // 如果碼表時間小於1小時又大於1分鐘
      {
        setNumber(Cminute*100 + Csecond); //設定當前分秒
        colonswitch(); //冒號動作 裡頭有延遲一個秒單位
        Csecond++; //秒數加1
        if(Csecond == 60) // 59秒上數到60秒的時候
        {
          Csecond = 59;
          Cminute++;
        }
      }
    else // 如果碼表時間小於1分鐘
    {
      time_previous = millis();
        setNumber(Csecond*100+Ccs); //設定當前秒厘秒 (厘秒未測試)
        unsigned long time_now = millis();
        for(i=0;i<100;i++)
        {
          time_now = millis();
          if(time_now - time_previous > 10)
          {
            Ccs++;
            time_previous += 10; //依次使時間計數器加10毫秒
            setNumber(Csecond*100+Ccs);
          }
        }
        colon = !colon;
        digitalWrite(pin_4_a,colon);
        digitalWrite(pin_4_b,colon); //冒號動作 
        Csecond++; //秒數加1
      }
      /*while(Csecond > 0) // 如果碼表時間大於1分鐘
      {
        setNumber(Csecond*100); // 設定當前秒
        colonswitch(); // 冒號動作 有延遲1秒
        Csecond++; //秒數加1
      }
    }*/
}
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
// 設定時間計數器   Setup Timers
time_previous = millis();    
}

void loop() {
// 檢查是否有資料可供讀取
if (Serial.available() > 0|| BT.available() > 0) 
{
  char c = Serial.read();
  mode = Serial.read();
  switch(mode)
  {
    
    //從0000上數到9999
    case '0':
    while(Serial.available() == 0) 
      from0000to9999();
    break;
    
    // 從9999下數到0000
    case '1':
    while(Serial.available() == 0) 
      from9999to0000();
    break;

    // 輸入一個數字並顯示
    case '2':
    number = Serial.read();
    if(number > 9999 || number < 0) // 如果數字輸入錯誤則退出
      break;
    while(Serial.available() == 0)
    {
      setNumber(number);
      colonswitch();
    }
    break;

    // 倒數計時器
    case '3':
    //
    // 從手機端取得三個時間數值
    Chour = Serial.read();
    Cminute = Serial.read();
    Csecond = Serial.read();
    countDowntimer();
    while(Serial.available() == 0)
    {
      setNumber(0);
      colonswitch();
    }
    break;

    //
    case '4':
    //碼表
    break;

    
    // 當前時分 預設
    default:
    // 從手機端取得三個時間數值
    Chour = Serial.read();
    Cminute = Serial.read();
    Csecond = Serial.read();
    while(Serial.available() == 0)
    {
      setTimenow();
      pf("Time = %d : %d : %d ",Chour,Cminute,Csecond);
    }
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
