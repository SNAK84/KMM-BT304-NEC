#include <Arduino.h>

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
#define REMA A0        // A0 аналоговый вход с делителя напряжения
#define LED 13
#define ADDRESS 0xB9    //протокол
#define HOLDTIME 500    //время, после которого кнопка считается зажатой
uint16_t ALLDOWN = 940; //все кнопки отпущены
#define DELTA 30        //погрешность измерения аналогового входа
//если показания подскочат больше чем на 10, увеличить значение (+10)

#define KVUP 230 // vol +
#define KVDW 170 // vol -
#define KNEX 710 //up
#define KBAC 520 //preset
#define KMOD 390 //mod

//#define KSOR 1024 //mute
#define KOFF 1000 //on/off
#define KCAN 60   //cancel
#define KRES 590  //res
#define KSET 820  //set

bool flag;          //метка о повторении кнопки
long analogA = 0;   // читаем аналоговый вход
unsigned long time; // переменная времени
unsigned long AllDownChTame = 0;
//uint16_t KeysUp = 0;

void SendCommand(unsigned char value);
unsigned char GetInput(void);
void SendZero();
void SendOne();
void Preamble();
void Postamble();

void setup()
{

  MCUCR != (1 << BODS) | (1 << BODSE);
  MCUCR &= ~(1 << BODSE);

  Serial.begin(9600); //монитор для вывода информации (для настройки)
  pinMode(OUTPUTPIN, INPUT_PULLUP);
  //digitalWrite(OUTPUTPIN, LOW);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);

  analogA = 0;
  for (uint8_t i = 0; i < 32; i++)
  {
    delay(10);
    analogA += analogRead(REMA);
  }
  analogA /= 32;

  //KeysUp = analogA;
  ALLDOWN = analogA;

  Serial.print("analogA ");
  Serial.println(analogA);
  // delay(1000);

  Serial.println("Start...");

  digitalWrite(LED, LOW);
}
unsigned char LastKey;
unsigned long TimeSend = 0;
void loop()
{

  unsigned char Key = GetInput();

  if (Key)
  { //если кнопки не нажимаются, функция вернет 0 и команда не будет отправлена
    SendCommand(Key);
    LastKey = Key;
    TimeSend = millis();
    Serial.print("KEY "); //(для настройки)
    Serial.println(Key);
  }
  //delay(50);                 //(для настройки)
  //if (abs(ALLDOWN - analogA) > DELTA)     //(для настройки)
  //{                                       //(для настройки)
  Serial.print("ALLDOWN ");             //(для настройки)
  Serial.print(ALLDOWN);                //для просмотра данный от аналогового входа (для настройки)
  Serial.print(" analogA ");            //(для настройки)
  Serial.print(analogA);                //для просмотра данный от аналогового входа (для настройки)
  Serial.print(" KEY_A ");              //(для настройки)
  Serial.print(abs(ALLDOWN - analogA)); //для просмотра данный от аналогового входа (для настройки)
  Serial.print(" time ");               //(для настройки)
  Serial.print(millis() - time);        //для просмотра данный от аналогового входа (для настройки)
  Serial.print(" LastKey ");            //(для настройки)
  Serial.println(LastKey);              //для просмотра данный от аналогового входа (для настройки)
  //}                                       //(для настройки)
}

