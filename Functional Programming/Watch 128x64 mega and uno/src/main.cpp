
#include <Arduino.h>
#include <U8g2lib.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>
#include <TimerOne.h>
#include <EEPROM.h>
#include "MyData.h"
#include <avr/interrupt.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#define PinLight 4

#define ShowTimeAndDateScreenTimeColumn 25
#define ShowTimeAndDateScreenTimeRow 25

#define ShowTimeAndDateScreenAlarmTimeColumn 25
#define ShowTimeAndDateScreenAlarmTimeRow 25

#define ShowTimeAndDateScreenDateColumn 10
#define ShowTimeAndDateScreenDateRow 45

#define ShowTimeAndDateScreenDayColumn 50
#define ShowTimeAndDateScreenDayRow 45

#define Time12Hours_AM_PM_Column 54
#define Time12Hours_AM_PM_Row 45

#define SetAlarmScreenAlarmTimeColumn 25
#define SetAlarmScreenAlarmTimeRow 25

#define SetTimeAndDateScreenDateColumn 10
#define SetTimeAndDateScreenDateRow 25

#define SetTimeAndDateScreenTimeColumn 25
#define SetTimeAndDateScreenTimeRow 25

#define SetTimeAndDateScreenDayColumn 50
#define SetTimeAndDateScreenDayRow 25



#define MilliSecondColumn 90
#define MilliSecondRow 45

bool FirstEnterShowTimeOnLcd12Hours = true;

bool IsBeforeEnterShowDate = false;
bool IsBeforeEnterShowDay = false;

bool EnterStopWatchFirst = true;

U8G2_ST7920_128X64_F_SW_SPI lcd(U8G2_R2, /* clock=*/ A5, /* data=*/ A4, /* CS=*/ A3,  /*reset=*/ A2);


#define PushButtonRightDown 8
#define PushButtonRightTop 9
#define PushButtonMiddle 10
#define PushButtonLeftTop  11
#define PushButtonLeftDown 12

#define Buzzer A1




#define InterruptPin 21



ThreeWire myWire(6, 5, 7);        // DAT, CLK, RST
RtcDS1302<ThreeWire> Rtc(myWire);  // RTC Object

RtcDateTime now = Rtc.GetDateTime();



void lcd_prepare(void) {
 lcd.setFont(u8g_font_10x20);
  lcd.setFontRefHeightExtendedText();
  lcd.setDrawColor(1);
  lcd.setFontPosTop();
}







#define SetSeconds 0
#define SetMinutes 1
#define SetHours 2
#define SetGregorianDay 3
#define SetGregorianMonth 4
#define SetGregorianYear 5
#define SetHijriDay 6
#define SetHijriMonth 7
#define SetHijriYear 8
#define SetDayName 9

#define Address_EEPROM_DayName 0
#define Address_EEPROM_MinuteAlarm 1
#define Address_EEPROM_HourAlarm 2
#define Address_EEPROM_HijriDay 3
#define Address_EEPROM_HijriMonth 4
#define Address_EEPROM_HijriYear_Left 5
#define Address_EEPROM_HijriYear_Right 6
#define Address_EEPROM_GregorianDay 7
#define Address_EEPROM_GregorianMonth 8
#define Address_EEPROM_GregorianYear_Left 9
#define Address_EEPROM_GregorianYear_Right 10

/*
#define HijriDate 0 
#define GregorianDate 1
*/

#define NotSameLine 0
#define SameLine 1

#define WithoutIncludeEndDay 0
#define IncludeEndDay 1



sTime Time;
sTime AlarmTime;
sTime AlarmNapTime;
sTime StopWatchTime;


#define Input_PullUp 0
#define Input_PullDown 1
bool Input_PullType = Input_PullUp;

#define _24Hours 0
#define _12Hours 1
bool HoursType = _12Hours;

bool IsIncreaseHijriDateAndDayName = 0;

byte CurrentSetDigit = 0;
byte PreviousSetDigit = 0;
bool ChangeSetDigit = 0;

int Read2ByteEEPROM(int LeftHalf, int RightHalf) {
  int LefttHalfValue = EEPROM.read(LeftHalf);
  LefttHalfValue = LefttHalfValue << 8;

  return LefttHalfValue + EEPROM.read(RightHalf);
}

void Update2ByteEEPROM(int LeftHalf, int RightHalf, int Value) {
  EEPROM.update(LeftHalf, byte(Value >> 8));
  EEPROM.update(RightHalf, byte(Value));
}

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



#define Minutes_AlarmNapTime_Period 5  //

bool IsAllowAlarm = false;  //
bool IsAllowAlarmNap = false;

bool Previous_PushButtonLeftTop_Value = 0;  //
bool Previous_PushButtonLeftDown_Value = 0;  //
bool Previous_PushButtonRightDown_Value = 0;  //
bool Previous_PushButtonRightTop_Value = 0;  //
bool Previous_PushButtonMiddle_Value = 0;  //

bool PushButtonLeftTop_Value = 0;  //
bool PushButtonLeftDown_Value = 0;  //
bool PushButtonRightDown_Value = 0;  //
bool PushButtonRightTop_Value = 0;  //
bool PushButtonMiddle_Value = 0;  //

byte DayInSet = EEPROM.read(Address_EEPROM_DayName);  //

sDate HijriDate;
sDate GregorianDate;

byte CountForNapTimes = 0;  //

bool IsAllowCountStopWatch = 0;  //

int CountPushButtonRightTopForDate = 0;              //
int CountPushButtonRightTopForSetAlarm = 0;          //
int CountPushButtonRightTopForTurnOffAlarm = 0;      //
int CountPushButtonRightTopForSetTimeAndDate = 0;    //
int CountPushButtonRightTopForStopWatch_Peeb = 0;    //
int CountPushButtonRightTopForTurnAlarmOnClick = 0;  //

int CountPushButtonLeftDownForChangeScreen = 0;  //

int CountPushButtonRightDownForStopWatch = 0;         //
int CountPushButtonRightDownForSetAlarm = 0;          //
int CountPushButtonRightDownForSetTimeAndDate = 0;    //
int CountPushButtonRightDownForTurnOffAlarm = 0;      //
int CountPushButtonRightDownForTurnAlarmOnClick = 0;  //

int CountPushButtonMiddleForSetAlarm = 0;        //
int CountPushButtonMiddleForSetTimeAndDate = 0;  //

int ChangeTypeOfDate = 0;  //


byte CurrentScreen = 0;  //

bool EnterShowDate = 0;
bool EnterShowDay  = 0;

int CountAlarmPeep      = 0;
int CountFullPointsShow = 0;
int CountMilliSeconds   = 0; 

#define MaxCountAlarmPeep      200
#define MaxCountFullPointsShow 400
#define MaxCountMilliSeconds   999

int  PreviousMilliSecond = 0;

bool IsLastButton4StopWatch = 0;
bool Button3AsButton4StopWatch = 0;

long StartPeriodForAutoDayOrder = 0;
long EndPeriodForAutoDayOrder = 0;
bool ReadStartPeriodForAutoDayOrder = 0;

long StartPeriodForTurnLight = 0;

bool IsChangedHoursType = 0;
bool IsReadStartPeriodForChangeHoursType = 0;
unsigned long StartPeriodForChangeHoursType = 0;

bool TurnOffAlarm = false;

bool IsShowAlarmTime = false;

bool IsShowDay = false;

#define refPeriodForChangeHoursType 3000
#define refPeriodChangeValue_IncreaseButton4_SetAlarm 1000
#define refPeriod_EnterTime_IncreaseButton4_SetAlarm 100
#define refPeriodChangeValue_DecreaseButton5_SetAlarm 1000
#define refPeriod_EnterTime_DecreaseButton5_SetAlarm 100
#define refPeriodChangeValue_IncreaseButton4_SetDateAndTime 1000
#define refPeriod_EnterTime_IncreaseButton4_SetDateAndTime 100
#define refPeriodChangeValue_DecreaseButton5_SetDateAndTime 1000
#define refPeriod_EnterTime_DecreaseButton5_SetDateAndTime 100
#define refPeriodStartPeriodForAutoDayOrder 3000
#define refPeriodEndPeriodForAutoDayOrder 3000

//LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
// LiquidCrystal_I2C lcd(0x27, 16, 2);

// ThreeWire myWire(9, 10, 8);        // DAT, CLK, RST
// RtcDS1302<ThreeWire> Rtc(myWire);  // RTC Object

// RtcDateTime now = Rtc.GetDateTime();
//RtcDateTime now;



void CheckLight() {
  if (digitalRead(PushButtonLeftTop)) {
    PushButtonLeftTop_Value = 1;
  } else {
    PushButtonLeftTop_Value = 0;
  }
  if (PushButtonLeftTop_Value == 0 ) {
    StartPeriodForTurnLight = millis();
  }

  if (millis() - StartPeriodForTurnLight < 5000) {
    pinMode(PinLight, OUTPUT);
    analogWrite(PinLight, 155);
  }

  else {
    analogWrite(PinLight, 30);
  }
}

void WriteFullPointsOnLcd(byte Column, byte Row, byte Number, bool ThereSendBufferAtNext = false) {
  lcd.drawBox(Column, Row,10*Number,20);
  if(!ThereSendBufferAtNext)
 lcd.sendBuffer();
}
void WriteOnColumnAndRowOnLcd(byte Column, byte Row, String Text) {
   
  lcd.drawStr(Column, Row, Text.c_str());
  lcd.sendBuffer();
}


void ClearRow(byte Row, int Hight, bool ThereSendBufferAtNext = false) {
   
  lcd.setDrawColor(0); 
  lcd.drawBox(0, Row, 128, Hight);
  lcd.setDrawColor(1);
  if(!ThereSendBufferAtNext)
     lcd.sendBuffer();
  }


void WriteOnCleanRowOnLcd(byte Column, byte Row, String Text, bool ThereSendBufferAtNext = false) {
   
  ClearRow(Row, 20, true);
  lcd.drawStr(Column, Row,(Text).c_str());
  if(!ThereSendBufferAtNext)
     lcd.sendBuffer();
}

bool EventWhilePressingPushButton(int PinName, bool& Push_Value, bool InputPull_Type) {
  Push_Value = digitalRead(PinName);
  if (InputPull_Type == Input_PullDown) {
    if (Push_Value == HIGH) {
      return true;
    }

    else {
      return false;
    }
  } else {
    if (Push_Value == LOW) {
      return true;
    }

    else {
      return false;
    }
  }
}

void TurnOnBuzzer(byte _Buzzer, int Frequency) {
  tone(_Buzzer, Frequency);
}

void TurnOffBuzzer(byte _Buzzer) {
  noTone(_Buzzer);
  pinMode(_Buzzer, INPUT);
}

