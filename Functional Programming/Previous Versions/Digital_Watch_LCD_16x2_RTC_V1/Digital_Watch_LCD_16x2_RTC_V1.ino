#include <ThreeWire.h>
#include <RtcDS1302.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>


#define PushButton1 A1
#define PushButton2 A2
#define PushButton3 A3
#define PushButton4 A4
#define PushButton5 A5
#define Light 11
#define Buzzer A0

#define SetSeconds        0
#define SetMinutes        1
#define SetHours          2
#define SetGregorianDay   3
#define SetGregorianMonth 4
#define SetGregorianYear  5
#define SetHijriDay       6
#define SetHijriMonth     7
#define SetHijriYear      8
#define SetDayName        9

#define Address_EEPROM_DayName              0 
#define Address_EEPROM_MinuteAlarm          1
#define Address_EEPROM_HourAlarm            2 
#define Address_EEPROM_HijriDay             3 
#define Address_EEPROM_HijriMonth           4 
#define Address_EEPROM_HijriYear_Right      5
#define Address_EEPROM_HijriYear_Left       6
#define Address_EEPROM_GregorianDay         7 
#define Address_EEPROM_GregorianMonth       8 
#define Address_EEPROM_GregorianYear_Right  9
#define Address_EEPROM_GregorianYear_Left   10


#define Input_PullUp   0  
#define Input_PullDown 1 
bool Input_PullType = Input_PullUp;

#define _24Hours  0 
#define _12Hours  1 
bool HoursType = _12Hours; 

bool IsIncreaseHijriDateAndDayName = 0;
int Read2ByteEEPROM(int RightHalf, int LeftHalf)
{
  int RightHalfValue = EEPROM.read(RightHalf);
  RightHalfValue = RightHalfValue << 8;

  return RightHalfValue + EEPROM.read(LeftHalf);

}

void Update2ByteEEPROM(int RightHalf, int LeftHalf, int Value)
{
    EEPROM.update(RightHalf, byte(Value>>8));
  EEPROM.update(LeftHalf, byte(Value));

}

unsigned long Period = 0;


long long PeriodChangeValue_IncreaseButton4_SetDateTime = 0;
long long EnterTime_IncreaseButton4_SetDateTime = 0;

long long PeriodChangeValue_DecreaseButton5_SetDateTime = 0;
long long EnterTime_DecreaseButton5_SetDateTime = 0;

long long PeriodChangeValue_IncreaseButton4_SetAlarm = 0;
long long EnterTime_IncreaseButton4_SetAlarm = 0;

long long PeriodChangeValue_DecreaseButton5_SetAlarm = 0;
long long EnterTime_DecreaseButton5_SetAlarm = 0;

bool In_IncreaseButton4_SetDateTime = 0;
bool In_DecreaseButton5_SetDateTime = 0;

bool In_IncreaseButton4_SetAlarm = 0;
bool In_DecreaseButton5_SetAlarm = 0;

byte Seconds = 0;   //
byte Minutes = 10;  //
byte Hours = 10;    //

byte Minutes_Alarm = EEPROM.read(Address_EEPROM_MinuteAlarm);  //
byte Hours_Alarm = EEPROM.read(Address_EEPROM_HourAlarm);;    //

byte Minutes_Alarm_Nap = 10;  //
byte Hours_Alarm_Nap = 5;     //

byte Minutes_Alarm_Nap_Period = 5;  //

bool _AllowAlarm = false;     //
bool _AllowAlarmNap = false;  //

byte Seconds_StopWatch = 0;  //
byte Minutes_StopWatch = 0;  //
byte Hours_StopWatch = 0;    //

int Previous_PushButton1_Value = 0;  //
int Previous_PushButton2_Value = 0;  //
int Previous_PushButton3_Value = 0;  //
int Previous_PushButton4_Value = 0;  //
int Previous_PushButton5_Value = 0;  //

int PushButton1_Value = 0;  //
int PushButton2_Value = 0;  //
int PushButton3_Value = 0;  //
int PushButton4_Value = 0;  //
int PushButton5_Value = 0;  //

byte DayInSet = EEPROM.read(Address_EEPROM_DayName);  //

byte HijriDay = EEPROM.read(Address_EEPROM_HijriDay);             //
byte HijriMonthInSet = EEPROM.read(Address_EEPROM_HijriMonth);  ;      //
int HijriYear = Read2ByteEEPROM(Address_EEPROM_HijriYear_Right,Address_EEPROM_HijriYear_Left);        //
byte GregorianDay = 1;         //
byte GregorianMonthInSet  = 0;  //
int GregorianYear = 2010;    //


byte CountForNapTimes = 0;  //

int CountPushButton4ForDate = 0;              //
int CountPushButton4ForStopWatch = 0;         //
int CountPushButton4ForSetAlarm = 0;          //
int CountPushButton4ForTuenOffAlarm = 0;      //
int CountPushButton4ForSetTimeAndDate = 0;    //
int CountPushButton4ForStopWatch_Peeb = 0;    //
int CountPushButton4ForTurnAlarmOnClick = 0;  //

int CountPushButton2ForChangeScreen = 0;  //

int CountPushButton3ForStopWatch = 0;         //
int CountPushButton3ForSetAlarm = 0;          //
int CountPushButton3ForSetTimeAndDate = 0;    //
int CountPushButton3ForTuenOffAlarm = 0;      //
int CountPushButton3ForTurnAlarmOnClick = 0;  //

int CountPushButton5ForSetAlarm = 0;        //
int CountPushButton5ForSetTimeAndDate = 0;  //

int ChangeTypeOfDate = 0;  //


byte CurrentScreen = 0;  //

bool EnterShowDate = 0;
bool EnterShowDay = 0;

int CountAlarmPeep = 0;
int CountFullPointsShow = 0;

#define MaxCountAlarmPeep 40
#define MaxCountFullPointsShow 60


int PreviousSecond = 0;

bool IsLastButton4StopWatch = 0;
bool Button3AsButton4StopWatch = 0;
byte NumberDayInMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

long  StartPeriodForAutoDayOrder = 0; 
long  EndPeriodForAutoDayOrder   = 0; 
bool  ReadStartPeriodForAutoDayOrder = 0;

bool IsChangedHoursType = 0;
bool ReadStartPeriodForChangeHoursType = 0;
long StartPeriodForChangeHoursType = 0;


LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

ThreeWire myWire(9, 10, 8);        // DAT, CLK, RST
RtcDS1302<ThreeWire> Rtc(myWire);  // RTC Object

RtcDateTime now = Rtc.GetDateTime();



