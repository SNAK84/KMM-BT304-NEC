#include <Arduino.h>

#include <AnalogButtons.h>

AnalogButtons CruiseButtons(A1, INPUT);
AnalogButtons AudioButtons(A2, INPUT);

#define VOLUP 0x14      //громкость +
#define VOLDOWN 0x15    //громкость -
#define ATT 0x16        // Mute/ОК
#define AUDIO 0x17      // управление/мод
#define MODE 0x13       // управление/мод
#define TRACKFORW 0x0B  // следующий трек
#define TRACKBACK 0x0A  // предыдущий трек
#define FOLDERFORW 0x0D // следующая папка
#define FOLDERBACK 0x0C // предыдущая папка
#define PAUSE 0x0E      //пауза
#define SEARCH 0x85     //назад
#define PHONE 0x92      //ответить/выход

#define PULSEWIDTH 560 // Время связи
#define OUTPUTPIN 10   // D5 на транзистор 2N2222
#define LED 13
#define ADDRESS 0xB9 //протокол

void SendCommand(unsigned char value);
unsigned char GetInput(void);
void SendZero();
void SendOne();
void Preamble();
void Postamble();

void CruiseMainClick()
{
  Serial.println("Cruise Main button clicked");
  digitalWrite(5, LOW);
  delay(100);
  digitalWrite(5, HIGH);
}
void CruiseCancelClick()
{
  Serial.println("Cruise Cancel button clicked");
  digitalWrite(6, HIGH);
  delay(100);
  digitalWrite(6, LOW);
}
void CruiseResumeClick()
{
  Serial.println("Cruise Resume button clicked");
  digitalWrite(7, HIGH);
  delay(100);
  digitalWrite(7, LOW);
}
void CruiseSetClick()
{
  Serial.println("Cruise Set button clicked");
  digitalWrite(8, HIGH);
  delay(100);
  digitalWrite(8, LOW);
}

void AudioModeClick()
{
  Serial.println("Audio Mode button clicked");
  SendCommand(MODE);
}
void AudioModeHold()
{
  Serial.println("Audio Mode button holded");
  SendCommand(ATT);
}
void AudioUpClick()
{
  Serial.println("Audio Up button clicked");
  SendCommand(TRACKFORW);
}
void AudioUpHold()
{
  Serial.println("Audio Up button holded");
  SendCommand(FOLDERFORW);
}
void AudioDownClick()
{
  Serial.println("Audio Down button clicked");
  SendCommand(TRACKBACK);
}
void AudioDownHold()
{
  Serial.println("Audio Down button holded");
  SendCommand(FOLDERBACK);
}
void AudioVolUpClick()
{
  Serial.println("Audio VolUp button clicked");
  SendCommand(VOLUP);
}
void AudioVolUpHold()
{
  Serial.println("Audio VolUp button holded");
  SendCommand(VOLUP);
}
void AudioVolDownClick()
{
  Serial.println("Audio VolDown button clicked");
  SendCommand(VOLDOWN);
}
void AudioVolDownHold()
{
  Serial.println("Audio VolDown button holded");
  SendCommand(VOLDOWN);
}

Button CruiseMain = Button(655, &CruiseMainClick);
Button CruiseCancel = Button(0, &CruiseCancelClick);
Button CruiseResume = Button(300, &CruiseResumeClick);
Button CruiseSet = Button(93, &CruiseSetClick);

Button AudioMode = Button(500, &AudioModeClick, &AudioModeHold, 250, 50);
Button AudioUp = Button(167, &AudioUpClick, &AudioUpHold, 250, 50);
Button AudioDown = Button(341, &AudioDownClick, &AudioDownHold, 250, 50);
Button AudioVolUp = Button(697, &AudioVolUpClick, &AudioVolUpHold, 250, 50);
Button AudioVolDown = Button(774, &AudioVolDownClick, &AudioVolDownHold, 250, 50);

void setup()
{
  /*
  MCUCR != (1 << BODS) | (1 << BODSE);
  MCUCR &= ~(1 << BODSE);
*/
  Serial.begin(9600); //монитор для вывода информации (для настройки)
  pinMode(OUTPUTPIN, OUTPUT);
  digitalWrite(OUTPUTPIN, LOW);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  digitalWrite(5, HIGH);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);

  CruiseButtons.add(CruiseMain);
  CruiseButtons.add(CruiseCancel);
  CruiseButtons.add(CruiseResume);
  CruiseButtons.add(CruiseSet);

  AudioButtons.add(AudioMode);
  AudioButtons.add(AudioUp);
  AudioButtons.add(AudioDown);
  AudioButtons.add(AudioVolUp);
  AudioButtons.add(AudioVolDown);

  Serial.println("Start...");

  digitalWrite(LED, LOW);
}

void loop()
{
  CruiseButtons.check();
  AudioButtons.check();
/*
  Serial.print(" analogA1 ");            //(для настройки)
  Serial.print(analogRead(A1));                //для просмотра данный от аналогового входа (для настройки)
  Serial.print(" analogA2 ");            //(для настройки)
  Serial.println(analogRead(A2));                //для просмотра данный от аналогового входа (для настройки)
  delay(250);*/
}

void SendValue(unsigned char value)
{

  digitalWrite(LED, HIGH);

  unsigned char i, tmp = 1;
  for (i = 0; i < sizeof(value) * 8; i++)
  {
    if (value & tmp)
      SendOne();
    else
      SendZero();
    tmp = tmp << 1;
  }
  tmp = 1;
  for (i = 0; i < sizeof(value) * 8; i++)
  {
    if (value & tmp)
      SendZero();
    else
      SendOne();
    tmp = tmp << 1;
  }

  digitalWrite(LED, LOW);
}

void SendCommand(unsigned char value)
{
  Preamble();
  SendValue(ADDRESS);
  SendValue((unsigned char)value);
  Postamble();
}

void SendZero()
{
  digitalWrite(OUTPUTPIN, HIGH);
  delayMicroseconds(PULSEWIDTH);
  digitalWrite(OUTPUTPIN, LOW);
  delayMicroseconds(PULSEWIDTH);
}

void SendOne()
{
  digitalWrite(OUTPUTPIN, HIGH);
  delayMicroseconds(PULSEWIDTH);
  digitalWrite(OUTPUTPIN, LOW);
  delayMicroseconds(PULSEWIDTH * 3);
}

void Preamble()
{
  digitalWrite(OUTPUTPIN, LOW);
  delayMicroseconds(PULSEWIDTH * 1);
  digitalWrite(OUTPUTPIN, HIGH);
  delayMicroseconds(PULSEWIDTH * 16);
  digitalWrite(OUTPUTPIN, LOW);
  delayMicroseconds(PULSEWIDTH * 8);
}

void Postamble()
{
  SendOne();
}