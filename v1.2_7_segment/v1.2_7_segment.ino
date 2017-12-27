/*七段顯示器1.2版 2017/12/15
using Arduino Mega 2560
版本更新:從一段變四段
功能如下:
0000~9999
0000~2359(還沒
顯示輸入的四位數字(還沒

四個七段 + 冒號(由左至右排列)
8/8/8/8/:
每個代碼
0/1/2/3/4
0用上的腳位定義為:22,24,26,28,30,32,34
1用上的腳位定義為:23,25,27,29,31,33,35
2用上的腳位定義為:36,38,40,42,44,46,48
3用上的腳位定義為:37,39,41,43,45,47,49
4用上的腳位定義為:50(暫定)
*/
//
//定義腳位
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
  #define pin_4 50
//定義七段顯示器位數
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
    va_list args;
    va_start (args, fmt );
    vsnprintf(tmp, 128, fmt, args);
    va_end (args);
    Serial.print(tmp);
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
  // 注意，此處以delay(5)隔開每個位數的更新 
  setDigit(0, n3); delay(5);
  setDigit(1, n2); delay(5);
  setDigit(2, n1); delay(5);
  setDigit(3, n0); delay(5);
}

unsigned long time_previous;
void setup() {
  
Serial.begin(9600);
for(int i = 0;i < 4;i++)
  for(int j = 0;j < 7;j++)
    pinMode(seg4[i][j],OUTPUT);
time_previous = millis();    
}
int number = 0;
void loop() {

// 經過一秒後就讓number加1 
unsigned long time_now = millis();
if(time_now - time_previous > 1000)
{
  number++;
  time_previous += 1000;
  pf("number=%d\n", number);
}
// 不斷地寫入數字
  setNumber(number);
}
