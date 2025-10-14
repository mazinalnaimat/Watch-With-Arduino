#include <Arduino.h>
#include <Wire.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <TimerOne.h>
#include <avr/interrupt.h>
#include <MyData.h>


byte PushButton1 = 2;//the left up PushButton
byte PushButton2 = 3;  //the left down PushButton
#define PushButton3  4   //the right down PushButton
#define PushButton4 5  //the right Up PushButton
#define PushButton5 6  //the middle PushButton

#define InterruptPin 19

#define Buzzer 11

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

bool Previous_PushButton1_Value = 0;  //
bool Previous_PushButton2_Value = 0;  //
bool Previous_PushButton3_Value = 0;  //
bool Previous_PushButton4_Value = 0;  //
bool Previous_PushButton5_Value = 0;  //

bool PushButton1_Value = 0;  //
bool PushButton2_Value = 0;  //
bool PushButton3_Value = 0;  //
bool PushButton4_Value = 0;  //
bool PushButton5_Value = 0;  //

byte DayInSet = EEPROM.read(Address_EEPROM_DayName);  //

sDate HijriDate;
sDate GregorianDate;

byte CountForNapTimes = 0;  //

bool IsAllowCountStopWatch = 0;  //

int CountPushButton4ForDate = 0;              //
int CountPushButton4ForSetAlarm = 0;          //
int CountPushButton4ForTurnOffAlarm = 0;      //
int CountPushButton4ForSetTimeAndDate = 0;    //
int CountPushButton4ForStopWatch_Peeb = 0;    //
int CountPushButton4ForTurnAlarmOnClick = 0;  //

int CountPushButton2ForChangeScreen = 0;  //

int CountPushButton3ForStopWatch = 0;         //
int CountPushButton3ForSetAlarm = 0;          //
int CountPushButton3ForSetTimeAndDate = 0;    //
int CountPushButton3ForTurnOffAlarm = 0;      //
int CountPushButton3ForTurnAlarmOnClick = 0;  //

int CountPushButton5ForSetAlarm = 0;        //
int CountPushButton5ForSetTimeAndDate = 0;  //

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
LiquidCrystal_I2C lcd(0x27, 16, 2);

ThreeWire myWire(9, 10, 8);        // DAT, CLK, RST
RtcDS1302<ThreeWire> Rtc(myWire);  // RTC Object

RtcDateTime now = Rtc.GetDateTime();
//RtcDateTime now;



void CheckLight() {
  if (digitalRead(PushButton1)) {
    PushButton1_Value = 1;
  } else {
    PushButton1_Value = 0;
  }
  if (PushButton1_Value == 0 ) {
    StartPeriodForTurnLight = millis();
  }

  if (millis() - StartPeriodForTurnLight < 5000) {
    pinMode(7, OUTPUT);
    digitalWrite(7, HIGH);
  }

  else {
    pinMode(7, INPUT);
  }
}

