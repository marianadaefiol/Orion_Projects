#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <iostream>

#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h> //ioctl() call defenitions

// ref.: https://www.xanthium.in/Controlling-RTS-and-DTR-pins-SerialPort-in-Linux

int main(int argc, char *argv[])
{
  int state; 
  int fd;
  fd = open("/dev/ttyUSB1",O_RDWR | O_NOCTTY ); 	//Open Serial Port
  
  int DTR_flag;
  DTR_flag = TIOCM_DTR;
  
  while(1){
  	
  	scanf("%d", &state);
  	printf("state is: %d\n", state); 
  	if (state == 1)
  	{
  		ioctl(fd,TIOCMBIS,&DTR_flag); 	//Set DTR pin
  		printf("estou dentro do set");
  	}
  	else if (state == 0 )
  	{
  		 ioctl(fd,TIOCMBIC,&DTR_flag);	//Clear DTR pin
  		 printf("estou dentro do reset");
  	}
  }
 /* 
  int pulses = strtol(argv[2], NULL, 10);
  int delay = strtol(argv[3], NULL, 10);
  
  printf("Porta    : %s\n", argv[1]);
  printf("Pulsos   : %d\n", pulses);
  printf("Intervalo: %d ms\n", delay);
  delay = delay * 1000; // converte de ms para us

  
  int fd;
  fd = open(argv[1],O_RDWR | O_NOCTTY ); //Open Serial Port
  
  int DTR_flag;
  DTR_flag = TIOCM_DTR;
  
  ioctl(fd,TIOCMBIS,&DTR_flag);//Set DTR pin
  while(1)
  {
    usleep(delay);
    printf("Enviando %d pulsos\n", pulses);
    for(int i = 0; i < pulses; i ++)
    {
      ioctl(fd,TIOCMBIC,&DTR_flag);//Clear DTR pin
      usleep(50000);
      ioctl(fd,TIOCMBIS,&DTR_flag);//Set DTR pin
      usleep(50000);
    }
    printf("Pulsos enviados... aguarde %d segundos\n", delay);
  }
  */
  close(fd);
}
