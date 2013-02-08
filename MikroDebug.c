/****** MikroDebug.c *****************************************************
 *
 * @HANDLE = SWV
 * @AUTHOR = JOSEPH STOWE AND WON HWANG
 * 
 * Debugging functions created and tested for turning on two LED's on the
 * top right corner of the display. Display an int or char as binary on
 * a specified row using DisplayInt and DisplayByte.
 **********************************************************************/



/****** Global variables **********************************************/




/****** Function prototypes *******************************************/

void DisplayByte(char Row, char NUM);
void DisplayInt(char Row, int NUM);
void LED1on(void);
void LED1off(void);
void LED2on(void);
void LED2off(void);

//void BlinkAlive2(void);

/****** Macros ********************************************************/

#define WHITE RGB(255,255,255)


/****** DisplayByte ********************************************************
 *
 * Display NUM character at specified Row
 *
 **********************************************************************/
void DisplayByte(char Row, char NUM)
{
   char OutStr[] = "\001\02300000000";

   OutStr[0] = Row;
   int i;                       // Index into OutStr

   for (i = 1; i <= 8; i++)
   {
      OutStr[10 - i] = '0' + (NUM & 0b00000001);
      NUM = NUM >> 1;
   }

   Display(BKGD, OutStr);
}

/****** DisplayInt ********************************************************
 *
 * Display NUM integer at specified Row
 *
 **********************************************************************/
void DisplayInt(char Row, int NUM)
{
   char OutStr[] = "\001\0130000000000000000";

   OutStr[0] = Row;
   int i;                       // Index into OutStr

   for (i = 1; i <= 16; i++)
   {
      OutStr[18 - i] = '0' + (NUM & 0b0000000000000001);
      NUM = NUM >> 1;
   }

   Display(BKGD, OutStr);
}

/****** LED2on ********************************************************
 *
 * Turn on white "LED" at Row 1, Col 26 of the display.
 *
 **********************************************************************/
void LED2on()
{
   DrawRectangle(305, 0, 316, 3, WHITE);        // Draw white LED
}

/****** LED2off ********************************************************
 *
 * Turn off white "LED" at Row 1, Col 26 of the display.
 *
 **********************************************************************/
void LED2off()
{
   DrawRectangle(305, 0, 316, 3, BKGD); // Erase white LED
}

/****** LED1on ********************************************************
 *
 * Turn on white "LED" at Row 1, Col 24 of the display.
 *
 **********************************************************************/
void LED1on()
{
   DrawRectangle(281, 0, 292, 3, WHITE);        // Draw white LED
}

/****** LED1off ********************************************************
 *
 * Turn off white "LED" at Row 1, Col 24 of the display.
 *
 **********************************************************************/
void LED1off()
{
   DrawRectangle(281, 0, 292, 3, BKGD); // Erase white LED
}


