


#include <string.h>
#include <LiquidCrystal.h>

unsigned long StartTime = 0;
unsigned long EndTime   = 0;
unsigned long Period    = 0;

unsigned long PreviuosTime = 0;

#define  PushButton1       6
#define  PushButton2       7
#define  PushButton3       8
#define  PushButton4       9
#define  PushButton5      12
#define  Light            11
#define  Buzzer           A0

bool IsDelayDone = false;

byte Seconds            =   0;
byte Minutes            =   10;
byte Hours              =   10;


byte Minutes_Alarm      =  10;
byte Hours_Alarm        =  2;

byte  Minutes_Alarm_Nap  =  10;
byte  Hours_Alarm_Nap    =   5;

byte  Minutes_Alarm_Nap_Period   = 5;

bool _AllowAlarm    = false;
bool _AllowAlarmNap = false;

byte Seconds_StopWatch  =  0;
byte Minutes_StopWatch  =  0;
byte Hours_StopWatch    =  0;


int PushButton1_Value  =  0;
int PushButton2_Value  =  0;
int PushButton3_Value  =  0;
int PushButton4_Value  =  0;
int PushButton5_Value  =  0;


byte DayInSet = 0;

byte HijriDay             =    1;
byte HijriMonthInSet      =    0;
short HijriYear           = 1440;
byte GregorianDay         =    1;
byte GregorianMonthInSet  =    0;
short GregorianYear       = 2010;



byte CountForNapTimes = 0;

int CountPushButton2ForDate             = 0;
int CountPushButton2ForStopWatch        = 0;
int CountPushButton2ForSetAlarm         = 0;
int CountPushButton2ForTuenOffAlarm     = 0;
int CountPushButton2ForSetTimeAndDate   = 0;
int CountPushButton2ForStopWatch_Peeb   = 0;
int CountPushButton2ForTurnAlarmOnClick = 0;

int  CountPushButton3ForChangeScreen    = 0;

int CountPushButton4ForStopWatch        = 0;
int CountPushButton4ForSetAlarm         = 0;
int CountPushButton4ForSetTimeAndDate   = 0;
int CountPushButton4ForTuenOffAlarm     = 0;
int CountPushButton4ForTurnAlarmOnClick = 0;

int CountPushButton5ForSetAlarm         = 0;
int CountPushButton5ForSetTimeAndDate   = 0;

int ChangeTypeOfDate= 0;


byte CurrentScreen       = 0;



LiquidCrystal lcd_1(13, 10, 5, 4, 3, 2);

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
       lcd_1.createChar(1, FullPoints1);

  lcd_1.setCursor(Culomn, Row);
  for(byte i = 0 ; i < Number; i++)
  {
    lcd_1.write(1);
  }
} 