void WriteFullPointsOnLcd(byte Culomn, byte Row, byte Number) {
  byte FullPoints1[8] = {
    B11111,
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

void TurnOnBuzzer(int _Buzzer, int Frequency) {
  tone(_Buzzer, Frequency);
}

void TurnOffBuzzer(int _Buzzer) {
  noTone(_Buzzer);
}

bool fnIsAllowAlarm(sTime _Time, sTime _AlarmTime, sTime _AlarmNapTime, byte _PushButton3, byte _PushButton4, bool& _PushButton3_Value, bool& _PushButton4_Value, bool _IsAllowAlarm, bool _IsAllowAlarmNap, bool& _TurnOffAlarm, byte _CurrentScreen, bool InputPull_Type) {

  bool _Is_AllowAlarm = 0;
  _PushButton3_Value = digitalRead(_PushButton3);
  _PushButton4_Value = digitalRead(_PushButton4);

  if (_Time.Hours != _AlarmTime.Hours || _Time.Minutes != _AlarmTime.Minutes) {
    _TurnOffAlarm = false;
  }

  if ((_Time.Hours == _AlarmTime.Hours && _Time.Minutes == _AlarmTime.Minutes && _IsAllowAlarm && !_TurnOffAlarm) || (_Time.Hours == _AlarmNapTime.Hours && _Time.Minutes == _AlarmNapTime.Minutes && _IsAllowAlarmNap && !_TurnOffAlarm))
    _Is_AllowAlarm = true;

  else if (InputPull_Type == Input_PullDown) {
    if (_PushButton4_Value == HIGH && _PushButton3_Value == HIGH && _CurrentScreen == 0)
      _Is_AllowAlarm = true;

  } else if (InputPull_Type == Input_PullUp) {
    if (_PushButton4_Value != HIGH && _PushButton3_Value != HIGH && _CurrentScreen == 0)
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
int EventAfterPressingPushButton(int PinName, bool& Push_Value, bool& Previous_Push_Value, int& CountPush, int CasesNumber, bool InputPull_Type) {

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

bool EventAfterPressingPushButtonAndResetValue(int PinName, bool& Push_Value, bool& Previous_Push_Value, int& CountPush, bool InputPull_Type) {
  bool Value = EventAfterPressingPushButton(PinName, Push_Value, Previous_Push_Value, CountPush, 2, InputPull_Type);

  if (Value) {
    CountPush = 0;
  }

  return Value;
}

bool MoveScreen(byte& _CurrentScreen) {
  int PreviuosScreen = _CurrentScreen;
  _CurrentScreen = EventAfterPressingPushButton(PushButton2, PushButton2_Value, Previous_PushButton2_Value, CountPushButton2ForChangeScreen, 4, Input_PullType);
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
  Date += "  ";

  Date += GregorianMonthsSet[_Date.Month];

  Date += "  ";

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
  Date += "  ";

  Date += HijriMonthsSet[_Date.Month];

  Date += "  ";

  Date += String(_Date.Year);
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

void ShowTimeOnLcd12Hours(int Column, int Row, String Time, bool AM_PM, bool IsSameLine) {

  if (IsSameLine == 0) {
    WriteOnCleanRowOnLcd(Column, Row, Time);
    if (AM_PM == 0)
      WriteOnCleanRowOnLcd(7, Row + 1, "AM");
    else
      WriteOnCleanRowOnLcd(7, Row + 1, "PM");
  } else {
    WriteOnColumnAndRowOnLcd(Column - 2, Row, String(Time + "  "));
    if (AM_PM == 0)
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

void ShowAlarmScreenOnLcd(int Column, int Row, String AlarmTime, bool HoursType, bool AM_PM) {
  if (HoursType == _24Hours)
    ShowTimeOnLcd24Hours(Column, Row, AlarmTime);
  else {
    ShowTimeOnLcd12Hours(Column, Row, AlarmTime, AM_PM, NotSameLine);
  }
}

void ClearRow(int Row) {
  lcd.setCursor(0, Row);
  lcd.print("                ");
}

void ExchangeAndShowDateOnLcd(int Column, int Row, byte _PushButton4, bool _PushButton4_Value, String GregorianDate, String HijriDate, int Change) {
  //int NumberOfClearRow = 1;
  if (EventWhilePressingPushButton(_PushButton4, _PushButton4_Value, Input_PullType)) {
    if (Change == 0)
      ShowHijriDateOnLcd(Column, Row, HijriDate);
    else
      ShowGregorianDateOnLcd(Column, Row, GregorianDate);
  } else {
    ClearRow(1);
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
   /* else
    {
         Rtc.SetDateTime(RtcDateTime(now.Year(), now.Month(), now.Day(), now.Hour(), now.Minute(), 0));
    }*/
  }
    UpdateStopTime(Time, StopWatchTime, PreviousMilliSecond, IsAllowCountStopWatch);

}

void IncreaseCountersValuesByTimer1() {
  if(IsAllowAlarm)
  RingTone( Buzzer,  CountAlarmPeep);
  IncreaseCountAlarmPeep(CountAlarmPeep, MaxCountAlarmPeep);
  IncreaseCountFullPointsShow(CountFullPointsShow, MaxCountFullPointsShow);
  IncreaseMilliSeconds(CountMilliSeconds,MaxCountMilliSeconds);
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

  if (_Now.Hour() == 0 && _Now.Minute() == 0 && _Now.Second() == 0 && Time.MilliSeconds == 0 && _IsIncreaseHijriDateAndDayName == 0 && _Now.Day() != 0)  // Day if the RTC not connected
  {
    IncreaseHijriDateByOneDay(_HijriDate);
    IncreaseDayNameByOneDay(_DayInSet);
    _IsIncreaseHijriDateAndDayName = 1;
    UpdateHijriDateIn_EEPROM(_HijriDate, _Address_EEPROM_HijriYear_Left, _Address_EEPROM_HijriYear_Right, _Address_EEPROM_HijriMonth, _Address_EEPROM_HijriDay);
    EEPROM.update(_Address_EEPROM_DayName, _DayInSet);
  }
  if (_Now.Hour() != 0 && _Now.Minute() != 0 && _Now.Second() != 0 && Time.MilliSeconds != 0) {
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
  IsAllowAlarm = fnIsAllowAlarm(Time, AlarmTime, AlarmNapTime, PushButton3, PushButton4, PushButton3_Value, PushButton4_Value, IsAllowAlarm, IsAllowAlarmNap, TurnOffAlarm, CurrentScreen, Input_PullUp);
  UpdateAlarmTimeOperations(Time, Buzzer, IsAllowAlarm, IsAllowAlarmNap, CountForNapTimes, CountAlarmPeep);
  ReturnPointerForSetAlarmAndSetTimeAndDate(CountPushButton3ForSetAlarm, CountPushButton3ForSetTimeAndDate);
  AllowAlarm(Time, AlarmTime, IsAllowAlarm);
}

void ChangeValueAfterPressingTwo_btn_In_SpecificPeriod(byte PushButton1, byte PushButton2, unsigned long& StartPeriodForChangeValue, bool& IsReadStartPeriodForChangeValue, bool& IsChangedValue, bool& Value, unsigned int refPeriodForChangeValue, bool Input_PullType) {
  bool btn1_Value = digitalRead(PushButton1);
  bool btn2_Value = digitalRead(PushButton2);

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

    if (HoursType == _24Hours) {
      ShowTimeOnLcd24Hours(4, 0, GetStringTime(GetTime(Time.Hours, Time.Minutes, Time.Seconds)));
    } else {
      byte _Hours = now.Hour();
      bool _AM_PM = 0;
      ChangeTheHourValueWhen24HoursTo12Hours(_Hours, _AM_PM);

      if (digitalRead(PushButton4) == LOW || digitalRead(PushButton5) == LOW)
        ShowTimeOnLcd12Hours(4, 0, GetStringTime(GetTime(_Hours, now.Minute(), now.Second())), _AM_PM, SameLine);
      else
        ShowTimeOnLcd12Hours(4, 0, GetStringTime(GetTime(_Hours, now.Minute(), now.Second())), _AM_PM, NotSameLine);
    }
  }
}

void CheckAndExecuteChangeTypeOfDate_UpdateAlarmOperationbtn4() {
  if (EventAfterPressingPushButtonAndResetValue(PushButton4, PushButton4_Value, Previous_PushButton4_Value, CountPushButton4ForDate, Input_PullUp)) {
    ChangeTypeOfDate = ChangeTypeOfDate ^ 1;
    if (IsAllowAlarm) {
      UpdateAlarmOperationbtn4(TurnOffAlarm, IsAllowAlarm, IsAllowAlarmNap, CountForNapTimes);
    }
    // CountPushButton2ForDate = 0;
  }
}

void CheckAndExecuteUpdateAlarmNapTimeOperationsbtn3() {
  if (EventAfterPressingPushButtonAndResetValue(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForTurnOffAlarm, Input_PullUp) && IsAllowAlarm) {
    UpdateAlarmNapTimeOperationsbtn3(AlarmTime, AlarmNapTime, TurnOffAlarm, IsAllowAlarm, IsAllowAlarmNap, CountForNapTimes, Minutes_AlarmNapTime_Period);
  }
}

void CheckAndExecuteExchangeAndShowDateOnLcd(bool& _IsBlankRow1) {
  if (EventWhilePressingPushButton(PushButton4, PushButton4_Value, Input_PullUp) && !EnterShowDay && digitalRead(PushButton3) == 1) {

    ExchangeAndShowDateOnLcd(2, 1, PushButton4, PushButton4_Value, GetStringGregorianDate(GetDate(GregorianDate.Year, GregorianDate.Month - 1, GregorianDate.Day)), GetStringHijriDate(GetDate(HijriDate.Year, HijriDate.Month, HijriDate.Day)), ChangeTypeOfDate);
    _IsBlankRow1 = 1;
    EnterShowDate = 1;
  } else {
    EnterShowDate = 0;
  }
}


void CheckAndExecuteShowAlarmTimeOnLcd(bool _IsShowDay) {
  IsShowAlarmTime = false;
  if (!_IsShowDay) {
    if (EventWhilePressingPushButton(PushButton3, PushButton3_Value, Input_PullUp)) {
      IsShowAlarmTime = true;
      if (HoursType == _24Hours)
        ShowTimeOnLcd24Hours(4, 0, GetStringTime(GetTime(AlarmTime.Hours, AlarmTime.Minutes, 0)));
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
        ShowTimeOnLcd12Hours(4, 0, GetStringTime(GetTime(_Hours_Alarm, AlarmTime.Minutes, 0)), _AM_PM, NotSameLine);
      }
    }
  }
}


void CheckAndExecuteShowDayNameOnLcd(bool& _IsBlankRow1) {
  IsShowDay = false;
  if (!IsShowAlarmTime) {
    if (EventWhilePressingPushButton(PushButton5, PushButton5_Value, Input_PullUp) && !EnterShowDate) {
      ShowDayOnLcd(6, 1, GetDayName(DayInSet));
      IsShowDay = true;
      _IsBlankRow1 = 1;
      EnterShowDay = 1;

    } else {
      EnterShowDay = 0;
    }
  }
}
void CheckAndExecuteClearRow1OnLcd(bool& _IsBlankRow1) {
  if (!_IsBlankRow1 && HoursType == _24Hours)
    ClearRow(1);
}

//////////////////////////////////////////////////////////////////////////////////////////StopWatchScreen///////////////////////////////////////////////////////////////////////////

void OperationOfShowStopWatchScreen() {
  IsAllowAlarm = fnIsAllowAlarm(Time, AlarmTime, AlarmNapTime, PushButton3, PushButton4, PushButton3_Value, PushButton4_Value, IsAllowAlarm, IsAllowAlarmNap, TurnOffAlarm, CurrentScreen, Input_PullUp);
  UpdateAlarmTimeOperations(Time, Buzzer, IsAllowAlarm, IsAllowAlarmNap, CountForNapTimes, CountAlarmPeep);
  AllowAlarm(Time, AlarmTime, IsAllowAlarm);
}

void CheckAndExecute_Start_Or_Countinue_StopWatch() {
  if (EventAfterPressingPushButtonAndResetValue(PushButton4, PushButton4_Value, Previous_PushButton4_Value, CountPushButton4ForStopWatch_Peeb, Input_PullUp)) {
    TurnOnBuzzer(Buzzer, 3000);
    TurnOffBuzzer(Buzzer);
    //  CountPushButton2ForStopWatch_Peeb =0;
    IsAllowCountStopWatch ^= 1;
    IsLastButton4StopWatch = 1;
    Button3AsButton4StopWatch = 0;
    if (IsAllowAlarm) {
      UpdateAlarmOperationbtn4(TurnOffAlarm, IsAllowAlarm, IsAllowAlarmNap, CountForNapTimes);
    }
  }
}

void CheckAndExecute_Countinue_Or_Restart_StopWatch() {
  if (EventAfterPressingPushButtonAndResetValue(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForStopWatch, Input_PullUp)) {
    if (IsAllowAlarm)
      UpdateAlarmNapTimeOperationsbtn3(AlarmTime, AlarmNapTime, TurnOffAlarm, IsAllowAlarm, IsAllowAlarmNap, CountForNapTimes, Minutes_AlarmNapTime_Period);

    if ((IsAllowCountStopWatch == 1 || Button3AsButton4StopWatch == 1)) {
      TurnOnBuzzer(Buzzer, 3000);
      TurnOffBuzzer(Buzzer);
      //  CountPushButton2ForStopWatch_Peeb =0;
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
      // Previous_PushButton2_Value =0;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////SetAlarmScreen/////////////////////////////////////////////////////////////////////////////
void OperationOfShowSetAlarmScreen() {
  PreviousSetDigit = CurrentSetDigit;
  CurrentSetDigit = EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetAlarm, 2, Input_PullUp);
  IsAllowAlarm = fnIsAllowAlarm(Time, AlarmTime, AlarmNapTime, PushButton3, PushButton4, PushButton3_Value, PushButton4_Value, IsAllowAlarm, IsAllowAlarmNap, TurnOffAlarm, CurrentScreen, Input_PullUp);
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
  if ((EventAfterPressingPushButtonAndResetValue(PushButton4, PushButton4_Value, Previous_PushButton4_Value, CountPushButton4ForSetAlarm, Input_PullUp)) || (((refPeriodChangeValue_IncreaseButton4_SetAlarm < millis() - PeriodChangeValue_IncreaseButton4_SetAlarm) && PeriodChangeValue_IncreaseButton4_SetAlarm != millis()) && (millis() - EnterTime_IncreaseButton4_SetAlarm > _refPeriod_EnterTime_IncreaseButton4_SetAlarm))) {
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
      //CountPushButton4ForSetAlarm = 0;
    } else if (CurrentSetDigit == 1) {
      IsAllowAlarmNap = 0;
      CountForNapTimes = 0;

      if (AlarmTime.Hours < 23) {
        AlarmTime.Hours++;
      } else {
        AlarmTime.Hours = 0;
      }
      //CountPushButton4ForSetAlarm = 0;
    }
  }
}

void CheckAndExecuteDecreaseDigit(long long _refPeriod_EnterTime_DecreaseButton5_SetAlarm) {
  if ((EventAfterPressingPushButtonAndResetValue(PushButton5, PushButton5_Value, Previous_PushButton5_Value, CountPushButton5ForSetAlarm, Input_PullUp)) || (((refPeriodChangeValue_DecreaseButton5_SetAlarm < millis() - PeriodChangeValue_DecreaseButton5_SetAlarm) && PeriodChangeValue_DecreaseButton5_SetAlarm != millis()) && (millis() - EnterTime_DecreaseButton5_SetAlarm > _refPeriod_EnterTime_DecreaseButton5_SetAlarm))) {
    EnterTime_DecreaseButton5_SetAlarm = millis();
    if (CurrentSetDigit == 0) {
      IsAllowAlarmNap = 0;
      CountForNapTimes = 0;
      if (AlarmTime.Minutes > 0) {

        AlarmTime.Minutes--;
      } else {
        AlarmTime.Minutes = 59;
      }
      //  CountPushButton5ForSetAlarm = 0;
    }

    else if (CurrentSetDigit == 1) {
      IsAllowAlarmNap = 0;
      CountForNapTimes = 0;

      if (AlarmTime.Hours > 0) {
        AlarmTime.Hours--;
      } else {
        AlarmTime.Hours = 23;
      }
      // CountPushButton5ForSetAlarm = 0;
    }
  }
}

void ShowAlarmTimeInSetAlarmScreen() {
  if (CurrentSetDigit == 0) {
    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
      if (HoursType == _24Hours)
        ShowTimeOnLcd24Hours(4, 0, GetStringTime(GetTime(AlarmTime.Hours, AlarmTime.Minutes, 0)));
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
        ShowTimeOnLcd12Hours(4, 0, GetStringTime(GetTime(_Hours_Alarm, AlarmTime.Minutes, 0)), _AM_PM, NotSameLine);
      }
      ////delay(5);
    } else {
      WriteFullPointsOnLcd(7, 0, 2);
      // //delay(5);
    }

  }

  else if (CurrentSetDigit == 1) {

    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
      if (HoursType == _24Hours)
        ShowTimeOnLcd24Hours(4, 0, GetStringTime(GetTime(AlarmTime.Hours, AlarmTime.Minutes, 0)));
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
        ShowTimeOnLcd12Hours(4, 0, GetStringTime(GetTime(_Hours_Alarm, AlarmTime.Minutes, 0)), _AM_PM, NotSameLine);
      }
      //  //delay(5);
    } else {
      WriteFullPointsOnLcd(4, 0, 2);
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
        ShowTimeOnLcd24Hours(4, 0, GetStringTime(GetTime(now.Hour(), now.Minute(), now.Second())));
      else {
        byte _Hours = now.Hour();
        bool _AM_PM = 0;
        ChangeTheHourValueWhen24HoursTo12Hours(_Hours, _AM_PM);

        ShowTimeOnLcd12Hours(4, 0, GetStringTime(GetTime(_Hours, now.Minute(), now.Second())), _AM_PM, NotSameLine);
      }

      //delay(5);
    } else {

      WriteFullPointsOnLcd(10, 0, 2);
      //delay(5);
    }



  }

  else if (_CurrentSetDigit == SetMinutes) {

    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
      if (HoursType == _24Hours)
        ShowTimeOnLcd24Hours(4, 0, GetStringTime(GetTime(now.Hour(), now.Minute(), now.Second())));
      else {
        byte _Hours = now.Hour();
        bool _AM_PM = 0;
        ChangeTheHourValueWhen24HoursTo12Hours(_Hours, _AM_PM);

        ShowTimeOnLcd12Hours(4, 0, GetStringTime(GetTime(_Hours, now.Minute(), now.Second())), _AM_PM, NotSameLine);
      }

      //delay(5);
    } else {
      WriteFullPointsOnLcd(7, 0, 2);
      //delay(5);
    }

  }

  else if (_CurrentSetDigit == SetHours) {

    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
      if (HoursType == _24Hours)
        ShowTimeOnLcd24Hours(4, 0, GetStringTime(GetTime(now.Hour(), now.Minute(), now.Second())));
      else {
        byte _Hours = now.Hour();
        bool _AM_PM = 0;
        ChangeTheHourValueWhen24HoursTo12Hours(_Hours, _AM_PM);

        ShowTimeOnLcd12Hours(4, 0, GetStringTime(GetTime(_Hours, now.Minute(), now.Second())), _AM_PM, NotSameLine);
      }

      //delay(5);
    } else {
      WriteFullPointsOnLcd(4, 0, 2);



      //delay(5);
    }

  }

  else if (_CurrentSetDigit == SetGregorianDay) {
    ClearRow(1);


    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
      ShowGregorianDateOnLcd(2, 0, GetStringGregorianDate(GetDate(GregorianDate.Year, GregorianDate.Month - 1, GregorianDate.Day)));

      //delay(5);
    } else {
      WriteFullPointsOnLcd(2, 0, 2);


      //delay(5);
    }



  }

  else if (_CurrentSetDigit == SetGregorianMonth) {

    ClearRow(1);
    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
      ShowGregorianDateOnLcd(2, 0, GetStringGregorianDate(GetDate(GregorianDate.Year, GregorianDate.Month - 1, GregorianDate.Day)));

      //delay(5);
    } else {
      WriteFullPointsOnLcd(6, 0, 3);


      //delay(5);
    }


  }

  else if (_CurrentSetDigit == SetGregorianYear) {
    ClearRow(1);
    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
      ShowGregorianDateOnLcd(2, 0, GetStringGregorianDate(GetDate(GregorianDate.Year, GregorianDate.Month - 1, GregorianDate.Day)));

      //delay(5);
    } else {
      WriteFullPointsOnLcd(11, 0, 4);




      //delay(5);
    }



  }

  else if (_CurrentSetDigit == SetHijriDay) {
    ClearRow(1);
    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
      ShowHijriDateOnLcd(2, 0, GetStringHijriDate(GetDate(HijriDate.Year, HijriDate.Month, HijriDate.Day)));


      //delay(5);
    } else {
      WriteFullPointsOnLcd(2, 0, 2);



      //delay(5);
    }



  }

  else if (_CurrentSetDigit == SetHijriMonth) {
    ClearRow(1);
    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
      ShowHijriDateOnLcd(2, 0, GetStringHijriDate(GetDate(HijriDate.Year, HijriDate.Month, HijriDate.Day)));


      //delay(5);
    } else {
      WriteFullPointsOnLcd(6, 0, 3);


      //delay(5);
    }


  }

  else if (_CurrentSetDigit == SetHijriYear) {

    ClearRow(1);
    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
      ShowHijriDateOnLcd(2, 0, GetStringHijriDate(GetDate(HijriDate.Year, HijriDate.Month, HijriDate.Day)));


      //delay(5);
    } else {
      WriteFullPointsOnLcd(11, 0, 4);


      //delay(5);
    }




  }

  else if (_CurrentSetDigit == SetDayName) {
    // ClearRow(1);
    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
      ShowDayOnLcd(6, 0, GetDayName(DayInSet));


      //delay(5);
    } else {
      WriteFullPointsOnLcd(6, 0, 3);


      //delay(5);
    }
  }
}

