/*
 * SPL Full Memory or Complinace Test
 * (C) Copyright 2015 Siemens Schweiz AG
 * (C) Boris Schmidt boris.schmidt@siemens.com
 *
 * - memory test with total SDRAM size (also usefull as one EMC test case) 
 *		- anti pattern for row hammer walking trough full ram
 *		- refresh over temperature torture (write all, read all)
 * - compliance test 
 * 		- for DDRx oscilloscope test suite and
 *		- stimulus for DDR detective 
 *		- programmable alternating pattern to test and measure coross talk of bad lines from simulation 
 *		- programmable mode (read only, write only, read write)
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */



#include "../draco/board.h"
#include "../include/configs/thuban.h"
#include <asm/arch/ddr_defs.h>

//DECLARE_GLOBAL_DATA_PTR;

extern volatile struct draco_baseboard_id __attribute__((section(".data"))) settings;


/* Test checks the SDRAM from beginning to end with
	 *
	 * using BitFlip Metod
	 *
	 * Output		: detected Error @ Address 
	 * Inncremment  : on successfull loop run
	*/


static void spl_full_memory_test(void)
{
	
	 
	 unsigned long  *ct_pointer32;
	 //unsigned short *ct_pointer16;
	 //unsigned short *f_pointer;
	 unsigned char i=0;
	 
	 unsigned long ct_start;
	 unsigned long ct_size;
	 unsigned long ct_end;
	 

	 unsigned short 	loopc = 0;
	 unsigned long 		rpattern = 0x0;
	 unsigned long 		ebyte1 = 0x0;
	 unsigned long 		ebyte2 = 0x0;

	 ct_start 	= PHYS_DRAM_1;
	 ct_size	= get_ram_size(
			(void *)CONFIG_SYS_SDRAM_BASE,
			CONFIG_MAX_RAM_BANK_SIZE); /*Detected size */
	 ct_end		=  ct_start+ct_size - sizeof(ct_pointer32);

	 printf("memory test start   : %08X\r\n",(unsigned int) ct_start);
	 printf("memory test size    : %08X\r\n",(unsigned int) ct_size);
	 printf("memory test end     : %08X\r\n",(unsigned int) ct_end);
	 printf("pointer size (byte) : %08X\r\n",(unsigned int) sizeof(ct_pointer32));
	 printf("for loop end       : %08X\r\n",(unsigned int) ( (unsigned long*) (ct_end - sizeof(ct_pointer32)) ) );

	 puts("\n\r...Full Memory Test is working \n\r");
	 while (1)
		{	 
	
		/* fill ram with 0x00-0xFF all bytes */	

		 for(i=0;i<255;i++)
  		 {

  		 ebyte1 = ( ((unsigned long) (i<<24) & 0xFF000000)  | ((unsigned long) (i<<16) & 0x00FF0000) | ((unsigned long) (i<<8) & 0x0000FF00) | ((unsigned long) i & 0x000000FF) );
  		 ebyte2 = ~ebyte1;
#ifdef CONFIG_SPL_CMT_DEBUG  		 
  		 printf("ebyte1: %08X\n\r",(unsigned int) ebyte1);
  		 printf("ebyte2: %08X\n\r",(unsigned int) ebyte2);
#endif
		 for (ct_pointer32 = (unsigned long*) ct_start; ct_pointer32 <= (unsigned long*) (ct_end - sizeof(ct_pointer32)); ct_pointer32++)
  		  {
  		 	 *ct_pointer32 = ebyte1;
  		 	 ct_pointer32++;
  		 	 *ct_pointer32 = ebyte2;  		 	
		  } 	 	
		 
		 /* check 0x00-0xFF all bytes */
		 for (ct_pointer32 = (unsigned long*) ct_start; ct_pointer32 <=  (unsigned long*) (ct_end - sizeof(ct_pointer32)); ct_pointer32++)
  		   {
  		    rpattern = *ct_pointer32; 
  		 	if (rpattern != ebyte1) {printf("Error! Read: %08X Wrote: %08X Address: %08X \n\r",(unsigned int) rpattern,(unsigned int) ebyte1,(unsigned int) ct_pointer32);};
  		 	ct_pointer32++;	 	
  		 	rpattern = *ct_pointer32; 
  		 	if (rpattern != ebyte2) {printf("Error! Read: %08X Wrote: %08X Address: %08X \n\r",(unsigned int) rpattern,(unsigned int) ebyte2,(unsigned int) ct_pointer32);}
  		   }
  		  }

  		loopc++;
		printf("loop: %08X\r",(unsigned int) loopc);

  		 

  	 	}//while
	
}