bool fnIsAllowAlarm(sTime _Time, sTime _AlarmTime, sTime _AlarmNapTime, byte _PushButtonRightDown, byte _PushButtonRightTop, bool& _PushButtonRightDown_Value, bool& _PushButtonRightTop_Value, bool _IsAllowAlarm, bool _IsAllowAlarmNap, bool& _TurnOffAlarm, byte _CurrentScreen, bool InputPull_Type) {

  bool _Is_AllowAlarm = 0;
  _PushButtonRightDown_Value = digitalRead(_PushButtonRightDown);
  _PushButtonRightTop_Value = digitalRead(_PushButtonRightTop);

  if (_Time.Hours != _AlarmTime.Hours || _Time.Minutes != _AlarmTime.Minutes) {
    _TurnOffAlarm = false;
  }

  if ((_Time.Hours == _AlarmTime.Hours && _Time.Minutes == _AlarmTime.Minutes && _IsAllowAlarm && !_TurnOffAlarm) || (_Time.Hours == _AlarmNapTime.Hours && _Time.Minutes == _AlarmNapTime.Minutes && _IsAllowAlarmNap && !_TurnOffAlarm))
    _Is_AllowAlarm = true;

  else if (InputPull_Type == Input_PullDown) {
    if (_PushButtonRightTop_Value == HIGH && _PushButtonRightDown_Value == HIGH && _CurrentScreen == 0)
      _Is_AllowAlarm = true;

  } else if (InputPull_Type == Input_PullUp) {
    if (_PushButtonRightTop_Value != HIGH && _PushButtonRightDown_Value != HIGH && _CurrentScreen == 0)
      _Is_AllowAlarm = true;

  } else
    _Is_AllowAlarm = false;

  return _Is_AllowAlarm;
}

void AllowAlarm(sTime _Time, sTime _AlarmTime, bool& _IsAllowAlarm) {
  if (_Time.Hours == _AlarmTime.Hours && _Time.Minutes == _AlarmTime.Minutes && _Time.Seconds == 0) {
    _IsAllowAlarm = true;
  }
}
byte EventAfterPressingPushButton(byte PinName, bool& Push_Value, bool& Previous_Push_Value, int& CountPush, int CasesNumber, bool InputPull_Type) {

  Push_Value = digitalRead(PinName);

  if (InputPull_Type == Input_PullDown) {

    if (Push_Value == 1 && Previous_Push_Value == 0) {
      CountPush = CountPush + 1;

      if (CountPush == CasesNumber) {
        CountPush = 0;
      }
    }


  } else {
    if (Push_Value != 1 && Previous_Push_Value != 0) {
      CountPush = CountPush + 1;

      if (CountPush == CasesNumber) {
        CountPush = 0;
      }
    }
  }
  Previous_Push_Value = Push_Value;

  return CountPush;
}

bool EventAfterPressingPushButtonAndResetValue(byte PinName, bool& Push_Value, bool& Previous_Push_Value, int& CountPush, bool InputPull_Type) {
  bool Value = EventAfterPressingPushButton(PinName, Push_Value, Previous_Push_Value, CountPush, 2, InputPull_Type);

  if (Value) {
    CountPush = 0;
  }

  return Value;
}

bool MoveScreen(byte& _CurrentScreen) {
  byte PreviuosScreen = _CurrentScreen;
  _CurrentScreen = EventAfterPressingPushButton(PushButtonLeftDown, PushButtonLeftDown_Value, Previous_PushButtonLeftDown_Value, CountPushButtonLeftDownForChangeScreen, 4, Input_PullType);
  return _CurrentScreen != PreviuosScreen;
}

bool IsLeapYear(int Year) {
  return ((Year % 400 == 0) || (Year % 4 == 0 && Year % 100 != 0));
}

String GetStringTime(sTime _Time) {
  String StringTime = "";

  //Set The Hours Part
  if (_Time.Hours < 10) {
    StringTime += "0";
  }

  StringTime += String(_Time.Hours);
  StringTime += ":";

  //Set The Minutes Part
  if (_Time.Minutes < 10) {
    StringTime += "0";
  }

  StringTime += String(_Time.Minutes);
  StringTime += ":";

  //Set The Seconds Part
  if (_Time.Seconds < 10) {
    StringTime += "0";
  }

  StringTime += String(_Time.Seconds);
  return StringTime;
}

sDate GetDate(int _Years, byte _MonthNumber, byte _Day) {
  sDate _Date;
  _Date.Year = _Years;
  _Date.Month = _MonthNumber;
  _Date.Day = _Day;
  return _Date;
}


