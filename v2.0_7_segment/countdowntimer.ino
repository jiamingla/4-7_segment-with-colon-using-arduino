// 倒數計時器
void countDowntimer()
{
   if(Chour >= 1)// 如果倒數時間大於等於1小時
     {
        setNumber(Chour*100 + Cminute); //設定當前時分
        colonswitch(); //冒號動作 
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
   else if(Cminute >= 1)// 如果倒數時間小於1小時又大於1分鐘
     {
        
        setNumber(Cminute*100 + Csecond); //設定當前分秒
        colonswitch(); //冒號動作 
        Csecond--; //秒數減1
        if(Csecond == -1) // 0秒下數到59秒的時候
        {
          Csecond = 59;
          Cminute--;
        }
     }
   //byte Ccs = 100; // 厘秒(10^-2秒)
   /*time_previous = millis();
   if(Csecond > 0) // 如果倒數時間小於1分鐘
     {
        setNumber(Csecond*100+Ccs); //設定當前秒厘秒 (厘秒未測試)
        unsigned long time_now = millis();
        if(Ccs ==-1)
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
      while(Csecond > 0) // 如果倒數時間小於1分鐘
      {
        setNumber(Csecond*100); // 設定當前秒
        colonswitch(); // 冒號動作 有延遲1秒
        Csecond--; //秒數減1
      }*/
}
