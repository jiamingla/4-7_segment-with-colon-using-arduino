//碼表
void countUptimer()
{
  //byte Ccs = 0; // 厘秒(10^-2秒)
  if(Chour >= 1)// 如果碼表時間大於等於1小時
    {
       setNumber(Chour*100 + Cminute); //設定當前時：分
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
    else if(Cminute == 0 ) // 如果碼表時間小於1分鐘
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
