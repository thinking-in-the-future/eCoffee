#define ActiveLedPin 13
// 20180920 fix Runjob using Showcolor' problems , instead of using 
//  ChangeBulbColor(RedValue,GreenValue,BlueValue,LightValue) ;


//-------------------------
#include <EEPROM.h>
#define eDataAddress 100
int ParaCount = 0 ;
int PreHotParameter[2] = {180,170};
int HotParameter[10][2] = { 
                            {180 ,150 },{360 ,175 },
                            {660 ,210 },{0 ,0 }, 
                            {0 ,0 },{0 ,0 }, 
                            {0 ,0 },{0 ,0 }, 
                            {0 ,0 },{0 ,0 }
};
//--------------------
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
//----------------------
//----------------------
#include <Keypad.h>
char KeyPadChar[] = {'0','1','2','3','4','5','6','7','8','9','*','#','A','B','L','R','U','D','E','X'} ;
char KeyPadNum[] = {'0','1','2','3','4','5','6','7','8','9','L' ,'E','X'} ;
char KeyPadPage[] = {'U','D','E','X'} ;
char KeyPadCmd[] = {'A','B','#','*'} ;
char KeyPadYesNo[] = {'0','1'} ;
const byte ROWS = 4; //four rows
const byte COLS = 5; //three columns
char keys[ROWS][COLS] = 
  {
    {'L','7','4','1','A'},
    {'0','8','5','2','B'},
    {'R','9','6','3','#'},
    {'X','E','D','U','*'}
  };
  
byte rowPins[ROWS] = {22, 24, 26, 28 }; //connect to the row pinouts of the keypad
byte colPins[COLS] = {30 ,32,34 ,36 ,38}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS );

//--------------------
boolean InputStatus = false ;
boolean WorkStatus = false ;
unsigned stepnum = 1 ;
unsigned posnum = 0 ;
char inptxt[3]  ;
int pos1[3][2] = {{1,13},{1,14},{1,15}} ;
int pos2[3][2] = {{2,13},{2,14},{2,15}} ;
char pchar ;
int rulenum = 1 ;
//--------------------

//---------------

#define PIN1            7

void setup()
{
  initAll() ;
   ShowStartUP() ;
  delay(1000) ;
  ShowScreen() ;
           
}


void loop()
{
     Dialog("F1 PreHot,'F2 Run") ;
  
  pchar = InstantKeyInput() ;
  Serial.print("[") ;
  Serial.print(pchar) ;
   Serial.print("]\n") ;  
 if ( CheckKeyPadCMD(pchar)=='A' )
  {
    Serial.println("Run Pre Hot") ;
    InputStatus = true ;
    Dialog("Run Pre Hot") ;
      //initinptxt() ;
  }   // end of ( CheckKeyPadStart(InstantKeyInput()) )

  if ( CheckKeyPadCMD(pchar)=='B' )
  {
    Serial.println("Run Bean Roaster") ;
    InputStatus = true ;
    Dialog("Run Bean Roaster") ;
  }   // end of ( CheckKeyPadStart(InstantKeyInput()) )
 if ( CheckKeyPadCMD(pchar)=='#' )
  {
    Serial.println("Set Pre Hot") ;
    Dialog("Set Pre Hot") ;
      if (SetPreHot())
            savePreHotParameter() ;
         HideInputCursor() ;     
        ShowScreen() ;
        Dialog("F1 PreHot,'F2 Run") ;
  }   // end of ( CheckKeyPadStart(InstantKeyInput()) )
 if ( CheckKeyPadCMD(pchar)=='*' )
  {
    Serial.println("Set Temperature & Time") ;
    HotMenu() ;
      ShowScreen() ;
    Dialog("F1 PreHot,'F2 Run") ;
      //initinptxt() ;
  }   // end of ( CheckKeyPadStart(InstantKeyInput()) )








}   // end of loop
//---------------

boolean HotMenu()
{
    Dialog("UP/Down Enter:Edit") ;
    pchar =1  ;
    while ( true)
    {
        if (pchar > 0 ) 
        {
            Serial.print("Menu:(") ;
            Serial.print(pchar) ;
            Serial.print(")\n") ;
            Dialog("UP/Down Enter:Edit") ;
            if (CheckPageView(pchar) =='U')
                rulenum -- ;
             if (CheckPageView(pchar) =='D')
                rulenum ++ ;
             if (rulenum >10)
                  rulenum = 1 ;
             if (rulenum < 1)
                  rulenum = 10 ;
            PageViewScreen(rulenum,HotParameter[rulenum-1][0], HotParameter[rulenum-1][1], "View Control Rules" ) ;

            }
      if (CheckPageView(pchar) =='X') 
      {
          if (SetHot(rulenum))
            {
                saveHotParameter(rulenum) ;
            }
        HideInputCursor() ;    
            PageViewScreen(rulenum,HotParameter[rulenum-1][0], HotParameter[rulenum-1][1], "View Control Rules" ) ;
        
      }
          if (CheckPageView(pchar) =='E')
              {
                return true ;
              }      
        pchar = InstantKeyInput() ; 
      }
}   // end of function