String GetStringGregorianDate(sDate _Date) {
  String GregorianMonthsSet[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
  String Date = "";
  if (_Date.Day < 10) {
    Date += "0";
  }
  Date += String(_Date.Day);
  Date += " ";

  Date += GregorianMonthsSet[_Date.Month];

  Date += " ";

  Date += String(_Date.Year);
  return Date;
}
String GetStringHijriDate(sDate _Date) {
  String HijriMonthsSet[12] = { "Moh", "Sef", "Ra1", "Ra2", "Jm1", "Jm2", "Rej", "She", "Ram", "Sho", "ked", "Heg" };
  String Date = "";
  if (_Date.Day < 10) {
    Date += "0";
  }
  Date += String(_Date.Day);
  Date += " ";

  Date += HijriMonthsSet[_Date.Month];

  Date += " ";

  Date += String(_Date.Year);
  return Date;
}
String GetDayName(byte DayNumber) {

  String DaysSet[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
  return DaysSet[DayNumber];
  
}



void ShowTimeOnLcd24Hours(byte Column, byte Row, String Time) {
  if(EnterStopWatchFirst)
   {
    ClearRow(Time12Hours_AM_PM_Row, 20, true);
    EnterStopWatchFirst =false;
   }
   WriteOnCleanRowOnLcd(Column, Row, Time);
  FirstEnterShowTimeOnLcd12Hours = true;
}

void ShowTimeOnLcd12Hours(byte Column, byte Row, String Time, bool AM_PM, bool IsSameLine) {
   if( (IsBeforeEnterShowDate||IsBeforeEnterShowDay) && !EnterShowDate && !EnterShowDay )
   {
    IsBeforeEnterShowDate = false;
    IsBeforeEnterShowDay= false;
    ClearRow(Time12Hours_AM_PM_Row, 20, true);
   }
  if (IsSameLine == 0) 
  {
    WriteOnCleanRowOnLcd(Column , Row, Time);

    if (AM_PM == 0)
      WriteOnColumnAndRowOnLcd(Time12Hours_AM_PM_Column,  Time12Hours_AM_PM_Row, "AM");
    else
      WriteOnColumnAndRowOnLcd(Time12Hours_AM_PM_Column,  Time12Hours_AM_PM_Row, "PM");
  
  } 
  else 
  {
    //////WriteOnColumnAndRowOnLcd(Column - 20, Row, String(Time + "  "));
    WriteOnCleanRowOnLcd(Column - 20, Row, String(Time + "  "), true);
      if (AM_PM == 0)
      WriteOnColumnAndRowOnLcd(Time12Hours_AM_PM_Column + 50,  ShowTimeAndDateScreenTimeRow,  "AM");
    else
      WriteOnColumnAndRowOnLcd(Time12Hours_AM_PM_Column + 50,  ShowTimeAndDateScreenTimeRow, "PM");
  }
}

void ShowGregorianDateOnLcd(byte Column, byte Row, String Date) {
  WriteOnCleanRowOnLcd(Column, Row, Date);
}

void ShowHijriDateOnLcd(byte Column, byte Row, String Date) {
  WriteOnCleanRowOnLcd(Column, Row, Date);
}

void ShowDayOnLcd(byte Column, byte Row, String DayName) {
  WriteOnCleanRowOnLcd(Column, Row, DayName, false);
}

void ShowAlarmScreenOnLcd(byte Column, byte Row, String AlarmTime, bool HoursType, bool AM_PM) {
  if (HoursType == _24Hours)
    ShowTimeOnLcd24Hours(Column, Row, AlarmTime);
  else {
    ShowTimeOnLcd12Hours(Column, Row, AlarmTime, AM_PM, NotSameLine);
  }
}



void ExchangeAndShowDateOnLcd(byte Column, byte Row, byte _PushButtonRightTop, bool _PushButtonRightTop_Value, String GregorianDate, String HijriDate, int Change) {
  //int NumberOfClearRow, = 1;
  if (EventWhilePressingPushButton(_PushButtonRightTop, _PushButtonRightTop_Value, Input_PullType)) {
    if (Change == 0)
      ShowHijriDateOnLcd(Column, Row, HijriDate);
    else
      ShowGregorianDateOnLcd(Column, Row, GregorianDate);
  } else {
    ClearRow(Time12Hours_AM_PM_Row, 20, true);
  }
}

void IncreaseCountAlarmPeep(int& _CountAlarmPeep, int _MaxCountAlarmPeep) {
  _CountAlarmPeep++;
  if (_CountAlarmPeep == _MaxCountAlarmPeep)  //40
  {
    _CountAlarmPeep = 0;
  }
}

void IncreaseCountFullPointsShow(int& _CountFullPointsShow, int _MaxCountFullPointsShow) {
  _CountFullPointsShow++;
  if (_CountFullPointsShow == _MaxCountFullPointsShow) {
    _CountFullPointsShow = 0;
  }
}


void UpdateStopTime(sTime _Time, sTime& _StopWatchTime, int& _PreviousMilliSecond, bool _IsAllowCountStopWatch) {
  if (_PreviousMilliSecond != _Time.MilliSeconds) {
    _PreviousMilliSecond = _Time.Seconds;
    if (_IsAllowCountStopWatch == 1) {
      _StopWatchTime.MilliSeconds++;
    }
    if( _StopWatchTime.MilliSeconds == MaxCountMilliSeconds)
    {
      _StopWatchTime.MilliSeconds = 0;
      _StopWatchTime.Seconds++;
    }

    if (_StopWatchTime.Seconds == 60) {
      _StopWatchTime.Seconds = 0;
      _StopWatchTime.Minutes++;
    }

     if (_StopWatchTime.Minutes == 60) {
      _StopWatchTime.Minutes = 0;
      _StopWatchTime.Hours++;
    }

    if (_StopWatchTime.Hours == 24) {
      _StopWatchTime.Hours = 0;
    }
  }
}

void IncreaseMilliSeconds(int& _CountMilliSeconds, int _MaxCountMilliSeconds) {
  PreviousMilliSecond =  _CountMilliSeconds;
  _CountMilliSeconds++;
  
  if (_CountMilliSeconds == _MaxCountMilliSeconds) {
    _CountMilliSeconds = 0;
    if(now.Second() <59)
     {
         Rtc.SetDateTime(RtcDateTime(now.Year(), now.Month(), now.Day(), now.Hour(), now.Minute(), now.Second()+1));

       // UpdateHijriDateAndDayNameFrom_EEPROM(GetDate(now.Year(), now.Month(), now.Day()), GregorianDate, HijriDate, DayInSet, Address_EEPROM_HijriYear_Left, Address_EEPROM_HijriYear_Right, Address_EEPROM_HijriMonth, Address_EEPROM_HijriDay, Address_EEPROM_DayName);

     }
 

  }
    UpdateStopTime(Time, StopWatchTime, PreviousMilliSecond, IsAllowCountStopWatch);

}

void RingTone(byte _Buzzer, int _CountAlarmPeep) {
  if (_CountAlarmPeep / 2 < 40) {
    TurnOnBuzzer(_Buzzer, 3000);

  } else if (_CountAlarmPeep / 2 < 70) {
    TurnOffBuzzer(_Buzzer);
  } else if (_CountAlarmPeep / 2 < 50 || (_CountAlarmPeep / 2 >= 100 && _CountAlarmPeep / 2 < 150)) {
   TurnOnBuzzer(_Buzzer, 3000);
  } else {
    TurnOffBuzzer(_Buzzer);
  }
}


void IncreaseCountersValuesByTimer1() {
  if(IsAllowAlarm)
  RingTone( Buzzer,  CountAlarmPeep);
  IncreaseCountAlarmPeep(CountAlarmPeep, MaxCountAlarmPeep);
  IncreaseCountFullPointsShow(CountFullPointsShow, MaxCountFullPointsShow);
  IncreaseMilliSeconds(CountMilliSeconds,MaxCountMilliSeconds);
}


void UpdateTime(sTime& _Time, byte Hours, byte Minutes, byte Seconds) {
  _Time.Hours = Hours;
  _Time.Minutes = Minutes;
  _Time.Seconds = Seconds;
}

void UpdateDate(sDate& _Date, int Year, byte Month, byte Day) {

  _Date.Day = Day;
  _Date.Month = Month;
  _Date.Year = Year;
}

void UpdateTimeAndDate(sTime& _Time, byte Hours, byte Minutes, byte Seconds, sDate& _Date, int Year, byte Month, byte Day) {
  UpdateTime(_Time, Hours, Minutes, Seconds);
  UpdateDate(_Date, Year, Month, Day);
}
void UpdateGregorianDateIn_EEPROM_Rtc_D1302(RtcDateTime _Now, sDate _GregorianDate, int _Address_EEPROM_GregorianYear_Left, int _Address_EEPROM_GregorianYear_Right, int _Address_EEPROM_GregorianMonth, int _Address_EEPROM_GregorianDay)  // to edit the hijri Date and Day Name
{
  UpdateDate(_GregorianDate, _Now.Year(), _Now.Month(), _Now.Day());
  EEPROM.update(_Address_EEPROM_GregorianDay, _GregorianDate.Day);
  EEPROM.update(_Address_EEPROM_GregorianMonth, _GregorianDate.Month);
  Update2ByteEEPROM(_Address_EEPROM_GregorianYear_Left, _Address_EEPROM_GregorianYear_Right, _GregorianDate.Year);
}



void UpdateHijriDateIn_EEPROM(sDate _HijriDate, int _Address_EEPROM_HijriYear_Left, int _Address_EEPROM_HijriYear_Right, int _Address_EEPROM_HijriMonth, int _Address_EEPROM_HijriDay)  // to edit the hijri Date and Day Name
{
  EEPROM.update(_Address_EEPROM_HijriDay, _HijriDate.Day);
  EEPROM.update(_Address_EEPROM_HijriMonth, _HijriDate.Month);
  Update2ByteEEPROM(_Address_EEPROM_HijriYear_Left, _Address_EEPROM_HijriYear_Right, _HijriDate.Year);
}

int NumberOfDaysInAMonth(int Year, byte Month) {
  return Month = Month == 2 ? (IsLeapYear(Year) ? 29 : 28) : (Month == 11 || Month == 9 || Month == 6 || Month == 4) ? 30
                                                                                                                     : 31;
}

bool IsDate1BeforeDate2(sDate Date1, sDate Date2) {
  return (Date1.Year < Date2.Year) ? true : (Date1.Month < Date2.Month) ? true
                                                                        : (Date1.Day < Date2.Day ? true : false);
}

void IncreaseDateByOneDay(sDate& _Date) {
  _Date.Day++;
  if (_Date.Day > NumberOfDaysInAMonth(_Date.Month, _Date.Year)) {
    _Date.Day = 1;
    _Date.Month++;
  }
  if (_Date.Month > 12) {

    _Date.Month = 1;
    _Date.Year++;
  }

  // return Date;
}
long GetDifferenceInDays(sDate Date1, sDate Date2, bool IsIncludeEndDay) {
  long NumDays = 0;
  while (IsDate1BeforeDate2(Date1, Date2)) {
    IncreaseDateByOneDay(Date1);
    NumDays++;
  }
  return IsIncludeEndDay ? ++NumDays : NumDays;
}

void IncreaseHijriDateByOneDay(sDate& _HijriDate) {
  ++_HijriDate.Day;
  if (_HijriDate.Day > 30) {
    _HijriDate.Day = 1;
    ++_HijriDate.Month;
  }
  if (_HijriDate.Month > 11) {
    _HijriDate.Month = 0;
    ++_HijriDate.Year;
  }
}


void IncreaseDayNameByOneDay(byte& _DayInSet) {
  ++_DayInSet;
  if (_DayInSet > 6) {
    _DayInSet = 0;
  }
}

void UpdateHijriDateAndDayName(RtcDateTime _Now, sDate& _HijriDate, byte& _DayInSet, bool& _IsIncreaseHijriDateAndDayName, int _Address_EEPROM_HijriYear_Left, int _Address_EEPROM_HijriYear_Right, int _Address_EEPROM_HijriMonth, int _Address_EEPROM_HijriDay, int _Address_EEPROM_DayName) {

 //--- 14/15/2025 update these function wednesday
  if (_Now.Hour() == 0 && _Now.Minute() == 0 && _Now.Second() == 0 /*&& Time.MilliSeconds > 500*/ && _IsIncreaseHijriDateAndDayName == 0 && _Now.Day() != 0)  // Day if the RTC not connected
  {
    IncreaseHijriDateByOneDay(_HijriDate);
    IncreaseDayNameByOneDay(_DayInSet);
    _IsIncreaseHijriDateAndDayName = 1;
    UpdateHijriDateIn_EEPROM(_HijriDate, _Address_EEPROM_HijriYear_Left, _Address_EEPROM_HijriYear_Right, _Address_EEPROM_HijriMonth, _Address_EEPROM_HijriDay);
    EEPROM.update(_Address_EEPROM_DayName, _DayInSet);
  }
  if (_Now.Hour() == 0 && _Now.Minute() == 0 && _Now.Second() >= 1 ) {
    _IsIncreaseHijriDateAndDayName = 0;
  }
}



void UpdateHijriDateAndDayNameFrom_EEPROM(sDate _Now_Gregorian, sDate EEPROM_Date_Gregorian, sDate& _HijriDate, byte& _DayInSet, int _Address_EEPROM_HijriYear_Left, int _Address_EEPROM_HijriYear_Right, int _Address_EEPROM_HijriMonth, int _Address_EEPROM_HijriDay, int _Address_EEPROM_DayName) {

  long DayNumber = GetDifferenceInDays(EEPROM_Date_Gregorian, _Now_Gregorian, WithoutIncludeEndDay);

  while (DayNumber > 0) {
    DayNumber--;
    IncreaseDayNameByOneDay(_DayInSet);
    IncreaseHijriDateByOneDay(_HijriDate);
  }

  UpdateHijriDateIn_EEPROM(_HijriDate, _Address_EEPROM_HijriYear_Left, _Address_EEPROM_HijriYear_Right, _Address_EEPROM_HijriMonth, _Address_EEPROM_HijriDay);
  EEPROM.update(_Address_EEPROM_DayName, _DayInSet);
}


byte DayOrder(sDate _Date) {
  long a = (14 - _Date.Month) / 12;
  _Date.Year = _Date.Year - a;
  _Date.Month = _Date.Month + 12 * a - 2;
  return (_Date.Day + _Date.Year + (_Date.Year / 4) - (_Date.Year / 100) + (_Date.Year / 400) + (31 * _Date.Month / 12)) % 7;
}
void AutoDayOrder(RtcDateTime _Now, byte& _DayInSet, int _Address_EEPROM_DayName) {
  _DayInSet = DayOrder(GetDate(_Now.Year(), _Now.Month(), _Now.Day()));
  EEPROM.update(_Address_EEPROM_DayName, _DayInSet);
}

void ChangeTheHourValueWhen24HoursTo12Hours(byte& _Hours, bool& _AM_PM) {
  if (_Hours >= 12) {
    _Hours = _Hours - 12;
    _AM_PM = 1;
  }
  if (_Hours == 0) {
    _Hours = 12;
  }
}

void UpdateAlarmTimeOperations(sTime _Time, byte _Buzzer, bool _IsAllowAlarm, bool& _IsAllowAlarmNap, byte& _CountForNapTimes, int _CountAlarmPeep) {
  IsAllowAlarm = _IsAllowAlarm;
  if (_IsAllowAlarm) {
    if (_Time.Seconds == 59) {
      _CountForNapTimes = 0;
      _IsAllowAlarmNap = false;
    }
    
    //RingTone(_Buzzer, _CountAlarmPeep);

  } else {
    TurnOffBuzzer(_Buzzer);
  }
}

void UpdateAlarmNapTimeOperationsbtn3(sTime& _AlarmTime, sTime& _AlarmNapTime, bool& _TurnOffAlarm, bool& _IsAllowAlarm, bool& _IsAllowAlarmNap, byte& _CountForNapTimes, int _Minutes_AlarmNapTime_Period) {
  _TurnOffAlarm = true;
  _IsAllowAlarm = false;
  _IsAllowAlarmNap = true;
  _CountForNapTimes++;
  _AlarmNapTime.Minutes = _AlarmTime.Minutes + _Minutes_AlarmNapTime_Period * _CountForNapTimes;
  _AlarmNapTime.Hours = _AlarmTime.Hours;

  if (_AlarmNapTime.Minutes >= 60) {
    _AlarmNapTime.Minutes = _AlarmNapTime.Minutes - 60;
    _AlarmNapTime.Hours++;
  }
  if (_AlarmNapTime.Hours >= 24) {
    _AlarmNapTime.Hours = 0;
  }

  if (_CountForNapTimes == 13) {
    _CountForNapTimes = 0;
    _IsAllowAlarmNap = false;
  }
}

void UpdateAlarmOperationbtn4(bool& _TurnOffAlarm, bool& _IsAllowAlarm, bool& _IsAllowAlarmNap, byte& _CountForNapTimes) {

  _TurnOffAlarm = true;
  _IsAllowAlarm = false;
  _IsAllowAlarmNap = false;
  _CountForNapTimes = 0;
}

sTime GetTime(byte Hours, byte Minutes, byte Seconds) {
  sTime _Time;
  _Time.Hours = Hours;
  _Time.Minutes = Minutes;
  _Time.Seconds = Seconds;
  return _Time;
}
void SaveAlarmTimeTo_EEPROM(sTime _AlarmTime, int _Address_EEPROM_MinuteAlarm, int _Address_EEPROM_HourAlarm) {
  EEPROM.update(_Address_EEPROM_MinuteAlarm, _AlarmTime.Minutes);
  EEPROM.update(_Address_EEPROM_HourAlarm, _AlarmTime.Hours);
}


////////////////////////////////////////////////////////////////////////////////////////////////*************************//////////////////////////////////////////////////////////
void ReturnPointerForSetAlarmAndSetTimeAndDate(int& _CountPushButton_X_ForSetAlarm, int& _CountPushButton_X_ForSetTimeAndDate) {
  _CountPushButton_X_ForSetAlarm = 0;        //to reback the pointer
  _CountPushButton_X_ForSetTimeAndDate = 0;  //to reback the pointer {}
}
///////////////////////////////////////////////////TimeAndDateScreen///////////////////////////////////////////////////////////////////////////////////////////////////////////

void OperationOfShowTimeAndDateScreen() {
  
  UpdateHijriDateIn_EEPROM(HijriDate, Address_EEPROM_HijriYear_Left, Address_EEPROM_HijriYear_Right, Address_EEPROM_HijriMonth, Address_EEPROM_HijriDay);  //To Store the Hijiri Date and Gregorian Date In EEPROM and Save for longest time
  UpdateGregorianDateIn_EEPROM_Rtc_D1302(now, GregorianDate, Address_EEPROM_GregorianYear_Left, Address_EEPROM_GregorianYear_Right, Address_EEPROM_GregorianMonth, Address_EEPROM_GregorianDay);
  IsAllowAlarm = fnIsAllowAlarm(Time, AlarmTime, AlarmNapTime, PushButtonRightDown, PushButtonRightTop, PushButtonRightDown_Value, PushButtonRightTop_Value, IsAllowAlarm, IsAllowAlarmNap, TurnOffAlarm, CurrentScreen, Input_PullUp);
  UpdateAlarmTimeOperations(Time, Buzzer, IsAllowAlarm, IsAllowAlarmNap, CountForNapTimes, CountAlarmPeep);
  ReturnPointerForSetAlarmAndSetTimeAndDate(CountPushButtonRightDownForSetAlarm, CountPushButtonRightDownForSetTimeAndDate);
  AllowAlarm(Time, AlarmTime, IsAllowAlarm);
}

void ChangeValueAfterPressingTwo_btn_In_SpecificPeriod(byte _PushButtonLeftTop, byte _PushButtonLeftDown, unsigned long& StartPeriodForChangeValue, bool& IsReadStartPeriodForChangeValue, bool& IsChangedValue, bool& Value, unsigned int refPeriodForChangeValue, bool Input_PullType) {
  bool btn1_Value = digitalRead(_PushButtonLeftTop);
  bool btn2_Value = digitalRead(_PushButtonLeftDown);

  if (Input_PullType == Input_PullDown) {
    btn1_Value = !btn1_Value;
    btn2_Value = !btn2_Value;
  }

  if (!(btn1_Value == LOW && btn2_Value == LOW) && IsReadStartPeriodForChangeHoursType) {
    IsChangedValue = 0;
    IsReadStartPeriodForChangeValue = 0;
  }
  if (btn1_Value == LOW && btn2_Value == LOW && IsChangedValue == 0) {
    if (IsReadStartPeriodForChangeValue == 0) {
      StartPeriodForChangeValue = millis();
      IsReadStartPeriodForChangeValue = 1;
    }
    if ((millis() - StartPeriodForChangeValue) > refPeriodForChangeValue) {
      Value ^= 1;
      IsChangedValue = 1;
    }
  }
}

void ShowTimeOnMainScreen() {
  if ((!IsShowAlarmTime)) {

    if (HoursType == _24Hours)
    {
      ShowTimeOnLcd24Hours(ShowTimeAndDateScreenTimeColumn, ShowTimeAndDateScreenTimeRow, GetStringTime(GetTime(now.Hour(), now.Minute(), now.Second())));
    } 
    else {
      byte _Hours = now.Hour();
      bool _AM_PM = 0;
      ChangeTheHourValueWhen24HoursTo12Hours(_Hours, _AM_PM);

      if (PushButtonRightTop_Value == LOW || digitalRead(PushButtonMiddle) == LOW)
        ShowTimeOnLcd12Hours(ShowTimeAndDateScreenTimeColumn, ShowTimeAndDateScreenTimeRow, GetStringTime(GetTime(_Hours, now.Minute(), now.Second())), _AM_PM, SameLine);
      else
        ShowTimeOnLcd12Hours(ShowTimeAndDateScreenTimeColumn, ShowTimeAndDateScreenTimeRow, GetStringTime(GetTime(_Hours, now.Minute(), now.Second())), _AM_PM, NotSameLine);
    }
  }
}

void CheckAndExecuteChangeTypeOfDate_UpdateAlarmOperationbtn4() {
  if (EventAfterPressingPushButtonAndResetValue(PushButtonRightTop, PushButtonRightTop_Value, Previous_PushButtonRightTop_Value, CountPushButtonRightTopForDate, Input_PullUp)) {
    ChangeTypeOfDate = ChangeTypeOfDate ^ 1;
    if (IsAllowAlarm) {
      UpdateAlarmOperationbtn4(TurnOffAlarm, IsAllowAlarm, IsAllowAlarmNap, CountForNapTimes);
    }
    // CountPushButtonLeftDownForDate = 0;
  }
}

void CheckAndExecuteUpdateAlarmNapTimeOperationsbtn3() {
  if (EventAfterPressingPushButtonAndResetValue(PushButtonRightDown, PushButtonRightDown_Value, Previous_PushButtonRightDown_Value, CountPushButtonRightDownForTurnOffAlarm, Input_PullUp) && IsAllowAlarm) {
    UpdateAlarmNapTimeOperationsbtn3(AlarmTime, AlarmNapTime, TurnOffAlarm, IsAllowAlarm, IsAllowAlarmNap, CountForNapTimes, Minutes_AlarmNapTime_Period);
  }
}

void CheckAndExecuteExchangeAndShowDateOnLcd(bool& _IsBlankRow1) {
  if (EventWhilePressingPushButton(PushButtonRightTop, PushButtonRightTop_Value, Input_PullUp) && !EnterShowDay && digitalRead(PushButtonRightDown) == 1) {

    ExchangeAndShowDateOnLcd(ShowTimeAndDateScreenDateColumn, ShowTimeAndDateScreenDateRow, PushButtonRightTop, PushButtonRightTop_Value, GetStringGregorianDate(GetDate(GregorianDate.Year, GregorianDate.Month - 1, GregorianDate.Day)), GetStringHijriDate(GetDate(HijriDate.Year, HijriDate.Month, HijriDate.Day)), ChangeTypeOfDate);
    _IsBlankRow1 = 1;
    EnterShowDate = 1;
    IsBeforeEnterShowDate = true;
  } else {
    EnterShowDate = 0;
  }
}


void CheckAndExecuteShowAlarmTimeOnLcd(bool _IsShowDay) {
  IsShowAlarmTime = false;
  if (!_IsShowDay) {
    if (EventWhilePressingPushButton(PushButtonRightDown, PushButtonRightDown_Value, Input_PullUp)) {
      IsShowAlarmTime = true;
      if (HoursType == _24Hours)
        ShowTimeOnLcd24Hours(ShowTimeAndDateScreenAlarmTimeColumn,  ShowTimeAndDateScreenAlarmTimeRow, GetStringTime(GetTime(AlarmTime.Hours, AlarmTime.Minutes, 0)));
      else {
        byte _Hours_Alarm = AlarmTime.Hours;
        bool _AM_PM = 0;
        if (_Hours_Alarm >= 12) {
          _Hours_Alarm = _Hours_Alarm - 12;
          _AM_PM = 1;
        }
        if (_Hours_Alarm == 0) {
          _Hours_Alarm = 12;
        }
        ShowTimeOnLcd12Hours(ShowTimeAndDateScreenAlarmTimeColumn,  ShowTimeAndDateScreenAlarmTimeRow, GetStringTime(GetTime(_Hours_Alarm, AlarmTime.Minutes, 0)), _AM_PM, NotSameLine);
      }
    }
  }
}


void CheckAndExecuteShowDayNameOnLcd(bool& _IsBlankRow1) {
 IsShowDay = false;
  if (!IsShowAlarmTime) {
    if (EventWhilePressingPushButton(PushButtonMiddle, PushButtonMiddle_Value, Input_PullUp) && !EnterShowDate) {
      ShowDayOnLcd(ShowTimeAndDateScreenDayColumn,  ShowTimeAndDateScreenDayRow, GetDayName(DayInSet));
      IsShowDay = true;
      _IsBlankRow1 = 1;
      EnterShowDay = 1;
      IsBeforeEnterShowDay = true;

    } else {
      EnterShowDay = 0;
    }
  }
}
void CheckAndExecuteClearRow1_OnLcd(bool& _IsBlankRow1) {
  if (!_IsBlankRow1 && HoursType == _24Hours)
    ClearRow(Time12Hours_AM_PM_Row, 20, true);
}

//////////////////////////////////////////////////////////////////////////////////////////StopWatchScreen///////////////////////////////////////////////////////////////////////////

void OperationOfShowStopWatchScreen() {
  IsAllowAlarm = fnIsAllowAlarm(Time, AlarmTime, AlarmNapTime, PushButtonRightDown, PushButtonRightTop, PushButtonRightDown_Value, PushButtonRightTop_Value, IsAllowAlarm, IsAllowAlarmNap, TurnOffAlarm, CurrentScreen, Input_PullUp);
  UpdateAlarmTimeOperations(Time, Buzzer, IsAllowAlarm, IsAllowAlarmNap, CountForNapTimes, CountAlarmPeep);
  AllowAlarm(Time, AlarmTime, IsAllowAlarm);
}

void CheckAndExecute_Start_Or_Countinue_StopWatch() {
  if (EventAfterPressingPushButtonAndResetValue(PushButtonRightTop, PushButtonRightTop_Value, Previous_PushButtonRightTop_Value, CountPushButtonRightTopForStopWatch_Peeb, Input_PullUp)) {
    TurnOnBuzzer(Buzzer, 3000);
    TurnOffBuzzer(Buzzer);
    //  CountPushButtonLeftDownForStopWatch_Peeb =0;
    // IsAllowCountStopWatch ^= 1;
    IsLastButton4StopWatch = 1;
    Button3AsButton4StopWatch = 0;
    if (IsAllowAlarm) {
      UpdateAlarmOperationbtn4(TurnOffAlarm, IsAllowAlarm, IsAllowAlarmNap, CountForNapTimes);
    }
  }
}

void CheckAndExecute_Countinue_Or_Restart_StopWatch() {
  if (EventAfterPressingPushButtonAndResetValue(PushButtonRightDown, PushButtonRightDown_Value, Previous_PushButtonRightDown_Value, CountPushButtonRightDownForStopWatch, Input_PullUp)) {
    if (IsAllowAlarm)
      UpdateAlarmNapTimeOperationsbtn3(AlarmTime, AlarmNapTime, TurnOffAlarm, IsAllowAlarm, IsAllowAlarmNap, CountForNapTimes, Minutes_AlarmNapTime_Period);

    if ((IsAllowCountStopWatch == 1 || Button3AsButton4StopWatch == 1)) {
     TurnOnBuzzer(Buzzer, 3000);
      TurnOffBuzzer(Buzzer);
      //  CountPushButtonLeftDownForStopWatch_Peeb =0;
      IsAllowCountStopWatch ^= 1;
      IsLastButton4StopWatch = 0;
      Button3AsButton4StopWatch = 1;
    } else if (IsLastButton4StopWatch == 1 && IsAllowCountStopWatch == 0 && Button3AsButton4StopWatch == 0) {
      TurnOnBuzzer(Buzzer, 3000);
      TurnOffBuzzer(Buzzer);
      IsAllowCountStopWatch = 0;
      StopWatchTime.Hours        = 0;
      StopWatchTime.Minutes      = 0;
      StopWatchTime.Seconds      = 0;
      StopWatchTime.MilliSeconds = 0;
      // Previous_PushButtonLeftDown_Value =0;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////SetAlarmScreen/////////////////////////////////////////////////////////////////////////////
void OperationOfShowSetAlarmScreen() {
  PreviousSetDigit = CurrentSetDigit;
  CurrentSetDigit = EventAfterPressingPushButton(PushButtonRightDown, PushButtonRightDown_Value, Previous_PushButtonRightDown_Value, CountPushButtonRightDownForSetAlarm, 2, Input_PullUp);
  IsAllowAlarm = fnIsAllowAlarm(Time, AlarmTime, AlarmNapTime, PushButtonRightDown, PushButtonRightTop, PushButtonRightDown_Value, PushButtonRightTop_Value, IsAllowAlarm, IsAllowAlarmNap, TurnOffAlarm, CurrentScreen, Input_PullUp);
  UpdateAlarmTimeOperations(Time, Buzzer, IsAllowAlarm, IsAllowAlarmNap, CountForNapTimes, CountAlarmPeep);

  if (PreviousSetDigit != CurrentSetDigit)
    ChangeSetDigit = 1;

  else
    ChangeSetDigit = 0;

  if (IsAllowAlarm && ChangeSetDigit)
    UpdateAlarmNapTimeOperationsbtn3(AlarmTime, AlarmNapTime, TurnOffAlarm, IsAllowAlarm, IsAllowAlarmNap, CountForNapTimes, Minutes_AlarmNapTime_Period);

  AllowAlarm(Time, AlarmTime, IsAllowAlarm);
}

void CheckAndExecuteIncreaseDigit(long long _refPeriod_EnterTime_IncreaseButton4_SetAlarm) {
  if ((EventAfterPressingPushButtonAndResetValue(PushButtonRightTop, PushButtonRightTop_Value, Previous_PushButtonRightTop_Value, CountPushButtonRightTopForSetAlarm, Input_PullUp)) || (((refPeriodChangeValue_IncreaseButton4_SetAlarm < millis() - PeriodChangeValue_IncreaseButton4_SetAlarm) && PeriodChangeValue_IncreaseButton4_SetAlarm != millis()) && (millis() - EnterTime_IncreaseButton4_SetAlarm > _refPeriod_EnterTime_IncreaseButton4_SetAlarm))) {
    if (IsAllowAlarm) {
      UpdateAlarmOperationbtn4(TurnOffAlarm, IsAllowAlarm, IsAllowAlarmNap, CountForNapTimes);
    }
    EnterTime_IncreaseButton4_SetAlarm = millis();

    if (CurrentSetDigit == 0) {
      IsAllowAlarmNap = 0;
      CountForNapTimes = 0;
      if (AlarmTime.Minutes < 59) {
        AlarmTime.Minutes++;
      } else {
        AlarmTime.Minutes = 0;
      }
      //CountPushButtonRightTopForSetAlarm = 0;
    } else if (CurrentSetDigit == 1) {
      IsAllowAlarmNap = 0;
      CountForNapTimes = 0;

      if (AlarmTime.Hours < 23) {
        AlarmTime.Hours++;
      } else {
        AlarmTime.Hours = 0;
      }
      //CountPushButtonRightTopForSetAlarm = 0;
    }
  }
}

void CheckAndExecuteDecreaseDigit(long long _refPeriod_EnterTime_DecreaseButton5_SetAlarm) {
  if ((EventAfterPressingPushButtonAndResetValue(PushButtonMiddle, PushButtonMiddle_Value, Previous_PushButtonMiddle_Value, CountPushButtonMiddleForSetAlarm, Input_PullUp)) || (((refPeriodChangeValue_DecreaseButton5_SetAlarm < millis() - PeriodChangeValue_DecreaseButton5_SetAlarm) && PeriodChangeValue_DecreaseButton5_SetAlarm != millis()) && (millis() - EnterTime_DecreaseButton5_SetAlarm > _refPeriod_EnterTime_DecreaseButton5_SetAlarm))) {
    EnterTime_DecreaseButton5_SetAlarm = millis();
    if (CurrentSetDigit == 0) {
      IsAllowAlarmNap = 0;
      CountForNapTimes = 0;
      if (AlarmTime.Minutes > 0) {

        AlarmTime.Minutes--;
      } else {
        AlarmTime.Minutes = 59;
      }
      //  CountPushButtonMiddleForSetAlarm = 0;
    }

    else if (CurrentSetDigit == 1) {
      IsAllowAlarmNap = 0;
      CountForNapTimes = 0;

      if (AlarmTime.Hours > 0) {
        AlarmTime.Hours--;
      } else {
        AlarmTime.Hours = 23;
      }
      // CountPushButtonMiddleForSetAlarm = 0;
    }
  }
}

void ShowAlarmTimeInSetAlarmScreen() {
  if (CurrentSetDigit == 0) {
    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
      if (HoursType == _24Hours)
        ShowTimeOnLcd24Hours(SetAlarmScreenAlarmTimeColumn,  SetAlarmScreenAlarmTimeRow, GetStringTime(GetTime(AlarmTime.Hours, AlarmTime.Minutes, 0)));
      else {
        byte _Hours_Alarm = AlarmTime.Hours;
        bool _AM_PM = 0;
        if (_Hours_Alarm >= 12) {
          _Hours_Alarm = _Hours_Alarm - 12;
          _AM_PM = 1;
        }
        if (_Hours_Alarm == 0) {
          _Hours_Alarm = 12;
        }
        ShowTimeOnLcd12Hours(SetAlarmScreenAlarmTimeColumn,  SetAlarmScreenAlarmTimeRow, GetStringTime(GetTime(_Hours_Alarm, AlarmTime.Minutes, 0)), _AM_PM, NotSameLine);
      }
      ////delay(5);
    } 
    else {
      WriteFullPointsOnLcd(SetAlarmScreenAlarmTimeColumn+30, SetAlarmScreenAlarmTimeRow, 2, false);
      // //delay(5);
    }

  }

  else if (CurrentSetDigit == 1) {

    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
      if (HoursType == _24Hours)
        ShowTimeOnLcd24Hours(SetAlarmScreenAlarmTimeColumn,  SetAlarmScreenAlarmTimeRow, GetStringTime(GetTime(AlarmTime.Hours, AlarmTime.Minutes, 0)));
      else {
        byte _Hours_Alarm = AlarmTime.Hours;
        bool _AM_PM = 0;
        if (_Hours_Alarm >= 12) {
          _Hours_Alarm = _Hours_Alarm - 12;
          _AM_PM = 1;
        }
        if (_Hours_Alarm == 0) {
          _Hours_Alarm = 12;
        }
        ShowTimeOnLcd12Hours(SetAlarmScreenAlarmTimeColumn,  SetAlarmScreenAlarmTimeRow, GetStringTime(GetTime(_Hours_Alarm, AlarmTime.Minutes, 0)), _AM_PM, NotSameLine);
      }
      //  //delay(5);
    } else {
      WriteFullPointsOnLcd(SetAlarmScreenAlarmTimeColumn, SetTimeAndDateScreenTimeRow, 2, false);
      // //delay(5);
    }
  }
}
//////////////////////////////////////////////////////////////////////-----------------------------------------------------//////////////////////////////////////////////////////
void OperationsOfExecuteProcessRepeatlyWhilePressingbtn(int PushButton, bool& PushButton_Value, long long& Period, bool& In, bool InputPull_Type) {
  PushButton_Value = digitalRead(PushButton);
  if ((PushButton_Value == 0 && !In && InputPull_Type == Input_PullUp) || (PushButton_Value == 1 && !In && InputPull_Type == Input_PullDown)) {

    Period = millis();
    In = 1;
  }
  PushButton_Value = digitalRead(PushButton);
  if ((PushButton_Value == 1 && InputPull_Type == Input_PullUp) || (PushButton_Value == 0 && InputPull_Type == Input_PullDown)) {
    Period = millis();
    In = 0;
  }
}

//////////////////////////////////////////////////////////////////////-----------------------------------------------------//////////////////////////////////////////////////////

void ShowTimeOrDateInSetTimeAndDateScreen(byte _CurrentSetDigit) {
  if (_CurrentSetDigit == SetSeconds) {

    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
      if (HoursType == _24Hours)
        ShowTimeOnLcd24Hours(SetTimeAndDateScreenTimeColumn,  SetTimeAndDateScreenTimeRow, GetStringTime(GetTime(now.Hour(), now.Minute(), now.Second())));
      else {
        byte _Hours = now.Hour();
        bool _AM_PM = 0;
        ChangeTheHourValueWhen24HoursTo12Hours(_Hours, _AM_PM);

        ShowTimeOnLcd12Hours(SetTimeAndDateScreenTimeColumn,  SetTimeAndDateScreenTimeRow,GetStringTime(GetTime(_Hours, now.Minute(), now.Second())), _AM_PM, NotSameLine);
      }

      //delay(5);
    } else {
      WriteFullPointsOnLcd(SetTimeAndDateScreenTimeColumn+60, SetTimeAndDateScreenTimeRow, 2, false);

      //delay(5);
    }



  }

  else if (_CurrentSetDigit == SetMinutes) {

    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
      if (HoursType == _24Hours)
        ShowTimeOnLcd24Hours(SetTimeAndDateScreenTimeColumn,  SetTimeAndDateScreenTimeRow, GetStringTime(GetTime(now.Hour(), now.Minute(), now.Second())));
      else {
        byte _Hours = now.Hour();
        bool _AM_PM = 0;
        ChangeTheHourValueWhen24HoursTo12Hours(_Hours, _AM_PM);

        ShowTimeOnLcd12Hours(SetTimeAndDateScreenTimeColumn,  SetTimeAndDateScreenTimeRow, GetStringTime(GetTime(_Hours, now.Minute(), now.Second())), _AM_PM, NotSameLine);
      }

      //delay(5);
    } else {
      WriteFullPointsOnLcd(SetTimeAndDateScreenTimeColumn+30, SetTimeAndDateScreenTimeRow, 2, false);
      //delay(5);
    }

  }

  else if (_CurrentSetDigit == SetHours) {

    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
      if (HoursType == _24Hours)
        ShowTimeOnLcd24Hours(SetTimeAndDateScreenTimeColumn,  SetTimeAndDateScreenTimeRow, GetStringTime(GetTime(now.Hour(), now.Minute(), now.Second())));
      else {
        byte _Hours = now.Hour();
        bool _AM_PM = 0;
        ChangeTheHourValueWhen24HoursTo12Hours(_Hours, _AM_PM);

        ShowTimeOnLcd12Hours(SetTimeAndDateScreenTimeColumn,  SetTimeAndDateScreenTimeRow, GetStringTime(GetTime(_Hours, now.Minute(), now.Second())), _AM_PM, NotSameLine);
      }

      //delay(5);
    } else {
      WriteFullPointsOnLcd(SetTimeAndDateScreenTimeColumn, SetTimeAndDateScreenTimeRow, 2, false);



      //delay(5);
    }

  }

  else if (_CurrentSetDigit == SetGregorianDay) {
    ClearRow(Time12Hours_AM_PM_Row, 20, true);


    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
      ShowGregorianDateOnLcd(SetTimeAndDateScreenDateColumn,  SetTimeAndDateScreenDateRow, GetStringGregorianDate(GetDate(GregorianDate.Year, GregorianDate.Month - 1, GregorianDate.Day)));

      //delay(5);
    } else {
           
    WriteFullPointsOnLcd(SetTimeAndDateScreenDateColumn, SetTimeAndDateScreenDateRow, 2, false);



      //delay(5);
    }



  }

  else if (_CurrentSetDigit == SetGregorianMonth) {

    ClearRow(Time12Hours_AM_PM_Row, 20, true);
    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
      ShowGregorianDateOnLcd(SetTimeAndDateScreenDateColumn,  SetTimeAndDateScreenDateRow, GetStringGregorianDate(GetDate(GregorianDate.Year, GregorianDate.Month - 1, GregorianDate.Day)));

      //delay(5);
    } else {
    WriteFullPointsOnLcd(SetTimeAndDateScreenDateColumn + 30 , SetTimeAndDateScreenDateRow, 3, false);


      //delay(5);
    }


  }

  else if (_CurrentSetDigit == SetGregorianYear) {
    ClearRow(Time12Hours_AM_PM_Row, 20, true);
    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
      ShowGregorianDateOnLcd(SetTimeAndDateScreenDateColumn,  SetTimeAndDateScreenDateRow, GetStringGregorianDate(GetDate(GregorianDate.Year, GregorianDate.Month - 1, GregorianDate.Day)));

      //delay(5);
    } else {
    WriteFullPointsOnLcd(SetTimeAndDateScreenDateColumn + 70 , SetTimeAndDateScreenDateRow, 4, false);




      //delay(5);
    }



  }

  else if (_CurrentSetDigit == SetHijriDay) {
    ClearRow(Time12Hours_AM_PM_Row, 20, true);
    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
      ShowHijriDateOnLcd(SetTimeAndDateScreenDateColumn,  SetTimeAndDateScreenDateRow, GetStringHijriDate(GetDate(HijriDate.Year, HijriDate.Month, HijriDate.Day)));


      //delay(5);
    } else {
         WriteFullPointsOnLcd(SetTimeAndDateScreenDateColumn, SetTimeAndDateScreenDateRow, 2, false);




      //delay(5);
    }



  }

  else if (_CurrentSetDigit == SetHijriMonth) {
    ClearRow(Time12Hours_AM_PM_Row, 20, true);
    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
      ShowHijriDateOnLcd(SetTimeAndDateScreenDateColumn,  SetTimeAndDateScreenDateRow, GetStringHijriDate(GetDate(HijriDate.Year, HijriDate.Month, HijriDate.Day)));


      //delay(5);
    } else {
          WriteFullPointsOnLcd(SetTimeAndDateScreenDateColumn + 30 , SetTimeAndDateScreenDateRow, 3, false);



      //delay(5);
    }


  }

  else if (_CurrentSetDigit == SetHijriYear) {

    ClearRow(Time12Hours_AM_PM_Row, 20, true);
    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
      ShowHijriDateOnLcd(SetTimeAndDateScreenDateColumn,  SetTimeAndDateScreenDateRow, GetStringHijriDate(GetDate(HijriDate.Year, HijriDate.Month, HijriDate.Day)));


      //delay(5);
    } else {
          WriteFullPointsOnLcd(SetTimeAndDateScreenDateColumn + 70 , SetTimeAndDateScreenDateRow, 4, false);



      //delay(5);
    }




  }

  else if (_CurrentSetDigit == SetDayName) {
    // ClearRow(Time12Hours_AM_PM_Row, 20, true);
    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
      ShowDayOnLcd(SetTimeAndDateScreenDayColumn,  SetTimeAndDateScreenDayRow, GetDayName(DayInSet));


      //delay(5);
    } else {
    WriteFullPointsOnLcd(SetTimeAndDateScreenDayColumn, SetTimeAndDateScreenDayRow, 3, false);


      //delay(5);
    }
  }
}