void WriteOnCleanRowOnLcd( int  Column , int  Row, String Text )
{
  lcd_1.setCursor(0,Row);
  for(int  i = 0 ; i < Column;i++)
  {
  lcd_1.print(" ");
  }

  lcd_1.print(Text);
  lcd_1.print("               ");

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
     PushButton2_Value = digitalRead(PushButton2);
    PushButton4_Value = digitalRead(PushButton4);

  if((Hours == Hours_Alarm && Minutes == Minutes_Alarm && _AllowAlarm) || (Hours == Hours_Alarm_Nap && Minutes == Minutes_Alarm_Nap && _AllowAlarmNap) ||     (PushButton2_Value == HIGH && PushButton4_Value == HIGH && CurrentScreen == 0))
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


bool EventAfterPressingPushButtonAndResetValue(int PinName ,int & Push_Value ,int & CountPush)
{
  Push_Value = digitalRead(PinName);
  bool Reset = false;
    
  if(CountPush == 0 && Push_Value == HIGH )
  {
    CountPush =1;
  }

  if( CountPush == 1 && Push_Value == LOW)
  {
  
    Reset     =true; 
  }

  if(Reset)
  {
    CountPush = 0;
  }

  return Reset;
}

int  EventAfterPressingPushButton(int PinName ,int & Push_Value, int & CountPush, int  CasesNumber )
{
  
  Push_Value = digitalRead(PinName);
  for(int i = 0; i <2*CasesNumber; i++ )
  {   
    
    if(CountPush == i && Push_Value == HIGH && (CountPush % 2 ==0))
    {
      CountPush =CountPush +1;
    }
 
    if( CountPush == i+1 && Push_Value == LOW && (CountPush % 2 ==1) )
    {
      CountPush = CountPush  +1;
     break;
    }
  }

  if(CountPush /2   ==CasesNumber )
  {
    CountPush = 0;
  }

  return CountPush/2;
  
}


bool MoveScreen() 
{   
  int PreviuosScreen = CurrentScreen;   
    CurrentScreen =   EventAfterPressingPushButton(PushButton3,PushButton3_Value, CountPushButton3ForChangeScreen,4) ;  
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
  analogWrite(Light,120);
 //  analogWrite(Light,255);

}

void TurnOffLight()
{  
  analogWrite(Light,160);
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

void UpdateTheTimeAndDate(byte &_Hours, byte & _Minutes, byte & _Seconds, byte & _Hours_StopWatch, byte & _Minutes_StopWatch, byte & _Seconds_StopWatch, byte & _GregorianDay, byte & _HijriDay, byte & _DayInSet)
{

     if(_Seconds != 59)
     {              
    if(CountPushButton2ForStopWatch==2)
      {            
          _Seconds_StopWatch++;    
      }

       _Seconds++;
     }
      
      else
        {
        _Seconds = 0;
        _Minutes++;

      }

       if(_Minutes == 60)
     {
       _Minutes = 0;
      _Hours++;

     }

         if(_Hours == 24)
     {
       _Hours = 0;
       _HijriDay++;
       _GregorianDay++;
       _DayInSet++;

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


void ClearRow(int Row)
{
  lcd_1.setCursor(0, Row);
  lcd_1.print("                ");
}



void ExchangeAndShowDateOnLcd(int Column, int Row, String GregorianDate, String HijriDate, int Change)
{
  //int NumberOfClearRow = 1;

  

   if(EventWhilePressingPushButton(PushButton2,PushButton2_Value))
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

   for(int i = 0; i < 20;i++)        
      { 
    IsDelayDone = false;
  
         StartTime = millis();
        

       if(CheckAlarmTime())
       {
          if(Seconds == 59)
          {
            CountForNapTimes = 0;
                      _AllowAlarmNap = false;

          }
         if(EventAfterPressingPushButtonAndResetValue(PushButton2,PushButton2_Value,CountPushButton2ForTuenOffAlarm))
         {
           _AllowAlarm = false; 
           _AllowAlarmNap = false;
           CountForNapTimes = 0;
         }
         else  if(EventAfterPressingPushButtonAndResetValue(PushButton4,PushButton4_Value,CountPushButton4ForTuenOffAlarm))
         {
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
         else
         {
            if(i<4)
            {
              TurnOnBuzzer(Buzzer,3000);
            
            }
            else if(i<7)
            {
                    TurnOffBuzzer(Buzzer);
            }
            else if(i<5 || (i>=10 && i < 15))
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
            CountPushButton2ForSetTimeAndDate = 0;
            CountPushButton4ForSetAlarm       = 0;
            CountPushButton4ForSetTimeAndDate = 0;
            AllowAlarm();
             UpdateLight();
            if(MoveScreen())
            {
              break;
            }
           
            if(PushButton3_Value == LOW && PushButton4_Value == LOW)
                  ShowTimeOnLcd(4, 0, GetTime(Hours, Minutes, Seconds)); 
            
            if(EventAfterPressingPushButtonAndResetValue( PushButton2, PushButton2_Value, CountPushButton2ForDate))
             {
               ChangeTypeOfDate= ChangeTypeOfDate^ 1;
             }

           if(EventWhilePressingPushButton(PushButton2,PushButton2_Value))
           { 
            
             ExchangeAndShowDateOnLcd(2, 1,GetGregorianDate(GregorianYear, GregorianMonthInSet, GregorianDay),GetHijriDate(HijriYear, HijriMonthInSet, HijriDay),ChangeTypeOfDate);
              if(IsDelayDone == false)
            {               
                  delay(29);  
                                   IsDelayDone =  true;
            }
            }
            

            
            if(EventWhilePressingPushButton(PushButton4,PushButton4_Value))
              
              {
                 ShowAlarmScreenOnLcd(4, 0, GetTime( Hours_Alarm, Minutes_Alarm, 0));  
            
                  if(IsDelayDone == false)
                  {               
                        delay(40);  
                                        IsDelayDone =  true;
                  }

              }
            if(EventWhilePressingPushButton(PushButton5,PushButton5_Value))
            
            {
              ShowDayOnLcd(6,1,GetDayName(DayInSet));
                 if(IsDelayDone == false)
            {               
              delay(32);
               IsDelayDone =  true;
            }
            }
         
           
           if(IsDelayDone == false)
           {     
                        ClearRow(1);
                   delay(35);
           }

       EndTime = millis();
       Period = EndTime - StartTime;
       Serial.println(Period);
      }
        

}

void ShowStopWatchScreen()
{
          
         
           for(int i = 0; i < 20;i++)        
            {
                       StartTime = millis();

      if(CheckAlarmTime())
       {
          if(Seconds == 59)
          {
            CountForNapTimes = 0;
                      _AllowAlarmNap = false;

          }
         if(EventAfterPressingPushButtonAndResetValue(PushButton2,PushButton2_Value,CountPushButton2ForTuenOffAlarm))
         {
           _AllowAlarm = false; 
           _AllowAlarmNap = false;
           CountForNapTimes = 0;
         }
         else  if(EventAfterPressingPushButtonAndResetValue(PushButton4,PushButton4_Value,CountPushButton4ForTuenOffAlarm))
         {
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


          //Hours_Alarm_Nap    =  5;
          if(CountForNapTimes == 13)
          {
            CountForNapTimes = 0;
            _AllowAlarmNap = false;
          
          }
         }
         else
         {
            if(i<4)
            {
              TurnOnBuzzer(Buzzer,3000);
            
            }
            else if(i<7)
            {
                    TurnOffBuzzer(Buzzer);
            }
            else if(i<5 || (i>=10 && i < 15))
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
              break;
            }
         UpdateLight();
         AllowAlarm();

           if( EventAfterPressingPushButtonAndResetValue(PushButton2,PushButton2_Value,CountPushButton2ForStopWatch_Peeb))
             {
                  TurnOnBuzzer(Buzzer,3000);
                  delay(5);
                  TurnOffBuzzer(Buzzer);
                               
             }
          
             EventAfterPressingPushButton(PushButton2,PushButton2_Value,CountPushButton2ForStopWatch,2);
         
          
               if(EventAfterPressingPushButtonAndResetValue(PushButton4,PushButton4_Value,CountPushButton4ForStopWatch))
              {
                               TurnOnBuzzer(Buzzer,3000);
                               delay(5);
                                      TurnOffBuzzer(Buzzer);


                 CountPushButton2ForStopWatch = 0;
                  Hours_StopWatch = 0;
                  Minutes_StopWatch = 0;
                  Seconds_StopWatch = 0;
              }
          
          
             ShowTimeOnLcd(4,0,GetTime(Hours_StopWatch, Minutes_StopWatch,Seconds_StopWatch) ); 
              delay(40);
               EndTime = millis();
       Period = EndTime - StartTime;
       Serial.println(Period);
          }
}

void ShowSetAlarmScreen()
{
  for(int i = 0; i < 20;i++)        
  { 
    StartTime = millis();

    if(CheckAlarmTime())
       {
          if(Seconds == 59)
          {
            CountForNapTimes = 0;
                      _AllowAlarmNap = false;

          }
         if(EventAfterPressingPushButtonAndResetValue(PushButton2,PushButton2_Value,CountPushButton2ForTuenOffAlarm))
         {
           _AllowAlarm = false; 
           _AllowAlarmNap = false;
           CountForNapTimes = 0;
         }
         else  if(EventAfterPressingPushButtonAndResetValue(PushButton4,PushButton4_Value,CountPushButton4ForTuenOffAlarm))
         {
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


         // Hours_Alarm_Nap    =  5;
          if(CountForNapTimes == 13)
          {
            CountForNapTimes = 0;
            _AllowAlarmNap = false;
          
          }
         }
         else
         {
            if(i<4)
            {
              TurnOnBuzzer(Buzzer,3000);
            
            }
            else if(i<7)
            {
                    TurnOffBuzzer(Buzzer);
            }
            else if(i<5 || (i>=10 && i < 15))
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
          

       AllowAlarm();
      if(MoveScreen())
       {
          break;
       }

        UpdateLight();

             
        if(EventAfterPressingPushButtonAndResetValue(PushButton2, PushButton2_Value, CountPushButton2ForSetAlarm))
        {
          if(EventAfterPressingPushButton(PushButton4,PushButton4_Value,CountPushButton4ForSetAlarm,2)==0)
          {
            if(Minutes_Alarm < 59)
            {
              
              Minutes_Alarm++;
            }
            else
            {
              Minutes_Alarm = 0;
            }
            CountPushButton2ForSetAlarm = 0;
          }
    
          else if(EventAfterPressingPushButton(PushButton4,PushButton4_Value,CountPushButton4ForSetAlarm,2)==1)
          {
            
            if(Hours_Alarm < 23)
            {
              Hours_Alarm++;
            }
            else
            {
              Hours_Alarm = 0;
            }
            CountPushButton2ForSetAlarm = 0;
          }
          
        }
    
        if(EventAfterPressingPushButtonAndResetValue(PushButton5, PushButton5_Value, CountPushButton5ForSetAlarm))
        {
          if(EventAfterPressingPushButton(PushButton4,PushButton4_Value,CountPushButton4ForSetAlarm,2)==0)
            {if(Minutes_Alarm >0)
            {
              
              Minutes_Alarm--;
            }
            else
            {
              Minutes_Alarm = 59;
            }
            CountPushButton5ForSetAlarm = 0;
            }
    
          else if(EventAfterPressingPushButton(PushButton4,PushButton4_Value,CountPushButton4ForSetAlarm,2)==1)
          {
            
            if(Hours_Alarm >0 )
            {
              Hours_Alarm--;
            }
            else
            {
              Hours_Alarm = 23;
            }
            CountPushButton5ForSetAlarm = 0;
          }
          
        }
    
        if(EventAfterPressingPushButton(PushButton4,PushButton4_Value,CountPushButton4ForSetAlarm,2)==0)
        {              
          if(i<5 || (i>=10 && i < 15))  
          {
            ShowTimeOnLcd(4,0,GetTime(Hours_Alarm,Minutes_Alarm,0));
            delay(5);           
          }
          else
          {
              WriteFullPointsOnLcd(7,0,2);
              delay(5);        
          }

        }
    
          else if(EventAfterPressingPushButton(PushButton4,PushButton4_Value,CountPushButton4ForSetAlarm,2)==1)
        {

          if(i<5 || (i>=10 && i < 15))  
          {
          ShowTimeOnLcd(4,0,GetTime(Hours_Alarm,Minutes_Alarm,0));
          delay(5);       
          }
            else
            {
            WriteFullPointsOnLcd(4,0,2);                
          delay(5);   
            }
      

        }

              
        delay(38);
            EndTime = millis();
       Period = EndTime - StartTime;
       Serial.println(Period);

  }
            
            
}


void ShowSetDateAndTimeScreen()
{ 
    for(int i = 0; i < 20;i++)        
      { 
       StartTime = millis();
     if(CheckAlarmTime())
       {
          if(Seconds == 59)
          {
            CountForNapTimes = 0;
                      _AllowAlarmNap = false;

          }
         if(EventAfterPressingPushButtonAndResetValue(PushButton2,PushButton2_Value,CountPushButton2ForTuenOffAlarm))
         {
           _AllowAlarm = false; 
           _AllowAlarmNap = false;
           CountForNapTimes = 0;
         }
         else  if(EventAfterPressingPushButtonAndResetValue(PushButton4,PushButton4_Value,CountPushButton4ForTuenOffAlarm))
         {
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


          //Hours_Alarm_Nap    =  5;
          if(CountForNapTimes == 13)
          {
            CountForNapTimes = 0;
            _AllowAlarmNap = false;
          
          }
         }
         else
         {
            if(i<4)
            {
              TurnOnBuzzer(Buzzer,3000);
            
            }
            else if(i<7)
            {
                    TurnOffBuzzer(Buzzer);
            }
            else if(i<5 || (i>=10 && i < 15))
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
              break;
            }
                  UpdateLight();
        

               if(EventAfterPressingPushButtonAndResetValue(PushButton2,PushButton2_Value,CountPushButton2ForSetTimeAndDate))
                {
                  if(EventAfterPressingPushButton(PushButton4,PushButton4_Value,CountPushButton4ForSetTimeAndDate,10)==0)
                   {
                      Seconds = 0;
                    
                    CountPushButton2ForSetTimeAndDate = 0;
                    ShowTimeOnLcd(4,0,GetTime(Hours,Minutes,Seconds));
                      
                   }
                  else if(EventAfterPressingPushButton(PushButton4,PushButton4_Value,CountPushButton4ForSetTimeAndDate,10)==1)
                   {
                      if(Minutes < 59)
                      {

                       Minutes++;
                      }
                      else
                      {
                        Minutes = 0;
                      }
                      ShowTimeOnLcd(4,0,GetTime(Hours,Minutes,Seconds));
                        

                   }
            
                 else  if(EventAfterPressingPushButton(PushButton4,PushButton4_Value,CountPushButton4ForSetTimeAndDate,10)==2)
                  {
                    
                      if(Hours < 23)
                      {
                       Hours++;
                      }
                      else
                      {
                        Hours = 0;
                      }
                  
                      ShowTimeOnLcd(4,0,GetTime(Hours,Minutes,Seconds));
                        
                    }
                 
                   else if(EventAfterPressingPushButton(PushButton4,PushButton4_Value,CountPushButton4ForSetTimeAndDate,10)==3)
                  {
                    
                    if(GregorianDay < 31)
                    {
                     GregorianDay++;
                    }
                    else
                    {
                      GregorianDay = 1;
                    }
                    ShowGregorianDateOnLcd(2,0,GetGregorianDate(GregorianYear, GregorianMonthInSet, GregorianDay));
                      

                  }
                 
                   else if(EventAfterPressingPushButton(PushButton4,PushButton4_Value,CountPushButton4ForSetTimeAndDate,10)==4)
                  {
                    
                    if(GregorianMonthInSet < 11)
                    {
                     GregorianMonthInSet++;
                    }
                    else
                    {
                      GregorianMonthInSet = 0;
                    }
                    ShowGregorianDateOnLcd(2,0,GetGregorianDate(GregorianYear, GregorianMonthInSet, GregorianDay));
                       
                     
                  }
                   else if(EventAfterPressingPushButton(PushButton4,PushButton4_Value,CountPushButton4ForSetTimeAndDate,10)==5)
                  {
                    
                    
                     GregorianYear++;
                    
                     
                    ShowGregorianDateOnLcd(2,0,GetGregorianDate(GregorianYear, GregorianMonthInSet, GregorianDay));
                      
                  }
                 
                   else if(EventAfterPressingPushButton(PushButton4,PushButton4_Value,CountPushButton4ForSetTimeAndDate,10)==6)
                  {
                    
                    if(HijriDay < 30)
                    {
                     HijriDay++;
                    }
                    else
                    {
                      HijriDay = 1;
                    }
                     
                    ShowHijriDateOnLcd(2,0,GetHijriDate(HijriYear, HijriMonthInSet, HijriDay));
                      
                 }
                 
                  else  if(EventAfterPressingPushButton(PushButton4,PushButton4_Value,CountPushButton4ForSetTimeAndDate,10)==7)
                  {
                    
                    if(HijriMonthInSet < 11)
                    {
                     HijriMonthInSet++;
                    }
                    else
                    {
                      HijriMonthInSet = 0;
                    }
                      
                    ShowHijriDateOnLcd(2,0,GetHijriDate(HijriYear, HijriMonthInSet, HijriDay));
                      
                  }
                 
                   else if(EventAfterPressingPushButton(PushButton4,PushButton4_Value,CountPushButton4ForSetTimeAndDate,10) == 8)
                  {                    
                    HijriYear++;                   
                     
                    ShowHijriDateOnLcd(2,0,GetHijriDate(HijriYear, HijriMonthInSet, HijriDay));
                      
                  }
                 
                    else if(EventAfterPressingPushButton(PushButton4,PushButton4_Value,CountPushButton4ForSetTimeAndDate,10) == 9)
                  {
                    
                    if(DayInSet < 6)
                    {
                     DayInSet++;
                    }
                    else
                    {
                      DayInSet = 0;
                    }
                     
                      ShowDayOnLcd(4,0,GetDayName(DayInSet));
                        

                  }
                 
               }
            
               if(EventAfterPressingPushButtonAndResetValue(PushButton5,PushButton5_Value,CountPushButton5ForSetTimeAndDate))
                {
                   if(EventAfterPressingPushButton(PushButton4,PushButton4_Value,CountPushButton4ForSetTimeAndDate,10)==0)
                   {
                      Seconds = 0;
                    
                     
                      ShowTimeOnLcd(4,0,GetTime(Hours,Minutes,Seconds));
                       
                   }
                  else if(EventAfterPressingPushButton(PushButton4,PushButton4_Value,CountPushButton4ForSetTimeAndDate,10)==1)
                   {
                    if(Minutes >0)
                    {
                     
                     Minutes--;
                    }
                    else
                    {
                      Minutes = 59;
                    }
                     
                      ShowTimeOnLcd(4,0,GetTime(Hours,Minutes,Seconds));
                     
                   }
            
                  else if(EventAfterPressingPushButton(PushButton4,PushButton4_Value,CountPushButton4ForSetTimeAndDate,10)==2)
                  {
                    
                    if(Hours >0)
                    {
                     Hours--;
                    }
                    else
                    {
                      Hours = 23;
                    }
                      ShowTimeOnLcd(4,0,GetTime(Hours,Minutes,Seconds));
                      
                  }
                 
                  else if(EventAfterPressingPushButton(PushButton4,PushButton4_Value,CountPushButton4ForSetTimeAndDate,10)==3)
                  {
                    
                    if(GregorianDay > 1)
                    {
                     GregorianDay--;
                    }
                    else
                    {
                      GregorianDay = 31;
                    }
                
                    ShowGregorianDateOnLcd(2,0,GetGregorianDate(GregorianYear, GregorianMonthInSet, GregorianDay));
                          
                  }
                 
                  else if(EventAfterPressingPushButton(PushButton4,PushButton4_Value,CountPushButton4ForSetTimeAndDate,10)==4)
                   {
                    
                      if(GregorianMonthInSet > 0)
                      {
                       GregorianMonthInSet--;
                      }
                      else
                      {
                        GregorianMonthInSet = 11;
                      }
                    ShowGregorianDateOnLcd(2,0,GetGregorianDate(GregorianYear, GregorianMonthInSet, GregorianDay));
                        

                  }
                  else if(EventAfterPressingPushButton(PushButton4,PushButton4_Value,CountPushButton4ForSetTimeAndDate,10)==5)
                  {                   
                    
                     GregorianYear--;                    
                    ShowGregorianDateOnLcd(2,0,GetGregorianDate(GregorianYear, GregorianMonthInSet, GregorianDay));
                       

                  }
                 
                  else if(EventAfterPressingPushButton(PushButton4,PushButton4_Value,CountPushButton4ForSetTimeAndDate,10)==6)
                  {
                    
                    if(HijriDay >1)
                    {
                     HijriDay--;
                    }
                    else
                    {
                      HijriDay = 30;
                    }
                    ShowHijriDateOnLcd(2,0,GetHijriDate(HijriYear, HijriMonthInSet, HijriDay));
                          
                  }
                 
                  else if(EventAfterPressingPushButton(PushButton4,PushButton4_Value,CountPushButton4ForSetTimeAndDate,10)==7)
                  {
                    
                    if(HijriMonthInSet > 0)
                    {
                     HijriMonthInSet--;
                    }
                    else
                    {
                      HijriMonthInSet = 11;
                    }
                    ShowHijriDateOnLcd(2,0,GetHijriDate(HijriYear, HijriMonthInSet, HijriDay));
                       
                     
                  }
                 
                  else if(EventAfterPressingPushButton(PushButton4,PushButton4_Value,CountPushButton4ForSetTimeAndDate,10)==8)
                  {                   
                    
                     HijriYear--;
                    
                    ShowHijriDateOnLcd(2,0,GetHijriDate(HijriYear, HijriMonthInSet, HijriDay));

                      

                  }
                 
                  else if(EventAfterPressingPushButton(PushButton4,PushButton4_Value,CountPushButton4ForSetTimeAndDate,10)==9)
                    {
                    
                      if(DayInSet >0 )
                      {
                       DayInSet--;
                      }
                      else
                      {
                        DayInSet = 6;
                      }
                      ShowDayOnLcd(4,0,GetDayName(DayInSet));
                      

                   
                  }
              
                 
                }
                
            
               
               
               
                                 delay(5);               
               if(EventAfterPressingPushButton(PushButton4,PushButton4_Value,CountPushButton4ForSetTimeAndDate,10)==0)
               {

                 if(i<5 || (i>=10 && i < 15))  
                 {
                      ShowTimeOnLcd(4,0,GetTime(Hours,Minutes,Seconds));
                  
                  
                   delay(5);                  }
                   else
                   {

                     WriteFullPointsOnLcd(10, 0, 2);
                           delay(5);   
                                          }
                
               
      
                }
            
                  else if(EventAfterPressingPushButton(PushButton4,PushButton4_Value,CountPushButton4ForSetTimeAndDate,10)==1)
                {   
                 
                         if(i<5 || (i>=10 && i < 15))  
                 {
                      ShowTimeOnLcd(4,0,GetTime(Hours,Minutes,Seconds));
                  
                  
                   delay(5);               }
                   else
                   {
            WriteFullPointsOnLcd(7, 0, 2);
                  delay(5);         
                           }
                
                }
            
                  else if(EventAfterPressingPushButton(PushButton4,PushButton4_Value,CountPushButton4ForSetTimeAndDate,10)==2)
                {
               
                if(i<5 || (i>=10 && i < 15))  
                 {
                      ShowTimeOnLcd(4,0,GetTime(Hours,Minutes,Seconds));
                  
                  
                   delay(5);               }
                   else
                   {
                                 WriteFullPointsOnLcd(4, 0, 2);

                

                  delay(5);                  }
                
                }
             
             else if(EventAfterPressingPushButton(PushButton4,PushButton4_Value,CountPushButton4ForSetTimeAndDate,10)==3)
                {


             
                if(i<5 || (i>=10 && i < 15))  
                 {
                    ShowGregorianDateOnLcd(2,0,GetGregorianDate(GregorianYear, GregorianMonthInSet, GregorianDay));
                  
                  
                   delay(5);               }
                   else
                   {
                                 WriteFullPointsOnLcd(2, 0, 2);

         
                  delay(5);                  }
                            
             

                }
            
                  else if(EventAfterPressingPushButton(PushButton4,PushButton4_Value,CountPushButton4ForSetTimeAndDate,10)==4)
                {  
                  
                
                if(i<5 || (i>=10 && i < 15))  
                 {
                    ShowGregorianDateOnLcd(2,0,GetGregorianDate(GregorianYear, GregorianMonthInSet, GregorianDay));
                  
                  
                   delay(5);               }
                   else
                   {
                    WriteFullPointsOnLcd(6, 0, 3);


                  delay(5);                  }
                       
                 
                }
            
                  else if(EventAfterPressingPushButton(PushButton4,PushButton4_Value,CountPushButton4ForSetTimeAndDate,10)==5)
                {

                 if(i<5 || (i>=10 && i < 15))  
                 {
                    ShowGregorianDateOnLcd(2,0,GetGregorianDate(GregorianYear, GregorianMonthInSet, GregorianDay));
                  
                  
                   delay(5);               }
                   else
                   {
                                 WriteFullPointsOnLcd(11, 0, 4);


      

                  delay(5);                  }
                       


                }
            
                  else if(EventAfterPressingPushButton(PushButton4,PushButton4_Value,CountPushButton4ForSetTimeAndDate,10)==6)
                {

                  if(i<5 || (i>=10 && i < 15))  
                 {
                    ShowHijriDateOnLcd(2,0,GetHijriDate(HijriYear, HijriMonthInSet, HijriDay));
                  
                  
                   delay(5);                 }
                   else
                   {
                                 WriteFullPointsOnLcd(2, 0, 2);

         

                  delay(5);                  }
                   

             
                }
            
                  else if(EventAfterPressingPushButton(PushButton4,PushButton4_Value,CountPushButton4ForSetTimeAndDate,10)==7)
                {

                   if(i<5 || (i>=10 && i < 15))  
                 {
                    ShowHijriDateOnLcd(2,0,GetHijriDate(HijriYear, HijriMonthInSet, HijriDay));
                  
                  
                   delay(5);                 }
                   else
                   {
                    WriteFullPointsOnLcd(6, 0, 3);


                  delay(5);                  }

            
                }
            
                  else if(EventAfterPressingPushButton(PushButton4,PushButton4_Value,CountPushButton4ForSetTimeAndDate,10)==8)
                {


                   if(i<5 || (i>=10 && i < 15))  
                 {
                    ShowHijriDateOnLcd(2,0,GetHijriDate(HijriYear, HijriMonthInSet, HijriDay));
                  
                  
                   delay(5);                  }
                   else
                   {
              WriteFullPointsOnLcd(11, 0, 4);


                  delay(5);                  }

             
              

                }
            
                  else if(EventAfterPressingPushButton(PushButton4,PushButton4_Value,CountPushButton4ForSetTimeAndDate,10)==9)
                {

                     if(i<5 || (i>=10 && i < 15))  
                 {
                      ShowDayOnLcd(6,0,GetDayName(DayInSet));
                  
                  
                   delay(5);               }
                   else
                   {
            WriteFullPointsOnLcd(6, 0, 3);
               

                  delay(5);                  }
              

                }
            
            
               
            delay(30);
                EndTime = millis();
       Period = EndTime - StartTime;
       Serial.println(Period);
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

            UpdateTheTimeAndDate(Hours, Minutes, Seconds, Hours_StopWatch, Minutes_StopWatch, Seconds_StopWatch, GregorianDay, HijriDay, DayInSet);
 

}
  


void setup()
{
  lcd_1.begin(16, 2); 
  Serial.begin(9600);
  pinMode(Buzzer,OUTPUT);

}

void loop()
{
  ShowScreen();    
}