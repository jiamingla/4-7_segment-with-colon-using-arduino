void nowhourcolonmin()
{
  setNumber(Chour*100 + Cminute); //設定當前時：分
  Normalclock;
  pf("Time = %d : %d : %d ",Chour,Cminute,Csecond);
}
void nowmincolonsec()
{
  setNumber(Cminute*100 + Csecond); //設定當前時：分
  Normalclock;
  pf("Time = %d : %d : %d ",Chour,Cminute,Csecond);
}

