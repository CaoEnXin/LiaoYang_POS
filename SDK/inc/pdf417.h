#ifndef     _PDF417_H_
#define     _PDF417_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "posapi.h"


//  t_pdf417 option parameter:
#define PDF417_USE_ASPECT_RATIO     0
#define PDF417_FIXED_RECTANGLE      1
#define PDF417_FIXED_COLUMNS        2
#define PDF417_FIXED_ROWS           4
#define PDF417_AUTO_ERROR_LEVEL     0
#define PDF417_USE_ERROR_LEVEL      16
#define PDF417_USE_RAW_CODEWORDS    64
#define PDF417_INVERT_BITMAP        128

//  the error code of generate barcode
#define PDF417_ERROR_SUCCESS        0
#define PDF417_ERROR_TEXT_TOO_BIG   1
#define PDF417_ERROR_INVALID_PARAMS 2



typedef struct
{
	void 	*text;			//  the text want to be printed
	int		lentext;		//  the length of text, set to < 0 means strlen(text);
	int		col_unit;		//  the col unit in pixel (should be set as 2 for printer)
	int		row_unit;		//  the row unit in pixel (should be set as 2 for printer)
	int		maxwidth;		//  the max width pixel, if PDF417_FIXED_COLUMNS appointed.
	int		maxheight;		//  the max height pixel, if PDF417_FIXED_ROWS appointed.
	uint32_t	options;	//  the options to generate the barcode
}t_pdf417;
/*  If the strlen(text) is short, such as less than 100, the application can use PDF417_FIXED_RECTANGLE or PDF417_USE_ASPECT_RATIO
*	to print. Also the application can use bigger unit (such as col_unit=3) to print a bigger barcode, thus it is easy to be scanned.
*	If print the barcode horizontally and text is long, it is recommended to set option PDF417_FIXED_COLUMNS. The maxwidth set to 384;
*	If print the barcode vertically, it is recommended to set option PDF417_FIXED_ROWS. The maxheight set to 384;
*/

/*
*print PDF417 barcode by given text.
*  Rotate -- the Rotate of bitmap:
*				EM_BMP_ROTATE0
*				EM_BMP_ROTATE90
*				EM_BMP_ROTATE180
*				EM_BMP_ROTATE270
*  pbarinfo -- the parameter to print barcode
*
* return code:
* ==0  ->  success
* <0   ->  generate barcode error(PDF417_ERROR_TEXT_TOO_BIG, PDF417_ERROR_INVALID_PARAMS)
* >0   ->  print failed, please refer to printer module.
*/
int pdf417print(int Rotate, const t_pdf417 *pbarinfo);




#ifdef __cplusplus
}
#endif


#endif