void CheckAndExecuteDecreaseDigitInSetTimeAndDateScreen(byte _CurrentSetDigit) {
  if ((EventAfterPressingPushButtonAndResetValue(PushButton5, PushButton5_Value, Previous_PushButton5_Value, CountPushButton5ForSetTimeAndDate, Input_PullUp)) || (((refPeriodChangeValue_DecreaseButton5_SetDateAndTime < millis() - PeriodChangeValue_DecreaseButton5_SetDateTime) && PeriodChangeValue_DecreaseButton5_SetDateTime != millis()) && (millis() - EnterTime_DecreaseButton5_SetDateTime > refPeriod_EnterTime_DecreaseButton5_SetDateAndTime))) {
    EnterTime_DecreaseButton5_SetDateTime = millis();
    if (_CurrentSetDigit == SetSeconds) {
      Time.Seconds = 0;

      //CountPushButton5ForSetTimeAndDate = 0;
      Rtc.SetDateTime(RtcDateTime(now.Year(), now.Month(), now.Day(), now.Hour(), now.Minute(), Time.Seconds));

      if (HoursType == _24Hours)
        ShowTimeOnLcd24Hours(4, 0, GetStringTime(GetTime(now.Hour(), now.Minute(), now.Second())));
      else {
        byte _Hours = now.Hour();
        bool _AM_PM = 0;
        ChangeTheHourValueWhen24HoursTo12Hours(_Hours, _AM_PM);

        ShowTimeOnLcd12Hours(4, 0, GetStringTime(GetTime(_Hours, now.Minute(), now.Second())), _AM_PM, NotSameLine);
      }
    } else if (_CurrentSetDigit == SetMinutes) {
      if (Time.Minutes > 0) {

        Time.Minutes--;
      } else {
        Time.Minutes = 59;
      }
      //CountPushButton5ForSetTimeAndDate = 0;
      Rtc.SetDateTime(RtcDateTime(now.Year(), now.Month(), now.Day(), now.Hour(), Time.Minutes, now.Second()));

      if (HoursType == _24Hours)
        ShowTimeOnLcd24Hours(4, 0, GetStringTime(GetTime(now.Hour(), now.Minute(), now.Second())));
      else {
        byte _Hours = now.Hour();
        bool _AM_PM = 0;
        ChangeTheHourValueWhen24HoursTo12Hours(_Hours, _AM_PM);

        ShowTimeOnLcd12Hours(4, 0, GetStringTime(GetTime(_Hours, now.Minute(), now.Second())), _AM_PM, NotSameLine);
      }
    }

    else if (_CurrentSetDigit == SetHours) {

      if (Time.Hours > 0) {
        Time.Hours--;
      } else {
        Time.Hours = 23;
      }
      //CountPushButton5ForSetTimeAndDate = 0;
      Rtc.SetDateTime(RtcDateTime(now.Year(), now.Month(), now.Day(), Time.Hours, now.Minute(), now.Second()));

      if (HoursType == _24Hours)
        ShowTimeOnLcd24Hours(4, 0, GetStringTime(GetTime(now.Hour(), now.Minute(), now.Second())));
      else {
        byte _Hours = now.Hour();
        bool _AM_PM = 0;
        ChangeTheHourValueWhen24HoursTo12Hours(_Hours, _AM_PM);

        ShowTimeOnLcd12Hours(4, 0, GetStringTime(GetTime(_Hours, now.Minute(), now.Second())), _AM_PM, NotSameLine);
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
      //CountPushButton5ForSetTimeAndDate = 0;
      Rtc.SetDateTime(RtcDateTime(now.Year(), now.Month(), GregorianDate.Day, now.Hour(), now.Minute(), now.Second()));

      ShowGregorianDateOnLcd(2, 0, GetStringGregorianDate(GetDate(GregorianDate.Year, GregorianDate.Month - 1, GregorianDate.Day)));
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
      //CountPushButton5ForSetTimeAndDate = 0;
      Rtc.SetDateTime(RtcDateTime(now.Year(), GregorianDate.Month, GregorianDate.Day, now.Hour(), now.Minute(), now.Second()));

      ShowGregorianDateOnLcd(2, 0, GetStringGregorianDate(GetDate(GregorianDate.Year, GregorianDate.Month - 1, GregorianDate.Day)));

    } else if (_CurrentSetDigit == SetGregorianYear) {

      GregorianDate.Year--;
      if (GregorianDate.Day > NumberOfDaysInAMonth(GregorianDate.Year, GregorianDate.Month)) {
        GregorianDate.Day = NumberOfDaysInAMonth(GregorianDate.Year, GregorianDate.Month);
        if (IsLeapYear(GregorianDate.Year) && GregorianDate.Month == 2)
          GregorianDate.Day = 29;
      }
      //CountPushButton5ForSetTimeAndDate = 0;
      Rtc.SetDateTime(RtcDateTime(GregorianDate.Year, now.Month(), GregorianDate.Day, now.Hour(), now.Minute(), now.Second()));

      ShowGregorianDateOnLcd(2, 0, GetStringGregorianDate(GetDate(GregorianDate.Year, GregorianDate.Month - 1, GregorianDate.Day)));

    }

    else if (_CurrentSetDigit == SetHijriDay) {

      if (HijriDate.Day > 1) {
        HijriDate.Day--;
      } else {
        HijriDate.Day = 30;
      }
      //CountPushButton5ForSetTimeAndDate = 0;

      ShowHijriDateOnLcd(2, 0, GetStringHijriDate(GetDate(HijriDate.Year, HijriDate.Month, HijriDate.Day)));

    }

    else if (_CurrentSetDigit == SetHijriMonth) {

      if (HijriDate.Month > 0) {
        HijriDate.Month--;
      } else {
        HijriDate.Month = 11;
      }
      ShowHijriDateOnLcd(2, 0, GetStringHijriDate(GetDate(HijriDate.Year, HijriDate.Month, HijriDate.Day)));
      //CountPushButton5ForSetTimeAndDate = 0;


    }

    else if (_CurrentSetDigit == SetHijriYear) {

      HijriDate.Year--;

      ShowHijriDateOnLcd(2, 0, GetStringHijriDate(GetDate(HijriDate.Year, HijriDate.Month, HijriDate.Day)));
      //CountPushButton5ForSetTimeAndDate = 0;



    }

    else if (_CurrentSetDigit == SetDayName) {
      if (digitalRead(PushButton4) == LOW && digitalRead(PushButton5) == LOW) {
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

      //CountPushButton5ForSetTimeAndDate = 0;
    }
  }
}

void CheckAndExecuteIncreaseDigitInSetTimeAndDateScreen(byte _CurrentSetDigit) {
  if ((EventAfterPressingPushButtonAndResetValue(PushButton4, PushButton4_Value, Previous_PushButton4_Value, CountPushButton4ForSetTimeAndDate, Input_PullUp)) || (((refPeriodChangeValue_IncreaseButton4_SetDateAndTime < millis() - PeriodChangeValue_IncreaseButton4_SetDateTime) && PeriodChangeValue_IncreaseButton4_SetDateTime != millis()) && (millis() - EnterTime_IncreaseButton4_SetDateTime > refPeriod_EnterTime_IncreaseButton4_SetDateAndTime))) {
    EnterTime_IncreaseButton4_SetDateTime = millis();
    if (IsAllowAlarm) {
      UpdateAlarmOperationbtn4(TurnOffAlarm, IsAllowAlarm, IsAllowAlarmNap, CountForNapTimes);
    }
    if (_CurrentSetDigit == SetSeconds) {
      Time.Seconds = 0;
      Rtc.SetDateTime(RtcDateTime(now.Year(), now.Month(), now.Day(), now.Hour(), now.Minute(), Time.Seconds));

      //CountPushButton4ForSetTimeAndDate = 0;
      if (HoursType == _24Hours)
        ShowTimeOnLcd24Hours(4, 0, GetStringTime(GetTime(now.Hour(), now.Minute(), now.Second())));
      else {
        byte _Hours = now.Hour();
        bool _AM_PM = 0;
        ChangeTheHourValueWhen24HoursTo12Hours(_Hours, _AM_PM);

        ShowTimeOnLcd12Hours(4, 0, GetStringTime(GetTime(_Hours, now.Minute(), now.Second())), _AM_PM, NotSameLine);
      }
    } else if (_CurrentSetDigit == SetMinutes) {
      if (Time.Minutes < 59) {

        Time.Minutes++;
      } else {
        Time.Minutes = 0;
      }
      //   //CountPushButton4ForSetTimeAndDate = 0;
      Rtc.SetDateTime(RtcDateTime(now.Year(), now.Month(), now.Day(), now.Hour(), Time.Minutes, now.Second()));

      if (HoursType == _24Hours)
        ShowTimeOnLcd24Hours(4, 0, GetStringTime(GetTime(now.Hour(), now.Minute(), now.Second())));
      else {
        byte _Hours = now.Hour();
        bool _AM_PM = 0;
        ChangeTheHourValueWhen24HoursTo12Hours(_Hours, _AM_PM);
        ShowTimeOnLcd12Hours(4, 0, GetStringTime(GetTime(_Hours, now.Minute(), now.Second())), _AM_PM, NotSameLine);
      }

    }

    else if (_CurrentSetDigit == SetHours) {

      if (Time.Hours < 23) {
        Time.Hours++;
      } else {
        Time.Hours = 0;
      }
      //              //CountPushButton4ForSetTimeAndDate = 0;
      Rtc.SetDateTime(RtcDateTime(now.Year(), now.Month(), now.Day(), Time.Hours, now.Minute(), now.Second()));
      if (HoursType == _24Hours)
        ShowTimeOnLcd24Hours(4, 0, GetStringTime(GetTime(now.Hour(), now.Minute(), now.Second())));
      else {
        byte _Hours = now.Hour();
        bool _AM_PM = 0;
        ChangeTheHourValueWhen24HoursTo12Hours(_Hours, _AM_PM);

        ShowTimeOnLcd12Hours(4, 0, GetStringTime(GetTime(_Hours, now.Minute(), now.Second())), _AM_PM, NotSameLine);
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
      //CountPushButton4ForSetTimeAndDate = 0;
      Rtc.SetDateTime(RtcDateTime(now.Year(), now.Month(), GregorianDate.Day, now.Hour(), now.Minute(), now.Second()));


      ShowGregorianDateOnLcd(2, 0, GetStringGregorianDate(GetDate(GregorianDate.Year, GregorianDate.Month - 1, GregorianDate.Day)));

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
      //CountPushButton4ForSetTimeAndDate = 0;
      Rtc.SetDateTime(RtcDateTime(now.Year(), GregorianDate.Month, GregorianDate.Day, now.Hour(), now.Minute(), now.Second()));

      ShowGregorianDateOnLcd(2, 0, GetStringGregorianDate(GetDate(GregorianDate.Year, GregorianDate.Month - 1, GregorianDate.Day)));

    } else if (_CurrentSetDigit == SetGregorianYear) {


      GregorianDate.Year++;
      if (GregorianDate.Day > NumberOfDaysInAMonth(GregorianDate.Year, GregorianDate.Month)) {
        GregorianDate.Day = NumberOfDaysInAMonth(GregorianDate.Year, GregorianDate.Month);
        if (IsLeapYear(GregorianDate.Year) && GregorianDate.Month == 2)
          GregorianDate.Day = 29;
      }
      //CountPushButton4ForSetTimeAndDate = 0;
      Rtc.SetDateTime(RtcDateTime(GregorianDate.Year, now.Month(), GregorianDate.Day, now.Hour(), now.Minute(), now.Second()));
      ShowGregorianDateOnLcd(2, 0, GetStringGregorianDate(GetDate(GregorianDate.Year, GregorianDate.Month - 1, GregorianDate.Day)));
    }

    else if (_CurrentSetDigit == SetHijriDay) {

      if (HijriDate.Day < 30) {
        HijriDate.Day++;
      } else {
        HijriDate.Day = 1;
      }
      //CountPushButton4ForSetTimeAndDate = 0;

      ShowHijriDateOnLcd(2, 0, GetStringHijriDate(GetDate(HijriDate.Year, HijriDate.Month, HijriDate.Day)));

    }

    else if (_CurrentSetDigit == SetHijriMonth) {

      if (HijriDate.Month < 11) {
        HijriDate.Month++;
      } else {
        HijriDate.Month = 0;
      }
      //CountPushButton4ForSetTimeAndDate = 0;

      ShowHijriDateOnLcd(2, 0, GetStringHijriDate(GetDate(HijriDate.Year, HijriDate.Month, HijriDate.Day)));

    }

    else if (_CurrentSetDigit == SetHijriYear) {
      HijriDate.Year++;
      //CountPushButton4ForSetTimeAndDate = 0;

      ShowHijriDateOnLcd(2, 0, GetStringHijriDate(GetDate(HijriDate.Year, HijriDate.Month, HijriDate.Day)));

    }

    else if (_CurrentSetDigit == SetDayName) {
      if (digitalRead(PushButton4) == LOW && digitalRead(PushButton5) == LOW) {
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
      //CountPushButton4ForSetTimeAndDate = 0;
    }
  }
}

void OperationOfShowSetDateAndTimeScreen() {
  //To save Alarm Time to EEPROM and save (EEPROM) for longest time
  SaveAlarmTimeTo_EEPROM(AlarmTime, Address_EEPROM_MinuteAlarm, Address_EEPROM_HourAlarm);


  PreviousSetDigit = CurrentSetDigit;
  CurrentSetDigit = EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetTimeAndDate, 10, Input_PullUp);

  IsAllowAlarm = fnIsAllowAlarm(Time, AlarmTime, AlarmNapTime, PushButton3, PushButton4, PushButton3_Value, PushButton4_Value, IsAllowAlarm, IsAllowAlarmNap, TurnOffAlarm, CurrentScreen, Input_PullUp);
  UpdateAlarmTimeOperations(Time, Buzzer, IsAllowAlarm, IsAllowAlarmNap, CountForNapTimes, CountAlarmPeep);

  if (PreviousSetDigit != CurrentSetDigit)
    ChangeSetDigit = 1;

  else
    ChangeSetDigit = 0;

  if (IsAllowAlarm && ChangeSetDigit)
    UpdateAlarmNapTimeOperationsbtn3(AlarmTime, AlarmNapTime, TurnOffAlarm, IsAllowAlarm, IsAllowAlarmNap, CountForNapTimes, Minutes_AlarmNapTime_Period);




  AllowAlarm(Time, AlarmTime, IsAllowAlarm);




  if (!(digitalRead(PushButton4) == LOW && digitalRead(PushButton5) == LOW)) {
    StartPeriodForAutoDayOrder = millis();
    ReadStartPeriodForAutoDayOrder = 0;
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void ShowTimeAndDateScreen() {
  bool IsBlankRow1 = 0;
  OperationOfShowTimeAndDateScreen();
  ChangeValueAfterPressingTwo_btn_In_SpecificPeriod(PushButton4, PushButton5, StartPeriodForChangeHoursType, IsReadStartPeriodForChangeHoursType, IsChangedHoursType, HoursType, refPeriodForChangeHoursType, Input_PullType);
  ShowTimeOnMainScreen();
  CheckAndExecuteChangeTypeOfDate_UpdateAlarmOperationbtn4();
  CheckAndExecuteUpdateAlarmNapTimeOperationsbtn3();
  CheckAndExecuteExchangeAndShowDateOnLcd(IsBlankRow1);
  CheckAndExecuteShowAlarmTimeOnLcd(IsShowDay);
  CheckAndExecuteShowDayNameOnLcd(IsBlankRow1);
  CheckAndExecuteClearRow1OnLcd(IsBlankRow1);


  if (MoveScreen(CurrentScreen)) {
    PreviousSetDigit = 0;
    CurrentSetDigit = 0;
    return;
  }
}

void ShowStopWatchScreen() {

  OperationOfShowStopWatchScreen();
  CheckAndExecute_Start_Or_Countinue_StopWatch();
    WriteMilliSecondOnLcd();
  CheckAndExecute_Countinue_Or_Restart_StopWatch();
  ShowTimeOnLcd24Hours(4, 0, GetStringTime(GetTime(StopWatchTime.Hours, StopWatchTime.Minutes, StopWatchTime.Seconds)));
 // ClearRow(1);
}

void ShowSetAlarmScreen() {

  OperationOfShowSetAlarmScreen();
  OperationsOfExecuteProcessRepeatlyWhilePressingbtn(PushButton4, PushButton4_Value, PeriodChangeValue_IncreaseButton4_SetAlarm, In_IncreaseButton4_SetAlarm, Input_PullUp);
  CheckAndExecuteIncreaseDigit(refPeriod_EnterTime_IncreaseButton4_SetAlarm);
  OperationsOfExecuteProcessRepeatlyWhilePressingbtn(PushButton5, PushButton5_Value, PeriodChangeValue_DecreaseButton5_SetAlarm, In_DecreaseButton5_SetAlarm, Input_PullUp);
  CheckAndExecuteDecreaseDigit(refPeriod_EnterTime_DecreaseButton5_SetAlarm);
  ShowAlarmTimeInSetAlarmScreen();
}

void ShowSetDateAndTimeScreen() {

  OperationOfShowSetDateAndTimeScreen();
  OperationsOfExecuteProcessRepeatlyWhilePressingbtn(PushButton4, PushButton4_Value, PeriodChangeValue_IncreaseButton4_SetDateTime, In_IncreaseButton4_SetDateTime, Input_PullUp);
  CheckAndExecuteIncreaseDigitInSetTimeAndDateScreen(CurrentSetDigit);
  OperationsOfExecuteProcessRepeatlyWhilePressingbtn(PushButton5, PushButton5_Value, PeriodChangeValue_DecreaseButton5_SetDateTime, In_DecreaseButton5_SetDateTime, Input_PullUp);
  CheckAndExecuteDecreaseDigitInSetTimeAndDateScreen(CurrentSetDigit);
  ShowTimeOrDateInSetTimeAndDateScreen(CurrentSetDigit);
}


void ShowScreen(RtcDateTime& _Now, byte _CurrentScreen) {

  if (_CurrentScreen == 0) {
    ShowTimeAndDateScreen();
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
    return;
  }
}

void WriteMilliSecondOnLcd()
{
   lcd.setCursor(0,1);
 lcd.print("             ");
 if(StopWatchTime.MilliSeconds < 10) 
 {
   lcd.print("00");
 }
 else if(StopWatchTime.MilliSeconds < 100) 
  lcd.print("0");
lcd.print(StopWatchTime.MilliSeconds);

}

void GetCurrentButton()
{
   if(digitalRead(PushButton1))
    PushButton1_Value = HIGH;
   else
    PushButton1_Value = LOW;

   if(digitalRead(PushButton2))
    PushButton2_Value = HIGH;
   else
    PushButton2_Value = LOW;

   if(digitalRead(PushButton3))
    PushButton3_Value = HIGH;
   else
    PushButton3_Value = LOW;

    if(digitalRead(PushButton4))
    PushButton4_Value = HIGH;
   else
    PushButton4_Value = LOW;

    if(digitalRead(PushButton5))
    PushButton5_Value = HIGH;
   else
    PushButton5_Value = LOW;

}

void InterruptOperations()
{
  GetCurrentButton();
  if(CurrentScreen==1 && IsAllowCountStopWatch ==1 && ( PushButton4_Value = 1))
  {
    IsAllowCountStopWatch = 0;
  }

}

void setup() {
  lcd.init();
  lcd.backlight();
  Rtc.Begin();
 // Serial.begin(9600);


  pinMode(InterruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(InterruptPin), InterruptOperations, CHANGE );

  
  AlarmTime.Hours = EEPROM.read(Address_EEPROM_HourAlarm);
  AlarmTime.Minutes = EEPROM.read(Address_EEPROM_MinuteAlarm);

  
 HijriDate.Day = EEPROM.read(Address_EEPROM_HijriDay);  //
  HijriDate.Month = EEPROM.read(Address_EEPROM_HijriMonth);
  HijriDate.Year = Read2ByteEEPROM(Address_EEPROM_HijriYear_Left, Address_EEPROM_HijriYear_Right);


   // HijriDate.Day = 16;
  //HijriDate.Month = 11;
 // HijriDate.Year = 1445;
  
  /*
  HijriDate.Day =  EEPROM.read(Address_EEPROM_GregorianDay);  //
   HijriDate.Month = EEPROM.read(Address_EEPROM_GregorianMonth);
   HijriDate.Year = Read2ByteEEPROM(Address_EEPROM_GregorianYear_Left, Address_EEPROM_GregorianYear_Right);
*/

    GregorianDate.Day = EEPROM.read(Address_EEPROM_GregorianDay);  //
  GregorianDate.Month = EEPROM.read(Address_EEPROM_GregorianMonth);
  GregorianDate.Year = Read2ByteEEPROM(Address_EEPROM_GregorianYear_Left, Address_EEPROM_GregorianYear_Right);

  pinMode(PushButton1, INPUT_PULLUP);
  pinMode(PushButton2, INPUT_PULLUP);
  pinMode(PushButton3, INPUT_PULLUP);
  pinMode(PushButton4, INPUT_PULLUP);
  pinMode(PushButton5, INPUT_PULLUP);


  
  UpdateHijriDateAndDayNameFrom_EEPROM(GetDate(now.Year(), now.Month(), now.Day()), GregorianDate, HijriDate, DayInSet, Address_EEPROM_HijriYear_Left, Address_EEPROM_HijriYear_Right, Address_EEPROM_HijriMonth, Address_EEPROM_HijriDay, Address_EEPROM_DayName);
  Timer1.initialize(1000);
  Timer1.attachInterrupt(IncreaseCountersValuesByTimer1);
 



  // RtcDateTime currentTime = RtcDateTime(__DATE__ , __TIME__);
   //Rtc.SetDateTime(currentTime);


  //Rtc.SetDateTime(RtcDateTime("1 15 2010","15:05:02"));
}


void loop() {

  ShowScreen(now, CurrentScreen);
}
