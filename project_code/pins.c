#include "pins.h"
#include <avr/io.h>

const Pin pins[] =
  {
    //0
    {
      .in_register=&PINE,
      .out_register=&PORTE,
      .dir_register=&DDRE
    },
    //1
    {
      .in_register=&PINE,
      .out_register=&PORTE,
      .dir_register=&DDRE
    },
    //2
    {
      .in_register=&PINE,
      .out_register=&PORTE,
      .dir_register=&DDRE
    },
    //3
    {
      .in_register=&PINE,
      .out_register=&PORTE,
      .dir_register=&DDRE
    },
    //4
    {
      .in_register=&PING,
      .out_register=&PORTG,
      .dir_register=&DDRG
    },
    //5
    {
      .in_register=&PINE,
      .out_register=&PORTE,
      .dir_register=&DDRE
    },
    //6
    {
      .in_register=&PINH,
      .out_register=&PORTH,
      .dir_register=&DDRH
    },
    //7
    {
      .in_register=&PINH,
      .out_register=&PORTH,
      .dir_register=&DDRH
    },
    //8
    {
      .in_register=&PINH,
      .out_register=&PORTH,
      .dir_register=&DDRH
    },
    //9
    {
      .in_register=&PINH,
      .out_register=&PORTH,
      .dir_register=&DDRH
    },
    //10
    {
      .in_register=&PINB,
      .out_register=&PORTB,
      .dir_register=&DDRB
    },
    //11
    {
      .in_register=&PINB,
      .out_register=&PORTB,
      .dir_register=&DDRB
    },
    //12
    {
      .in_register=&PINB,
      .out_register=&PORTB,
      .dir_register=&DDRB
    },
    //13
    {
      .in_register=&PINB,
      .out_register=&PORTB,
      .dir_register=&DDRB
    }
  };