boolean SetHot(int rl)
{
    Dialog("ESC Abort, Enter OK") ;
      int temp1 ,temp2 ;
      temp1 = HotParameter[rl-1][0] ;
      temp2 = HotParameter[rl-1][1] ;
      
     OperatingScreen(0, temp1 , temp2 , "Set Hot & Time") ;
          Serial.println("Wait for input for Controling Temperature time") ;
          Dialog("Input Hot Time") ;
        posnum = 0 ;
        Setinptxt(temp1) ;
        while (posnum < 3)  // input six number for schedule
          {
              ShowInputCursor(pos1[posnum][0],pos1[posnum][1])   ;
              pchar = CheckInputfromKeyPad(KeyInput()) ;
              // check input backward
              if (pchar == 'L')
              {
                  if (posnum > 0)
                    {
                      posnum -- ;
                    }
                  continue  ;
              }   // end of pchar == '*' check
              if (pchar == 'E')
              {
                    return false ;
              }          
              if (pchar == 'X')
              {
                      temp1 = ((int)inptxt[0]-48)*100+((int)inptxt[1]-48)*10+((int)inptxt[1]-48) ;
                       Serial.println(temp1) ;
                       Serial.println(temp2) ;
                      
                   pchar = CheckYesNo(KeyYesNo()) ;
                    if (pchar == '0')
                      {
                          return false ;
                      }
                      else
                      {
                              
                          HotParameter[rl-1][0] =temp1  ;
                          HotParameter[rl-1][1] = temp2 ;
                          return true ;
                      }                       
              }   
              // now check real input
            if (pchar != 0)
            {
                inptxt[posnum] = pchar ;
                ShowInputChar(pos1[posnum][0],pos1[posnum][1], inptxt[posnum])  ;
                posnum ++ ;
                
            }

          }   // end of while (posnum < 3) for Check  six char input
        temp1 = ((int)inptxt[0]-48)*100+((int)inptxt[1]-48)*10+((int)inptxt[1]-48) ;
        Serial.println(temp1) ;
        
           Serial.println("Wait for input for Control Temperature") ;
          Dialog("Input Temperature") ;
        posnum = 0 ;
        Setinptxt(temp2) ;
        while (posnum < 3)  // input six number for schedule
          {
              ShowInputCursor(pos2[posnum][0],pos2[posnum][1])   ;
              pchar = CheckInputfromKeyPad(KeyInput()) ;
              // check input backward
              if (pchar == 'L')
              {
                  if (posnum > 0)
                    {
                      posnum -- ;
                    }
                  continue  ;
              }   // end of pchar == '*' check
              if (pchar == 'E')
              {
                    return false ;
              }    
               if (pchar == 'X')
              {
                      temp2 = ((int)inptxt[0]-48)*100+((int)inptxt[1]-48)*10+((int)inptxt[2]-48) ;
                      Serial.println(temp2) ;
                      
                   pchar = CheckYesNo(KeyYesNo()) ;
                    if (pchar == '0')
                      {
                          return false ;
                      }
                      else
                      {
                              
                          HotParameter[rl-1][0] =temp1  ;
                          HotParameter[rl-1][1] = temp2 ;
                          return true ;
                      }                       
              }              
              // now check real input
            if (pchar != 0)
            {
                inptxt[posnum] = pchar ;
                ShowInputChar(pos2[posnum][0],pos2[posnum][1], inptxt[posnum])  ;
                posnum ++ ;
                
            }

          }   // end of while (posnum < 3) for Check  six char input
        temp2 = ((int)inptxt[0]-48)*100+((int)inptxt[1]-48)*10+((int)inptxt[2]-48) ;
        Serial.println(temp2) ;
        
     pchar = CheckYesNo(KeyYesNo()) ;
      if (pchar == '0')
        {
            return false ;
        }
        else
        {
                
            HotParameter[rl-1][0] =temp1  ;
            HotParameter[rl-1][1] = temp2 ;
            return true ;
        }
}   // end of function