void CheckAndExecuteDecreaseDigitInSetTimeAndDateScreen(byte _CurrentSetDigit) {
  if ((EventAfterPressingPushButtonAndResetValue(PushButtonMiddle, PushButtonMiddle_Value, Previous_PushButtonMiddle_Value, CountPushButtonMiddleForSetTimeAndDate, Input_PullUp)) || (((refPeriodChangeValue_DecreaseButton5_SetDateAndTime < millis() - PeriodChangeValue_DecreaseButton5_SetDateTime) && PeriodChangeValue_DecreaseButton5_SetDateTime != millis()) && (millis() - EnterTime_DecreaseButton5_SetDateTime > refPeriod_EnterTime_DecreaseButton5_SetDateAndTime))) {
    EnterTime_DecreaseButton5_SetDateTime = millis();
    if (_CurrentSetDigit == SetSeconds) {
      Time.Seconds = 0;

      //CountPushButtonMiddleForSetTimeAndDate = 0;
      Rtc.SetDateTime(RtcDateTime(now.Year(), now.Month(), now.Day(), now.Hour(), now.Minute(), Time.Seconds));

      if (HoursType == _24Hours)
        ShowTimeOnLcd24Hours(SetTimeAndDateScreenTimeColumn,  SetTimeAndDateScreenTimeRow, GetStringTime(GetTime(now.Hour(), now.Minute(), now.Second())));
      else {
        byte _Hours = now.Hour();
        bool _AM_PM = 0;
        ChangeTheHourValueWhen24HoursTo12Hours(_Hours, _AM_PM);

        ShowTimeOnLcd12Hours(SetTimeAndDateScreenTimeColumn,  SetTimeAndDateScreenTimeRow, GetStringTime(GetTime(_Hours, now.Minute(), now.Second())), _AM_PM, NotSameLine);
      }
    } else if (_CurrentSetDigit == SetMinutes) {
      if (Time.Minutes > 0) {

        Time.Minutes--;
      } else {
        Time.Minutes = 59;
      }
      //CountPushButtonMiddleForSetTimeAndDate = 0;
      Rtc.SetDateTime(RtcDateTime(now.Year(), now.Month(), now.Day(), now.Hour(), Time.Minutes, now.Second()));

      if (HoursType == _24Hours)
         ShowTimeOnLcd24Hours(SetTimeAndDateScreenTimeColumn,  SetTimeAndDateScreenTimeRow,  GetStringTime(GetTime(now.Hour(), now.Minute(), now.Second())));
      else {
        byte _Hours = now.Hour();
        bool _AM_PM = 0;
        ChangeTheHourValueWhen24HoursTo12Hours(_Hours, _AM_PM);

         ShowTimeOnLcd12Hours(SetTimeAndDateScreenTimeColumn,  SetTimeAndDateScreenTimeRow,  GetStringTime(GetTime(_Hours, now.Minute(), now.Second())), _AM_PM, NotSameLine);
      }
    }

    else if (_CurrentSetDigit == SetHours) {

      if (Time.Hours > 0) {
        Time.Hours--;
      } else {
        Time.Hours = 23;
      }
      //CountPushButtonMiddleForSetTimeAndDate = 0;
      Rtc.SetDateTime(RtcDateTime(now.Year(), now.Month(), now.Day(), Time.Hours, now.Minute(), now.Second()));

      if (HoursType == _24Hours)
         ShowTimeOnLcd24Hours(SetTimeAndDateScreenTimeColumn,  SetTimeAndDateScreenTimeRow,  GetStringTime(GetTime(now.Hour(), now.Minute(), now.Second())));
      else {
        byte _Hours = now.Hour();
        bool _AM_PM = 0;
        ChangeTheHourValueWhen24HoursTo12Hours(_Hours, _AM_PM);

         ShowTimeOnLcd12Hours(SetTimeAndDateScreenTimeColumn,  SetTimeAndDateScreenTimeRow,  GetStringTime(GetTime(_Hours, now.Minute(), now.Second())), _AM_PM, NotSameLine);
      }
    }

    else if (_CurrentSetDigit == SetGregorianDay) {

      if (GregorianDate.Day > 1) {
        GregorianDate.Day--;
      } else if (IsLeapYear(GregorianDate.Year) && GregorianDate.Month == 2 && GregorianDate.Day == 1) {
        GregorianDate.Day = 29;
      } else {
        GregorianDate.Day = NumberOfDaysInAMonth(GregorianDate.Year, GregorianDate.Month);
      }
      //CountPushButtonMiddleForSetTimeAndDate = 0;
      Rtc.SetDateTime(RtcDateTime(now.Year(), now.Month(), GregorianDate.Day, now.Hour(), now.Minute(), now.Second()));

      ShowGregorianDateOnLcd(SetTimeAndDateScreenDateColumn,  SetTimeAndDateScreenDateRow, GetStringGregorianDate(GetDate(GregorianDate.Year, GregorianDate.Month - 1, GregorianDate.Day)));
    }

    else if (_CurrentSetDigit == SetGregorianMonth) {

      if (GregorianDate.Month > 1) {
        GregorianDate.Month--;
      } else {
        GregorianDate.Month = 12;
      }
      if (GregorianDate.Day > NumberOfDaysInAMonth(GregorianDate.Year, GregorianDate.Month)) {
        GregorianDate.Day = NumberOfDaysInAMonth(GregorianDate.Year, GregorianDate.Month);
        if (IsLeapYear(GregorianDate.Year) && GregorianDate.Month == 2)
          GregorianDate.Day = 29;
      }
      //CountPushButtonMiddleForSetTimeAndDate = 0;
      Rtc.SetDateTime(RtcDateTime(now.Year(), GregorianDate.Month, GregorianDate.Day, now.Hour(), now.Minute(), now.Second()));

      ShowGregorianDateOnLcd(SetTimeAndDateScreenDateColumn,  SetTimeAndDateScreenDateRow, GetStringGregorianDate(GetDate(GregorianDate.Year, GregorianDate.Month - 1, GregorianDate.Day)));

    } else if (_CurrentSetDigit == SetGregorianYear) {

      GregorianDate.Year--;
      if (GregorianDate.Day > NumberOfDaysInAMonth(GregorianDate.Year, GregorianDate.Month)) {
        GregorianDate.Day = NumberOfDaysInAMonth(GregorianDate.Year, GregorianDate.Month);
        if (IsLeapYear(GregorianDate.Year) && GregorianDate.Month == 2)
          GregorianDate.Day = 29;
      }
      //CountPushButtonMiddleForSetTimeAndDate = 0;
      Rtc.SetDateTime(RtcDateTime(GregorianDate.Year, now.Month(), GregorianDate.Day, now.Hour(), now.Minute(), now.Second()));

      ShowGregorianDateOnLcd(SetTimeAndDateScreenDateColumn,  SetTimeAndDateScreenDateRow, GetStringGregorianDate(GetDate(GregorianDate.Year, GregorianDate.Month - 1, GregorianDate.Day)));

    }

    else if (_CurrentSetDigit == SetHijriDay) {

      if (HijriDate.Day > 1) {
        HijriDate.Day--;
      } else {
        HijriDate.Day = 30;
      }
      //CountPushButtonMiddleForSetTimeAndDate = 0;

      ShowHijriDateOnLcd(SetTimeAndDateScreenDateColumn,  SetTimeAndDateScreenDateRow, GetStringHijriDate(GetDate(HijriDate.Year, HijriDate.Month, HijriDate.Day)));

    }

    else if (_CurrentSetDigit == SetHijriMonth) {

      if (HijriDate.Month > 0) {
        HijriDate.Month--;
      } else {
        HijriDate.Month = 11;
      }
      ShowHijriDateOnLcd(SetTimeAndDateScreenDateColumn,  SetTimeAndDateScreenDateRow, GetStringHijriDate(GetDate(HijriDate.Year, HijriDate.Month, HijriDate.Day)));
      //CountPushButtonMiddleForSetTimeAndDate = 0;


    }

    else if (_CurrentSetDigit == SetHijriYear) {

      HijriDate.Year--;

      ShowHijriDateOnLcd(SetTimeAndDateScreenDateColumn,  SetTimeAndDateScreenDateRow, GetStringHijriDate(GetDate(HijriDate.Year, HijriDate.Month, HijriDate.Day)));
      //CountPushButtonMiddleForSetTimeAndDate = 0;



    }

    else if (_CurrentSetDigit == SetDayName) {
      if (PushButtonRightTop_Value == LOW && digitalRead(PushButtonMiddle) == LOW) {
        if (ReadStartPeriodForAutoDayOrder == 0) {
          StartPeriodForAutoDayOrder = millis();
          ReadStartPeriodForAutoDayOrder = 1;
        }
        if (millis() - StartPeriodForAutoDayOrder > refPeriodStartPeriodForAutoDayOrder) {
          AutoDayOrder(now, DayInSet, Address_EEPROM_DayName);
          EndPeriodForAutoDayOrder = millis();
        }
      } else if (millis() - EndPeriodForAutoDayOrder > refPeriodEndPeriodForAutoDayOrder) {
        if (DayInSet > 0) {
          DayInSet--;
        } else {
          DayInSet = 6;
        }
      }
      EEPROM.update(Address_EEPROM_DayName, DayInSet);

      //CountPushButtonMiddleForSetTimeAndDate = 0;
    }
  }
}