void WriteFullPointsOnLcd(byte Culomn, byte Row, byte Number) {
  byte FullPoints1[8] = {
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
  for (byte i = 0; i < Number; i++) {
    lcd.write(1);
  }
}
void WriteOnColumnAndRowOnLcd(int Column, int Row, String Text) {
  lcd.setCursor(Column, Row);
  lcd.print(Text);

}
void WriteOnCleanRowOnLcd(int Column, int Row, String Text) {
  lcd.setCursor(0, Row);
  for (byte i = 0; i < Column; i++) {
    lcd.print(" ");
  }

  lcd.print(Text);
  lcd.print("               ");
}

bool EventWhilePressingPushButton(int PinName, int& Push_Value, bool InputPull_Type =0) {
  Push_Value = digitalRead(PinName);
  if(InputPull_Type == 1)
  {
      if (Push_Value == HIGH) {
      return true;
    }

    else {
      return false;
    }
  }
  else
  {
       if (Push_Value == LOW) {
      return true;
    }

    else {
      return false;
    }
  }
}

void TurnOnBuzzer(int _Buzzer, int Frequency) {
  tone(_Buzzer, Frequency);
}

void TurnOffBuzzer(int _Buzzer) {

  noTone(_Buzzer);
}

bool CheckAlarmTime(bool InputPull_Type = 0) {
  PushButton4_Value = digitalRead(PushButton4);
  PushButton3_Value = digitalRead(PushButton3);
  if(InputPull_Type == 1)
  {
    if ((Hours == Hours_Alarm && Minutes == Minutes_Alarm && _AllowAlarm) || (Hours == Hours_Alarm_Nap && Minutes == Minutes_Alarm_Nap && _AllowAlarmNap) || (PushButton4_Value == HIGH && PushButton3_Value == HIGH && CurrentScreen == 0)) {
      return true;
    } else {
      return false;
    }
  }
  else
  {
      if ((Hours == Hours_Alarm && Minutes == Minutes_Alarm && _AllowAlarm) || (Hours == Hours_Alarm_Nap && Minutes == Minutes_Alarm_Nap && _AllowAlarmNap) || (PushButton4_Value != HIGH && PushButton3_Value != HIGH && CurrentScreen == 0)) {
      return true;
    } else {
      return false;
    }
  }
}

void AllowAlarm() {
  if (Hours == Hours_Alarm && Minutes == Minutes_Alarm && Seconds == 0) {
    _AllowAlarm = true;
  }
}

int EventAfterPressingPushButton(int PinName, int& Push_Value, int& Previous_Push_Value, int& CountPush, int CasesNumber,bool InputPull_Type = Input_PullType) {
  
  Push_Value = digitalRead(PinName);
  
  if(InputPull_Type  == 1)//Input_PullUp
  {

    if (Push_Value == 1 && Previous_Push_Value == 0) {
      CountPush = CountPush + 1;

      if (CountPush == CasesNumber) {
        CountPush = 0;
      }
    }

    Previous_Push_Value = Push_Value;
  }
  else
  {
    if (Push_Value != 1 && Previous_Push_Value != 0) {
      CountPush = CountPush + 1;

      if (CountPush == CasesNumber) {
        CountPush = 0;
      }
    }

    Previous_Push_Value = Push_Value;
  }
  return CountPush;
}

bool EventAfterPressingPushButtonAndResetValue(int PinName, int& Push_Value, int& Previous_Push_Value, int& CountPush,bool InputPull_Type = Input_PullType) {
  bool Value = EventAfterPressingPushButton(PinName, Push_Value, Previous_Push_Value, CountPush, 2,InputPull_Type);

  if (Value) {
    CountPush = 0;
  }

  return Value;
}


bool MoveScreen() {
  int PreviuosScreen = CurrentScreen;
  CurrentScreen = EventAfterPressingPushButton(PushButton2, PushButton2_Value, Previous_PushButton2_Value, CountPushButton2ForChangeScreen, 4);
  return CurrentScreen != PreviuosScreen;
}

bool IsLeapYear(int Year)
{
    return ((Year % 400 == 0) || (Year % 4 == 0 && Year % 100 != 0));
}

String GetTime(int _Hours, int _Minutes, int _Seconds) {
  String Time = "";

  //Set The Hours Part
  if (_Hours < 10) {
    Time += "0";
  }

  Time += String(_Hours);
  Time += ":";

  //Set The Minutes Part
  if (_Minutes < 10) {
    Time += "0";
  }

  Time += String(_Minutes);
  Time += ":";

  //Set The Seconds Part
  if (_Seconds < 10) {
    Time += "0";
  }

  Time += String(_Seconds);
  return Time;
}

String GetGregorianDate(int _Years, int _MonthNumber, int _Day) {
  String GregorianMonthsSet[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
  String Date = "";
  if (_Day < 10) {
    Date += "0";
  }
  Date += String(_Day);
  Date += "  ";

  Date += GregorianMonthsSet[_MonthNumber];

  Date += "  ";

  Date += String(_Years);
  return Date;
}

String GetHijriDate(int _Years, int _MonthNumber, int _Day) {
  String HijriMonthsSet[12] = { "Moh", "Sef", "Ra1", "Ra2", "Jm1", "Jm2", "Rej", "She", "Ram", "Sho", "ked", "Heg" };
  String Date = "";
  if (_Day < 10) {
    Date += "0";
  }
  Date += String(_Day);
  Date += "  ";

  Date += HijriMonthsSet[_MonthNumber];

  Date += "  ";

  Date += String(_Years);
  return Date;
}

String GetDayName(int DayNumber) {

  String DaysSet[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
  String Day = DaysSet[DayNumber];
  return Day;
}


void ShowTimeOnLcd24Hours(int Column, int Row, String Time) {
  WriteOnCleanRowOnLcd(Column, Row, Time);
}

void ShowTimeOnLcd12Hours(int Column, int Row, String Time, bool AM_PM ,bool SameLine = 0) {
  
  if(SameLine == 0)
  {
      WriteOnCleanRowOnLcd(Column, Row, Time);
    if(AM_PM == 0)
    WriteOnCleanRowOnLcd(7, Row+1, "AM");
    else
    WriteOnCleanRowOnLcd(7, Row+1, "PM");
  }
  else
  {
     WriteOnCleanRowOnLcd(Column -2 , Row, Time);
    if(AM_PM == 0)
    WriteOnColumnAndRowOnLcd(12, Row, "AM");
    else
    WriteOnColumnAndRowOnLcd(12, Row, "PM");
  }
}

void ShowGregorianDateOnLcd(int Column, int Row, String Date) {
  WriteOnCleanRowOnLcd(Column, Row, Date);
}

void ShowHijriDateOnLcd(int Column, int Row, String Date) {
  WriteOnCleanRowOnLcd(Column, Row, Date);
}

void ShowDayOnLcd(int Column, int Row, String DayName) {
  WriteOnCleanRowOnLcd(Column, Row, DayName);
}

void ShowAlarmScreenOnLcd(int Column, int Row, String AlarmTime, bool HoursType, bool AM_PM = 0) {
  if(HoursType == _24Hours)
  ShowTimeOnLcd24Hours(Column, Row, AlarmTime);
  else
  {
    ShowTimeOnLcd12Hours(Column, Row, AlarmTime ,AM_PM); 
  }
}

void TurnOnLight() {
  analogWrite(Light, 0);
  //  analogWrite(Light,255);
}

void TurnOffLight() {
  analogWrite(Light, 150);
  //analogWrite(Light,0);
}

void UpdateLight() {

  if (EventWhilePressingPushButton(PushButton1, PushButton1_Value)) {
    TurnOnLight();

  } else {
    TurnOffLight();
  }
}

void ClearRow(int Row) {
  lcd.setCursor(0, Row);
  lcd.print("                ");
}

void ExchangeAndShowDateOnLcd(int Column, int Row, String GregorianDate, String HijriDate, int Change) {
   //int NumberOfClearRow = 1;
 if (EventWhilePressingPushButton(PushButton4, PushButton4_Value)) {
    if (Change == 0)
      ShowHijriDateOnLcd(Column, Row, HijriDate);
    else
      ShowGregorianDateOnLcd(Column, Row, GregorianDate);
  } else {
    ClearRow(1);
  }
}

void  RingTone()
{ 
  if (CountAlarmPeep / 2 < 4) {
        TurnOnBuzzer(Buzzer, 3000);

      } else if (CountAlarmPeep / 2 < 7) {
        TurnOffBuzzer(Buzzer);
      } else if (CountAlarmPeep / 2 < 5 || (CountAlarmPeep / 2 >= 10 && CountAlarmPeep / 2 < 15)) {
        TurnOnBuzzer(Buzzer, 3000);
      } else {
        TurnOffBuzzer(Buzzer);
      }
}
void UpdateStopTime() {
  if (PreviousSecond != Seconds) {
    PreviousSecond = Seconds;
    if (CountPushButton4ForStopWatch == 1) {
      Seconds_StopWatch++;
    }

    if (Seconds_StopWatch == 60) {
      Seconds_StopWatch = 0;
      Minutes_StopWatch++;
    }

    if (Minutes_StopWatch == 60) {
      Minutes_StopWatch = 0;
      Hours_StopWatch++;
    }

    if (Hours_StopWatch == 24) {
      Hours_StopWatch = 0;
    }
  }
}

void UpdateTimeAndDate() {
  Hours = now.Hour();
  Minutes = now.Minute();
  Seconds = now.Second();
  GregorianDay = now.Day();
  GregorianMonthInSet  = now.Month()  ;
  GregorianYear = now.Year();
}

void UpdateGregorianDateAndTimeIn_EEPROM() // to edit the hijri Date and Day Name 
{
  UpdateTimeAndDate();
  EEPROM.update(Address_EEPROM_GregorianDay,GregorianDay);
  EEPROM.update(Address_EEPROM_GregorianMonth,GregorianMonthInSet);
  Update2ByteEEPROM( Address_EEPROM_GregorianYear_Right,  Address_EEPROM_GregorianYear_Left,  GregorianYear);
  
}

void UpdateHijriDateIn_EEPROM() // to edit the hijri Date and Day Name 
{
  EEPROM.update(Address_EEPROM_HijriDay, HijriDay);
  EEPROM.update(Address_EEPROM_HijriMonth,HijriMonthInSet );
  Update2ByteEEPROM(Address_EEPROM_HijriYear_Right, Address_EEPROM_HijriYear_Left, HijriYear);
  
}

int NumberOfDaysInAMonth(int Month, int Year)
{
    return Month = Month == 2 ? (IsLeapYear(Year) ? 29 : 28) : (Month == 11 || Month == 9 || Month == 6 || Month == 4) ? 30 : 31;
}

int NumberOfDaysFromTheBeginningOfTheYear(int GregorianYear, int GregorianMonth, int GregorainDay)
{
    int NumDays = 0;
    for (int i = 1; i < GregorianMonth; i++)
    {
        NumDays += NumberOfDaysInAMonth(i, GregorianYear);
    }
    NumDays +=GregorainDay;
    return NumDays;
}

bool IsDate1BeforeDate2(int FirstGregorianYear, int FirstGregorianMonth, int FirstGregorainDay, int SecondGregorianYear, int SecondGregorianMonth, int SecondGregorainDay )
{
    return (FirstGregorianYear < SecondGregorianYear) ? true : (FirstGregorianMonth< SecondGregorianMonth) ? true : (FirstGregorainDay < SecondGregorainDay ? true : false);
}

void IncreaseDateByOneDay(int &GregorianYear, int &GregorianMonth, int &GregorainDay)
{
    GregorainDay++;
    if (GregorainDay > NumberOfDaysInAMonth(GregorianMonth, GregorianYear))
    {
        GregorainDay = 1;
        GregorianMonth++;
    }
    if (GregorianMonth  > 12)
    {

        GregorianMonth = 1;
        GregorianYear++;
    }

   // return Date;
}

long GetDifferenceInDays(int FirstGregorianYear, int FirstGregorianMonth, int FirstGregorainDay, int SecondGregorianYear, int SecondGregorianMonth, int SecondGregorainDay,bool IncludeEndDay = false)
{  
    int NumDays = 0;
    while (IsDate1BeforeDate2(FirstGregorianYear,  FirstGregorianMonth,  FirstGregorainDay, SecondGregorianYear,  SecondGregorianMonth,  SecondGregorainDay))
    {
        IncreaseDateByOneDay(FirstGregorianYear,FirstGregorianMonth,FirstGregorainDay);
        NumDays++;
        
    } 
    return IncludeEndDay ? ++NumDays : NumDays;
}

void IncreaseHijriDateByOneDay()
{ 
  HijriDay++;
  if(HijriDay >30)
  {
     HijriDay = 1;
      HijriMonthInSet++;
  }
  if(HijriMonthInSet>11)
  {
    HijriMonthInSet = 0;
    HijriYear++;
  }

}

void IncreaseDayNameByOneDay()
{
  DayInSet++;
  if(DayInSet>6)
  {
    DayInSet = 0;
  }

}

void UpdateHijriDateAndDayName()
{

  if(now.Hour() == 0 && now.Minute() == 0 && now.Second() == 0 && IsIncreaseHijriDateAndDayName == 0 && now.Day() !=0) // Day if the RTC not connected 
  {
    IncreaseHijriDateByOneDay();
    IncreaseDayNameByOneDay();
    IsIncreaseHijriDateAndDayName = 1 ;
    UpdateHijriDateIn_EEPROM();
    EEPROM.update(Address_EEPROM_DayName, DayInSet);

  }
  if(now.Hour() !=0 && now.Minute() !=0 && now.Second() != 0 )
  {
    IsIncreaseHijriDateAndDayName = 0;
  }
  
}

void UpdateHijriDateAndDayNameFrom_EEPROM()
{  
  int FirstGregorianYear     = Read2ByteEEPROM(Address_EEPROM_GregorianYear_Right,Address_EEPROM_GregorianYear_Left);
  byte FirstGregorianMonth   = EEPROM.read(Address_EEPROM_GregorianMonth);
  byte FirstGregorainDay     = EEPROM.read(Address_EEPROM_GregorianDay);
  int SecondGregorianYear    = now.Year();
  byte SecondGregorianMonth  = now.Month();
  byte SecondGregorainDay    = now.Day(); 

  long DayNumber= GetDifferenceInDays(FirstGregorianYear,  FirstGregorianMonth,  FirstGregorainDay,  SecondGregorianYear,  SecondGregorianMonth,  SecondGregorainDay);

  while(DayNumber > 0)
  {  
    

    DayNumber--;
    IncreaseDayNameByOneDay();
    IncreaseHijriDateByOneDay();

  }
UpdateHijriDateIn_EEPROM();

  EEPROM.update(Address_EEPROM_DayName, DayInSet);

}

byte  DayOrder(int GregorianYear, int GregorianMonth, int GregorianDay) 
{
    long a = (14 - GregorianMonth) / 12;
    GregorianYear = GregorianYear - a;
    GregorianMonth = GregorianMonth + 12 * a - 2;
    return(GregorianDay + GregorianYear + (GregorianYear / 4) - (GregorianYear /100) + (GregorianYear / 400) + (31 * GregorianMonth / 12)) % 7;
}

void AutoDayOrder()
{
 DayInSet = DayOrder(now.Year(), now.Month(), now.Day());
 EEPROM.update(Address_EEPROM_DayName, DayInSet);
}

void ShowTimeAndDateScreen() {
  UpdateHijriDateIn_EEPROM();   //To Store the Hijiri Date and Gregorian Date In EEPROM and Save for longest time  
  UpdateGregorianDateAndTimeIn_EEPROM();
      if(!(digitalRead(PushButton4) == LOW && digitalRead(PushButton5) == LOW) && millis() )
       {
         IsChangedHoursType = 0 ;
         ReadStartPeriodForChangeHoursType= 0;
       }
       if(digitalRead(PushButton4) == LOW && digitalRead(PushButton5) == LOW && IsChangedHoursType == 0)
        {
          if(ReadStartPeriodForChangeHoursType == 0)
          {
          StartPeriodForChangeHoursType  = millis();
          ReadStartPeriodForChangeHoursType  = 1;
          }
          if(millis() - StartPeriodForChangeHoursType  > 3000)
          {
            HoursType ^=1;
            IsChangedHoursType =1;
          }
        }

  bool IsBlankRow1 = 0;

  if (CheckAlarmTime()) {

    if (Seconds == 59) {
      CountForNapTimes = 0;
      _AllowAlarmNap = false;
    }

    if (EventAfterPressingPushButtonAndResetValue(PushButton4, PushButton4_Value, Previous_PushButton4_Value, CountPushButton4ForTuenOffAlarm)) {

      _AllowAlarm = false;
      _AllowAlarmNap = false;
      CountForNapTimes = 0;
      // CountPushButton2ForTuenOffAlarm =0;    //must write with (EventAfterPressingPushButtonAndResetValue)
    }

    else if (EventAfterPressingPushButtonAndResetValue(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForTuenOffAlarm)) {
      CountPushButton4ForTuenOffAlarm = 0;  //must write with (EventAfterPressingPushButtonAndResetValue)
      _AllowAlarm = false;
      _AllowAlarmNap = true;
      CountForNapTimes++;
      Minutes_Alarm_Nap = Minutes_Alarm + Minutes_Alarm_Nap_Period * CountForNapTimes;
      Hours_Alarm_Nap = Hours_Alarm;
      if (Minutes_Alarm_Nap >= 60) {
        Minutes_Alarm_Nap = Minutes_Alarm_Nap - 60;
        Hours_Alarm_Nap++;
      }
      if (Hours_Alarm_Nap >= 24) {
        Hours_Alarm_Nap = 0;
      }


      //  Hours_Alarm_Nap    =  5;
      if (CountForNapTimes == 13) {
        CountForNapTimes = 0;
        _AllowAlarmNap = false;
      }
    }

    else  //not all
    {
    RingTone();
    }
  } else {
    TurnOffBuzzer(Buzzer);
  }

  CountPushButton3ForSetAlarm = 0;
  CountPushButton3ForSetTimeAndDate = 0;
  AllowAlarm();
  UpdateLight();

  if (MoveScreen()) {
    return;
  }

  if(Input_PullType == Input_PullDown)
  {
      if ((PushButton2_Value == LOW && PushButton3_Value == LOW))
        { 
           if(HoursType == _24Hours)
          ShowTimeOnLcd24Hours(4, 0, GetTime(now.Hour(), now.Minute(), now.Second()));
          else
          {
            byte _Hours = now.Hour();
            bool _AM_PM = 0;
            if(_Hours >=12)
            {
              _Hours = _Hours -12;
              _AM_PM = 1;
            }
            if(_Hours ==0)
            {
              _Hours = 12;
            }
           if(digitalRead(PushButton4) == LOW || digitalRead(PushButton5) == LOW)
            ShowTimeOnLcd12Hours(4, 0, GetTime(_Hours, now.Minute(), now.Second()), _AM_PM,1);
          else
             ShowTimeOnLcd12Hours(4, 0, GetTime(_Hours, now.Minute(), now.Second()), _AM_PM);

          }
         }
  }
  else
    {
        if (PushButton2_Value == HIGH && PushButton3_Value == HIGH)
         { 
         if(HoursType == _24Hours)
          ShowTimeOnLcd24Hours(4, 0, GetTime(now.Hour(), now.Minute(), now.Second()));
          else
          {
            byte _Hours = now.Hour();
            bool _AM_PM = 0;
            if(_Hours >=12)
            {
              _Hours = _Hours -12;
              _AM_PM = 1;
            }
                if(_Hours ==0)
            {
              _Hours = 12;
            }
          
           if(digitalRead(PushButton4) == LOW || digitalRead(PushButton5) == LOW)
            ShowTimeOnLcd12Hours(4, 0, GetTime(_Hours, now.Minute(), now.Second()), _AM_PM,1);
          else
             ShowTimeOnLcd12Hours(4, 0, GetTime(_Hours, now.Minute(), now.Second()), _AM_PM);

          }
         }
    }
  
  if (EventAfterPressingPushButtonAndResetValue(PushButton4, PushButton4_Value, Previous_PushButton4_Value, CountPushButton4ForDate)) {
    ChangeTypeOfDate = ChangeTypeOfDate ^ 1;
    // CountPushButton2ForDate = 0;
  }


  if (EventWhilePressingPushButton(PushButton4, PushButton4_Value) && !EnterShowDay && digitalRead(PushButton3) == 1) {
    

    ExchangeAndShowDateOnLcd(2, 1, GetGregorianDate(GregorianYear, GregorianMonthInSet -1, GregorianDay), GetHijriDate(HijriYear, HijriMonthInSet, HijriDay), ChangeTypeOfDate);
    IsBlankRow1 = 1;
    EnterShowDate = 1;
  } else {
    EnterShowDate = 0;
  }

  if (EventWhilePressingPushButton(PushButton3, PushButton3_Value)) 
  {
         if(HoursType == _24Hours)
    ShowTimeOnLcd24Hours(4, 0, GetTime(Hours_Alarm, Minutes_Alarm,0));
    else
    {      byte _Hours_Alarm = Hours_Alarm;
            bool _AM_PM = 0;
            if(_Hours_Alarm  >=12)
            {
              _Hours_Alarm = _Hours_Alarm -12;
              _AM_PM = 1;
            }
            if(_Hours_Alarm == 0)
            {
              _Hours_Alarm =12;
            }
      ShowTimeOnLcd12Hours(4, 0, GetTime(_Hours_Alarm, Minutes_Alarm,0) ,_AM_PM); 
    }
  }

  if (EventWhilePressingPushButton(PushButton5, PushButton5_Value) && !EnterShowDate) {
    ShowDayOnLcd(6, 1, GetDayName(DayInSet));
    IsBlankRow1 = 1;
    EnterShowDay = 1;

  } else {
    EnterShowDay = 0;
  }
  if (!IsBlankRow1 && HoursType == _24Hours)
    ClearRow(1);
}

void ShowStopWatchScreen() {
  if (CheckAlarmTime()) {

    if (Seconds == 59) {
      CountForNapTimes = 0;
      _AllowAlarmNap = false;
    }

    if (EventAfterPressingPushButtonAndResetValue(PushButton4, PushButton4_Value, Previous_PushButton4_Value, CountPushButton4ForTuenOffAlarm)) {

      _AllowAlarm = false;
      _AllowAlarmNap = false;
      CountForNapTimes = 0;
      // CountPushButton2ForTuenOffAlarm =0;    //must write with (EventAfterPressingPushButtonAndResetValue)
    }

    else if (EventAfterPressingPushButtonAndResetValue(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForTuenOffAlarm)) {
      CountPushButton4ForTuenOffAlarm = 0;  //must write with (EventAfterPressingPushButtonAndResetValue)
      _AllowAlarm = false;
      _AllowAlarmNap = true;
      CountForNapTimes++;
      Minutes_Alarm_Nap = Minutes_Alarm + Minutes_Alarm_Nap_Period * CountForNapTimes;
      Hours_Alarm_Nap = Hours_Alarm;
      if (Minutes_Alarm_Nap >= 60) {
        Minutes_Alarm_Nap = Minutes_Alarm_Nap - 60;
        Hours_Alarm_Nap++;
      }
      if (Hours_Alarm_Nap >= 24) {
        Hours_Alarm_Nap = 0;
      }


      //  Hours_Alarm_Nap    =  5;
      if (CountForNapTimes == 13) {
        CountForNapTimes = 0;
        _AllowAlarmNap = false;
      }
    }

    else  //not all
    {
    RingTone();
    }
  } else {
    TurnOffBuzzer(Buzzer);
  }
  UpdateLight();
  AllowAlarm();
  if (MoveScreen()) {
    return;
  }

  if (EventAfterPressingPushButtonAndResetValue(PushButton4, PushButton4_Value, Previous_PushButton4_Value, CountPushButton4ForStopWatch_Peeb)  ) {
    TurnOnBuzzer(Buzzer, 3000);
    TurnOffBuzzer(Buzzer);
    //  CountPushButton2ForStopWatch_Peeb =0;
    CountPushButton4ForStopWatch ^= 1;
    IsLastButton4StopWatch = 1;
    Button3AsButton4StopWatch = 0;
  }

  if(EventAfterPressingPushButtonAndResetValue(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForStopWatch))
  {
    if(( CountPushButton4ForStopWatch ==1  || Button3AsButton4StopWatch == 1))
    {TurnOnBuzzer(Buzzer, 3000);
    TurnOffBuzzer(Buzzer);
    //  CountPushButton2ForStopWatch_Peeb =0;
    CountPushButton4ForStopWatch ^= 1;
    IsLastButton4StopWatch = 0;
    Button3AsButton4StopWatch = 1;
    }
    else if(IsLastButton4StopWatch == 1 && CountPushButton4ForStopWatch == 0 && Button3AsButton4StopWatch == 0) 
    {
     TurnOnBuzzer(Buzzer, 3000);
    TurnOffBuzzer(Buzzer);
    CountPushButton4ForStopWatch = 0;
    Hours_StopWatch = 0;
    Minutes_StopWatch = 0;
    Seconds_StopWatch = 0;
    // Previous_PushButton2_Value =0;
    }
  }
  ShowTimeOnLcd24Hours(4, 0, GetTime(Hours_StopWatch, Minutes_StopWatch, Seconds_StopWatch));
      ClearRow(1);

}

void ShowSetAlarmScreen() {
  if (CheckAlarmTime()) {

    if (Seconds == 59) {
      CountForNapTimes = 0;
      _AllowAlarmNap = false;
    }

    if (EventAfterPressingPushButtonAndResetValue(PushButton4, PushButton4_Value, Previous_PushButton4_Value, CountPushButton4ForTuenOffAlarm)) {

      _AllowAlarm = false;
      _AllowAlarmNap = false;
      CountForNapTimes = 0;
      // CountPushButton2ForTuenOffAlarm =0;    //must write with (EventAfterPressingPushButtonAndResetValue)
    }

    else if (EventAfterPressingPushButtonAndResetValue(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForTuenOffAlarm)) {
      CountPushButton4ForTuenOffAlarm = 0;  //must write with (EventAfterPressingPushButtonAndResetValue)
      _AllowAlarm = false;
      _AllowAlarmNap = true;
      CountForNapTimes++;
      Minutes_Alarm_Nap = Minutes_Alarm + Minutes_Alarm_Nap_Period * CountForNapTimes;
      Hours_Alarm_Nap = Hours_Alarm;
      if (Minutes_Alarm_Nap >= 60) {
        Minutes_Alarm_Nap = Minutes_Alarm_Nap - 60;
        Hours_Alarm_Nap++;
      }
      if (Hours_Alarm_Nap >= 24) {
        Hours_Alarm_Nap = 0;
      }


      //  Hours_Alarm_Nap    =  5;
      if (CountForNapTimes == 13) {
        CountForNapTimes = 0;
        _AllowAlarmNap = false;
      }
    }

    else  //not all
    {
    RingTone();
    }
  } else {
    TurnOffBuzzer(Buzzer);
  }

  AllowAlarm();

  if (MoveScreen()) {
    return;
  }
  UpdateLight();
   PushButton4_Value = digitalRead(PushButton4);
  if(Input_PullType == Input_PullUp)
  {
    PushButton4_Value = !PushButton4_Value;
  }

  if (PushButton4_Value == 1 && !In_IncreaseButton4_SetAlarm) {
    
    PeriodChangeValue_IncreaseButton4_SetAlarm = millis();
    In_IncreaseButton4_SetAlarm = 1;
  }

   PushButton4_Value = digitalRead(PushButton4);
  if(Input_PullType == Input_PullUp)
  {
    PushButton4_Value = !PushButton4_Value;
  }
  if (PushButton4_Value == 0) {
    PeriodChangeValue_IncreaseButton4_SetAlarm = millis();
    In_IncreaseButton4_SetAlarm = 0;
  }



  if ((EventAfterPressingPushButtonAndResetValue(PushButton4, PushButton4_Value, Previous_PushButton4_Value, CountPushButton4ForSetAlarm)) || (((1000 < millis() - PeriodChangeValue_IncreaseButton4_SetAlarm) && PeriodChangeValue_IncreaseButton4_SetAlarm != millis()) && (millis() - EnterTime_IncreaseButton4_SetAlarm > 100))) {
    EnterTime_IncreaseButton4_SetAlarm = millis();
    if (EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetAlarm, 2) == 0) {
      if (Minutes_Alarm < 59) {

        Minutes_Alarm++;
      } else {
        Minutes_Alarm = 0;
      }

      // CountPushButton2ForSetAlarm = 0;
    }

    else if (EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetAlarm, 2) == 1) {

      if (Hours_Alarm < 23) {
        Hours_Alarm++;
      } else {
        Hours_Alarm = 0;
      }
      CountPushButton4ForSetAlarm = 0;
    }
  }

   PushButton5_Value = digitalRead(PushButton5);
  if(Input_PullType == Input_PullUp)
  {
    PushButton5_Value = !PushButton5_Value;
  }

  if (PushButton5_Value== 1 && !In_DecreaseButton5_SetAlarm) {
    PeriodChangeValue_DecreaseButton5_SetAlarm = millis();
    In_DecreaseButton5_SetAlarm = 1;
  }

   PushButton5_Value = digitalRead(PushButton5);
  if(Input_PullType == Input_PullUp)
  {
    PushButton5_Value = !PushButton5_Value;
  }

  if (PushButton5_Value == 0) {
    PeriodChangeValue_DecreaseButton5_SetAlarm = millis();
    In_DecreaseButton5_SetAlarm = 0;
  }



  if ((EventAfterPressingPushButtonAndResetValue(PushButton5, PushButton5_Value, Previous_PushButton5_Value, CountPushButton5ForSetAlarm)) || (((1000 < millis() - PeriodChangeValue_DecreaseButton5_SetAlarm) && PeriodChangeValue_DecreaseButton5_SetAlarm != millis()) && (millis() - EnterTime_DecreaseButton5_SetAlarm > 100))) {
    EnterTime_DecreaseButton5_SetAlarm = millis();
    if (EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetAlarm, 2) == 0) {
      if (Minutes_Alarm > 0) {

        Minutes_Alarm--;
      } else {
        Minutes_Alarm = 59;
      }
      //  CountPushButton5ForSetAlarm = 0;
    }

    else if (EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetAlarm, 2) == 1) {

      if (Hours_Alarm > 0) {
        Hours_Alarm--;
      } else {
        Hours_Alarm = 23;
      }
      // CountPushButton5ForSetAlarm = 0;
    }
  }
  ///////
  if (EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetAlarm, 2) == 0) {
    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
         if(HoursType == _24Hours)
    ShowTimeOnLcd24Hours(4, 0, GetTime(Hours_Alarm, Minutes_Alarm,0));
    else
    {      byte _Hours_Alarm = Hours_Alarm;
            bool _AM_PM = 0;
            if(_Hours_Alarm  >=12)
            {
              _Hours_Alarm = _Hours_Alarm -12;
              _AM_PM = 1;
            }
            if(_Hours_Alarm == 0)
            {
              _Hours_Alarm =12;
            }
      ShowTimeOnLcd12Hours(4, 0, GetTime(_Hours_Alarm, Minutes_Alarm,0) ,_AM_PM); 
    }
      ////delay(5);
    } else {
      WriteFullPointsOnLcd(7, 0, 2);
      // //delay(5);
    }

  }

  else if (EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetAlarm, 2) == 1) {

    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
      if(HoursType == _24Hours)
    ShowTimeOnLcd24Hours(4, 0, GetTime(Hours_Alarm, Minutes_Alarm,0));
    else
    {      byte _Hours_Alarm = Hours_Alarm;
            bool _AM_PM = 0;
            if(_Hours_Alarm  >=12)
            {
              _Hours_Alarm = _Hours_Alarm -12;
              _AM_PM = 1;
            }
            if(_Hours_Alarm == 0)
            {
              _Hours_Alarm =12;
            }
      ShowTimeOnLcd12Hours(4, 0, GetTime(_Hours_Alarm, Minutes_Alarm,0) ,_AM_PM); 
    }
      //  //delay(5);
    } else {
      WriteFullPointsOnLcd(4, 0, 2);
      // //delay(5);
    }
  }
}

