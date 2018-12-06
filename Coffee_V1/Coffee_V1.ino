#include <LiquidCrystal.h>
#include <max6675.h>
#define board mega2560
/*
#ifdef board
   #if (board == uno)  
    #define thermo_vcc_pin   13 
    #define thermo_sck_pin   10
    #define thermo_cs_pin    11 
    #define thermo_so_pin   12  
  #else  
    #define thermo_vcc_pin     22 
    #define thermo_sck_pin    48 
    #define thermo_cs_pin    50 
    #define thermo_so_pin   52  
  #endif
#endif
*/
// ThermoCouple
    #define thermo_vcc_pin     22 
    #define thermo_sck_pin    48 
    #define thermo_cs_pin    50 
    #define thermo_so_pin   52
MAX6675 thermocouple(thermo_sck_pin, thermo_cs_pin, thermo_so_pin);
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
//used to control SSR
int control_pin = 22;   

// default time period in seconds
int phase1=180;  // phase 1 from 0 to 240s
int phase2=360;  // phase 2 from 241 to 500s
int phase3=660;  // phase 3 from 501 to 660s
// default target temperature for each phase
int phase1_temp=150;
int phase2_temp=175;
int phase3_temp=210;

int pre_temp=160; //pre-heat temperature

int count=0,pcount=0,sec=0,power,num,max_power,flag;  // count for time, pcount for power
float ini_temp,temp=0,old_temp, scale=0,target,est_scale;
String value;

#define PLEVEL 8
#define WAITSEC 6


void setup() {
  
  lcd.begin(16, 2); 
  lcd.setCursor(0, 0); // 設定游標位置在第一行行首
  // Debug console
  Serial.begin(9600);
  Serial.println("start");

 //ThermoCouple initialization
  pinMode(thermo_vcc_pin, OUTPUT); 
  digitalWrite(thermo_vcc_pin, HIGH);
  pinMode(control_pin, OUTPUT);
  digitalWrite(control_pin,LOW);   
 // pinMode(3, OUTPUT);
 //                 digitalWrite(3,LOW);   // SSR ground
  digitalWrite(control_pin,LOW);
 
// set up parameters for each phase

  int x;
  // get pre-heat temp  
  while(1)
  {
    lcd.clear();
    lcd.setCursor(0,0);  
    value= String("")+"Pre-heat(oC)="+pre_temp;  
    lcd.print(value);    
    x = analogRead (0);    // read key pad value to determine which key is pressed
    if ((x<200)&&(x>60))   // up key
        pre_temp=pre_temp+5;
    if ((x<400)&&(x>200))  // down key
        pre_temp=pre_temp-5;
    if ((x<800)&&(x>600))  // select key to break this loop
        break;    
    delay(100);            
  }
  while((x = analogRead (0))>800);  //debouncing
  delay(500);  //debouncing
  // get phase 1's end time  
  while(1)
  {
    lcd.clear();
    lcd.setCursor(0,0);  
    value= String("")+"phase1(sec)="+phase1;  
    lcd.print(value);    
    x = analogRead (0);
    if ((x<200)&&(x>60))
        phase1=phase1+5;
    if ((x<400)&&(x>200)) 
        phase1=phase1-5;
    if ((x<800)&&(x>600)) 
        break;    
    delay(100);            
  }
  while((x = analogRead (0))>800); 
  delay(500);  
  //get phase 2's end time
  while(1)
  {
    lcd.clear();
    lcd.setCursor(0,0);  
    value= String("")+"phase2(sec)="+phase2;  
    lcd.print(value);    
    x = analogRead (0);
    if ((x<200)&&(x>60))
        phase2=phase2+5;
    if ((x<400)&&(x>200)) 
        phase2=phase2-5;
    if ((x<800)&&(x>600)) 
        break;    
    delay(100);            
  }
  while((x = analogRead (0))>800);  
  delay(500); 
  //get phase 3's end time
  while(1)
  {
    lcd.clear();
    lcd.setCursor(0,0);  
    value= String("")+"phase3(sec)="+phase3;  
    lcd.print(value);    
    x = analogRead (0);
    if ((x<200)&&(x>60))
        phase3=phase3+5;
    if ((x<400)&&(x>200)) 
        phase3=phase3-5;
    if ((x<800)&&(x>600)) 
        break;    
    delay(100);            
  }
  while((x = analogRead (0))>800);  
  delay(500);
  // get phase 1's target temp     
  while(1)     
  {
    lcd.clear();
    lcd.setCursor(0,0);  
    value= String("")+"Temp1(oC)="+phase1_temp;  
    lcd.print(value);    
    x = analogRead (0);
    if ((x<200)&&(x>60))
        phase1_temp=phase1_temp+5;
    if ((x<400)&&(x>200)) 
        phase1_temp=phase1_temp-5;
    if ((x<800)&&(x>600)) 
        break;    
    delay(100);            
  }
  while((x = analogRead (0))>800);
  delay(500); 
  // get phase 2's target temp         
  while(1)
  {
    lcd.clear();
    lcd.setCursor(0,0);  
    value= String("")+"Temp2(oC)="+phase2_temp;  
    lcd.print(value);    
    x = analogRead (0);
    if ((x<200)&&(x>60))
        phase2_temp=phase2_temp+5;
    if ((x<400)&&(x>200)) 
        phase2_temp=phase2_temp-5;
    if ((x<800)&&(x>600)) 
        break;    
    delay(100);            
  }  
  while((x = analogRead (0))>800);
  delay(500);         
  // get phase 3's target temp         
  while(1)
  {
    lcd.clear();
    lcd.setCursor(0,0);  
    value= String("")+"Temp3(oC)="+phase3_temp;  
    lcd.print(value);    
    x = analogRead (0);
    if ((x<200)&&(x>60))
        phase3_temp=phase3_temp+5;
    if ((x<400)&&(x>200)) 
        phase3_temp=phase3_temp-5;
    if ((x<800)&&(x>600)) 
        break;    
    delay(100);            
  } 

  lcd.clear();
  lcd.setCursor(0,0); 
  lcd.print("Pre-heat");     
  digitalWrite(control_pin,HIGH);  //turn on heater
  while((x = thermocouple.readCelsius())<pre_temp)
  {
    lcd.setCursor(0,1);  
    value= String("")+"Temp="+x;  
    lcd.print(value); 
    Serial.println(value);
    delay(1000);   
  }  
  digitalWrite(control_pin,LOW);   //turn off heater
  lcd.clear();
  lcd.setCursor(0,0); 
  lcd.print("Press Select to");
  lcd.setCursor(0,1); 
  lcd.print("start"); 
  while(1)
  {
    delay(250);
    x = thermocouple.readCelsius();
    value= String("")+"start. Temp="+x; 
    lcd.setCursor(0,1); 
    lcd.print(value); 
    Serial.println(value);
    x = analogRead (0);
    if ((x<800)&&(x>600)) 
        break;       
  }

  ini_temp=thermocouple.readCelsius();
  lcd.setCursor(0, 1); // 設定游標位置在第一行行首
  value= String("")+"initial="+ini_temp;
  lcd.print(value);   
  scale=(phase1_temp-ini_temp)/phase1;
  if (scale<0)
    scale=0;

//  Serial.println(scale);
  target=ini_temp;
  est_scale = scale;
  old_temp = ini_temp;


  if (scale>0.15) 
    max_power=6;
  else
    max_power=5;
  power=max_power;  // initial power level
  
  flag=0;
  num=0;
  Serial.println(value);
  Serial.println(scale);
  Serial.println(target); 
  delay(500); 
}  