void CheckAndExecuteIncreaseDigitInSetTimeAndDateScreen(byte _CurrentSetDigit) {
  if ((EventAfterPressingPushButtonAndResetValue(PushButtonRightTop, PushButtonRightTop_Value, Previous_PushButtonRightTop_Value, CountPushButtonRightTopForSetTimeAndDate, Input_PullUp)) || (((refPeriodChangeValue_IncreaseButton4_SetDateAndTime < millis() - PeriodChangeValue_IncreaseButton4_SetDateTime) && PeriodChangeValue_IncreaseButton4_SetDateTime != millis()) && (millis() - EnterTime_IncreaseButton4_SetDateTime > refPeriod_EnterTime_IncreaseButton4_SetDateAndTime))) {
    EnterTime_IncreaseButton4_SetDateTime = millis();
    if (IsAllowAlarm) {
      UpdateAlarmOperationbtn4(TurnOffAlarm, IsAllowAlarm, IsAllowAlarmNap, CountForNapTimes);
    }
    if (_CurrentSetDigit == SetSeconds) {
      Time.Seconds = 0;
      Rtc.SetDateTime(RtcDateTime(now.Year(), now.Month(), now.Day(), now.Hour(), now.Minute(), Time.Seconds));

      //CountPushButtonRightTopForSetTimeAndDate = 0;
      if (HoursType == _24Hours)
         ShowTimeOnLcd24Hours(SetTimeAndDateScreenTimeColumn,  SetTimeAndDateScreenTimeRow,  GetStringTime(GetTime(now.Hour(), now.Minute(), now.Second())));
      else {
        byte _Hours = now.Hour();
        bool _AM_PM = 0;
        ChangeTheHourValueWhen24HoursTo12Hours(_Hours, _AM_PM);

         ShowTimeOnLcd12Hours(SetTimeAndDateScreenTimeColumn,  SetTimeAndDateScreenTimeRow,  GetStringTime(GetTime(_Hours, now.Minute(), now.Second())), _AM_PM, NotSameLine);
      }
    } else if (_CurrentSetDigit == SetMinutes) {
      if (Time.Minutes < 59) {

        Time.Minutes++;
      } else {
        Time.Minutes = 0;
      }
      //   //CountPushButtonRightTopForSetTimeAndDate = 0;
      Rtc.SetDateTime(RtcDateTime(now.Year(), now.Month(), now.Day(), now.Hour(), Time.Minutes, now.Second()));

      if (HoursType == _24Hours)
         ShowTimeOnLcd24Hours(SetTimeAndDateScreenTimeColumn,  SetTimeAndDateScreenTimeRow,  GetStringTime(GetTime(now.Hour(), now.Minute(), now.Second())));
      else {
        byte _Hours = now.Hour();
        bool _AM_PM = 0;
        ChangeTheHourValueWhen24HoursTo12Hours(_Hours, _AM_PM);
         ShowTimeOnLcd12Hours(SetTimeAndDateScreenTimeColumn,  SetTimeAndDateScreenTimeRow,  GetStringTime(GetTime(_Hours, now.Minute(), now.Second())), _AM_PM, NotSameLine);
      }

    }

    else if (_CurrentSetDigit == SetHours) {

      if (Time.Hours < 23) {
        Time.Hours++;
      } else {
        Time.Hours = 0;
      }
      //              //CountPushButtonRightTopForSetTimeAndDate = 0;
      Rtc.SetDateTime(RtcDateTime(now.Year(), now.Month(), now.Day(), Time.Hours, now.Minute(), now.Second()));
      if (HoursType == _24Hours)
         ShowTimeOnLcd24Hours(SetTimeAndDateScreenTimeColumn,  SetTimeAndDateScreenTimeRow,  GetStringTime(GetTime(now.Hour(), now.Minute(), now.Second())));
      else {
        byte _Hours = now.Hour();
        bool _AM_PM = 0;
        ChangeTheHourValueWhen24HoursTo12Hours(_Hours, _AM_PM);

         ShowTimeOnLcd12Hours(SetTimeAndDateScreenTimeColumn,  SetTimeAndDateScreenTimeRow,  GetStringTime(GetTime(_Hours, now.Minute(), now.Second())), _AM_PM, NotSameLine);
      }
    }

    else if (_CurrentSetDigit == SetGregorianDay) {
      if (IsLeapYear(GregorianDate.Year) && GregorianDate.Month == 2 && GregorianDate.Day < 29) {
        GregorianDate.Day++;
      } else if (GregorianDate.Day < NumberOfDaysInAMonth(GregorianDate.Year, GregorianDate.Month)) {
        GregorianDate.Day++;
      } else {
        GregorianDate.Day = 1;
      }
      //CountPushButtonRightTopForSetTimeAndDate = 0;
      Rtc.SetDateTime(RtcDateTime(now.Year(), now.Month(), GregorianDate.Day, now.Hour(), now.Minute(), now.Second()));


      ShowGregorianDateOnLcd(SetTimeAndDateScreenDateColumn,  SetTimeAndDateScreenDateRow, GetStringGregorianDate(GetDate(GregorianDate.Year, GregorianDate.Month - 1, GregorianDate.Day)));

    }

    else if (_CurrentSetDigit == SetGregorianMonth) {

      if (GregorianDate.Month < 12) {
        GregorianDate.Month++;
      } else {
        GregorianDate.Month = 1;
      }
      if (GregorianDate.Day > NumberOfDaysInAMonth(GregorianDate.Year, GregorianDate.Month)) {
        GregorianDate.Day = NumberOfDaysInAMonth(GregorianDate.Year, GregorianDate.Month);
        if (IsLeapYear(GregorianDate.Year) && GregorianDate.Month == 2)
          GregorianDate.Day = 29;
      }
      //CountPushButtonRightTopForSetTimeAndDate = 0;
      Rtc.SetDateTime(RtcDateTime(now.Year(), GregorianDate.Month, GregorianDate.Day, now.Hour(), now.Minute(), now.Second()));

      ShowGregorianDateOnLcd(SetTimeAndDateScreenDateColumn,  SetTimeAndDateScreenDateRow, GetStringGregorianDate(GetDate(GregorianDate.Year, GregorianDate.Month - 1, GregorianDate.Day)));

    } else if (_CurrentSetDigit == SetGregorianYear) {


      GregorianDate.Year++;
      if (GregorianDate.Day > NumberOfDaysInAMonth(GregorianDate.Year, GregorianDate.Month)) {
        GregorianDate.Day = NumberOfDaysInAMonth(GregorianDate.Year, GregorianDate.Month);
        if (IsLeapYear(GregorianDate.Year) && GregorianDate.Month == 2)
          GregorianDate.Day = 29;
      }
      //CountPushButtonRightTopForSetTimeAndDate = 0;
      Rtc.SetDateTime(RtcDateTime(GregorianDate.Year, now.Month(), GregorianDate.Day, now.Hour(), now.Minute(), now.Second()));
      ShowGregorianDateOnLcd(SetTimeAndDateScreenDateColumn,  SetTimeAndDateScreenDateRow, GetStringGregorianDate(GetDate(GregorianDate.Year, GregorianDate.Month - 1, GregorianDate.Day)));
    }

    else if (_CurrentSetDigit == SetHijriDay) {

      if (HijriDate.Day < 30) {
        HijriDate.Day++;
      } else {
        HijriDate.Day = 1;
      }
      //CountPushButtonRightTopForSetTimeAndDate = 0;

      ShowHijriDateOnLcd(SetTimeAndDateScreenDateColumn,  SetTimeAndDateScreenDateRow, GetStringHijriDate(GetDate(HijriDate.Year, HijriDate.Month, HijriDate.Day)));

    }

    else if (_CurrentSetDigit == SetHijriMonth) {

      if (HijriDate.Month < 11) {
        HijriDate.Month++;
      } else {
        HijriDate.Month = 0;
      }
      //CountPushButtonRightTopForSetTimeAndDate = 0;

      ShowHijriDateOnLcd(SetTimeAndDateScreenDateColumn,  SetTimeAndDateScreenDateRow, GetStringHijriDate(GetDate(HijriDate.Year, HijriDate.Month, HijriDate.Day)));

    }

    else if (_CurrentSetDigit == SetHijriYear) {
      HijriDate.Year++;
      //CountPushButtonRightTopForSetTimeAndDate = 0;

      ShowHijriDateOnLcd(SetTimeAndDateScreenDateColumn,  SetTimeAndDateScreenDateRow, GetStringHijriDate(GetDate(HijriDate.Year, HijriDate.Month, HijriDate.Day)));

    }

    else if (_CurrentSetDigit == SetDayName) {
      if (PushButtonRightTop_Value == LOW && digitalRead(PushButtonMiddle) == LOW) {
        if (ReadStartPeriodForAutoDayOrder == 0) {
          StartPeriodForAutoDayOrder = millis();
          ReadStartPeriodForAutoDayOrder = 1;
        }
        if (millis() - StartPeriodForAutoDayOrder > refPeriodStartPeriodForAutoDayOrder) {
          AutoDayOrder(now, DayInSet, Address_EEPROM_DayName);
          EndPeriodForAutoDayOrder = millis();
        }
      } else if (millis() - EndPeriodForAutoDayOrder > refPeriodEndPeriodForAutoDayOrder) {
        if (DayInSet < 6) {
          DayInSet++;

        } else {
          DayInSet = 0;
        }
      }
      EEPROM.update(Address_EEPROM_DayName, DayInSet);
      //CountPushButtonRightTopForSetTimeAndDate = 0;
    }
  }
}