static void spl_compliance_test(void)
{	
	unsigned long cpattern1 = 0x0;
	unsigned long cpattern2 = 0x0;
	unsigned long rpattern  = 0x0;
	unsigned long i=0;
	unsigned long loopc=0;
	unsigned long hammer_hits_until_refresh = 10000;
	
	unsigned short cmode = 0x0;
	
	unsigned long ct_start;
	unsigned long ct_end;
	unsigned long ct_size;
	unsigned short  *ct_pointer16;
	//unsigned long 	*ct_pointer32;
	unsigned short  *fill_pointer16;
	
	


	/* MSB 4Bit => bus width */
	/* A = 8Bit		tbd		 */
	/* B = 16Bit			 */
	/* C = 32Bit	tbd		 */
	/* D = 64Bit	tbd		 */
	
	/* LSB 4Bit => Mode 	 */
	/* A = Write only        */
	/* B = Read only         */
	/* C = Row Hammer-slow   */
	/* D = Row Hammer-fast   */
	/* E =      			 */
	/* F =      			 */
	/* get eeprom or default */
	cpattern1	= settings.ddr3.sdramc_pattern1;
	cpattern2	= settings.ddr3.sdramc_pattern2;
	cmode		= settings.ddr3.sdramc_mode;
	ct_start	= settings.ddr3.sdramc_start_test;
	ct_end		= settings.ddr3.sdramc_end_test;
	hammer_hits_until_refresh 	= settings.ddr3.sdramc_hammer_hits;
	

	unsigned char onetime = 0;
	
	puts("Check for Compliance Test\n\r"); 
	
	if (ct_start == 0xFFFFFFFF) ct_start 	= PHYS_DRAM_1;
	if (ct_end   == 0xFFFFFFFF) 
	 {
	 	ct_size	= get_ram_size((void *)CONFIG_SYS_SDRAM_BASE,CONFIG_MAX_RAM_BANK_SIZE); /*Detected size */
	 	ct_end	=  ct_start+ct_size - sizeof(ct_pointer16);
	  }    
	
	if (hammer_hits_until_refresh == 0xFFFFFFFF) hammer_hits_until_refresh = 10000;
		
	printf("pattern1   : %08X\n\r",(unsigned int) cpattern1);
	printf("pattern2   : %08X\n\r",(unsigned int) cpattern2);
	printf("cmode      : %08X\n\r",(unsigned int) cmode);
	printf("startaddr  : %08X\n\r",(unsigned int) ct_start);
	printf("endaddr    : %08X\n\r",(unsigned int) ct_end);
    printf("hammer hits: %08X\n\r",(unsigned int) hammer_hits_until_refresh);
	/* test eeprom readout for needed compliance values */

	if ((cpattern1 != 0xFFFFFFFF) && (cpattern2 != 0xFFFFFFFF) && (cmode !=0xFFFF))
	{	
	puts("Compliance Test enabled\n\r");
	while (1)
	{
			/*  Test generates read only or write only access to SDRAM burst mode depends on SDRAM setup 			*/
				
		if ( ((cmode & 0x000F) == 0x000A)  || (onetime == 0)) /* write only mode and write once*/
		{
		 if(onetime == 0) {puts("Write only Compliance Test\n\r");	}
		 
		 ct_start 	= PHYS_DRAM_1;
		 ct_end		= 32*4; /* Max Burst Lengt */
  		 
  		 for (ct_pointer16 = (unsigned short*) ct_start; ct_pointer16 < (unsigned short*) ct_end ; ct_pointer16++)
  		 {
  		 	*ct_pointer16 = (unsigned short) cpattern1;
  		 	ct_pointer16++;
  		 	*ct_pointer16 = (unsigned short) cpattern2;
  		 }
  		 if(onetime == 0) onetime++;
  		}

  		if ((cmode & 0x000F) == 0x000B) /* read only mode*/
		{		 
		 if(onetime == 1){puts("Read only Compliance Test\n\r");}
  		 for (ct_pointer16 = (unsigned short*) ct_start; ct_pointer16 < (unsigned short*) ct_end ; ct_pointer16++)
  		 {
  		 	rpattern = *ct_pointer16;  		 	
  		 }
		}
		
		if ((cmode & 0x000F) == 0x000C) 
		{
  	    /* Row Hammer defined by Jörg Senn: nearly infinite working time,  aprox. 63 years for a loop without caches aktiv on 128MB*/
		/* This test is independend from internal SDRAM routing. (all row neighbours are checked)	*/
  	    puts("...Row-Hammer-Slow...working \n\r");

  	    for (ct_pointer16 = (unsigned short*) ct_start ; ct_pointer16 <= (unsigned short*) ct_end ; ct_pointer16++)
  		{
  			/* fill */
  		  	for (fill_pointer16 = (unsigned short*) ct_start ; fill_pointer16 <= (unsigned short*) ct_end ; fill_pointer16++)
  		    {
  		    	*fill_pointer16 = cpattern1; //Victim
  		    }
  		  	  		 	
  		  	// If I had a hammer...I hammer in the morning..I hammer in the evening.. endless test
  		  	for (i=0;i<hammer_hits_until_refresh;i++)
  		  	{	
  		 		*ct_pointer16 = cpattern2; //Agressor  		 	    
  		 	}

  		 	// check Victims 
  		  	for (fill_pointer16 = (unsigned short*) ct_start ; fill_pointer16 <= (unsigned short*) ct_end ; fill_pointer16++)
  		  	{
  		      	rpattern = *fill_pointer16;
  		      	if (fill_pointer16 == ct_pointer16) 
  		      		{printf("Hammer > Agressor: %08X Victims: %08X Address: %08X \n\r",(unsigned int) cpattern2,(unsigned int) cpattern1,(unsigned int) fill_pointer16);}
  		      		

  		 	    if ( (rpattern != cpattern1)  && (fill_pointer16 != ct_pointer16) )
  		 	     	{printf("Row-Hammer Error! Read: %08X Wrote: %08X Address: %08X \n\r",(unsigned int) rpattern,(unsigned int) cpattern1,(unsigned int) fill_pointer16);}
			}			  	     
		 }
		} // c-mode C
		 

		if ((cmode & 0x000F) == 0x000D) 
		{
		 /* about 49 Days for a loop, expecting more than 64ms for hammer_hits_until_refresh x 128MB */
        /* Row Hammer defined by Boris Schmidt: 16Bit bus width, expecting an internal linear routing */
		/* Address lines are hopefully routed that vitims and agressor are neighbours in the physical data rows   */

  	    puts("...Row-Hammer-Fast...working \n\r");

  	    for (ct_pointer16 = (unsigned short*) ct_start; ct_pointer16 <=  (unsigned short*) (ct_end - sizeof(ct_pointer16)); ct_pointer16=ct_pointer16+8*sizeof(ct_pointer16))
  		{
  		  	/* victims: first and last byte has only one neigbour victim*/
  		  	if (ct_pointer16 != (unsigned short*) ct_start) 
  		  	{
  		  		 ct_pointer16--;
  		  		 *ct_pointer16 = cpattern1;
  		  		 ct_pointer16++;
  		  	}
  		  	if (ct_pointer16 != (unsigned short*) ct_end)
  		  	{
  		  		 ct_pointer16++;
  		  		 *ct_pointer16 = cpattern1;
  		  	     ct_pointer16--;
 		  	}
  		  	
  		  	printf("Hammer > Agressor: %08X Victims: %08X Address: %08X \n\r",(unsigned int) cpattern2,(unsigned int) cpattern1,(unsigned int) ct_pointer16);    
  		  	
	  	  	for (i=0;i<hammer_hits_until_refresh;i++)
  		  	{	
  		 		 *ct_pointer16 = cpattern2; //Agressor  		 	    
  		 	}


  		   	// check Victims 
  		   	if (ct_pointer16 != (unsigned short*) ct_start) 
  		  	{
  		   	     ct_pointer16--;
  		   	     rpattern = *ct_pointer16; 
  		   	     if (rpattern != cpattern1) {printf("Error! Read: %08X Wrote: %08X Address: %08X \n\r",(unsigned int) rpattern,(unsigned int) cpattern1,(unsigned int) ct_pointer16);};
  		   	     ct_pointer16++;
  		   	}

  		   	if (ct_pointer16 != (unsigned short*) ct_end)
  		  	{
  		 	     ct_pointer16++;	 	
  		 	     rpattern = *ct_pointer16; 
  		 		 if (rpattern != cpattern1) {printf("Error! Read: %08X Wrote: %08X Address: %08X \n\r",(unsigned int) rpattern,(unsigned int) cpattern1,(unsigned int) ct_pointer16);}; 
  		 		 ct_pointer16--;
			}	     
		}//for
		}//c-mode D

		loopc++;
  		printf("loop: %08X\n\r",(unsigned int) loopc);

  		
  	 }//while
	} // if pattern set
	
}//void

/* Compliance or Memory Test */

void spl_siemens_cmt(void)
 {
 	/*
 	* check the test mode 
 	*
	* - compliance test (corvus,draco)
	* - full memory test (taurus,corvus,draco)
	*
 	*/

	/* check on draco and corvus the eeprom if a compliance test is wanted */

#if defined(CONFIG_SPL_DRACO) || defined(CONFIG_SPL_CORVUS) 
	spl_compliance_test();
#else
	puts("No Compliance Test\n\r");
#endif

	/* if no compliance test is needed we want the full memory test */
	spl_full_memory_test();
 }
