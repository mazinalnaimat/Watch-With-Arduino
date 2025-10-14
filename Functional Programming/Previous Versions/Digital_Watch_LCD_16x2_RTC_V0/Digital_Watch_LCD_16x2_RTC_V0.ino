#include <ThreeWire.h>
#include <RtcDS1302.h>
#include <LiquidCrystal.h>


#define  PushButton1      A1
#define  PushButton2      A2
#define  PushButton3      A3
#define  PushButton4      A4
#define  PushButton5      A5
#define  Light            11
#define  Buzzer           A0


byte Seconds            =   0;//
byte Minutes            =   10;//
byte Hours              =   10;//

byte Minutes_Alarm      =  10;//
byte Hours_Alarm        =  2;//

byte  Minutes_Alarm_Nap  =  10;//
byte  Hours_Alarm_Nap    =   5;//

byte  Minutes_Alarm_Nap_Period   = 5;//

bool _AllowAlarm    = false;//
bool _AllowAlarmNap = false;//

byte Seconds_StopWatch  =  0;//
byte Minutes_StopWatch  =  0;//
byte Hours_StopWatch    =  0;//

int Previous_PushButton1_Value =0;//
int Previous_PushButton2_Value =0;//
int Previous_PushButton3_Value =0;//
int Previous_PushButton4_Value =0;//
int Previous_PushButton5_Value =0;//

int PushButton1_Value  =  0;//
int PushButton2_Value  =  0;//
int PushButton3_Value  =  0;//
int PushButton4_Value  =  0;//
int PushButton5_Value  =  0;//

byte DayInSet = 0;//

byte HijriDay             =    1;//
byte HijriMonthInSet      =    0;//
short HijriYear           = 1440;//
byte GregorianDay         =    1;//
byte GregorianMonthInSet  =    0;//
short GregorianYear       = 2010;//


byte CountForNapTimes = 0;//

int CountPushButton4ForDate             = 0;//
int CountPushButton4ForStopWatch        = 0;//
int CountPushButton4ForSetAlarm         = 0;//
int CountPushButton4ForTuenOffAlarm     = 0;//
int CountPushButton4ForSetTimeAndDate   = 0;//
int CountPushButton4ForStopWatch_Peeb   = 0;//
int CountPushButton4ForTurnAlarmOnClick = 0;//

int  CountPushButton2ForChangeScreen    = 0;//

int CountPushButton3ForStopWatch        = 0;//
int CountPushButton3ForSetAlarm         = 0;//
int CountPushButton3ForSetTimeAndDate   = 0;//
int CountPushButton3ForTuenOffAlarm     = 0;//
int CountPushButton3ForTurnAlarmOnClick = 0;//

int CountPushButton5ForSetAlarm         = 0;//
int CountPushButton5ForSetTimeAndDate   = 0;//

int ChangeTypeOfDate= 0;//


byte CurrentScreen       = 0;//

bool EnterShowDate = 0;
bool EnterShowDay = 0;

short CountAlarmPeep = 0;


LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

ThreeWire myWire(9, 10, 8);        // DAT, CLK, RST
RtcDS1302<ThreeWire> Rtc(myWire);    // RTC Object

  RtcDateTime now = Rtc.GetDateTime();



void WriteFullPointsOnLcd(byte Culomn, byte Row, byte Number)
{
   byte  FullPoints1[8] =
    {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
   };
       lcd.createChar(1, FullPoints1);

  lcd.setCursor(Culomn, Row);
  for(byte i = 0 ; i < Number; i++)
  {
    lcd.write(1);
  }
} 

void WriteOnCleanRowOnLcd( int  Column , int  Row, String Text )
{
  lcd.setCursor(0,Row);
  for(int  i = 0 ; i < Column;i++)
  {
  lcd.print(" ");
  }

  lcd.print(Text);
  lcd.print("               ");

}

bool EventWhilePressingPushButton(int PinName,int & Push_Value)
{
   Push_Value = digitalRead(PinName);
  if(  Push_Value == HIGH)
  {
     return  true;
  }

   else
   {
       return false;
   }
}

void TurnOnBuzzer(int _Buzzer, int Frequency)
{  
   tone(_Buzzer, Frequency);    
}

void TurnOffBuzzer(int _Buzzer)
{
  
   noTone(_Buzzer); 
    
}