void ShowSetDateAndTimeScreen() {
  //To save Alarm Time to EEPROM and save (EEPROM) for longest time 
      EEPROM.update(Address_EEPROM_MinuteAlarm, Minutes_Alarm);
    EEPROM.update(Address_EEPROM_HourAlarm, Hours_Alarm);
  if (CheckAlarmTime()) {

    if (Seconds == 59) {
      CountForNapTimes = 0;
      _AllowAlarmNap = false;
    }

    if (EventAfterPressingPushButtonAndResetValue(PushButton4, PushButton4_Value, Previous_PushButton4_Value, CountPushButton4ForTuenOffAlarm)) {

      _AllowAlarm = false;
      _AllowAlarmNap = false;
      CountForNapTimes = 0;
      // CountPushButton2ForTuenOffAlarm =0;    //must write with (EventAfterPressingPushButtonAndResetValue)
    }

    else if (EventAfterPressingPushButtonAndResetValue(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForTuenOffAlarm)) {
      CountPushButton4ForTuenOffAlarm = 0;  //must write with (EventAfterPressingPushButtonAndResetValue)
      _AllowAlarm = false;
      _AllowAlarmNap = true;
      CountForNapTimes++;
      Minutes_Alarm_Nap = Minutes_Alarm + Minutes_Alarm_Nap_Period * CountForNapTimes;
      Hours_Alarm_Nap = Hours_Alarm;
      if (Minutes_Alarm_Nap >= 60) {
        Minutes_Alarm_Nap = Minutes_Alarm_Nap - 60;
        Hours_Alarm_Nap++;
      }
      if (Hours_Alarm_Nap >= 24) {
        Hours_Alarm_Nap = 0;
      }


      //  Hours_Alarm_Nap    =  5;
      if (CountForNapTimes == 13) {
        CountForNapTimes = 0;
        _AllowAlarmNap = false;
      }
    }

    else  //not all
    {
    RingTone();
    }
  } else {
    TurnOffBuzzer(Buzzer);
  }

  AllowAlarm();

  if (MoveScreen()) {
    return;
  }
  
  UpdateLight();

  if(!(digitalRead(PushButton4) == LOW && digitalRead(PushButton5) == LOW) )
        {
                  StartPeriodForAutoDayOrder = millis();
                  ReadStartPeriodForAutoDayOrder = 0;
        }

    PushButton4_Value = digitalRead(PushButton4);
    if(Input_PullType == Input_PullUp)
    {
      PushButton4_Value = !PushButton4_Value;
    }

    if (PushButton4_Value == 0) {
      PeriodChangeValue_IncreaseButton4_SetDateTime = millis();
      In_IncreaseButton4_SetDateTime = 0;
    }



    if ((EventAfterPressingPushButtonAndResetValue(PushButton4, PushButton4_Value, Previous_PushButton4_Value, CountPushButton4ForSetTimeAndDate)) || (((1000 < millis() - PeriodChangeValue_IncreaseButton4_SetDateTime) && PeriodChangeValue_IncreaseButton4_SetDateTime != millis()) && (millis() - EnterTime_IncreaseButton4_SetDateTime > 100))) {
      EnterTime_IncreaseButton4_SetDateTime = millis();
      if (EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetTimeAndDate, 10) == SetSeconds) {
        Seconds = 0;
        Rtc.SetDateTime(RtcDateTime(now.Year(), now.Month(), now.Day(), now.Hour(), now.Minute(), Seconds));

        //CountPushButton4ForSetTimeAndDate = 0;
    if(HoursType == _24Hours)
            ShowTimeOnLcd24Hours(4, 0, GetTime(now.Hour(), now.Minute(), now.Second()));
            else
            {
              byte _Hours = now.Hour();
              bool _AM_PM = 0;
              if(_Hours >=12)
              {
                _Hours = _Hours -12;
                _AM_PM = 1;
              }
                  if(_Hours ==0)
              {
                _Hours = 12;
              }
            
              ShowTimeOnLcd12Hours(4, 0, GetTime(_Hours, now.Minute(), now.Second()), _AM_PM);

            }
      } else if (EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetTimeAndDate, 10) == SetMinutes) {
        if (Minutes < 59) {

          Minutes++;
        } else {
          Minutes = 0;
        }
        //   //CountPushButton4ForSetTimeAndDate = 0;
        Rtc.SetDateTime(RtcDateTime(now.Year(), now.Month(), now.Day(), now.Hour(), Minutes, now.Second()));

  if(HoursType == _24Hours)
            ShowTimeOnLcd24Hours(4, 0, GetTime(now.Hour(), now.Minute(), now.Second()));
            else
            {
              byte _Hours = now.Hour();
              bool _AM_PM = 0;
              if(_Hours >=12)
              {
                _Hours = _Hours -12;
                _AM_PM = 1;
              }
                  if(_Hours ==0)
              {
                _Hours = 12;
              }
            
              ShowTimeOnLcd12Hours(4, 0, GetTime(_Hours, now.Minute(), now.Second()), _AM_PM);

            }

      }

      else if (EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetTimeAndDate, 10) == SetHours) {

        if (Hours < 23) {
          Hours++;
        } else {
          Hours = 0;
        }
        //              //CountPushButton4ForSetTimeAndDate = 0;
        Rtc.SetDateTime(RtcDateTime(now.Year(), now.Month(), now.Day(), Hours, now.Minute(), now.Second()));
  if(HoursType == _24Hours)
            ShowTimeOnLcd24Hours(4, 0, GetTime(now.Hour(), now.Minute(), now.Second()));
            else
            {
              byte _Hours = now.Hour();
              bool _AM_PM = 0;
              if(_Hours >=12)
              {
                _Hours = _Hours -12;
                _AM_PM = 1;
              }
                  if(_Hours ==0)
              {
                _Hours = 12;
              }
            
              ShowTimeOnLcd12Hours(4, 0, GetTime(_Hours, now.Minute(), now.Second()), _AM_PM);

            }
      }

      else if (EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetTimeAndDate, 10) == SetGregorianDay) {
        if (IsLeapYear(GregorianYear) && GregorianMonthInSet == 2 && GregorianDay < 29) {
          GregorianDay++;
        } else if (GregorianDay < NumberDayInMonth[GregorianMonthInSet -1]) {
          GregorianDay++;
        } else {
          GregorianDay = 1;
        }
        //CountPushButton4ForSetTimeAndDate = 0;
        Rtc.SetDateTime(RtcDateTime(now.Year(), now.Month(), GregorianDay, now.Hour(), now.Minute(), now.Second()));


      ShowGregorianDateOnLcd(2, 0, GetGregorianDate(GregorianYear, GregorianMonthInSet -1, GregorianDay));

      }

      else if (EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetTimeAndDate, 10) == SetGregorianMonth) {

        if (GregorianMonthInSet  < 12) {
          GregorianMonthInSet++;
        } else {
          GregorianMonthInSet  = 1;
        }
        if (GregorianDay > NumberDayInMonth[GregorianMonthInSet -1]) {
          GregorianDay = NumberDayInMonth[GregorianMonthInSet -1];
          if (IsLeapYear(GregorianYear) && GregorianMonthInSet == 2)
            GregorianDay = 29;
        }
        //CountPushButton4ForSetTimeAndDate = 0;
        Rtc.SetDateTime(RtcDateTime(now.Year(), GregorianMonthInSet  , GregorianDay, now.Hour(), now.Minute(), now.Second()));

      ShowGregorianDateOnLcd(2, 0, GetGregorianDate(GregorianYear, GregorianMonthInSet -1, GregorianDay));

      } else if (EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetTimeAndDate, 10) == SetGregorianYear) {


        GregorianYear++;
        if (GregorianDay > NumberDayInMonth[GregorianMonthInSet -1]) {
          GregorianDay = NumberDayInMonth[GregorianMonthInSet -1];
          if (IsLeapYear(GregorianYear) && GregorianMonthInSet == 2)
            GregorianDay = 29;
        }
        //CountPushButton4ForSetTimeAndDate = 0;
        Rtc.SetDateTime(RtcDateTime(GregorianYear, now.Month(), GregorianDay, now.Hour(), now.Minute(), now.Second()));
      ShowGregorianDateOnLcd(2, 0, GetGregorianDate(GregorianYear, GregorianMonthInSet -1, GregorianDay));
      }

      else if (EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetTimeAndDate, 10) == SetHijriDay) {

        if (HijriDay < 30) {
          HijriDay++;
        } else {
          HijriDay = 1;
        }
          //CountPushButton4ForSetTimeAndDate = 0;

        ShowHijriDateOnLcd(2, 0, GetHijriDate(HijriYear, HijriMonthInSet, HijriDay));

      }

      else if (EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetTimeAndDate, 10) == SetHijriMonth) {

        if (HijriMonthInSet < 11) {
          HijriMonthInSet++;
        } else {
          HijriMonthInSet = 0;
        }
        //CountPushButton4ForSetTimeAndDate = 0;

        ShowHijriDateOnLcd(2, 0, GetHijriDate(HijriYear, HijriMonthInSet, HijriDay));

      }

      else if (EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetTimeAndDate, 10) == SetHijriYear) {
        HijriYear++;
        //CountPushButton4ForSetTimeAndDate = 0;

        ShowHijriDateOnLcd(2, 0, GetHijriDate(HijriYear, HijriMonthInSet, HijriDay));

      }

      else if (EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetTimeAndDate, 10) == SetDayName) {
    if(digitalRead(PushButton4) == LOW && digitalRead(PushButton5) == LOW )
        {
          if(ReadStartPeriodForAutoDayOrder == 0)
          {
          StartPeriodForAutoDayOrder = millis();
          ReadStartPeriodForAutoDayOrder = 1;
          }
          if(millis() - StartPeriodForAutoDayOrder > 5000)
          {
            AutoDayOrder();
            EndPeriodForAutoDayOrder = millis();
          }
        }
        else if(millis() - EndPeriodForAutoDayOrder >5000  )
        {
            if (DayInSet < 6) {
              DayInSet++;

            } else {
              DayInSet = 0;
            }
        }
        EEPROM.update(Address_EEPROM_DayName, DayInSet);
        //CountPushButton4ForSetTimeAndDate = 0;

        ShowDayOnLcd(4, 0, GetDayName(DayInSet));
      }
    }

    PushButton5_Value = digitalRead(PushButton5);
    if(Input_PullType == Input_PullUp)
    {
      PushButton5_Value = !PushButton5_Value;
    }

    if (PushButton5_Value == 1 && !In_DecreaseButton5_SetDateTime) {
      PeriodChangeValue_DecreaseButton5_SetDateTime = millis();
      In_DecreaseButton5_SetDateTime = 1;
    }

  PushButton5_Value = digitalRead(PushButton5);
    if(Input_PullType == Input_PullUp)
    {
      PushButton5_Value = !PushButton5_Value;
    }
    if (PushButton5_Value == 0) {
      PeriodChangeValue_DecreaseButton5_SetDateTime = millis();
      In_DecreaseButton5_SetDateTime = 0;
    }



    if ((EventAfterPressingPushButtonAndResetValue(PushButton5, PushButton5_Value, Previous_PushButton5_Value, CountPushButton5ForSetTimeAndDate)) || (((1000 < millis() - PeriodChangeValue_DecreaseButton5_SetDateTime) && PeriodChangeValue_DecreaseButton5_SetDateTime != millis()) && (millis() - EnterTime_DecreaseButton5_SetDateTime > 100))) {
      EnterTime_DecreaseButton5_SetDateTime = millis();
      if (EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetTimeAndDate, 10) == SetSeconds) {
        Seconds = 0;

        //CountPushButton5ForSetTimeAndDate = 0;
        Rtc.SetDateTime(RtcDateTime(now.Year(), now.Month(), now.Day(), now.Hour(), now.Minute(), Seconds));

  if(HoursType == _24Hours)
            ShowTimeOnLcd24Hours(4, 0, GetTime(now.Hour(), now.Minute(), now.Second()));
            else
            {
              byte _Hours = now.Hour();
              bool _AM_PM = 0;
              if(_Hours >=12)
              {
                _Hours = _Hours -12;
                _AM_PM = 1;
              }
                  if(_Hours ==0)
              {
                _Hours = 12;
              }
            
              ShowTimeOnLcd12Hours(4, 0, GetTime(_Hours, now.Minute(), now.Second()), _AM_PM);

            }
      } else if (EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetTimeAndDate, 10) == SetMinutes) {
        if (Minutes > 0) {

          Minutes--;
        } else {
          Minutes = 59;
        }
        //CountPushButton5ForSetTimeAndDate = 0;
        Rtc.SetDateTime(RtcDateTime(now.Year(), now.Month(), now.Day(), now.Hour(), Minutes, now.Second()));

  if(HoursType == _24Hours)
            ShowTimeOnLcd24Hours(4, 0, GetTime(now.Hour(), now.Minute(), now.Second()));
            else
            {
              byte _Hours = now.Hour();
              bool _AM_PM = 0;
              if(_Hours >=12)
              {
                _Hours = _Hours -12;
                _AM_PM = 1;
              }
                  if(_Hours ==0)
              {
                _Hours = 12;
              }
            
              ShowTimeOnLcd12Hours(4, 0, GetTime(_Hours, now.Minute(), now.Second()), _AM_PM);

            }
      }

      else if (EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetTimeAndDate, 10) == SetHours) {

        if (Hours > 0) {
          Hours--;
        } else {
          Hours = 23;
        }
        //CountPushButton5ForSetTimeAndDate = 0;
        Rtc.SetDateTime(RtcDateTime(now.Year(), now.Month(), now.Day(), Hours, now.Minute(), now.Second()));

  if(HoursType == _24Hours)
            ShowTimeOnLcd24Hours(4, 0, GetTime(now.Hour(), now.Minute(), now.Second()));
            else
            {
              byte _Hours = now.Hour();
              bool _AM_PM = 0;
              if(_Hours >=12)
              {
                _Hours = _Hours -12;
                _AM_PM = 1;
              }
                  if(_Hours ==0)
              {
                _Hours = 12;
              }
            
              ShowTimeOnLcd12Hours(4, 0, GetTime(_Hours, now.Minute(), now.Second()), _AM_PM);

            }
      }

      else if (EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetTimeAndDate, 10) == SetGregorianDay) {

        if (GregorianDay > 1) {
          GregorianDay--;
        } else if (IsLeapYear(GregorianYear) && GregorianMonthInSet == 2 && GregorianDay == 1) {
          GregorianDay = 29;
        } else {
          GregorianDay = NumberDayInMonth[GregorianMonthInSet -1];
        }
        //CountPushButton5ForSetTimeAndDate = 0;
        Rtc.SetDateTime(RtcDateTime(now.Year(), now.Month(), GregorianDay, now.Hour(), now.Minute(), now.Second()));

      ShowGregorianDateOnLcd(2, 0, GetGregorianDate(GregorianYear, GregorianMonthInSet -1, GregorianDay));
      }

      else if (EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetTimeAndDate, 10) == SetGregorianMonth) {

        if (GregorianMonthInSet  > 1) {
          GregorianMonthInSet --;
        } else {
          GregorianMonthInSet  = 12;
        }
        if (GregorianDay > NumberDayInMonth[GregorianMonthInSet -1]) {
          GregorianDay = NumberDayInMonth[GregorianMonthInSet -1];
          if (IsLeapYear(GregorianYear ) && GregorianMonthInSet == 2)
            GregorianDay = 29;
        }
        //CountPushButton5ForSetTimeAndDate = 0;
        Rtc.SetDateTime(RtcDateTime(now.Year(), GregorianMonthInSet ,GregorianDay, now.Hour(), now.Minute(), now.Second()));

      ShowGregorianDateOnLcd(2, 0, GetGregorianDate(GregorianYear, GregorianMonthInSet -1, GregorianDay));

      } else if (EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetTimeAndDate, 10) == SetGregorianYear) {

        GregorianYear--;
        if (GregorianDay > NumberDayInMonth[GregorianMonthInSet -1]) {
          GregorianDay = NumberDayInMonth[GregorianMonthInSet -1];
          if (IsLeapYear(GregorianYear) && GregorianMonthInSet == 2)
            GregorianDay = 29;
        }
        //CountPushButton5ForSetTimeAndDate = 0;
        Rtc.SetDateTime(RtcDateTime(GregorianYear, now.Month(), GregorianDay, now.Hour(), now.Minute(), now.Second()));

      ShowGregorianDateOnLcd(2, 0, GetGregorianDate(GregorianYear, GregorianMonthInSet -1, GregorianDay));

      }

      else if (EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetTimeAndDate, 10) == SetHijriDay) {

        if (HijriDay > 1) {
          HijriDay--;
        } else {
          HijriDay = 30;
        }
          //CountPushButton5ForSetTimeAndDate = 0;

        ShowHijriDateOnLcd(2, 0, GetHijriDate(HijriYear, HijriMonthInSet, HijriDay));

      }

      else if (EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetTimeAndDate, 10) == SetHijriMonth) {

        if (HijriMonthInSet > 0) {
          HijriMonthInSet--;
        } else {
          HijriMonthInSet = 11;
        }
        ShowHijriDateOnLcd(2, 0, GetHijriDate(HijriYear, HijriMonthInSet, HijriDay));
        //CountPushButton5ForSetTimeAndDate = 0;


      }

      else if (EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetTimeAndDate, 10) == SetHijriYear) {

        HijriYear--;

        ShowHijriDateOnLcd(2, 0, GetHijriDate(HijriYear, HijriMonthInSet, HijriDay));
        //CountPushButton5ForSetTimeAndDate = 0;



      }

      else if (EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetTimeAndDate, 10) == SetDayName) {
    if(digitalRead(PushButton4) == LOW && digitalRead(PushButton5) == LOW )
        {
          if(ReadStartPeriodForAutoDayOrder == 0)
          {
          StartPeriodForAutoDayOrder = millis();
          ReadStartPeriodForAutoDayOrder = 1;
          }
          if(millis() - StartPeriodForAutoDayOrder > 5000)
          {
            AutoDayOrder();
            EndPeriodForAutoDayOrder = millis();
          }
        }
        else if(millis() - EndPeriodForAutoDayOrder >5000  )
        {
            if (DayInSet > 0) {
            DayInSet--;
          } else {
            DayInSet = 6;
          }
        }
        EEPROM.update(Address_EEPROM_DayName, DayInSet);
        ShowDayOnLcd(4, 0, GetDayName(DayInSet));

        //CountPushButton5ForSetTimeAndDate = 0;
      }
    }






    ////delay(5);
    if (EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetTimeAndDate, 10) == SetSeconds) {
    
      if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
  if(HoursType == _24Hours)
            ShowTimeOnLcd24Hours(4, 0, GetTime(now.Hour(), now.Minute(), now.Second()));
            else
            {
              byte _Hours = now.Hour();
              bool _AM_PM = 0;
              if(_Hours >=12)
              {
                _Hours = _Hours -12;
                _AM_PM = 1;
              }
                  if(_Hours ==0)
              {
                _Hours = 12;
              }
            
              ShowTimeOnLcd12Hours(4, 0, GetTime(_Hours, now.Minute(), now.Second()), _AM_PM);

            }

        //delay(5);
      } else {

        WriteFullPointsOnLcd(10, 0, 2);
        //delay(5);
      }



    }

    else if (EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetTimeAndDate, 10) == SetMinutes) {

      if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
  if(HoursType == _24Hours)
            ShowTimeOnLcd24Hours(4, 0, GetTime(now.Hour(), now.Minute(), now.Second()));
            else
            {
              byte _Hours = now.Hour();
              bool _AM_PM = 0;
              if(_Hours >=12)
              {
                _Hours = _Hours -12;
                _AM_PM = 1;
              }
                  if(_Hours ==0)
              {
                _Hours = 12;
              }
            
              ShowTimeOnLcd12Hours(4, 0, GetTime(_Hours, now.Minute(), now.Second()), _AM_PM);

            }

        //delay(5);
      } else {
        WriteFullPointsOnLcd(7, 0, 2);
        //delay(5);
      }

    }

    else if (EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetTimeAndDate, 10) == SetHours) {

      if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
  if(HoursType == _24Hours)
          ShowTimeOnLcd24Hours(4, 0, GetTime(now.Hour(), now.Minute(), now.Second()));
          else
          {
            byte _Hours = now.Hour();
            bool _AM_PM = 0;
            if(_Hours >=12)
            {
              _Hours = _Hours -12;
              _AM_PM = 1;
            }
                if(_Hours ==0)
            {
              _Hours = 12;
            }
          
             ShowTimeOnLcd12Hours(4, 0, GetTime(_Hours, now.Minute(), now.Second()), _AM_PM);

          }

      //delay(5);
    } else {
      WriteFullPointsOnLcd(4, 0, 2);



      //delay(5);
    }

  }

  else if (EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetTimeAndDate, 10) == SetGregorianDay) {



    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
     ShowGregorianDateOnLcd(2, 0, GetGregorianDate(GregorianYear, GregorianMonthInSet -1, GregorianDay));

      //delay(5);
    } else {
      WriteFullPointsOnLcd(2, 0, 2);


      //delay(5);
    }



  }

  else if (EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetTimeAndDate, 10) == SetGregorianMonth) {


    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
     ShowGregorianDateOnLcd(2, 0, GetGregorianDate(GregorianYear, GregorianMonthInSet -1, GregorianDay));

      //delay(5);
    } else {
      WriteFullPointsOnLcd(6, 0, 3);


      //delay(5);
    }


  }

  else if (EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetTimeAndDate, 10) == SetGregorianYear) {

    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
     ShowGregorianDateOnLcd(2, 0, GetGregorianDate(GregorianYear, GregorianMonthInSet -1, GregorianDay));

      //delay(5);
    } else {
      WriteFullPointsOnLcd(11, 0, 4);




      //delay(5);
    }



  }

  else if (EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetTimeAndDate, 10) == SetHijriDay) {

    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
      ShowHijriDateOnLcd(2, 0, GetHijriDate(HijriYear, HijriMonthInSet, HijriDay));


      //delay(5);
    } else {
      WriteFullPointsOnLcd(2, 0, 2);



      //delay(5);
    }



  }

  else if (EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetTimeAndDate, 10) == SetHijriMonth) {

    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
      ShowHijriDateOnLcd(2, 0, GetHijriDate(HijriYear, HijriMonthInSet, HijriDay));


      //delay(5);
    } else {
      WriteFullPointsOnLcd(6, 0, 3);


      //delay(5);
    }


  }

  else if (EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetTimeAndDate, 10) == SetHijriYear) {


    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
      ShowHijriDateOnLcd(2, 0, GetHijriDate(HijriYear, HijriMonthInSet, HijriDay));


      //delay(5);
    } else {
      WriteFullPointsOnLcd(11, 0, 4);


      //delay(5);
    }




  }

  else if (EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetTimeAndDate, 10) == SetDayName) {
 
    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
      ShowDayOnLcd(6, 0, GetDayName(DayInSet));


      //delay(5);
    } else {
      WriteFullPointsOnLcd(6, 0, 3);


      //delay(5);
    }
  }

  
 
}


