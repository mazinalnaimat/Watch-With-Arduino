#include <ThreeWire.h>
#include <RtcDS1302.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <TimerOne.h>


//#define PushButton1 A1
#define PushButton2 A0
#define PushButton3 A1
#define PushButton4 A2
#define PushButton5 A3
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
#define Address_EEPROM_HijriYear_Right 5
#define Address_EEPROM_HijriYear_Left 6
#define Address_EEPROM_GregorianDay 7
#define Address_EEPROM_GregorianMonth 8
#define Address_EEPROM_GregorianYear_Right 9
#define Address_EEPROM_GregorianYear_Left 10

/*
#define HijriDate 0 
#define GregorianDate 1
*/

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

int Read2ByteEEPROM(int RightHalf, int LeftHalf) {
  int RightHalfValue = EEPROM.read(RightHalf);
  RightHalfValue = RightHalfValue << 8;

  return RightHalfValue + EEPROM.read(LeftHalf);
}

void Update2ByteEEPROM(int RightHalf, int LeftHalf, int Value) {
  EEPROM.update(RightHalf, byte(Value >> 8));
  EEPROM.update(LeftHalf, byte(Value));
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

struct sTime
{
  byte Seconds = 0;   //
  byte Minutes = 0;  //
  byte Hours   = 0;    //
};

struct sDate
{
 byte Day   = 1;     
 byte Month = 1;
 int Year  = 2000;  
};

sTime Time;
sTime AlarmTime;
sTime Alarm_Nap;
sTime StopWatchTime;

#define Minutes_Alarm_Nap_Period  5  //

bool _AllowAlarm = false;     //
bool _AllowAlarmNap = false;
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

sDate HijriDate;
sDate GregorianDate;

byte CountForNapTimes = 0;  //

int CountPushButton4ForDate = 0;              //
int CountPushButton4ForStopWatch = 0;         //
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
bool EnterShowDay = 0;

int CountAlarmPeep = 0;
float CountFullPointsShow = 0;

#define MaxCountAlarmPeep 20
#define MaxCountFullPointsShow 40


int PreviousSecond = 0;

bool IsLastButton4StopWatch = 0;
bool Button3AsButton4StopWatch = 0;
//byte NumberDayInMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

long StartPeriodForAutoDayOrder = 0;
long EndPeriodForAutoDayOrder = 0;
bool ReadStartPeriodForAutoDayOrder = 0;

bool IsChangedHoursType = 0;
bool ReadStartPeriodForChangeHoursType = 0;
long StartPeriodForChangeHoursType = 0;

bool TurnOffAlarm = false;

#define refPeriodForChangeHoursType 3000
#define refPeriodChangeValue_IncreaseButton4_SetAlarm 1000
#define refPeriod_EnterTime_IncreaseButton4_SetAlarm  100
#define refPeriodChangeValue_DecreaseButton5_SetAlarm 1000
#define refPeriod_EnterTime_DecreaseButton5_SetAlarm  100
#define refPeriodChangeValue_IncreaseButton4_SetDateAndTime 1000
#define refPeriod_EnterTime_IncreaseButton4_SetDateAndTime  100
#define refPeriodChangeValue_DecreaseButton5_SetDateAndTime 1000
#define refPeriod_EnterTime_DecreaseButton5_SetDateAndTime  100
#define refPeriodStartPeriodForAutoDayOrder 3000 
#define refPeriodEndPeriodForAutoDayOrder   3000

//LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
LiquidCrystal_I2C lcd(0x27,  16, 2);

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

bool EventWhilePressingPushButton(int PinName, int& Push_Value, bool InputPull_Type = Input_PullType) {
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

bool IsAllowAlarm(bool InputPull_Type = Input_PullUp) {

  PushButton4_Value = digitalRead(PushButton4);
  PushButton3_Value = digitalRead(PushButton3);
  if(Time.Hours !=AlarmTime.Hours || Time.Minutes != AlarmTime.Minutes)
  {
    TurnOffAlarm = false;
  }
  if (InputPull_Type == Input_PullDown) {
    if ((Time.Hours ==AlarmTime.Hours&&Time.Minutes ==AlarmTime.Minutes&& _AllowAlarm && !TurnOffAlarm) ||(Time.Hours==Alarm_Nap.Hours&&Time.Minutes == Alarm_Nap.Minutes && _AllowAlarmNap && !TurnOffAlarm) || (PushButton4_Value == HIGH && PushButton3_Value == HIGH && CurrentScreen == 0)) {
      return true;
    } else {
      return false;
    }
  } else {
    if ((Time.Hours ==AlarmTime.Hours&&Time.Minutes ==AlarmTime.Minutes&& _AllowAlarm && !TurnOffAlarm) ||(Time.Hours==Alarm_Nap.Hours&&Time.Minutes == Alarm_Nap.Minutes && _AllowAlarmNap && !TurnOffAlarm) || (PushButton4_Value != HIGH && PushButton3_Value != HIGH && CurrentScreen == 0)) {
      return true;
    } else {
      return false;
    }
  }
}

void AllowAlarm() {
  if(Time.Hours==AlarmTime.Hours&&Time.Minutes ==AlarmTime.Minutes&&Time.Seconds == 0) {
    _AllowAlarm = true;
  }
}

int EventAfterPressingPushButton(int PinName, int& Push_Value, int& Previous_Push_Value, int& CountPush, int CasesNumber, bool InputPull_Type = Input_PullType) {
  
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

  return CountPush =CountPush;
}

bool EventAfterPressingPushButtonAndResetValue(int PinName, int& Push_Value, int& Previous_Push_Value, int& CountPush, bool InputPull_Type = Input_PullType) {
  bool Value = EventAfterPressingPushButton(PinName, Push_Value, Previous_Push_Value, CountPush, 2, InputPull_Type);

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

bool IsLeapYear(int Year) {
  return ((Year % 400 == 0) || (Year % 4 == 0 && Year % 100 != 0));
}

String GetStringTime(sTime _Time);

sDate GetDate(int _Years, byte _MonthNumber, byte _Day);


String GetStringGregorianDate(sDate _Date);

String GetStringHijriDate(sDate _Date); 

String GetDayName(int DayNumber) {

  String DaysSet[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
  String Day = DaysSet[DayNumber];
  return Day;
}


void ShowTimeOnLcd24Hours(int Column, int Row, String Time) {
  WriteOnCleanRowOnLcd(Column, Row, Time);
}

void ShowTimeOnLcd12Hours(int Column, int Row, String Time, bool AM_PM, bool SameLine = 0) {

  if (SameLine == 0) {
    WriteOnCleanRowOnLcd(Column, Row, Time);
    if (AM_PM == 0)
      WriteOnCleanRowOnLcd(7, Row + 1, "AM");
    else
      WriteOnCleanRowOnLcd(7, Row + 1, "PM");
  } else {
    WriteOnCleanRowOnLcd(Column - 2, Row, Time);
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

void ShowAlarmScreenOnLcd(int Column, int Row, String AlarmTime, bool HoursType, bool AM_PM = 0) {
  if (HoursType == _24Hours)
    ShowTimeOnLcd24Hours(Column, Row, AlarmTime);
  else {
    ShowTimeOnLcd12Hours(Column, Row, AlarmTime, AM_PM);
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

void IncreaseCountAlarmPeep() {
  CountAlarmPeep++;
  if (CountAlarmPeep == MaxCountAlarmPeep)  //40
  {
    CountAlarmPeep = 0;
  }
}

void IncreaseCountFullPointsShow()
{  
  CountFullPointsShow++;

  if (CountFullPointsShow == MaxCountFullPointsShow) {
    CountFullPointsShow = 0;
  }
}

void  IncreaseCountersValuesByTimer1()
{
  IncreaseCountAlarmPeep();
  IncreaseCountFullPointsShow();
}

void RingTone() {
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
  if (PreviousSecond != Time.Seconds) {
    PreviousSecond = Time.Seconds;
    if (CountPushButton4ForStopWatch == 1) {
      StopWatchTime.Seconds++;
    }

    if (StopWatchTime.Seconds == 60) {
      StopWatchTime.Seconds = 0;
      StopWatchTime.Minutes++;
    }

    if (StopWatchTime.Minutes == 60) {
      StopWatchTime.Minutes = 0;
      StopWatchTime.Hours++;
    }

    if (StopWatchTime.Hours == 24) {
      StopWatchTime.Hours = 0;
    }
  }
}

 void UpdateTime(sTime & _Time, byte Hours, byte Minutes, byte Seconds = 0);
 

void UpdateDate(sDate & _Date,int Year, byte Month, byte Day);

void UpdateTimeAndDate(sTime & _Time,byte Hours, byte Minutes, byte Seconds, sDate & _Date, int Year, byte Month, byte Day);

void UpdateGregorianDateIn_EEPROM()  // to edit the hijri Date and Day Name
{
  UpdateDate( GregorianDate, now.Year(), now.Month(), now.Day());
  EEPROM.update(Address_EEPROM_GregorianDay,GregorianDate.Day);
  EEPROM.update(Address_EEPROM_GregorianMonth,GregorianDate.Month);
  Update2ByteEEPROM(Address_EEPROM_GregorianYear_Right, Address_EEPROM_GregorianYear_Left, GregorianDate.Year);
}

void UpdateHijriDateIn_EEPROM()  // to edit the hijri Date and Day Name
{
  EEPROM.update(Address_EEPROM_HijriDay, HijriDate.Day);
  EEPROM.update(Address_EEPROM_HijriMonth, HijriDate.Month);
  Update2ByteEEPROM(Address_EEPROM_HijriYear_Right, Address_EEPROM_HijriYear_Left, HijriDate.Year);
}

int NumberOfDaysInAMonth(int Month, int Year) {
  return Month = Month == 2 ? (IsLeapYear(Year) ? 29 : 28) : (Month == 11 || Month == 9 || Month == 6 || Month == 4) ? 30
                                                                                                                     : 31;
}

bool IsDate1BeforeDate2(sDate Date1, sDate Date2); 

void IncreaseDateByOneDay(sDate _Date); 

long GetDifferenceInDays(sDate Date1, sDate Date2, bool IncludeEndDay = false); 

void IncreaseHijriDateByOneDay() {
 HijriDate.Day++;
  if (HijriDate.Day > 30) {
   HijriDate.Day = 1;
    HijriDate.Month++;
  }
  if (HijriDate.Month > 11) {
    HijriDate.Month = 0;
    HijriDate.Year++;
  }
}

void IncreaseDayNameByOneDay() {
  DayInSet++;
  if (DayInSet > 6) {
    DayInSet = 0;
  }
}

void UpdateHijriDateAndDayName() {

  if (now.Hour() == 0 && now.Minute() == 0 && now.Second() == 0 && IsIncreaseHijriDateAndDayName == 0 && now.Day() != 0)  // Day if the RTC not connected
  {
    IncreaseHijriDateByOneDay();
    IncreaseDayNameByOneDay();
    IsIncreaseHijriDateAndDayName = 1;
    UpdateHijriDateIn_EEPROM();
    EEPROM.update(Address_EEPROM_DayName, DayInSet);
  }
  if (now.Hour() != 0 && now.Minute() != 0 && now.Second() != 0) {
    IsIncreaseHijriDateAndDayName = 0;
  }
}

void UpdateHijriDateAndDayNameFrom_EEPROM() {
  sDate Date1;
  sDate Date2;
  Date1.Year = Read2ByteEEPROM(Address_EEPROM_GregorianYear_Right, Address_EEPROM_GregorianYear_Left);
  Date1.Month = EEPROM.read(Address_EEPROM_GregorianMonth);
  Date1.Day = EEPROM.read(Address_EEPROM_GregorianDay);
  Date2.Year = now.Year();
  Date2.Month = now.Month();
  Date2.Day = now.Day();

  long DayNumber = GetDifferenceInDays(Date1, Date2);

  while (DayNumber > 0) {


    DayNumber--;
    IncreaseDayNameByOneDay();
    IncreaseHijriDateByOneDay();
  }
  UpdateHijriDateIn_EEPROM();

  EEPROM.update(Address_EEPROM_DayName, DayInSet);
}

byte DayOrder(sDate _Date); 

void AutoDayOrder() {
  DayInSet = DayOrder(GetDate(now.Year(), now.Month(), now.Day()));
  EEPROM.update(Address_EEPROM_DayName, DayInSet);
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

void UpdateAlarmTimeOperations(byte _AllowAlarm) {
  if (_AllowAlarm) {
    if (Time.Seconds == 59) {
      CountForNapTimes = 0;
      _AllowAlarmNap = false;
    }
    RingTone();

  } else {
    TurnOffBuzzer(Buzzer);
    digitalWrite(Buzzer,0);
  }
}
void UpdateAlarmNapTimeOperationsbtn3() {
   TurnOffAlarm = true;
  _AllowAlarm = false;
  _AllowAlarmNap = true;
  CountForNapTimes++;
  Alarm_Nap.Minutes =AlarmTime.Minutes+ Minutes_Alarm_Nap_Period * CountForNapTimes;
 Alarm_Nap.Hours= AlarmTime.Hours;
  if (Alarm_Nap.Minutes >= 60) {
    Alarm_Nap.Minutes = Alarm_Nap.Minutes - 60;
    Alarm_Nap.Hours++;
  }
  if (Alarm_Nap.Hours>= 24) {
   Alarm_Nap.Hours= 0;
  }

  if (CountForNapTimes == 13) {
    CountForNapTimes = 0;
    _AllowAlarmNap = false;
  }
}

void UpdateAlarmOperationbtn4(){
  
      TurnOffAlarm = true;
       _AllowAlarm = false;
      _AllowAlarmNap = false;
      CountForNapTimes = 0;
}

sTime GetTime(byte Hours, byte Minutes, byte Seconds);

void ShowTimeAndDateScreen() {
  UpdateHijriDateIn_EEPROM();  //To Store the Hijiri Date and Gregorian Date In EEPROM and Save for longest time
  UpdateGregorianDateIn_EEPROM();
  if (!(digitalRead(PushButton4) == LOW && digitalRead(PushButton5) == LOW) && millis()) {
    IsChangedHoursType = 0;
    ReadStartPeriodForChangeHoursType = 0;
  }
  if (digitalRead(PushButton4) == LOW && digitalRead(PushButton5) == LOW && IsChangedHoursType == 0) {
    if (ReadStartPeriodForChangeHoursType == 0) {
      StartPeriodForChangeHoursType = millis();
      ReadStartPeriodForChangeHoursType = 1;
    }
    if (millis() - StartPeriodForChangeHoursType > refPeriodForChangeHoursType) {
      HoursType ^= 1;
      IsChangedHoursType = 1;
    }
  }

  bool IsBlankRow1 = 0;
  _AllowAlarm = IsAllowAlarm();
  UpdateAlarmTimeOperations(_AllowAlarm);


  CountPushButton3ForSetAlarm = 0;
  CountPushButton3ForSetTimeAndDate = 0;
  AllowAlarm();

  if (MoveScreen()) {
    PreviousSetDigit = 0;
    CurrentSetDigit = 0;
    return;
  }
  if (( PushButton3_Value == LOW && Input_PullType == Input_PullDown) || ( PushButton3_Value == HIGH && Input_PullType == Input_PullUp)) {
    if (HoursType == _24Hours)
    {
      ShowTimeOnLcd24Hours(4, 0, GetStringTime((Time)));
    }
    else {
      byte _Hours = now.Hour();
      bool _AM_PM = 0;
      ChangeTheHourValueWhen24HoursTo12Hours(_Hours, _AM_PM);

      if (digitalRead(PushButton4) == LOW || digitalRead(PushButton5) == LOW)
        ShowTimeOnLcd12Hours(4, 0, GetStringTime(GetTime(_Hours, now.Minute(), now.Second())), _AM_PM, 1);
      else
        ShowTimeOnLcd12Hours(4, 0, GetStringTime(GetTime(_Hours, now.Minute(), now.Second())), _AM_PM);
    }
  }

  if (EventAfterPressingPushButtonAndResetValue(PushButton4, PushButton4_Value, Previous_PushButton4_Value, CountPushButton4ForDate)) {
    ChangeTypeOfDate = ChangeTypeOfDate ^ 1;
    if (_AllowAlarm) {
       UpdateAlarmOperationbtn4();
    }
    // CountPushButton2ForDate = 0;
  }

  if (EventAfterPressingPushButtonAndResetValue(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForTurnOffAlarm) && _AllowAlarm) {
   UpdateAlarmNapTimeOperationsbtn3(); 
  }



  if (EventWhilePressingPushButton(PushButton4, PushButton4_Value) && !EnterShowDay && digitalRead(PushButton3) == 1) {


    ExchangeAndShowDateOnLcd(2, 1, GetStringGregorianDate(GetDate(GregorianDate.Year,GregorianDate.Month - 1,GregorianDate.Day)), GetStringHijriDate(GetDate(HijriDate.Year, HijriDate.Month,HijriDate.Day)), ChangeTypeOfDate);
    IsBlankRow1 = 1;
    EnterShowDate = 1;
  } else {
    EnterShowDate = 0;
  }

  if (EventWhilePressingPushButton(PushButton3, PushButton3_Value)) {
    if (HoursType == _24Hours)
      ShowTimeOnLcd24Hours(4, 0, GetStringTime(GetTime(Time.Hours, Time.Minutes, 0)));
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
      ShowTimeOnLcd12Hours(4, 0, GetStringTime(GetTime(_Hours_Alarm, AlarmTime.Minutes, 0)), _AM_PM);
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
  _AllowAlarm = IsAllowAlarm();
  UpdateAlarmTimeOperations(_AllowAlarm);
  AllowAlarm();
  if (MoveScreen()) {
    PreviousSetDigit = 0;
    CurrentSetDigit = 0;
    return;
  }


  if (EventAfterPressingPushButtonAndResetValue(PushButton4, PushButton4_Value, Previous_PushButton4_Value, CountPushButton4ForStopWatch_Peeb)) {
    TurnOnBuzzer(Buzzer, 3000);
    TurnOffBuzzer(Buzzer);
    //  CountPushButton2ForStopWatch_Peeb =0;
    CountPushButton4ForStopWatch ^= 1;
    IsLastButton4StopWatch = 1;
    Button3AsButton4StopWatch = 0;
    if (_AllowAlarm) {
    UpdateAlarmOperationbtn4();
    }
  }

  if (EventAfterPressingPushButtonAndResetValue(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForStopWatch)) {
    if (_AllowAlarm)
      UpdateAlarmNapTimeOperationsbtn3();

    if ((CountPushButton4ForStopWatch == 1 || Button3AsButton4StopWatch == 1)) {
      TurnOnBuzzer(Buzzer, 3000);
      TurnOffBuzzer(Buzzer);
      //  CountPushButton2ForStopWatch_Peeb =0;
      CountPushButton4ForStopWatch ^= 1;
      IsLastButton4StopWatch = 0;
      Button3AsButton4StopWatch = 1;
    } else if (IsLastButton4StopWatch == 1 && CountPushButton4ForStopWatch == 0 && Button3AsButton4StopWatch == 0) {
      TurnOnBuzzer(Buzzer, 3000);
      TurnOffBuzzer(Buzzer);
      CountPushButton4ForStopWatch = 0;
      StopWatchTime.Hours = 0;
      StopWatchTime.Minutes = 0;
      StopWatchTime.Seconds = 0;
      // Previous_PushButton2_Value =0;
    }
  }
  ShowTimeOnLcd24Hours(4, 0, GetStringTime(GetTime(StopWatchTime.Hours, StopWatchTime.Minutes, StopWatchTime.Seconds)));
  ClearRow(1);
}

void ShowSetAlarmScreen() {
  PreviousSetDigit = CurrentSetDigit;
  CurrentSetDigit = EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetAlarm, 2);
  _AllowAlarm = IsAllowAlarm();
  UpdateAlarmTimeOperations(_AllowAlarm);

  if (PreviousSetDigit != CurrentSetDigit)
    ChangeSetDigit = 1;

  else
    ChangeSetDigit = 0;


  if (_AllowAlarm && ChangeSetDigit)
    UpdateAlarmNapTimeOperationsbtn3();




  AllowAlarm();

  if (MoveScreen()) {
    PreviousSetDigit = 0;
    CurrentSetDigit = 0;
    return;
  }
  
  PushButton4_Value = digitalRead(PushButton4);
  if (Input_PullType == Input_PullUp) {
    PushButton4_Value = !PushButton4_Value;
  }

  if (PushButton4_Value == 1 && !In_IncreaseButton4_SetAlarm) {

    PeriodChangeValue_IncreaseButton4_SetAlarm = millis();
    In_IncreaseButton4_SetAlarm = 1;
  }

  PushButton4_Value = digitalRead(PushButton4);
  if (Input_PullType == Input_PullUp) {
    PushButton4_Value = !PushButton4_Value;
  }
  if (PushButton4_Value == 0) {
    PeriodChangeValue_IncreaseButton4_SetAlarm = millis();
    In_IncreaseButton4_SetAlarm = 0;
  }



  if ((EventAfterPressingPushButtonAndResetValue(PushButton4, PushButton4_Value, Previous_PushButton4_Value, CountPushButton4ForSetAlarm)) || (((refPeriodChangeValue_IncreaseButton4_SetAlarm < millis() - PeriodChangeValue_IncreaseButton4_SetAlarm) && PeriodChangeValue_IncreaseButton4_SetAlarm != millis()) && (millis() - EnterTime_IncreaseButton4_SetAlarm > refPeriod_EnterTime_IncreaseButton4_SetAlarm))) {
    if (_AllowAlarm) {
    UpdateAlarmOperationbtn4();
    }
    EnterTime_IncreaseButton4_SetAlarm = millis();

    if (CurrentSetDigit == 0) {
      _AllowAlarmNap = 0;
      CountForNapTimes = 0;
      if (AlarmTime.Minutes < 59) {
       AlarmTime.Minutes++;
      } else {
       AlarmTime.Minutes= 0;
      }
      //CountPushButton4ForSetAlarm = 0;
    } else if (CurrentSetDigit == 1) {
      _AllowAlarmNap = 0;
      CountForNapTimes = 0;

      if (AlarmTime.Hours < 23) {
        AlarmTime.Hours++;
      } else {
       AlarmTime.Hours= 0;
      }
      //CountPushButton4ForSetAlarm = 0;
    }
  }

  PushButton5_Value = digitalRead(PushButton5);
  if (Input_PullType == Input_PullUp) {
    PushButton5_Value = !PushButton5_Value;
  }

  if (PushButton5_Value == 1 && !In_DecreaseButton5_SetAlarm) {
    PeriodChangeValue_DecreaseButton5_SetAlarm = millis();
    In_DecreaseButton5_SetAlarm = 1;
  }

  PushButton5_Value = digitalRead(PushButton5);
  if (Input_PullType == Input_PullUp) {
    PushButton5_Value = !PushButton5_Value;
  }

  if (PushButton5_Value == 0) {
    PeriodChangeValue_DecreaseButton5_SetAlarm = millis();
    In_DecreaseButton5_SetAlarm = 0;
  }

 
  

  if ((EventAfterPressingPushButtonAndResetValue(PushButton5, PushButton5_Value, Previous_PushButton5_Value, CountPushButton5ForSetAlarm)) || (((refPeriodChangeValue_DecreaseButton5_SetAlarm < millis() - PeriodChangeValue_DecreaseButton5_SetAlarm) && PeriodChangeValue_DecreaseButton5_SetAlarm != millis()) && (millis() - EnterTime_DecreaseButton5_SetAlarm > refPeriod_EnterTime_DecreaseButton5_SetAlarm))) {
    EnterTime_DecreaseButton5_SetAlarm = millis();
    if (CurrentSetDigit == 0) {
      _AllowAlarmNap = 0;
      CountForNapTimes = 0;
      if (AlarmTime.Minutes > 0) {

       AlarmTime.Minutes--;
      } else {
       AlarmTime.Minutes= 59;
      }
      //  CountPushButton5ForSetAlarm = 0;
    }

    else if (CurrentSetDigit == 1) {
      _AllowAlarmNap = 0;
      CountForNapTimes = 0;

      if (AlarmTime.Hours > 0) {
        AlarmTime.Hours--;
      } else {
       AlarmTime.Hours= 23;
      }
      // CountPushButton5ForSetAlarm = 0;
    }
  }
  ///////
  if (CurrentSetDigit == 0) {
    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
      if (HoursType == _24Hours)
        ShowTimeOnLcd24Hours(4, 0, GetStringTime(GetTime(AlarmTime.Hours, AlarmTime.Minutes, 0)));
      else {
        byte _Hours_Alarm =AlarmTime.Hours;
        bool _AM_PM = 0;
        if (_Hours_Alarm >= 12) {
          _Hours_Alarm = _Hours_Alarm - 12;
          _AM_PM = 1;
        }
        if (_Hours_Alarm == 0) {
          _Hours_Alarm = 12;
        }
        ShowTimeOnLcd12Hours(4, 0, GetStringTime(GetTime(_Hours_Alarm, AlarmTime.Minutes, 0)), _AM_PM);
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
        byte _Hours_Alarm =AlarmTime.Hours;
        bool _AM_PM = 0;
        if (_Hours_Alarm >= 12) {
          _Hours_Alarm = _Hours_Alarm - 12;
          _AM_PM = 1;
        }
        if (_Hours_Alarm == 0) {
          _Hours_Alarm = 12;
        }
        ShowTimeOnLcd12Hours(4, 0, GetStringTime(GetTime(_Hours_Alarm, AlarmTime.Minutes, 0)), _AM_PM);
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
  EEPROM.update(Address_EEPROM_MinuteAlarm, AlarmTime.Minutes);
  EEPROM.update(Address_EEPROM_HourAlarm,AlarmTime.Hours);

  PreviousSetDigit = CurrentSetDigit;
  CurrentSetDigit = EventAfterPressingPushButton(PushButton3, PushButton3_Value, Previous_PushButton3_Value, CountPushButton3ForSetTimeAndDate, 10);

  _AllowAlarm = IsAllowAlarm();
  UpdateAlarmTimeOperations(_AllowAlarm);

  if (PreviousSetDigit != CurrentSetDigit)
    ChangeSetDigit = 1;

  else
    ChangeSetDigit = 0;

  if (_AllowAlarm && ChangeSetDigit)
    UpdateAlarmNapTimeOperationsbtn3();




  AllowAlarm();
  if (MoveScreen()) {
    PreviousSetDigit = 0;
    CurrentSetDigit = 0;
    return;
  }




  if (!(digitalRead(PushButton4) == LOW && digitalRead(PushButton5) == LOW)) {
    StartPeriodForAutoDayOrder = millis();
    ReadStartPeriodForAutoDayOrder = 0;
  }

  PushButton4_Value = digitalRead(PushButton4);
  if (Input_PullType == Input_PullUp) {
    PushButton4_Value = !PushButton4_Value;
  }

  if (PushButton4_Value == 0) {
    PeriodChangeValue_IncreaseButton4_SetDateTime = millis();
    In_IncreaseButton4_SetDateTime = 0;
  }
 
  



  if ((EventAfterPressingPushButtonAndResetValue(PushButton4, PushButton4_Value, Previous_PushButton4_Value, CountPushButton4ForSetTimeAndDate)) || (((refPeriodChangeValue_IncreaseButton4_SetDateAndTime < millis() - PeriodChangeValue_IncreaseButton4_SetDateTime) && PeriodChangeValue_IncreaseButton4_SetDateTime != millis()) && (millis() - EnterTime_IncreaseButton4_SetDateTime > refPeriod_EnterTime_IncreaseButton4_SetDateAndTime))) {
    EnterTime_IncreaseButton4_SetDateTime = millis();
    if (_AllowAlarm) {
    UpdateAlarmOperationbtn4();
    }
    if (CurrentSetDigit == SetSeconds) {
     Time.Seconds = 0;
      Rtc.SetDateTime(RtcDateTime(now.Year(), now.Month(), now.Day(), now.Hour(), now.Minute(), Time.Seconds));

      //CountPushButton4ForSetTimeAndDate = 0;
      if (HoursType == _24Hours)
        ShowTimeOnLcd24Hours(4, 0, GetStringTime(GetTime(now.Hour(), now.Minute(), now.Second())));
      else {
        byte _Hours = now.Hour();
        bool _AM_PM = 0;
        ChangeTheHourValueWhen24HoursTo12Hours(_Hours, _AM_PM);

        ShowTimeOnLcd12Hours(4, 0, GetStringTime(GetTime(_Hours, now.Minute(), now.Second())), _AM_PM);
      }
    } else if (CurrentSetDigit == SetMinutes) {
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
        ShowTimeOnLcd12Hours(4, 0, GetStringTime(GetTime(_Hours, now.Minute(), now.Second())), _AM_PM);
      }

    }

    else if (CurrentSetDigit == SetHours) {

      if (Time.Hours< 23) {
      Time.Hours++;
      } else {
      Time.Hours= 0;
      }
      //              //CountPushButton4ForSetTimeAndDate = 0;
      Rtc.SetDateTime(RtcDateTime(now.Year(), now.Month(), now.Day(), Time.Hours, now.Minute(), now.Second()));
      if (HoursType == _24Hours)
        ShowTimeOnLcd24Hours(4, 0, GetStringTime(GetTime(now.Hour(), now.Minute(), now.Second())));
      else {
        byte _Hours = now.Hour();
        bool _AM_PM = 0;
        ChangeTheHourValueWhen24HoursTo12Hours(_Hours, _AM_PM);

        ShowTimeOnLcd12Hours(4, 0, GetStringTime(GetTime(_Hours, now.Minute(), now.Second())), _AM_PM);
      }
    }

    else if (CurrentSetDigit == SetGregorianDay) {
      if (IsLeapYear(GregorianDate.Year) &&GregorianDate.Month == 2 &&GregorianDate.Day < 29) {
       GregorianDate.Day++;
      } else if (GregorianDate.Day < NumberOfDaysInAMonth(GregorianDate.Month, GregorianDate.Year)) {
       GregorianDate.Day++;
      } else {
       GregorianDate.Day = 1;
      }
      //CountPushButton4ForSetTimeAndDate = 0;
      Rtc.SetDateTime(RtcDateTime(now.Year(), now.Month(),GregorianDate.Day, now.Hour(), now.Minute(), now.Second()));


      ShowGregorianDateOnLcd(2, 0, GetStringGregorianDate(GetDate(GregorianDate.Year,GregorianDate.Month - 1,GregorianDate.Day)));

    }

    else if (CurrentSetDigit == SetGregorianMonth) {

      if (GregorianDate.Month < 12) {
       GregorianDate.Month++;
      } else {
       GregorianDate.Month = 1;
      }
      if (GregorianDate.Day >NumberOfDaysInAMonth(GregorianDate.Month, GregorianDate.Year)) {
       GregorianDate.Day =NumberOfDaysInAMonth(GregorianDate.Month, GregorianDate.Year);
        if (IsLeapYear(GregorianDate.Year) &&GregorianDate.Month == 2)
         GregorianDate.Day = 29;
      }
      //CountPushButton4ForSetTimeAndDate = 0;
      Rtc.SetDateTime(RtcDateTime(now.Year(),GregorianDate.Month,GregorianDate.Day, now.Hour(), now.Minute(), now.Second()));

      ShowGregorianDateOnLcd(2, 0, GetStringGregorianDate(GetDate(GregorianDate.Year,GregorianDate.Month - 1,GregorianDate.Day)));

    } else if (CurrentSetDigit == SetGregorianYear) {


      GregorianDate.Year++;
      if (GregorianDate.Day >NumberOfDaysInAMonth(GregorianDate.Month, GregorianDate.Year)) {
       GregorianDate.Day =NumberOfDaysInAMonth(GregorianDate.Month, GregorianDate.Year);
        if (IsLeapYear(GregorianDate.Year) &&GregorianDate.Month == 2)
         GregorianDate.Day = 29;
      }
      //CountPushButton4ForSetTimeAndDate = 0;
      Rtc.SetDateTime(RtcDateTime(GregorianDate.Year, now.Month(),GregorianDate.Day, now.Hour(), now.Minute(), now.Second()));
      ShowGregorianDateOnLcd(2, 0, GetStringGregorianDate(GetDate(GregorianDate.Year,GregorianDate.Month - 1,GregorianDate.Day)));
    }

    else if (CurrentSetDigit == SetHijriDay) {

      if (HijriDate.Day < 30) {
       HijriDate.Day++;
      } else {
       HijriDate.Day = 1;
      }
      //CountPushButton4ForSetTimeAndDate = 0;

      ShowHijriDateOnLcd(2, 0, GetStringHijriDate(GetDate(HijriDate.Year, HijriDate.Month,HijriDate.Day)));

    }

    else if (CurrentSetDigit == SetHijriMonth) {

      if (HijriDate.Month < 11) {
        HijriDate.Month++;
      } else {
        HijriDate.Month = 0;
      }
      //CountPushButton4ForSetTimeAndDate = 0;

      ShowHijriDateOnLcd(2, 0, GetStringHijriDate(GetDate(HijriDate.Year, HijriDate.Month,HijriDate.Day)));

    }

    else if (CurrentSetDigit == SetHijriYear) {
      HijriDate.Year++;
      //CountPushButton4ForSetTimeAndDate = 0;

      ShowHijriDateOnLcd(2, 0, GetStringHijriDate(GetDate(HijriDate.Year, HijriDate.Month,HijriDate.Day)));

    }

    else if (CurrentSetDigit == SetDayName) {
      if (digitalRead(PushButton4) == LOW && digitalRead(PushButton5) == LOW) {
        if (ReadStartPeriodForAutoDayOrder == 0) {
          StartPeriodForAutoDayOrder = millis();
          ReadStartPeriodForAutoDayOrder = 1;
        }
        if (millis() - StartPeriodForAutoDayOrder > refPeriodStartPeriodForAutoDayOrder) {
          AutoDayOrder();
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

  PushButton5_Value = digitalRead(PushButton5);
  if (Input_PullType == Input_PullUp) {
    PushButton5_Value = !PushButton5_Value;
  }

  if (PushButton5_Value == 1 && !In_DecreaseButton5_SetDateTime) {
    PeriodChangeValue_DecreaseButton5_SetDateTime = millis();
    In_DecreaseButton5_SetDateTime = 1;
  }

  PushButton5_Value = digitalRead(PushButton5);
  if (Input_PullType == Input_PullUp) {
    PushButton5_Value = !PushButton5_Value;
  }
  if (PushButton5_Value == 0) {
    PeriodChangeValue_DecreaseButton5_SetDateTime = millis();
    In_DecreaseButton5_SetDateTime = 0;
  }

 
  

  if ((EventAfterPressingPushButtonAndResetValue(PushButton5, PushButton5_Value, Previous_PushButton5_Value, CountPushButton5ForSetTimeAndDate)) || (((refPeriodChangeValue_DecreaseButton5_SetDateAndTime < millis() - PeriodChangeValue_DecreaseButton5_SetDateTime) && PeriodChangeValue_DecreaseButton5_SetDateTime != millis()) && (millis() - EnterTime_DecreaseButton5_SetDateTime > refPeriod_EnterTime_DecreaseButton5_SetDateAndTime))) {
    EnterTime_DecreaseButton5_SetDateTime = millis();
    if (CurrentSetDigit == SetSeconds) {
     Time.Seconds = 0;

      //CountPushButton5ForSetTimeAndDate = 0;
      Rtc.SetDateTime(RtcDateTime(now.Year(), now.Month(), now.Day(), now.Hour(), now.Minute(), Time.Seconds));

      if (HoursType == _24Hours)
        ShowTimeOnLcd24Hours(4, 0, GetStringTime(GetTime(now.Hour(), now.Minute(), now.Second())));
      else {
        byte _Hours = now.Hour();
        bool _AM_PM = 0;
        ChangeTheHourValueWhen24HoursTo12Hours(_Hours, _AM_PM);

        ShowTimeOnLcd12Hours(4, 0, GetStringTime(GetTime(_Hours, now.Minute(), now.Second())), _AM_PM);
      }
    } else if (CurrentSetDigit == SetMinutes) {
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

        ShowTimeOnLcd12Hours(4, 0, GetStringTime(GetTime(_Hours, now.Minute(), now.Second())), _AM_PM);
      }
    }

    else if (CurrentSetDigit == SetHours) {

      if (Time.Hours> 0) {
      Time.Hours--;
      } else {
      Time.Hours= 23;
      }
      //CountPushButton5ForSetTimeAndDate = 0;
      Rtc.SetDateTime(RtcDateTime(now.Year(), now.Month(), now.Day(), Time.Hours, now.Minute(), now.Second()));

      if (HoursType == _24Hours)
        ShowTimeOnLcd24Hours(4, 0, GetStringTime(GetTime(now.Hour(), now.Minute(), now.Second())));
      else {
        byte _Hours = now.Hour();
        bool _AM_PM = 0;
        ChangeTheHourValueWhen24HoursTo12Hours(_Hours, _AM_PM);

        ShowTimeOnLcd12Hours(4, 0, GetStringTime(GetTime(_Hours, now.Minute(), now.Second())), _AM_PM);
      }
    }

    else if (CurrentSetDigit == SetGregorianDay) {

      if (GregorianDate.Day > 1) {
       GregorianDate.Day--;
      } else if (IsLeapYear(GregorianDate.Year) &&GregorianDate.Month == 2 &&GregorianDate.Day == 1) {
       GregorianDate.Day = 29;
      } else {
       GregorianDate.Day =NumberOfDaysInAMonth(GregorianDate.Month, GregorianDate.Year);
      }
      //CountPushButton5ForSetTimeAndDate = 0;
      Rtc.SetDateTime(RtcDateTime(now.Year(), now.Month(),GregorianDate.Day, now.Hour(), now.Minute(), now.Second()));

      ShowGregorianDateOnLcd(2, 0, GetStringGregorianDate(GetDate(GregorianDate.Year,GregorianDate.Month - 1,GregorianDate.Day)));
    }

    else if (CurrentSetDigit == SetGregorianMonth) {

      if (GregorianDate.Month > 1) {
       GregorianDate.Month--;
      } else {
       GregorianDate.Month = 12;
      }
      if (GregorianDate.Day >NumberOfDaysInAMonth(GregorianDate.Month, GregorianDate.Year)) {
       GregorianDate.Day =NumberOfDaysInAMonth(GregorianDate.Month, GregorianDate.Year);
        if (IsLeapYear(GregorianDate.Year) &&GregorianDate.Month == 2)
         GregorianDate.Day = 29;
      }
      //CountPushButton5ForSetTimeAndDate = 0;
      Rtc.SetDateTime(RtcDateTime(now.Year(),GregorianDate.Month,GregorianDate.Day, now.Hour(), now.Minute(), now.Second()));

      ShowGregorianDateOnLcd(2, 0, GetStringGregorianDate(GetDate(GregorianDate.Year,GregorianDate.Month - 1,GregorianDate.Day)));

    } else if (CurrentSetDigit == SetGregorianYear) {

      GregorianDate.Year--;
      if (GregorianDate.Day >NumberOfDaysInAMonth(GregorianDate.Month, GregorianDate.Year)) {
       GregorianDate.Day =NumberOfDaysInAMonth(GregorianDate.Month, GregorianDate.Year);
        if (IsLeapYear(GregorianDate.Year) &&GregorianDate.Month == 2)
         GregorianDate.Day = 29;
      }
      //CountPushButton5ForSetTimeAndDate = 0;
      Rtc.SetDateTime(RtcDateTime(GregorianDate.Year, now.Month(),GregorianDate.Day, now.Hour(), now.Minute(), now.Second()));

      ShowGregorianDateOnLcd(2, 0, GetStringGregorianDate(GetDate(GregorianDate.Year,GregorianDate.Month - 1,GregorianDate.Day)));

    }

    else if (CurrentSetDigit == SetHijriDay) {

      if (HijriDate.Day > 1) {
       HijriDate.Day--;
      } else {
       HijriDate.Day = 30;
      }
      //CountPushButton5ForSetTimeAndDate = 0;

      ShowHijriDateOnLcd(2, 0, GetStringHijriDate(GetDate(HijriDate.Year, HijriDate.Month,HijriDate.Day)));

    }

    else if (CurrentSetDigit == SetHijriMonth) {

      if (HijriDate.Month > 0) {
        HijriDate.Month--;
      } else {
        HijriDate.Month = 11;
      }
      ShowHijriDateOnLcd(2, 0, GetStringHijriDate(GetDate(HijriDate.Year, HijriDate.Month,HijriDate.Day)));
      //CountPushButton5ForSetTimeAndDate = 0;


    }

    else if (CurrentSetDigit == SetHijriYear) {

      HijriDate.Year--;

      ShowHijriDateOnLcd(2, 0, GetStringHijriDate(GetDate(HijriDate.Year, HijriDate.Month,HijriDate.Day)));
      //CountPushButton5ForSetTimeAndDate = 0;



    }

    else if (CurrentSetDigit == SetDayName) {
      if (digitalRead(PushButton4) == LOW && digitalRead(PushButton5) == LOW) {
        if (ReadStartPeriodForAutoDayOrder == 0) {
          StartPeriodForAutoDayOrder = millis();
          ReadStartPeriodForAutoDayOrder = 1;
        }
        if (millis() - StartPeriodForAutoDayOrder > refPeriodStartPeriodForAutoDayOrder) {
          AutoDayOrder();
          EndPeriodForAutoDayOrder = millis();
        }
      } else if (millis() - EndPeriodForAutoDayOrder > refPeriodEndPeriodForAutoDayOrder  ) {
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






  ////delay(5);
  if (CurrentSetDigit == SetSeconds) {

    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
      if (HoursType == _24Hours)
        ShowTimeOnLcd24Hours(4, 0, GetStringTime(GetTime(now.Hour(), now.Minute(), now.Second())));
      else {
        byte _Hours = now.Hour();
        bool _AM_PM = 0;
        ChangeTheHourValueWhen24HoursTo12Hours(_Hours, _AM_PM);

        ShowTimeOnLcd12Hours(4, 0, GetStringTime(GetTime(_Hours, now.Minute(), now.Second())), _AM_PM);
      }

      //delay(5);
    } else {

      WriteFullPointsOnLcd(10, 0, 2);
      //delay(5);
    }



  }

  else if (CurrentSetDigit == SetMinutes) {

    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
      if (HoursType == _24Hours)
        ShowTimeOnLcd24Hours(4, 0, GetStringTime(GetTime(now.Hour(), now.Minute(), now.Second())));
      else {
        byte _Hours = now.Hour();
        bool _AM_PM = 0;
        ChangeTheHourValueWhen24HoursTo12Hours(_Hours, _AM_PM);

        ShowTimeOnLcd12Hours(4, 0, GetStringTime(GetTime(_Hours, now.Minute(), now.Second())), _AM_PM);
      }

      //delay(5);
    } else {
      WriteFullPointsOnLcd(7, 0, 2);
      //delay(5);
    }

  }

  else if (CurrentSetDigit == SetHours) {

    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
      if (HoursType == _24Hours)
        ShowTimeOnLcd24Hours(4, 0, GetStringTime(GetTime(now.Hour(), now.Minute(), now.Second())));
      else {
        byte _Hours = now.Hour();
        bool _AM_PM = 0;
        ChangeTheHourValueWhen24HoursTo12Hours(_Hours, _AM_PM);

        ShowTimeOnLcd12Hours(4, 0, GetStringTime(GetTime(_Hours, now.Minute(), now.Second())), _AM_PM);
      }

      //delay(5);
    } else {
      WriteFullPointsOnLcd(4, 0, 2);



      //delay(5);
    }

  }

  else if (CurrentSetDigit == SetGregorianDay) {
    ClearRow(1);


    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
      ShowGregorianDateOnLcd(2, 0, GetStringGregorianDate(GetDate(GregorianDate.Year,GregorianDate.Month - 1,GregorianDate.Day)));

      //delay(5);
    } else {
      WriteFullPointsOnLcd(2, 0, 2);


      //delay(5);
    }



  }

  else if (CurrentSetDigit == SetGregorianMonth) {

    ClearRow(1);
    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
      ShowGregorianDateOnLcd(2, 0, GetStringGregorianDate(GetDate(GregorianDate.Year,GregorianDate.Month - 1,GregorianDate.Day)));

      //delay(5);
    } else {
      WriteFullPointsOnLcd(6, 0, 3);


      //delay(5);
    }


  }

  else if (CurrentSetDigit == SetGregorianYear) {
    ClearRow(1);
    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
      ShowGregorianDateOnLcd(2, 0, GetStringGregorianDate(GetDate(GregorianDate.Year,GregorianDate.Month - 1,GregorianDate.Day)));

      //delay(5);
    } else {
      WriteFullPointsOnLcd(11, 0, 4);




      //delay(5);
    }



  }

  else if (CurrentSetDigit == SetHijriDay) {
    ClearRow(1);
    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
      ShowHijriDateOnLcd(2, 0, GetStringHijriDate(GetDate(HijriDate.Year, HijriDate.Month,HijriDate.Day)));


      //delay(5);
    } else {
      WriteFullPointsOnLcd(2, 0, 2);



      //delay(5);
    }



  }

  else if (CurrentSetDigit == SetHijriMonth) {
    ClearRow(1);
    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
      ShowHijriDateOnLcd(2, 0, GetStringHijriDate(GetDate(HijriDate.Year, HijriDate.Month,HijriDate.Day)));


      //delay(5);
    } else {
      WriteFullPointsOnLcd(6, 0, 3);


      //delay(5);
    }


  }

  else if (CurrentSetDigit == SetHijriYear) {

    ClearRow(1);
    if (CountFullPointsShow < MaxCountFullPointsShow / 2) {
      ShowHijriDateOnLcd(2, 0, GetStringHijriDate(GetDate(HijriDate.Year, HijriDate.Month,HijriDate.Day)));


      //delay(5);
    } else {
      WriteFullPointsOnLcd(11, 0, 4);


      //delay(5);
    }




  }

  else if (CurrentSetDigit == SetDayName) {
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
 // UpdateTimeAndDate(Time, now.Hour(), now.Minute(), now.Second(), GregorianDate,now.Year(), now.Month(), now.Day());
  UpdateStopTime();
  UpdateHijriDateAndDayName();


}


void setup() {
  lcd.init();
  lcd.backlight();
  Rtc.Begin();
  Serial.begin(9600);

  Time.Minutes = 0;
  Time.Hours   = 0;
  Time.Seconds = 0;
  AlarmTime.Hours = EEPROM.read(Address_EEPROM_HourAlarm);
  AlarmTime.Minutes = EEPROM.read(Address_EEPROM_MinuteAlarm);  
  HijriDate.Day = EEPROM.read(Address_EEPROM_HijriDay);  //
  HijriDate.Month = EEPROM.read(Address_EEPROM_HijriMonth);                                                                                               
  HijriDate.Year = Read2ByteEEPROM(Address_EEPROM_HijriYear_Right, Address_EEPROM_HijriYear_Left); 
  pinMode(PushButton2, INPUT_PULLUP);
  pinMode(PushButton3, INPUT_PULLUP);
  pinMode(PushButton4, INPUT_PULLUP);
  pinMode(PushButton5, INPUT_PULLUP);
    Rtc.SetDateTime(RtcDateTime(now.Year(), now.Month(), 15, now.Hour(), now.Minute(), now.Second()));

  UpdateHijriDateAndDayNameFrom_EEPROM();
  Timer1.initialize(10000);
  Timer1.attachInterrupt(IncreaseCountersValuesByTimer1);

  //  RtcDateTime currentTime = RtcDateTime(__DATE__ , __TIME__);
  //Rtc.SetDateTime(currentTime);


  //Rtc.SetDateTime(RtcDateTime("1 15 2010","15:05:02"));
}

void loop() {

  ShowScreen();
}



 void UpdateTime(sTime & _Time, byte Hours, byte Minutes, byte Seconds = 0)
 {
  _Time.Hours = Hours;
  _Time.Minutes = Minutes;
  _Time.Seconds = Seconds;
 } 

void UpdateDate(sDate & _Date,int Year, byte Month, byte Day)
{
  
 _Date.Day = Day;
 _Date.Month = Month;
 _Date.Year = Year;
  
}

void UpdateTimeAndDate(sTime & _Time,byte Hours, byte Minutes, byte Seconds, sDate & _Date, int Year, byte Month, byte Day) {
   UpdateTime(_Time, Hours, Minutes, Seconds);
   UpdateDate(_Date, Year, Month, Day) ;
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

sTime GetTime(byte Hours, byte Minutes, byte Seconds)
{
  sTime _Time;
  _Time.Hours = Hours;
  _Time.Minutes = Minutes;
  _Time.Seconds = Seconds;
  return _Time ; 
}

void IncreaseDateByOneDay(sDate _Date) {
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

long GetDifferenceInDays(sDate Date1, sDate Date2, bool IncludeEndDay = false) {
  int NumDays = 0;
  while (IsDate1BeforeDate2(Date1, Date2)) {
    IncreaseDateByOneDay(Date1);
    NumDays++;
  }
  return IncludeEndDay ? ++NumDays : NumDays;
}

bool IsDate1BeforeDate2(sDate Date1, sDate Date2) {
  return (Date1.Year < Date2.Year) ? true : (Date1.Month < Date2.Month) ? true:(Date1.Day < Date2.Day ? true : false);
}

sDate GetDate(int _Years, byte _MonthNumber, byte _Day)
{
  sDate _Date;
  _Date.Year = _Years;
  _Date.Month = _MonthNumber;
  _Date.Day = _Day;
  return _Date;
}

String GetStringGregorianDate(sDate _Date)
 {
  String GregorianMonthsSet[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
  String Date = "";
  if (_Date.Day < 10) {
    Date += "0";
  }
  Date += String(_Date.Day);
  Date += "  ";

  Date +=GregorianMonthsSet[_Date.Month];

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

byte DayOrder(sDate _Date) {
  long a = (14 -_Date.Month) / 12;
    _Date.Year =   _Date.Year - a;
  _Date.Month =_Date.Month + 12 * a - 2;
  return (  _Date.Day +   _Date.Year + (_Date.Year / 4) - (_Date.Year / 100) + (_Date.Year / 400) + (31 *_Date.Month / 12)) % 7;
}