bool CheckAlarmTime()
{
     PushButton4_Value = digitalRead(PushButton4);
    PushButton3_Value = digitalRead(PushButton3);

  if((Hours == Hours_Alarm && Minutes == Minutes_Alarm && _AllowAlarm) || (Hours == Hours_Alarm_Nap && Minutes == Minutes_Alarm_Nap && _AllowAlarmNap) ||     (PushButton4_Value == HIGH && PushButton3_Value == HIGH && CurrentScreen == 0))
  {
    return true;
  }
  else
    {
    return false;
  }
  
}

void AllowAlarm()
{
  if(Hours == Hours_Alarm && Minutes == Minutes_Alarm && Seconds == 0)
  {
    _AllowAlarm = true;
  }
}

bool EventAfterPressingPushButtonAndResetValue(int PinName ,int & Push_Value, int & Previous_Push_Value,int & CountPush)
{
  bool Value = EventAfterPressingPushButton(PinName, Push_Value, Previous_Push_Value,CountPush,2);

  if(Value)
  {
   CountPush = 0;

  }

  return Value;


}

int  EventAfterPressingPushButton(int PinName ,int & Push_Value,  int &  Previous_Push_Value, int & CountPush,int  CasesNumber )
{
  Push_Value = digitalRead(PinName);
  if(Push_Value == 1 && Previous_Push_Value == 0   )
  {
      CountPush =CountPush +1;
   
  if(CountPush ==CasesNumber )
  {
    CountPush = 0;
  }
  }

  Previous_Push_Value = Push_Value;
  return CountPush;
}


bool MoveScreen() 
{   
  int PreviuosScreen = CurrentScreen;   
    CurrentScreen =   EventAfterPressingPushButton(PushButton2,PushButton2_Value,Previous_PushButton2_Value, CountPushButton2ForChangeScreen,4) ;  
   return CurrentScreen !=PreviuosScreen;
}

String GetTime(int  _Hours, int  _Minutes, int  _Seconds )
{ 
  String Time = "";
  
  //Set The Hours Part
  if(_Hours <10)
  {
    Time+="0";
  }
  
  Time += String(_Hours);
 Time +=":";
  
  //Set The Minutes Part
  if(_Minutes <10)
  {
    Time+="0";
  }
  
  Time +=String(_Minutes);
  Time +=":";

   //Set The Seconds Part
  if(_Seconds <10)
  {
    Time+="0";
  }
  
  Time += String(_Seconds);
  return Time;
}

