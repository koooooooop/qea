/******************************************************************************
* Copyright 2014-2015  Co.,Ltd.
* FileName: 	 IO.C 
* Desc: IO的操作
* 
* 
* Author: 	 
* Date: 	 
* Notes: 
* 
* -----------------------------------------------------------------
* Histroy: 
* 
******************************************************************************/

/*-------------------------------- Includes ----------------------------------*/
#include "IO.h"

/*-------------------- Global Definitions and Declarations -------------------*/


/*----------------------- Constant / Macro Definitions -----------------------*/


/*------------------------ Type Declarations ---------------------------------*/

/*------------------------ Variable Declarations -----------------------------*/


/*------------------------ Function Prototype --------------------------------*/


/*------------------------ Function Implement --------------------------------*/


/*----------------------------------------------------------------------------*/

/******************************************************************************
* Name:    SET_IO_OUT_OPEN_DRAIN()                          *
* Desc:   IO设置为开漏输出                           *
* Param:                                                                      *
* Return:                                                                     *
* Global:                                                                     *
* Note:                                                                       *
* Author:                                                                     *
* Log:                                                                        *
*******************************************************************************/

void SET_IO_OUT_OPEN_DRAIN(GPIO_TypeDef *port,U16 pin)
{
    U32 value;
	U8 lpin;
	lpin = pin * 2;

	value = port->MODER;
	value &= ~((U32)GPIO_MODER_MODER0 << lpin);
	
	value |= ((U32)GPIO_Mode_OUT << lpin);
	port->MODER = value;
	
	port->OTYPER  &= ~((GPIO_OTYPER_OT_0) << ((U16)pin)) ;
    port->OTYPER |= (U16)(((U16)GPIO_OType_OD) << ((U16)pin));
	
    
}
/******************************************************************************
* Name:    SET_IO_OUT()                          *
* Desc:   IO设置为推挽输出                           *
* Param:                                                                      *
* Return:                                                                     *
* Global:                                                                     *
* Note:                                                                       *
* Author:                                                                     *
* Log:                                                                        *
*******************************************************************************/
void SET_IO_OUT(GPIO_TypeDef *port,U16 pin)
{
	U32 value;
	U8 lpin;
	lpin = pin * 2;

	value = port->MODER;
	value &= ~((U32)GPIO_MODER_MODER0 << lpin);
	
	value |= ((U32)GPIO_Mode_OUT << lpin);
	port->MODER = value;
	
	port->OTYPER  &= ~((GPIO_OTYPER_OT_0) << ((U16)pin)) ;
    port->OTYPER |= (U16)(((U16)GPIO_OType_PP) << ((U16)pin));
  	
 }

void SET_IO_OTYPE_PP(GPIO_TypeDef *port,U16 pin){
	port->OTYPER  &= ~((GPIO_OTYPER_OT_0) << ((U16)pin)) ;
    port->OTYPER |= (U16)(((U16)GPIO_OType_PP) << ((U16)pin));
}

void SET_IO_OUT_HS(GPIO_TypeDef *port,U16 pin)
{
	U32 value;
	U8 lpin;
	lpin = pin * 2;

	value = port->MODER;
	value &= ~((U32)GPIO_MODER_MODER0 << lpin);
	
	value |= ((U32)GPIO_Mode_OUT << lpin);
	port->MODER = value;
	
	port->OTYPER  &= ~((GPIO_OTYPER_OT_0) << ((U16)pin)) ;
    port->OTYPER |= (U16)(((U16)GPIO_OType_PP) << ((U16)pin));
  	
 }

void SET_IO_SET_BIT(GPIO_TypeDef *port, U16 pin)  
{	
	port->BSRRL = ((U32)0x01<<pin);
}  
/******************************************************************************
* Name:    SET_IO_LOW()                          				*
* Desc:     推挽输出低                      		 					*
* Param:                                                                      *
* Return:                                                                     *
* Global:                                                                     *
* Note:                                                                       *
* Author:                                                                     *
* Log:                                                                        *
*******************************************************************************/
void SET_IO_RESET_BIT(GPIO_TypeDef *port, U16 pin)  
{
	port->BSRRH = ((U32)0x01<<pin);
} 

void SET_IO_IN_WITHOUTPULLUP(GPIO_TypeDef *port,U16 pin)
{
	U32 value;
	U8 lpin;
	lpin = pin * 2;

	value = port->MODER;
	value &= ~((U32)GPIO_MODER_MODER0 << lpin);
	
	value |= ((U32)GPIO_Mode_IN << lpin);
	port->MODER = value;
	
	port->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << ((U16)pin * 2));
	  
}
void SET_IO_IN_PULLUP(GPIO_TypeDef *port,U16 pin)
{
	U32 value;
	U8 lpin;
	lpin = pin * 2;

	value = port->MODER;
	value &= ~((U32)GPIO_MODER_MODER0 << lpin);
	
	value |= ((U32)GPIO_Mode_IN << lpin);
	port->MODER = value;
	
	port->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << ((U16)pin * 2));
    port->PUPDR |= (((U32)GPIO_PuPd_UP) << (pin * 2));
}
/******************************************************************************
* Name:    SET_IO_IN()                          				*
* Desc:   IO设置为输入                         		 *
* Param:                                                                      *
* Return:                                                                     *
* Global:                                                                     *
* Note:                                                                       *
* Author:                                                                     *
* Log:                                                                        *
*******************************************************************************/
void SET_IO_ANALOG(GPIO_TypeDef *port,U16 pin)
{
	
	U32 value;
	U8 lpin;
	lpin = pin * 2;

	value = port->MODER;
	value &= ~((U32)GPIO_MODER_MODER0 << lpin);
	
	value |= ((U32)GPIO_Mode_AN << lpin);
	port->MODER = value;
}