void loop() {
    int out,x;
    delay(250);  //read temp for every 250ms 
    temp=temp+thermocouple.readCelsius();

    count++;
    pcount++;

    if (pcount==PLEVEL) pcount=0;

    
    if (count==4)   // process every second
    {
      sec++;
      lcd.clear();    
      temp=temp/4;  //average temp for every second
      count=0;

      
      target=target+scale;
      if (sec==phase1)
      {
        scale=(phase2_temp-temp)*1.0/(phase2-phase1);
        target=temp+scale;
        max_power=5;
 //         Serial.println(scale);
      }
      if (sec==phase2)
      {
        scale=(phase3_temp-temp)*1.0/(phase3-phase2);
        target=temp+scale;
        max_power=5;
 //         Serial.println(scale);
      }
      if (sec==phase3)
      {
		    scale=0;  //freeze target temp
        max_power=1;
      }
      // power control  
//     num++;
      if ((sec%10)==0) //power control for every 10 seconds
      {
        est_scale=(temp-old_temp)/WAITSEC;
        old_temp=temp;
                
        if (power==0)  power=1; 
        if ((temp>(target+3)) && (power >0)) power--;
        if ((temp<(target-3)) && (power <max_power)) power++;
        if ((temp<(target+3)) && (est_scale <0)&& (power <max_power)) power++;  //boost power when slope is negative
   //     if (power==0) num=0;
        value= String("         ")+power+" "+scale+" "+est_scale+" "+max_power; 
        Serial.println(value);   
      }
     

      //LCD output

      value= String("")+phase1+" "+phase1_temp+" "+phase2+" "+phase2_temp;
      lcd.clear();
      lcd.setCursor(0, 0); // 設定游標位置在第一行行首
      lcd.print(value);
 //     Serial.println(value); 
      
      value= String("")+sec+" "+(int)target+" "+temp+" "+power;     
      lcd.setCursor(0, 1); // 設定游標位置在第二行行首
      lcd.print(value);
      Serial.println(value);
      
  
      temp=0;  // new cycle         
    }
    //heater control
    if (pcount<power) out=HIGH;
    if (pcount>=power) out=LOW;
    digitalWrite(control_pin,out);

    // freeze the temp at phase 3. You can press select at any time to freeze target temp
    x = analogRead (0);
    if ((x<800)&&(x>600)&&(sec>phase2)) 
        scale=0;
	// manually increase max_power at any time
    if ((x<200)&&(x>60)&&(max_power<8))
    {
        max_power=max_power+1;
        power=max_power;
      lcd.clear();
      lcd.setCursor(0, 0); // 設定游標位置在第一行行首
      lcd.print("max power up");
    }
	// manually decrease max_power at any time
    if ((x<400)&&(x>200)&&(max_power>2)) 
    {
        max_power=max_power-1;
        power=max_power;
      lcd.clear();
      lcd.setCursor(0, 0); // 設定游標位置在第一行行首
      lcd.print("max power down");        
    }                 
}

void initAll()
{
  
}