String GetGregorianDate(int _Years, int  _MonthNumber, int  _Day)
{
  String GregorianMonthsSet [12]= {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct" ,"Nov","Dec"};
  String Date = "";
  if(_Day < 10)
  {
     Date += "0";   
  }
  Date += String(_Day);
  Date += "  ";

  Date += GregorianMonthsSet[_MonthNumber];

  Date +="  ";
  
  Date +=String(_Years);
  return Date;
   
}

String GetHijriDate(int _Years, int  _MonthNumber, int  _Day)
{
  String HijriMonthsSet    [12]= {"Moh", "Sef",  "Ra1", "Ra2","Jm1", "Jm2", "Rej", "She", "Ram", "Sho" ,"ked","Heg"};
  String Date = "";
  if(_Day < 10)
  {
     Date += "0";   
  }
  Date += String(_Day);
  Date += "  ";

  Date += HijriMonthsSet[_MonthNumber];

  Date +="  ";
  
  Date +=String(_Years);
  return Date;
   
}

String GetDayName(int DayNumber)
{

   String DaysSet  [7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
   String  Day= DaysSet[DayNumber]; 
   return Day;
}


void ShowTimeOnLcd( int Column, int Row,String Time)
{
  WriteOnCleanRowOnLcd( Column, Row, Time);
} 

void ShowGregorianDateOnLcd( int Column, int Row, String Date)
{  
  WriteOnCleanRowOnLcd( Column, Row, Date);
}

void ShowHijriDateOnLcd(int Column, int Row, String Date)
{
  WriteOnCleanRowOnLcd( Column, Row, Date);

}

void ShowDayOnLcd( int Column, int Row, String DayName)
{
  WriteOnCleanRowOnLcd(Column, Row,  DayName);

}

void  ShowAlarmScreenOnLcd(int Column, int Row, String  AlarmTime)
{
    ShowTimeOnLcd(Column, Row, AlarmTime);
}

void TurnOnLight()
{
  analogWrite(Light,0);
 //  analogWrite(Light,255);

}

void TurnOffLight()
{  
  analogWrite(Light,150);
  //analogWrite(Light,0);
}

void UpdateLight()
{
    
  if(EventWhilePressingPushButton(PushButton1, PushButton1_Value))
    {
      TurnOnLight();

    }
     else
      {
        TurnOffLight();
        
      }
}

void ClearRow(int Row)
{
  lcd.setCursor(0, Row);
  lcd.print("                ");
}

void ExchangeAndShowDateOnLcd(int Column, int Row, String GregorianDate, String HijriDate, int Change)
{
  //int NumberOfClearRow = 1;

  

   if(EventWhilePressingPushButton(PushButton4,PushButton4_Value))
   {
     if(Change==0)
   ShowHijriDateOnLcd(Column, Row, HijriDate); 
   else
   ShowGregorianDateOnLcd(Column, Row, GregorianDate); 
   }
   else 
   {
     ClearRow(1);
    // NumberOfClearRow++;
   }
}

void ShowTimeAndDateScreen()
{
  CountAlarmPeep++;
  if(CountAlarmPeep==40)

  {
    CountAlarmPeep = 0;
  }
  bool IsBlankRow1 = 0;
   now = Rtc.GetDateTime();

   if(CheckAlarmTime())
   {

    if(Seconds == 59)
    {
      CountForNapTimes = 0;
     _AllowAlarmNap = false;
    }

    if(EventAfterPressingPushButtonAndResetValue(PushButton4,PushButton4_Value,Previous_PushButton4_Value,CountPushButton4ForTuenOffAlarm))
    {

     _AllowAlarm = false; 
     _AllowAlarmNap = false;
     CountForNapTimes = 0;
     // CountPushButton2ForTuenOffAlarm =0;    //must write with (EventAfterPressingPushButtonAndResetValue)
    }

    else  if(EventAfterPressingPushButtonAndResetValue(PushButton3,PushButton3_Value,Previous_PushButton3_Value,CountPushButton3ForTuenOffAlarm))
    {
      CountPushButton4ForTuenOffAlarm =0; //must write with (EventAfterPressingPushButtonAndResetValue)
      _AllowAlarm = false; 
      _AllowAlarmNap = true;
      CountForNapTimes++;
      Minutes_Alarm_Nap  =  Minutes_Alarm + Minutes_Alarm_Nap_Period*CountForNapTimes ;
      Hours_Alarm_Nap = Hours_Alarm;
      if(Minutes_Alarm_Nap>=60)
      {
        Minutes_Alarm_Nap = Minutes_Alarm_Nap -60;
        Hours_Alarm_Nap++ ;
      }
        if(Hours_Alarm_Nap>=24)
      {
      Hours_Alarm_Nap = 0;
      }


       //  Hours_Alarm_Nap    =  5;
      if(CountForNapTimes == 13)
      {
        CountForNapTimes = 0;
        _AllowAlarmNap = false;
      
      }
    }

    else//not all
    {
      if(CountAlarmPeep/2<4)
      {
        TurnOnBuzzer(Buzzer,3000);
      
      }
      else if(CountAlarmPeep/2<7)
      {
              TurnOffBuzzer(Buzzer);
      }
      else if(CountAlarmPeep/2<5 || (CountAlarmPeep/2>=10 && CountAlarmPeep /2< 15))
      {
        TurnOnBuzzer(Buzzer,3000);
      }
      else
      {
        TurnOffBuzzer(Buzzer);
      }      
    }
   }
   else
   {
        TurnOffBuzzer(Buzzer);
   }

   CountPushButton3ForSetAlarm       = 0;
   CountPushButton3ForSetTimeAndDate = 0;
   AllowAlarm();
   UpdateLight();

   if(MoveScreen())
   {
      return;
   }

   if(PushButton2_Value == LOW && PushButton3_Value == LOW)
                  ShowTimeOnLcd(4, 0, GetTime(now.Hour(),now.Minute(),now.Second())); 
           
  if(EventAfterPressingPushButtonAndResetValue( PushButton4, PushButton4_Value,Previous_PushButton4_Value, CountPushButton4ForDate))
  {
      ChangeTypeOfDate= ChangeTypeOfDate^ 1;
    // CountPushButton2ForDate = 0;
  }

  
  if(EventWhilePressingPushButton(PushButton4,PushButton4_Value)&& !EnterShowDay)
  {         
    GregorianYear = now.Year();
    GregorianMonthInSet =now.Month()-1 ;
    GregorianDay = now.Day();

    ExchangeAndShowDateOnLcd(2, 1,GetGregorianDate(GregorianYear, GregorianMonthInSet, GregorianDay),GetHijriDate(HijriYear, HijriMonthInSet, HijriDay),ChangeTypeOfDate);
     IsBlankRow1 =1;
     EnterShowDate = 1;
  }
  else
  {
    EnterShowDate = 0;
  
  }

  if(EventWhilePressingPushButton(PushButton3,PushButton3_Value))
  {
      ShowAlarmScreenOnLcd(4, 0, GetTime( Hours_Alarm, Minutes_Alarm, 0));  
  }

  if(EventWhilePressingPushButton(PushButton5,PushButton5_Value) && !EnterShowDate)  
  {
    ShowDayOnLcd(6,1,GetDayName(DayInSet)); 
         IsBlankRow1 =1;
         EnterShowDay= 1;

  }
   else
  {
    EnterShowDay = 0;
  
  }
  if(!IsBlankRow1)
   ClearRow(1);


}

bool TimeChange(short past)
{
   short Past = past;
   
   short Current = now.Second();
   if(Current != Past)
   {
      Past = now.Second();
     return 1; 
   }   

  return 0;
}
void UpdateStopTime(byte & _Hours_StopWatch, byte & _Minutes_StopWatch, byte & _Seconds_StopWatch)
{
   short past = now.Second();
  if(digitalRead(10))
  if(CountPushButton4ForStopWatch==1)
  {            
          _Seconds_StopWatch++;    
  }

  if(_Seconds_StopWatch == 60)
  {
    _Seconds_StopWatch = 0;
    _Minutes_StopWatch++;
  }

  if(_Minutes_StopWatch == 60)
  {
    _Minutes_StopWatch = 0;
    _Hours_StopWatch++;
  }

  if(_Hours_StopWatch == 24)
  {
    _Hours_StopWatch = 0;
  }
}
void ShowStopWatchScreen()
{
   if(CheckAlarmTime())
   {

    if(Seconds == 59)
    {
      CountForNapTimes = 0;
     _AllowAlarmNap = false;
    }

    if(EventAfterPressingPushButtonAndResetValue(PushButton4,PushButton4_Value,Previous_PushButton4_Value,CountPushButton4ForTuenOffAlarm))
    {

     _AllowAlarm = false; 
     _AllowAlarmNap = false;
     CountForNapTimes = 0;
     // CountPushButton2ForTuenOffAlarm =0;    //must write with (EventAfterPressingPushButtonAndResetValue)
    }

    else  if(EventAfterPressingPushButtonAndResetValue(PushButton3,PushButton3_Value,Previous_PushButton3_Value,CountPushButton3ForTuenOffAlarm))
    {
      CountPushButton4ForTuenOffAlarm =0; //must write with (EventAfterPressingPushButtonAndResetValue)
      _AllowAlarm = false; 
      _AllowAlarmNap = true;
      CountForNapTimes++;
      Minutes_Alarm_Nap  =  Minutes_Alarm + Minutes_Alarm_Nap_Period*CountForNapTimes ;
      Hours_Alarm_Nap = Hours_Alarm;
      if(Minutes_Alarm_Nap>=60)
      {
        Minutes_Alarm_Nap = Minutes_Alarm_Nap -60;
        Hours_Alarm_Nap++ ;
      }
        if(Hours_Alarm_Nap>=24)
      {
      Hours_Alarm_Nap = 0;
      }


       //  Hours_Alarm_Nap    =  5;
      if(CountForNapTimes == 13)
      {
        CountForNapTimes = 0;
        _AllowAlarmNap = false;
      
      }
    }

    else//not all
    {
      if(CountAlarmPeep/2<4)
      {
        TurnOnBuzzer(Buzzer,3000);
      
      }
      else if(CountAlarmPeep/2<7)
      {
              TurnOffBuzzer(Buzzer);
      }
      else if(CountAlarmPeep/2<5 || (CountAlarmPeep/2>=10 && CountAlarmPeep /2< 15))
      {
        TurnOnBuzzer(Buzzer,3000);
      }
      else
      {
        TurnOffBuzzer(Buzzer);
      }      
    }
   }
   else
   {
        TurnOffBuzzer(Buzzer);
   }
    UpdateLight();
        AllowAlarm();
  if(MoveScreen())
   {
      return;
   }

     if( EventAfterPressingPushButtonAndResetValue(PushButton4,PushButton4_Value,Previous_PushButton4_Value,CountPushButton4ForStopWatch_Peeb))
    {
          TurnOnBuzzer(Buzzer,3000);
          TurnOffBuzzer(Buzzer);
        //  CountPushButton2ForStopWatch_Peeb =0;
           CountPushButton4ForStopWatch ^=1;
    }

    if(EventAfterPressingPushButtonAndResetValue(PushButton3,PushButton3_Value,Previous_PushButton3_Value,CountPushButton3ForStopWatch))
    {
        TurnOnBuzzer(Buzzer,3000);
        TurnOffBuzzer(Buzzer);
        CountPushButton4ForStopWatch = 0;
        Hours_StopWatch = 0;
        Minutes_StopWatch = 0;
        Seconds_StopWatch = 0;
        // Previous_PushButton2_Value =0;
    }
   ShowTimeOnLcd(4,0,GetTime(Hours_StopWatch, Minutes_StopWatch,Seconds_StopWatch) ); 

}

void ShowSetAlarmScreen()
{
     if(CheckAlarmTime())
   {

    if(Seconds == 59)
    {
      CountForNapTimes = 0;
     _AllowAlarmNap = false;
    }

    if(EventAfterPressingPushButtonAndResetValue(PushButton4,PushButton4_Value,Previous_PushButton4_Value,CountPushButton4ForTuenOffAlarm))
    {

     _AllowAlarm = false; 
     _AllowAlarmNap = false;
     CountForNapTimes = 0;
     // CountPushButton2ForTuenOffAlarm =0;    //must write with (EventAfterPressingPushButtonAndResetValue)
    }

    else  if(EventAfterPressingPushButtonAndResetValue(PushButton3,PushButton3_Value,Previous_PushButton3_Value,CountPushButton3ForTuenOffAlarm))
    {
      CountPushButton4ForTuenOffAlarm =0; //must write with (EventAfterPressingPushButtonAndResetValue)
      _AllowAlarm = false; 
      _AllowAlarmNap = true;
      CountForNapTimes++;
      Minutes_Alarm_Nap  =  Minutes_Alarm + Minutes_Alarm_Nap_Period*CountForNapTimes ;
      Hours_Alarm_Nap = Hours_Alarm;
      if(Minutes_Alarm_Nap>=60)
      {
        Minutes_Alarm_Nap = Minutes_Alarm_Nap -60;
        Hours_Alarm_Nap++ ;
      }
        if(Hours_Alarm_Nap>=24)
      {
      Hours_Alarm_Nap = 0;
      }


       //  Hours_Alarm_Nap    =  5;
      if(CountForNapTimes == 13)
      {
        CountForNapTimes = 0;
        _AllowAlarmNap = false;
      
      }
    }

    else//not all
    {
      if(CountAlarmPeep/2<4)
      {
        TurnOnBuzzer(Buzzer,3000);
      
      }
      else if(CountAlarmPeep/2<7)
      {
              TurnOffBuzzer(Buzzer);
      }
      else if(CountAlarmPeep/2<5 || (CountAlarmPeep/2>=10 && CountAlarmPeep /2< 15))
      {
        TurnOnBuzzer(Buzzer,3000);
      }
      else
      {
        TurnOffBuzzer(Buzzer);
      }      
    }
   }
   else
   {
        TurnOffBuzzer(Buzzer);
   }
  if(MoveScreen())
   {
      return;
   }
}

void ShowSetDateAndTimeScreen()
{
  if(MoveScreen())
   {
      return;
   }
}

void ShowScreen()
{

    if(CurrentScreen == 0)
    {
        ShowTimeAndDateScreen();
    }
       if (CurrentScreen == 1)
    {
        ShowStopWatchScreen();
    }
      if (CurrentScreen == 2)
    {
      ShowSetAlarmScreen();
    }

     if (CurrentScreen == 3)
    {
      ShowSetDateAndTimeScreen();
    }

    UpdateStopTime( Hours_StopWatch, Minutes_StopWatch, Seconds_StopWatch);
    
}


void setup ()
{
  lcd.begin(16, 2); 
  lcd.clear();
  Rtc.Begin();
  Serial.begin(9600);

 //RtcDateTime currentTime = RtcDateTime(__DATE__ , __TIME__);
 //Rtc.SetDateTime(currentTime);

  //Rtc.SetDateTime(RtcDateTime(2010,5,15,15,5,2));
  
//Rtc.SetDateTime(RtcDateTime("1 15 2010","15:05:02"));

}

void loop()
{
  ShowScreen();  
}