boolean SetPreHot()
{
    Dialog("ESC Abort, Enter OK") ;
      int temp1 ,temp2 ;
      temp1 = PreHotParameter[0] ;
      temp2 = PreHotParameter[1] ;
      Serial.print("Prehot:(") ;
      Serial.print(temp1) ;
      Serial.print("/") ;
      Serial.print(temp2) ;
      Serial.print(")\n") ;
     OperatingScreen(0, temp1 , temp2 , "Set Pre Hot & Time") ;
          Serial.println("Wait for input for PreHot Time") ;
          Dialog("Input PreHot Time") ;
        posnum = 0 ;
        Setinptxt(temp1) ;
        while (posnum < 3)  // input six number for schedule
          {
              ShowInputCursor(pos1[posnum][0],pos1[posnum][1])   ;
              pchar = CheckInputfromKeyPad(KeyInput()) ;
              // check input backward
              if (pchar == 'L')
              {
                  if (posnum > 0)
                    {
                      posnum -- ;
                    }
                  continue  ;
              }   // end of pchar == '*' check
              if (pchar == 'E')
              {
                    return false ;
              }               // now check real input
              if (pchar == 'X')
              {
                temp1 = ((int)inptxt[0]-48)*100+((int)inptxt[1]-48)*10+((int)inptxt[2]-48) ;
                  Serial.println(temp2) ;
                  
               pchar = CheckYesNo(KeyYesNo()) ;
                if (pchar == '0')
                  {
                      return false ;
                  }
                  else
                  {
                          
                      PreHotParameter[0] = temp1  ;
                      PreHotParameter[1] = temp2 ;
                      return true ;
                  }
              }               // now check real input              
            if (pchar != 0)
            {
                inptxt[posnum] = pchar ;
                ShowInputChar(pos1[posnum][0],pos1[posnum][1], inptxt[posnum])  ;
                posnum ++ ;
                
            }

          }   // end of while (posnum < 3) for Check  six char input
        temp1 = ((int)inptxt[0]-48)*100+((int)inptxt[1]-48)*10+((int)inptxt[2]-48) ;
        Serial.println(temp1) ;
        
           Serial.println("Wait for input for prehot Temperature") ;
          Dialog("InputTemperature") ;
        posnum = 0 ;
        Setinptxt(PreHotParameter[1]) ;
        while (posnum < 3)  // input six number for schedule
          {
              ShowInputCursor(pos2[posnum][0],pos2[posnum][1])   ;
              pchar = CheckInputfromKeyPad(KeyInput()) ;
              // check input backward
              if (pchar == 'L')
              {
                  if (posnum > 0)
                    {
                      posnum -- ;
                    }
                  continue  ;
              }   // end of pchar == '*' check
              if (pchar == 'E')
              {
                    return false ;
              }               // now check real input
              if (pchar == 'X')
              {
                  Serial.println(temp1) ;
                 temp2 = ((int)inptxt[0]-48)*100+((int)inptxt[1]-48)*10+((int)inptxt[2]-48) ;
                Serial.println(temp2) ;
                 
               pchar = CheckYesNo(KeyYesNo()) ;
                if (pchar == '0')
                  {
                      return false ;
                  }
                  else
                  {
                          
                      PreHotParameter[0] = temp1  ;
                      PreHotParameter[1] = temp2 ;
                      return true ;
                  }
              }                   
            if (pchar != 0)
            {
                inptxt[posnum] = pchar ;
                ShowInputChar(pos2[posnum][0],pos2[posnum][1], inptxt[posnum])  ;
                posnum ++ ;
                
            }

          }   // end of while (posnum < 3) for Check  six char input
        temp2 = ((int)inptxt[0]-48)*100+((int)inptxt[1]-48)*10+((int)inptxt[2]-48) ;
        Serial.println(temp2) ;
        
     pchar = CheckYesNo(KeyYesNo()) ;
      if (pchar == '0')
        {
            return false ;
        }
        else
        {
                
            PreHotParameter[0] = temp1  ;
            PreHotParameter[1] = temp2 ;
            return true ;
        }
}   // end of function

void PageViewScreen(int st,int temp, int durtme, String Ti)
{
  lcd.clear() ;
   lcd.setCursor(0,0);
  lcd.print(Ti);
   lcd.setCursor(0,1);
  lcd.print("Rule:(") ;
    lcd.print(st) ;
  lcd.print(")") ;    
   lcd.setCursor(0,2);
   lcd.print("Time:") ;
    lcd.print(strzero(temp,3,10)) ;    
    lcd.print(" , Temp:") ;
    lcd.print(strzero(durtme,3,10)) ;  
    
}

void OperatingScreen(int st,int temp, int durtme, String Ti)
{
  lcd.clear() ;
   lcd.setCursor(0,0);
  lcd.print(Ti);
   lcd.setCursor(0,1);
  lcd.print("Stage:") ;
    lcd.print(st) ;
   lcd.setCursor(8,1);
  lcd.print("Time:") ;
    lcd.print(strzero(temp,3,10)) ;    
    lcd.setCursor(0,2);
  lcd.print("Temperature :") ;
    lcd.print(strzero(durtme,3,10)) ;  
    
}


//-------------
char CheckInputfromKeyPad(char kp)
{
    for(int i = 0 ; i <  sizeof(KeyPadNum) ; i++)
      {
          if  (KeyPadNum[i]  == kp) 
              return kp ;
      }
    return 0 ;
}





