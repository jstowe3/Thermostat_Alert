/****** I2C.c **********************************************************
 *
 * I2C definitions, variables, and functions.
 * Timing tweeked to generate an I2C clock rate of 100 kHz.
 *
 * These functions are intended for off-board I2C devices
 * using Mikro pins RG13 for SCL and RG14 for SDA, with 1.0 kohm pullup
 * resistor to 3.3V connected to each one.
 *
 * (The on-board accelerometer uses RA2 for SCL and RA3 for SDA.)
 *
 * SHT15 sensor functions have been added to enable interfacing with the
 * sensor's unique 2-wire digital interface. The SHT15 needs special
 * functions since it does not follow the I2C protocol.
 *
 * Based on "Using the I2C Bus" from Robot Electronics
 * http://www.robot-electronics.co.uk/acatalog/I2C_Tutorial.html
 **********************************************************************/

/****** Function prototypes *******************************************/
void Dly10(void);
void Dly5(void);
void Dly2(void);
void i2c_start(void);
void i2c_stop(void);
char i2c_read(char ack);
char i2c_write(char d);
int sht15_read_byte16(void);
void sht15_command(char byteout);
void sht15_start(void);


/****** Macros ********************************************************/
// For I2C devices not on Mikro Board, use Mikro pins RG13 and RG14
#define SCL  _LATG13
#define SDA  _LATG14
#define ReadSDA _RG14
#define ODCR  _ODG13 = 1; _ODG14 = 1; _TRISG13 = 0; _TRISG14 = 0

/****** Dly10 **********************************************************
 *
 * Pause for 10 us.
 **********************************************************************/
void Dly10()
{
   int Counter = 38;

   while (--Counter) ;
}

/****** Dly5 ***********************************************************
 *
 * Pause for 5 us.
 **********************************************************************/
void Dly5()
{
   int Counter = 18;

   while (--Counter) ;
}

 /****** Dly2 **********************************************************
 *
 * Pause for 2 us.
 **********************************************************************/
void Dly2()
{
   int Counter = 6;

   while (--Counter) ;
}

/****** i2c_start ******************************************************
 *
 * i2C start sequence
 **********************************************************************/
void i2c_start()
{
   ODCR;                         // Use open-drain outputs with SCL and SDA
   SDA = 1;
   Dly10();
   SCL = 1;
   Dly10();
   SDA = 0;                      // Drop data line while clock line is high
   Dly10();
   SCL = 0;
}

/****** SHT15_start ******************************************************
 *
 * Start sequence initiating communication with the SHT15 sensor.
 **********************************************************************/
void sht15_start()
{
	_TRISG13 = 0;                // Set SCK pin to output
	_TRISG14 = 0;                // Set Data pin to output
	SDA = 1;
	SCL = 1;
	SDA = 0;
	SCL = 0;
	SCL = 1;
	SDA = 1;
	SCL = 0;
}

/****** i2c_stop *******************************************************
 *
 * i2C stop sequence
 **********************************************************************/
void i2c_stop()
{
   SDA = 0;
   Dly10();
   SCL = 1;
   Dly10();
   SDA = 1;                      // Raise data line while clock line is high
   Dly10();
}

/****** i2c_write ******************************************************
 *
 * Send a byte to the I2C device.
 * Return a value of 1 if reception of the byte was acknowledged.
 * Otherwise return a value of 0.
 **********************************************************************/
char i2c_write(char byteout)
{
   char x;
   char ack;

   for (x = 8; x; x--)           // Send eight data bits
   {                             //   most-significant bit first
      if (byteout & 0x80)
         SDA = 1;
      else
         SDA = 0;
      Dly2();
      SCL = 1;
      Dly5();
      SCL = 0;
      Dly2();
      byteout <<= 1;
   }
   SDA = 1;                      // Release SDA
   Dly2();
   SCL = 1;
   Dly5();
   ack = !ReadSDA;               // Read back acknowledge bit !SDA
   SCL = 0;
   Dly10();

   return ack;
}

/****** sht15_command *************************************************
 *
 * Send a byte to the SHT15.
 * This is '000' address followed by 5-bit command code
 * We use this method to query the SHT15 for temp and humidity reads
 * 
 **********************************************************************/
void sht15_command(char byteout)
{
   char x;
   //char ack;

	_TRISG14 = 0;  // Set Data pin to output
	
   for (x = 8; x; x--)           // Send eight data bits
   {                             //   most-significant bit first
      SCL = 0;
	  if (byteout & 0x80)
         SDA = 1;
      else
         SDA = 0;
      Dly2();
      SCL = 1;
      Dly5();
      byteout <<= 1;
   }

	// Wait for SHT15 to acknowledge
   SCL = 0;
	_TRISG14 = 1;               // Set Data pin to input
   while(ReadSDA);              // Wait for sht15 to pull DATA low
   SCL = 1;
   SCL = 0;                     // Falling edge of 9th clock
   while (!ReadSDA);            //Wait for SHT to release line
   while (ReadSDA);             //Wait for SHT15 to pull SDA low to signal measurement completion.
                                //This can take up to 210ms for 14 bit measurements
}

/****** sht15_read_byte ***********************************************
 *
 * Read 16 bits from the SHT15 sensor.
 * SCL corresponds to SCK and SDA corresponds to DATA for the SHT15.
 * We use this method to read humidty and temperature measurements.
 * 
 **********************************************************************/
int sht15_read_byte16()
{
	char j;                    // index var
	int in_byte;
	
	SCL = 0;
	
	_TRISG14 = 1;             // Set Data pin to input

	for(j=0; j<17; j++)
	{
		if(j != 8)
		{
			Dly2();
			SCL = 1;
			in_byte <<= 1;
			Dly5();
      		if (ReadSDA)
         		in_byte |= 1;
     		SCL = 0;
      		Dly2();
		}
		else
		{
			_TRISG14 = 0;    // Set Data pin to output
			SDA = 0;
			Dly2();
			SCL = 1;         //clk #9 for acknoledge
			Dly5();
			SCL = 0;
			_TRISG14 = 1;    // Set Data pin to input
		}
	} 
	return(in_byte);
}

/****** i2c_read *******************************************************
 *
 * Read in a byte from the I2C device.
 * For all but the last byte in a message string, call with i2c_rc(1);
 * For the last byte to be received, call this function as   i2c_rx(0);
 * This will acknowledge all but the last byte (for which the 0 invokes a
 * "no-acknowledge" and signals that the device should send nothing more.
 **********************************************************************/
char i2c_read(char ack)
{
   char x, bytein = 0;

   SDA = 1;                      // Release SDA to the peripheral device
   Dly2();
   for (x = 0; x < 8; x++)       // Read in eight data bits
   {                             //   most-significant bit first
      bytein <<= 1;
      Dly2();
      SCL = 1;
      Dly5();
      if (ReadSDA)
         bytein |= 1;
      SCL = 0;
      Dly2();
   }
   if (ack)
      SDA = 0;                   // Acknowledge all but the last byte
   else                          //   of a string of received bytes
      SDA = 1;
   Dly2();
   SCL = 1;
   Dly5();
   SCL = 0;
   Dly2();
   SDA = 1;                      // Release SDA again
   Dly10();
   return bytein;
}