/******************************************************************************
* Name:    SET_IO_AFMODE()                          				*
* Desc:                            		 					*
* Param:                                                                      *
* Return:                                                                     *
* Global:                                                                     *
* Note:                                                                       *
* Author:                                                                     *
* Log:                                                                        *
*******************************************************************************/
void SET_IO_AFMODE_PP(GPIO_TypeDef *port, U16 pin)
{
	U32 value;
	U8 lpin;
	lpin = pin * 2;

	value = port->MODER;
	value &= ~((U32)GPIO_MODER_MODER0 << lpin);
	
	value |= ((U32)GPIO_Mode_AF << lpin);
	port->MODER = value;

	port->OTYPER  &= ~((GPIO_OTYPER_OT_0) << ((U16)pin)) ;
    port->OTYPER |= (U16)(((U16)GPIO_OType_PP) << ((U16)pin));
  	
}
void SET_IO_AFMODE_PP_HS(GPIO_TypeDef *port, U16 pin)
{
	
  	
}

/******************************************************************************
* Name:    SET_IO_HIGH()                          				*
* Desc:     推挽输出高                       		 					*
* Param:                                                                      *
* Return:                                                                     *
* Global:                                                                     *
* Note:                                                                       *
* Author:                                                                     *
* Log:                                                                        *
*******************************************************************************/
void SET_IO_HIGH(GPIO_TypeDef *port, U16 pin)  
{	 
	port->BSRRL = ((U32)0x01<<pin);
	SET_IO_OUT(port,pin);
	port->BSRRL = ((U32)0x01<<pin);
}  
/******************************************************************************
* Name:    SET_IO_LOW()                          				*
* Desc:     推挽输出低                      		 					*
* Param:                                                                      *
* Return:                                                                     *
* Global:                                                                     *
* Note:                                                                       *
* Author:                                                                     *
* Log:                                                                        *
*******************************************************************************/
void SET_IO_LOW(GPIO_TypeDef *port, U16 pin)  
{
	port->BSRRH = ((U32)0x01<<pin);
	SET_IO_OUT(port,pin);
	port->BSRRH = ((U32)0x01<<pin);
} 
/******************************************************************************
* Name:    SET_IO_OPEN_HIGH()                          				*
* Desc:      开漏输出高                      		 					*
* Param:                                                                      *
* Return:                                                                     *
* Global:                                                                     *
* Note:                                                                       *
* Author:                                                                     *
* Log:                                                                        *
*******************************************************************************/
void SET_IO_HIGH_OPEN_DRAIN(GPIO_TypeDef *port, U16 pin)
{
	port->BSRRL = ((U32)0x01<<pin);
	SET_IO_OUT_OPEN_DRAIN(port,pin);
	port->BSRRL = ((U32)0x01<<pin);
} 
/******************************************************************************
* Name:    SET_IO_LOW_OPEN_DRAIN()                          				*
* Desc:      开漏输出低                     		 					*
* Param:                                                                      *
* Return:                                                                     *
* Global:                                                                     *
* Note:                                                                       *
* Author:                                                                     *
* Log:                                                                        *
*******************************************************************************/
void SET_IO_LOW_OPEN_DRAIN(GPIO_TypeDef *port,U16 pin)
{
	port->BSRRH = ((U32)0x01<<pin);
	SET_IO_OUT_OPEN_DRAIN(port,pin);
	port->BSRRH = ((U32)0x01<<pin);
} 

void SET_IO_TOGGLE(GPIO_TypeDef *port,U16 pin) 
{	
	U32 value;
	value = ((U32)0x01<<pin);
	if(value & port->ODR){
		port->BSRRH = value;
	}else{
		port->BSRRL = value;
	}
	
	SET_IO_OUT(port,pin);
} 
U8 GET_IO_STA(GPIO_TypeDef *port,U16 pin) 
{
	if (port->IDR & ((U32)0x01<<pin))
	{
		return HIGH;
	}
	else
	{
		return  LOW;
	}
}
U8 GET_IO_OUTSTA(GPIO_TypeDef *port,U16 pin) 
{
	if (port->ODR & ((U32)0x01<<pin))
	{
		return HIGH;
	}
	else
	{
		return LOW;
	}
}
void SET_IO_AFConfig(GPIO_TypeDef *port, U16 pinsource, U16 af){

  U32 temp = 0x00;
  U32 temp_2 = 0x00;
  
  temp = ((U32)(af) << ((U32)((U32)pinsource & (U32)0x07) * 4)) ;
  port->AFR[pinsource >> 0x03] &= ~((U32)0xF << ((U32)((U32)pinsource & (U32)0x07) * 4)) ;
  temp_2 = port->AFR[pinsource >> 0x03] | temp;
  port->AFR[pinsource >> 0x03] = temp_2;
}