unsigned char GetInput(void)
{

  analogA = 0;
  for (uint8_t i = 0; i < 16; i++)
  {
    analogA += analogRead(REMA);
    //delay(1);
  }
  analogA /= 16;
  //analogA = analogRead(REMA);
  //
  //  if (abs(ALLDOWN - analogA) < DELTA || (millis() - AllDownChTame > 3000)
  if ((abs(ALLDOWN - analogA) < DELTA && millis() - time > 1000) ||
      (millis() - time > 2000 && millis() - AllDownChTame > 2000 && !flag))
  {
    ALLDOWN = analogA;
    AllDownChTame = millis();
  }

  analogA = analogRead(REMA);
  /*  analogA = 0;
  for (uint8_t i = 0; i < 8; i++)
  {
    analogA += analogRead(REMA);
    //delay(1);
  }
  analogA /= 8;*/

  if ((abs(ALLDOWN - analogA) < DELTA - 20))
  {
    time = 0;
    flag = false;
  }

  if (abs(ALLDOWN - analogA) > DELTA && time == 0)
  {
    time = millis();

    if (flag)
      time -= (HOLDTIME - 100);

    do
    {
    } while (abs(ALLDOWN - analogRead(REMA)) > DELTA && millis() - time <= HOLDTIME);

    if (millis() < time + HOLDTIME && flag == false)
    {

      if (abs(ALLDOWN - analogA - KVUP) < DELTA) //кнопка на руле VOL +
      {
        // return VOLUP;
        // delay(30);
        return VOLUP;
      }

      if (abs(ALLDOWN - analogA - KVDW) < DELTA) //кнопка на руле VOL-
      {
        //return VOLDOWN;
        //delay(100);
        return VOLDOWN;
      }

      if (abs(ALLDOWN - analogA - KNEX) < DELTA) //кнопка на руле UP
      {
        return TRACKFORW;
      }

      if (abs(ALLDOWN - analogA - KBAC) < DELTA) //кнопка на руле PRESET
      {
        return TRACKBACK;
      }

      if (abs(ALLDOWN - analogA - KMOD) < DELTA) //кнопка на руле MODE
      {
        return MODE;
      }

      /*if (abs(ALLDOWN - analogA - KSOR) < DELTA) //кнопка на руле MUTE (пауза)
      {
        //return SOURCE;
        // delay(50);
        return PAUSE;
      }*/
      /*
      if (abs(ALLDOWN - analogA - KOFF) < DELTA) //кнопка на руле ON OFF (поднять трубку\выйти)
      {
        return PHONE;
      }

      if (abs(ALLDOWN - analogA - KCAN) < DELTA) //кнопка на руле CANCEL (назад)
      {
        return SEARCH;
      }

      if (abs(ALLDOWN - analogA - KRES) < DELTA) //кнопка на руле NEXT
      {
        //return TRACKFORW;
        // delay(100);
        // flag = false;
        return TRACKFORW;
      }

      if (abs(ALLDOWN - analogA - KSET) < DELTA) //кнопка на руле PREVIOUS
      {
        //return TRACKBACK;
        // delay(100);
        return TRACKBACK;
      }*/
    }
    //----------------------Удержание кнопки----------------------------
    else if (millis() >= time + HOLDTIME)
    {

      if (abs(ALLDOWN - analogA - KVUP) < DELTA) //удержание кнопки на руле VOL+
      {
        time = 0;    //нажатая кнопка повторяется
        flag = true; //метка что кнопка может повторится
        return VOLUP;
      }

      if (abs(ALLDOWN - analogA - KVDW) < DELTA) //удержание кнопки на руле VOL-
      {
        time = 0;    //нажатая кнопка повторяется
        flag = true; //метка что кнопка может повторится
        return VOLDOWN;
      }
      /*
      if (abs(ALLDOWN - analogA - KRES) < DELTA) //удержание кнопки на руле PREVIOUS
      {
        time = 0;
        flag = true;
        return TRACKFORW;
      }

      if (abs(ALLDOWN - analogA - KSET) < DELTA) //удержание кнопки на руле PREVIOUS
      {
        time = 0;
        flag = true;
        return TRACKBACK;
      }
*/
      if (abs(ALLDOWN - analogA - KNEX) < DELTA) //удержание кнопки на руле NEXT
      {
        //flag = true; //(убрал штрих)
        return FOLDERFORW;
      }

      if (abs(ALLDOWN - analogA - KBAC) < DELTA) //удержание кнопки на руле PREVIOUS
      {
        //flag = true;
        return FOLDERBACK;
      }

      if (abs(ALLDOWN - analogA - KMOD) < DELTA) //удержание кнопки на руле MODE
      {
        flag = false;
        return ATT;
      }

      /*if (abs(ALLDOWN - analogA - KSOR) < DELTA) //удержание кнопки на руле MUTE (mute)
      {
        //flag = true;
        return ATT;
      }*/
    }
  }
  return 0;
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