/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2020  Luis Claudio Gambôa Lopes

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   For e-mail suggestions :  lcgamboa@yahoo.com
   ######################################################################## */


#include"bitbang_uart.h"

#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>


#define dprintf if (1) {} else printf 

void
bitbang_uart_rst(bitbang_uart_t *bu)
{
 bu->prx = 1;
 bu->insr = 0;
 bu->outsr = 1;
 bu->bcr = 0;
 bu->bcw = 0;
 bu->rxc = 0;
 bu->tcountr = 0;
 bu->tcountw = 0;
 bu->leds = 0;
 bu->data_recv=0;
 bu->data_to_send=0;
 dprintf ("rst uart\n");
}

void
bitbang_uart_init(bitbang_uart_t *bu)
{
 bitbang_uart_rst (bu);
 bu->speed = 9600;
 bu->cycle_count = 100;
 dprintf ("init uart\n");
}

void
bitbang_uart_end(bitbang_uart_t *bu) {
 }

void
bitbang_uart_set_clk(bitbang_uart_t *bu, unsigned long clk)
{
 bu->cycle_count = clk / (16 * bu->speed);
}

unsigned char
bitbang_uart_io(bitbang_uart_t *bu, unsigned char rx)
{

 //read rx
 if (bu->tcountr)
  {
   bu->tcountr++;

   if (!(bu->tcountr % bu->cycle_count))
    {
     if (rx)bu->rxc++;
    }

   if (bu->tcountr >= bu->cycle_count << 4)
    {
     bu->tcountr = 1;


     if (bu->rxc > 7)
      {
       bu->insr = (bu->insr >> 1) | 0x80;
      }
     else
      {
       bu->insr = (bu->insr >> 1) & 0xF7FF;
      }
     bu->bcr++;
     bu->rxc = 0;

     if (bu->bcr == 8)//start+eight bits+ stop
      {
       //dprintf ("uart byte in 0x%02X  out 0x%02X\n", bu->inbu & 0xFF, bu->outbu >> 8);
       //printf ("%c  0x%04X\n", bu->inbu >> 1, bu->inbu >> 1);

       bu->datar = bu->insr >> 1;
       bu->data_recv=1;
      }
    }

   //stop bit
   if ((bu->bcr > 7)&&(bu->prx == 0)&&(rx == 1))//rising edge
    {
     bu->tcountr = 0;
     bu->bcr = 0;
    }

  }
 else
  {
   //start bit
   if ((bu->prx == 1)&&(rx == 0))//falling edge
    {
     //dprintf ("uart start bit \n");
     bu->tcountr = 1;
     bu->bcr = 0;
     bu->insr = 0;
     bu->rxc = 0;
     bu->leds |= 0x01;
    }
  }

 bu->prx = rx;


 //write tx

 bu->tcountw++;

 if (bu->tcountw >= (bu->cycle_count << 4))
  {
   bu->tcountw = 0;

   if (!bu->bcw)
    {
     if(bu->data_to_send)
      { 
       bu->data_to_send=0;
       dprintf ("uart data rec %c \n", bu->dataw);
       bu->leds |= 0x02;
       bu->bcw = 1;
       bu->outsr = (bu->dataw << 1) | 0x200;
      }
    }
   else
    {
     bu->outsr = (bu->outsr >> 1);
     bu->bcw++;
     if (bu->bcw > 9)
      {
       bu->bcw = 0;
      }
    }

  }



 return (bu->outsr & 0x01);
}

void
bitbang_uart_set_speed(bitbang_uart_t *bu, unsigned int speed)
{
 bu->speed = speed;
}

unsigned char  
bitbang_uart_transmitting(bitbang_uart_t *bu)
{
 return (bu->bcw | bu->data_to_send | bu->tcountw);
}

void 
bitbang_uart_send(bitbang_uart_t *bu, unsigned char data)
{
  bu->dataw=data;
  bu->data_to_send=1;
}

unsigned char  
bitbang_uart_data_available(bitbang_uart_t *bu)
{
 return bu->data_recv;
}

unsigned char  
bitbang_uart_recv(bitbang_uart_t *bu)
{
  bu->data_recv=0;
  return bu->datar;
}