void ShowScreen() {

  if (CurrentScreen == 0) {
    ShowTimeAndDateScreen();
  }
  if (CurrentScreen == 1) {
    ShowStopWatchScreen();
  }
  if (CurrentScreen == 2) {
    ShowSetAlarmScreen();
  }

  if (CurrentScreen == 3) {
    ShowSetDateAndTimeScreen();
  }
  now = Rtc.GetDateTime();
  UpdateTimeAndDate();
  UpdateStopTime();
  UpdateHijriDateAndDayName();

  CountAlarmPeep++;
  if (CountAlarmPeep == MaxCountAlarmPeep)  //40
  {
    CountAlarmPeep = 0;
  }
  CountFullPointsShow++;
  
  if (CountFullPointsShow == MaxCountFullPointsShow) 
  {
    CountFullPointsShow = 0;
  }
}


void setup() {
  lcd.begin(16, 2);
  lcd.clear();
  Rtc.Begin();
  Serial.begin(9600);
  /*
  Rtc.SetDateTime(RtcDateTime(2024,6,9,now.Hour(), now.Minute(), now.Second()));
  now = Rtc.GetDateTime();
  */
  pinMode(PushButton1,INPUT_PULLUP);
  pinMode(PushButton2,INPUT_PULLUP);
  pinMode(PushButton3,INPUT_PULLUP);
  pinMode(PushButton4,INPUT_PULLUP);
  pinMode(PushButton5,INPUT_PULLUP);
  UpdateHijriDateAndDayNameFrom_EEPROM();

  //RtcDateTime currentTime = RtcDateTime(__DATE__ , __TIME__);
 // Rtc.SetDateTime(currentTime);


  //Rtc.SetDateTime(RtcDateTime("1 15 2010","15:05:02"));
}

void loop() {
  ShowScreen();
}