void OperationOfShowSetDateAndTimeScreen() {
  //To save Alarm Time to EEPROM and save (EEPROM) for longest time
  SaveAlarmTimeTo_EEPROM(AlarmTime, Address_EEPROM_MinuteAlarm, Address_EEPROM_HourAlarm);


  PreviousSetDigit = CurrentSetDigit;
  CurrentSetDigit = EventAfterPressingPushButton(PushButtonRightDown, PushButtonRightDown_Value, Previous_PushButtonRightDown_Value, CountPushButtonRightDownForSetTimeAndDate, 10, Input_PullUp);

  IsAllowAlarm = fnIsAllowAlarm(Time, AlarmTime, AlarmNapTime, PushButtonRightDown, PushButtonRightTop, PushButtonRightDown_Value, PushButtonRightTop_Value, IsAllowAlarm, IsAllowAlarmNap, TurnOffAlarm, CurrentScreen, Input_PullUp);
  UpdateAlarmTimeOperations(Time, Buzzer, IsAllowAlarm, IsAllowAlarmNap, CountForNapTimes, CountAlarmPeep);

  if (PreviousSetDigit != CurrentSetDigit)
    ChangeSetDigit = 1;

  else
    ChangeSetDigit = 0;

  if (IsAllowAlarm && ChangeSetDigit)
    UpdateAlarmNapTimeOperationsbtn3(AlarmTime, AlarmNapTime, TurnOffAlarm, IsAllowAlarm, IsAllowAlarmNap, CountForNapTimes, Minutes_AlarmNapTime_Period);




  AllowAlarm(Time, AlarmTime, IsAllowAlarm);




  if (!(PushButtonRightTop_Value == LOW && digitalRead(PushButtonMiddle) == LOW)) {
    StartPeriodForAutoDayOrder = millis();
    ReadStartPeriodForAutoDayOrder = 0;
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void ShowTimeAndDateScreen() {
  bool IsBlankRow1 = 0;
  OperationOfShowTimeAndDateScreen();
  ChangeValueAfterPressingTwo_btn_In_SpecificPeriod(PushButtonRightTop, PushButtonMiddle, StartPeriodForChangeHoursType, IsReadStartPeriodForChangeHoursType, IsChangedHoursType, HoursType, refPeriodForChangeHoursType, Input_PullType);
  ShowTimeOnMainScreen();
  CheckAndExecuteChangeTypeOfDate_UpdateAlarmOperationbtn4();
  CheckAndExecuteUpdateAlarmNapTimeOperationsbtn3();
   CheckAndExecuteExchangeAndShowDateOnLcd(IsBlankRow1);
  CheckAndExecuteShowAlarmTimeOnLcd(IsShowDay);
   CheckAndExecuteShowDayNameOnLcd(IsBlankRow1);
   CheckAndExecuteClearRow1_OnLcd(IsBlankRow1);


  if (MoveScreen(CurrentScreen)) {
    PreviousSetDigit = 0;
    CurrentSetDigit = 0;
    return;
  }
}

void WriteMilliSecondOnLcd(short _StopWatchTime_MilliSeconds)
{ 

  lcd.setCursor( MilliSecondColumn, MilliSecondRow );

if(_StopWatchTime_MilliSeconds < 10) 
 lcd.print("00"); 
 else if(_StopWatchTime_MilliSeconds < 100) 
  lcd.print("0");

lcd.print(_StopWatchTime_MilliSeconds);

lcd.sendBuffer();

}


void ShowStopWatchScreen() {

  OperationOfShowStopWatchScreen();
  CheckAndExecute_Start_Or_Countinue_StopWatch();
  WriteMilliSecondOnLcd(StopWatchTime.MilliSeconds);
  CheckAndExecute_Countinue_Or_Restart_StopWatch();
   ShowTimeOnLcd24Hours(SetTimeAndDateScreenTimeColumn,  SetTimeAndDateScreenTimeRow,  GetStringTime(GetTime(StopWatchTime.Hours, StopWatchTime.Minutes, StopWatchTime.Seconds)));
}

void ShowSetAlarmScreen() {

  OperationOfShowSetAlarmScreen();
  OperationsOfExecuteProcessRepeatlyWhilePressingbtn(PushButtonRightTop, PushButtonRightTop_Value, PeriodChangeValue_IncreaseButton4_SetAlarm, In_IncreaseButton4_SetAlarm, Input_PullUp);
  CheckAndExecuteIncreaseDigit(refPeriod_EnterTime_IncreaseButton4_SetAlarm);
  OperationsOfExecuteProcessRepeatlyWhilePressingbtn(PushButtonMiddle, PushButtonMiddle_Value, PeriodChangeValue_DecreaseButton5_SetAlarm, In_DecreaseButton5_SetAlarm, Input_PullUp);
  CheckAndExecuteDecreaseDigit(refPeriod_EnterTime_DecreaseButton5_SetAlarm);
  ShowAlarmTimeInSetAlarmScreen();
}

void ShowSetDateAndTimeScreen() {

  OperationOfShowSetDateAndTimeScreen();
  OperationsOfExecuteProcessRepeatlyWhilePressingbtn(PushButtonRightTop, PushButtonRightTop_Value, PeriodChangeValue_IncreaseButton4_SetDateTime, In_IncreaseButton4_SetDateTime, Input_PullUp);
  CheckAndExecuteIncreaseDigitInSetTimeAndDateScreen(CurrentSetDigit);
  OperationsOfExecuteProcessRepeatlyWhilePressingbtn(PushButtonMiddle, PushButtonMiddle_Value, PeriodChangeValue_DecreaseButton5_SetDateTime, In_DecreaseButton5_SetDateTime, Input_PullUp);
  CheckAndExecuteDecreaseDigitInSetTimeAndDateScreen(CurrentSetDigit);
  ShowTimeOrDateInSetTimeAndDateScreen(CurrentSetDigit);
}


void ShowScreen(RtcDateTime& _Now, byte _CurrentScreen) {

  if (_CurrentScreen == 0) {
     ShowTimeAndDateScreen();
 //   ShowSetDateAndTimeScreen();
  }
  if (_CurrentScreen == 1) {
    ShowStopWatchScreen();
  }
 if (_CurrentScreen == 2) {
    ShowSetAlarmScreen();
  }

  if (_CurrentScreen == 3) {
    ShowSetDateAndTimeScreen();
  }
  CheckLight();


  _Now = Rtc.GetDateTime();
  UpdateTimeAndDate(Time, _Now.Hour(), _Now.Minute(), _Now.Second(), GregorianDate, _Now.Year(), _Now.Month(), _Now.Day());
  UpdateHijriDateAndDayName(_Now, HijriDate, DayInSet, IsIncreaseHijriDateAndDayName, Address_EEPROM_HijriYear_Left, Address_EEPROM_HijriYear_Right, Address_EEPROM_HijriMonth, Address_EEPROM_HijriDay, Address_EEPROM_DayName);
  if (MoveScreen(CurrentScreen)) {
    PreviousSetDigit = 0;
    CurrentSetDigit = 0;
    EnterStopWatchFirst = true;
    ClearRow(45,20,false);
    ClearRow(25,20,false);

    return;
  }
}


void GetValueOfPushButtonRightTop()
{
   
    if(digitalRead(PushButtonRightTop))
    PushButtonRightTop_Value = HIGH;
   else
    PushButtonRightTop_Value = LOW;

}


void InterruptOperationsForPushButtonRightTopInStopWatch()
{
  static long long time = 0;
 if( millis() - time <10)
  return;
  static int ChangeValueForIsAllowCountStopWatch = 0; 
  //IsAllowCountStopWatch ^= 1;
  if(CurrentScreen==1 &&  ChangeValueForIsAllowCountStopWatch == 0)
  {
    //High To Low
    IsAllowCountStopWatch ^= 1;
    ChangeValueForIsAllowCountStopWatch = 1;
  }

  else  if(CurrentScreen==1 && ChangeValueForIsAllowCountStopWatch == 1)
  {
   //Low To High
    ChangeValueForIsAllowCountStopWatch = 0;
  }
 GetValueOfPushButtonRightTop();
    time = millis();
}


void setup() {
   lcd.begin();
  lcd_prepare();

  pinMode(PushButtonRightDown, INPUT_PULLUP);
  pinMode(PushButtonRightTop, INPUT_PULLUP);
  pinMode(PushButtonMiddle, INPUT_PULLUP);
  pinMode(PushButtonLeftTop, INPUT_PULLUP);
  pinMode(PushButtonLeftDown, INPUT_PULLUP);

 // pinMode(Buzzer, OUTPUT);
  Rtc.Begin();
 // Serial.begin(9600);


  //pinMode(InterruptPin, INPUT_PULLUP);
   attachInterrupt(digitalPinToInterrupt(InterruptPin), InterruptOperationsForPushButtonRightTopInStopWatch, CHANGE );
/*
    EEPROM.update(Address_EEPROM_HourAlarm,10);
  EEPROM.update(Address_EEPROM_MinuteAlarm,10);
*/
  AlarmTime.Hours = EEPROM.read(Address_EEPROM_HourAlarm);
  AlarmTime.Minutes = EEPROM.read(Address_EEPROM_MinuteAlarm);


  
 HijriDate.Day = EEPROM.read(Address_EEPROM_HijriDay);  //
  HijriDate.Month = EEPROM.read(Address_EEPROM_HijriMonth);
  HijriDate.Year = Read2ByteEEPROM(Address_EEPROM_HijriYear_Left, Address_EEPROM_HijriYear_Right);


  //   HijriDate.Day = 16;
  // HijriDate.Month = 11;
  // HijriDate.Year = 1445;
  
  /*
  HijriDate.Day =  EEPROM.read(Address_EEPROM_GregorianDay);  //
   HijriDate.Month = EEPROM.read(Address_EEPROM_GregorianMonth);
   HijriDate.Year = Read2ByteEEPROM(Address_EEPROM_GregorianYear_Left, Address_EEPROM_GregorianYear_Right);

*/

  //  GregorianDate.Day = 5;
  // GregorianDate.Month =3;
  // GregorianDate.Year = 2005;
  
    GregorianDate.Day = EEPROM.read(Address_EEPROM_GregorianDay);  //
  GregorianDate.Month = EEPROM.read(Address_EEPROM_GregorianMonth);
  GregorianDate.Year = Read2ByteEEPROM(Address_EEPROM_GregorianYear_Left, Address_EEPROM_GregorianYear_Right);


  
  UpdateHijriDateAndDayNameFrom_EEPROM(GetDate(now.Year(), now.Month(), now.Day()), GregorianDate, HijriDate, DayInSet, Address_EEPROM_HijriYear_Left, Address_EEPROM_HijriYear_Right, Address_EEPROM_HijriMonth, Address_EEPROM_HijriDay, Address_EEPROM_DayName);
  Timer1.initialize(1000);
  Timer1.attachInterrupt(IncreaseCountersValuesByTimer1);
 



  //RtcDateTime currentTime = RtcDateTime(__DATE__ , __TIME__);
   //Rtc.SetDateTime(currentTime);


 // Rtc.SetDateTime(RtcDateTime("2010 1 15","15:05:02"));
}



void loop() {
 
 
 ShowScreen(now, CurrentScreen);
 
}

