#ifndef ULTRASOUND_SENSOR_GROUP2_H
#define ULTRASOUND_SENSOR_GROUP2_H

class US_Sensor
{
private:
	//Internal trigger pin number
	int _TrigPin;
	unsigned long Distance;
	
	//static variables
	static int _EchoPin;
	static US_Sensor* ActiveSensor;
	
	//Ultrasonic sensor variables
	volatile unsigned long EchoStart = 0;
	volatile unsigned long EchoDuration = 0;
	volatile bool EchoComplete = false;
	
	//internal functions
	static void Echo_ISR();
	void Handle_Interrupt();
	
public:
	US_Sensor(int TrigPin);
	static bool Setup_Echo_Pin(int EchoPin);
	unsigned long Get_Distance_CM(void);
};
#endif