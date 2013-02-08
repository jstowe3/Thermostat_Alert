/****** MikroDisplay.c *************************************************
 *   @HANDLE = SWV
 *   @Author = Joseph Stowe AND Won Hwang
 *   10ms main loop time using T5IF.
 *   Print handle name with border and background color on 2nd row
 *   and centered on the screen.
 *   Measure time to print handle and it's box using Start, Stop
 *   and Send functions. Display measured time on bottom right corner                           
 *   of display. Characters are created on a 12x16 pixel matrix.
 *   Set and clear RD0 of microboard to measure BlinkAlive execution time
 *   CPU clock = 16 MHz.
 **********************************************************************/
#include "p24FJ256GB110.h"       // PIC24 register and bit definitions
#include "AlphaFont.h"           // 12x16-pixel font set
#include "Mikro.c"               // LCD variables, functions, macros
#include "MikroMeasureTime.c"    // Start, Stop, Send, ASCIIn, Blankn

/****** Configuration selections **************************************/
_CONFIG1(JTAGEN_OFF & GWRP_OFF & FWDTEN_OFF & ICS_PGx2);
_CONFIG2(PLLDIV_DIV2 & POSCMOD_HS & FNOSC_PRIPLL & IOL1WAY_OFF);

/****** Global variables **********************************************/
unsigned int ALIVECNT = 99;

//char RowOnestr[] = "\001\00112345678901234567890123456";
//char RowTwostr[] = "\002\001ABCDEFGHIJKLMNOPQRSTUVWXYZ";
//char RowThrstr[] = "\003\001abcdefghijklmnopqrstuvwxyz";
//char RowFoustr[] = { 4, 6, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25,
   // 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x00
//};
//char RowFivstr[] = "\005\0060123456789:;<=>?";
//char RowSixstr[] = "\006\006@ABCDEFGHIJKLMNO";
//char RowSevstr[] = "\007\006PQRSTUVWXYZ[\\]^_";
//char RowNineStr[] = "\014\006   ";
char HANDLEstr[] = "\002\014SWV";

/****** Function prototypes *******************************************/
void Initial(void);
void BlinkAlive(void);

//////// Main program //////////////////////////////////////////////////
int main()
{
   Initial();

   Start();                      // Measure execution time from here
   // DrawRectangle(132, 23, 176, 48, RGB(100, 200, 85)); // Draw the handle box
   Box(2, 12, 2, 14, RGB(0, 255, 0));   // Draw white border box around handle
   Display(RGB(0, 255, 0), HANDLEstr);  // Display the handle string
   Stop();                       // Stop measuring execution time here
   // DrawRectangle(193, 193, 308, 215, RGB(0, 255, 0)); // Draw the measurement box
   Box(9, 17, 9, 25, BKGD);      // Draw border around time measurement
   Send(9, 17);                  // Display the time measurement

   while (1)
   {
      // _LATA3 ^= 1; // Toggle pin 50 of Mikro board

      _LATD0 = 1;                // Set pin 18
      BlinkAlive();
      _LATD0 = 0;                // Clear pin 18
      while (!_T5IF) ;           // Loop time = 10 ms
      _T5IF = 0;
   }
}

/****** Initial ********************************************************
 *
 * Initialize LCD Screen (PMP + configuration + initial display).
 * Initialize Timer5 for a loop time of 10 ms.
 **********************************************************************/
void Initial()
{
   AD1PCFGL = 0xFFFF;            // Make all ADC pins default to digital pins
   PMP_Init();                   // Configure PMP module for LCD
   LCD_Init();                   // Configure LCD controller
   InitBackground();
   _TRISA3 = 0;                  // Make RA3 an output (pin 50 of Mikro board)
   _TRISD0 = 0;                  // Set RD0 for output pin
   TMR5 = 0;                     // Clear Timer5
   PR5 = 19999;                  // Set period of Timer5 to 10 ms
   T5CON = 0x8010;               // Clock Timer5 with Fcy/8 = 2 MHz

}

/****** BlinkAlive *****************************************************
 *
 * This function toggles a square of pixels every second.
 * With a loop time of 10 ms, this is every 100 looptimes.
 **********************************************************************/
void BlinkAlive()
{
   ALIVECNT++;
   if (ALIVECNT == 100)          // Write black square
   {
      DrawRectangle(0, 0, 5, 5, BLACK);
   }
   if (ALIVECNT >= 200)          // Clear black square
   {
      ALIVECNT = 0;
      DrawRectangle(0, 0, 5, 5, BKGD);
   }
}
