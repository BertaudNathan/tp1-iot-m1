#include <Arduino.h>

#include <LiquidCrystal.h>

#include <DIYables_Keypad.h>

#define C1 10
#define C2 11
#define C3 12
#define C4 13

#define L1 0
#define L2 1
#define L3 8
#define L4 9

const byte ligne = 4;
const byte colonne =4;



char matrix[ligne][colonne] =
{{'1','2','3','+'},
{'4','5','6','-'},
{'7','8','9','*'},
{'r','0','=','/'}};

byte pin_lignes[ligne] = {L1,L2,L3,L4};
byte pin_colonnes[colonne] = {C1,C2,C3,C4};

DIYables_Keypad clavier = DIYables_Keypad(makeKeymap(matrix),pin_lignes,pin_colonnes,ligne,colonne);

int seconds = 0;
String text1 = "";
LiquidCrystal lcd_1(7, 6, 5, 4, 3, 2);

void setup()
{
  lcd_1.begin(16, 2); // Set up the number of columns and rows on the LCD.
  text1 = "";
  // Print a message to the LCD.
  lcd_1.print(text1);
  
  Serial.begin(9600);
}

void write(String text, int column = 0){
  int i;
  for (i=0; i<= text.length(); i++){
    lcd_1.setCursor(i,column);
    lcd_1.print(text[i]);
  }
}

float calculateExpression(String expr) {
  // --- Étape 1 : Transformer l’expression en tableau de strings ---
  String tokens[20];  
  int tokenCount = 0;
  String current = "";

  for (int i = 0; i < expr.length(); i++) {
    char c = expr[i];

    if (c >= '0' && c <= '9') {
      current += c;              // accumulate numbers
    } else {
      if (current.length() > 0) {
        tokens[tokenCount++] = current;
        current = "";
      }
      tokens[tokenCount++] = String(c);   // operator
    }
  }
  if (current.length() > 0) {
    tokens[tokenCount++] = current;
  }

  // --- Étape 2 : traiter * et / ---
  for (int i = 0; i < tokenCount; i++) {
    if (tokens[i] == "*" || tokens[i] == "/") {
      float a = tokens[i - 1].toFloat();
      float b = tokens[i + 1].toFloat();
      float r = (tokens[i] == "*") ? (a * b) : (a / b);

      tokens[i - 1] = String(r);

      // Décaler le tableau
      for (int j = i; j < tokenCount - 2; j++) {
        tokens[j] = tokens[j + 2];
      }
      tokenCount -= 2;
      i = 0;  // on recommence
    }
  }

  // --- Étape 3 : traiter + et - ---
  float result = tokens[0].toFloat();
  for (int i = 1; i < tokenCount; i += 2) {
    String op = tokens[i];
    float value = tokens[i + 1].toFloat();

    if (op == "+") result += value;
    if (op == "-") result -= value;
  }

  return result;
}


void calculate(String text) {
  float result = calculateExpression(text);

  lcd_1.clear();
  lcd_1.setCursor(0, 0);
  lcd_1.print(text);

  lcd_1.setCursor(0, 1);
  lcd_1.print("= ");
  lcd_1.print(result);

  text1 = String(result);  // pour continuer à calculer
}
      
      bool isOperator(char character){
        return (character == '+' || character == '-' || character == '/' || character == '*');
      }


void SwitchChar(char pressed)
{
  if (pressed == 'r'){
    lcd_1.clear();
    lcd_1.setCursor(0, 0);
    text1="";
    return;
  }
  if (pressed == '='){
    calculate(text1);
    return;
  }
  
   if (isOperator(pressed)){
     char last = text1[text1.length()-1];
     if (isOperator(last)){
         return;
     }
  }
  
  if (pressed){
    text1 = text1 + pressed;
    write(text1);
    return;
  }
}

void loop()
{
  SwitchChar(clavier.getKey());

  
  
  //lcd_1.setCursor(0, 1);
  // print the number of seconds since reset:
  //lcd_1.print(seconds);
  //delay(1000); // Wait for 1000 millisecond(s)
  //seconds += 1;
}