void ShowScreen()
{
  lcd.clear() ;
   lcd.setCursor(0,0);
  lcd.print("Coffee Bean Roaster");
}





void ShowStartUP()
{
  lcd.clear() ;
   lcd.setCursor(0,0);
  lcd.print("Coffee Bean Roaster");
   lcd.setCursor(0,1);
  lcd.print("Research by NCNU EEE") ;
   lcd.setCursor(0,2);
  lcd.print(" Dr.Yaw-Wen Kuo");
   lcd.setCursor(0,3);
  lcd.print(" Dr.YC Tsao");

  
}

void initAll()
{
  Serial.begin(9600) ;
   Serial1.begin(9600); //  
   pinMode(ActiveLedPin,OUTPUT) ;
   digitalWrite(ActiveLedPin,HIGH) ;
    delay(200)  ;
   digitalWrite(ActiveLedPin,LOW) ;
    
  Serial.println("Program Start") ;
  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.clear() ;

  ParaCount = EEPROM.read(eDataAddress);
  if (!(ParaCount >=0 && ParaCount <=99))
      {
        ParaCount = 0 ;
      }
 

}

void Dialog(String str)
{
    DialogClear() ;
   lcd.setCursor(0,3);                                                          
   lcd.print(str);  
}

void DialogClear()
{
     lcd.setCursor(0,3);                                                          
   lcd.print("                    ");  
}

//--------------
char CheckPageView(char kp)
{
    for(int i = 0 ; i <  sizeof(KeyPadPage) ; i++)
      {
          if  (KeyPadPage[i]  == kp) 
              return kp ;
      }
    return 0 ;
}

char CheckKeyPadCMD(char kp)
{
    for(int i = 0 ; i <  sizeof(KeyPadCmd) ; i++)
      {
          if  (KeyPadCmd[i]  == kp) 
              return kp ;
      }
    return 0 ;
}


char InstantKeyInput()
{
    char key ;
    long st= millis() ;
    while (1)
    {
      if ((millis() - st) > 800)
        break ;
      
      key = keypad.getKey();
      
       if (key == 0)
          {
            delay(20) ;
            // Serial.println("Wait.....") ;
          }
          else
          {
           //  Serial.println("Got Key") ;
             break ;
            // delay(3000) ;
          }
   }
   return  key ;
}

char KeyInput()
{
    char key ;
    while (true)
    {
      key = keypad.getKey();
        Serial.print("[") ;
       Serial.print(key,HEX) ;
        Serial.print("]\n") ;
         if (key == 0)
            continue ;
        if (CheckKeyPadChar(key) >0)
        {
          //  Serial.print("Bingo\n") ;
            return  key ;
        }   
   }
   return  key ;
}
char CheckKeyPadChar(char kp)
{
    for(int i = 0 ; i <  sizeof(KeyPadChar) ; i++)
      {
          if  (KeyPadChar[i]  == kp) 
              return kp ;
      }
    return 0 ;
}
void ShowInputChar(int y, int x, char ct)
{
          lcd.setCursor(x,y);
          lcd.print(ct) ;
  
}
void HideInputCursor()
{
          lcd.noBlink();
  
}

void ShowInputCursor(int y, int x)
{
          lcd.setCursor(x,y);
          lcd.blink() ;
  
}

void Setinptxt(int no)
{
    String tmp = strzero(no,3,10) ;
   for(int i=0 ; i <3; i++)
      inptxt[i] = (char)(tmp.substring(i,i).toInt()+48) ;
}
char KeyYesNo()
{
    char key ;
    Dialog("0=Abort,1=Proceed") ;
    while (true)
    {
      key = keypad.getKey();
      //  Serial.print("[") ;
      //  Serial.print(key,HEX) ;
      //  Serial.print("]\n") ;
         if (key == 0)
            continue ;
        if (CheckYesNo(key) >0)
        {
          //  Serial.print("Bingo\n") ;
            return  key ;
        }   
   }
   return  key ;
}
char CheckYesNo(char kp)
{
    for(int i = 0 ; i <  sizeof(KeyPadYesNo) ; i++)
      {
          if  (KeyPadChar[i]  == kp) 
              return kp ;
      }
    return 0 ;
}

void savePreHotParameter()
{
    EEPROM.write(eDataAddress , PreHotParameter[0] ) ;
    EEPROM.write((eDataAddress +5), PreHotParameter[1]  ) ;
}
void saveHotParameter(int rnum)
{
    EEPROM.write((eDataAddress+100 +(rnum -1) * 10), HotParameter[rnum -1][0] ) ;
    EEPROM.write((eDataAddress+100 +5+(rnum -1) * 10), HotParameter[rnum -1][1] ) ;
}

void RestoreParemeter()
{
  
}

