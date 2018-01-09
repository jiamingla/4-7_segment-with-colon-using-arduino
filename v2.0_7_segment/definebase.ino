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
void colonswitch() //沒有DALAY啦
{
  colon = !colon;
  digitalWrite(pin_4_a,colon);
  digitalWrite(pin_4_b,colon);
}
// 從0000上數到9999
void from0000to9999()
{
  if(number == 9999)
    number = 0;
  number++;
  // 不斷地寫入數字
  setNumber(number);
  // 冒號動作 
  colonswitch();  
  /*
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
    */
}
// 從9999下數到0000
void from9999to0000()
{
  if(number == 0)
    number = 9999;
  number--;
  // 不斷地寫入數字
  setNumber(number);
  // 冒號動作 
  colonswitch();  
  /*
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
    */
}
// 設定當前時分
void Normalclock()
{
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

// 一支方便的函式，以格式字串輸出到序列埠
void pf(const char *fmt, ... ){
    char tmp[128]; // max is 128 chars
    va_list args; //宣告一個指標，讓它指向引數串列。
    va_start (args, fmt ); //初始化這個指標，讓它真正指向正確的引數串列開頭。
    vsnprintf(tmp, 128, fmt, args);
    va_end (args); //來取得va_list中的資料。
    Serial.print(tmp);//清除這個指標，把它設為NULL。
}
void BTpf(const char *fmt, ... ){
    char tmp[128]; // max is 128 chars
    va_list args; //宣告一個指標，讓它指向引數串列。
    va_start (args, fmt ); //初始化這個指標，讓它真正指向正確的引數串列開頭。
    vsnprintf(tmp, 128, fmt, args);
    va_end (args); //來取得va_list中的資料。
    BT.print(tmp);//清除這個指標，把它設為NULL。
}

// 從手機端取得三個時間數值
void phone3valuenow()
{
  Chourtens = BT.read();
  Chourones = BT.read();
  Cminutetens = BT.read();
  Cminuteones = BT.read();
  Csecondtens = BT.read();
  Csecondones = BT.read();
  Chour = Chourtens*10 + Chourones;
  Cminute = Cminutetens*10 +  Cminuteones;
  Csecond = Csecondtens*10 + Csecondones;
}
