#ifndef  _BARCODE_H_
#define  _BARCODE_H_

//=============================================================================


#include "posapi.h"

#ifdef __cplusplus
extern "C"{
#endif


#define     BAR_TYPE_CODE128        "CODE128"

#define     BAR_PRINT_EXPAND        0       //  print string expand to the width of bar image
#define     BAR_PRINT_LEFT          1       //  print string align to the left of bar image
#define     BAR_PRINT_RIGHT         2       //  print string align to the right of bar image
#define     BAR_PRINT_MIDDLE        3       //  print string in the middle of bar image

typedef struct
{
    char    code_type[32];      //  bar code type. eg. "CODE128"
    int     bar_height;         //  barcode height(pixel)
    int     image_width;        //  limit image width(pixel), if set to <=0, image depended on block_width
    int     block_width;        //  define the block width(pixel) if image_width <=0
    int     adj_percent;        //  adjust block width (recommend: 34)
    int     blank_width;        //  left & right blank width(pixel) (always big than 10X block_width)
    int     space;              //  the space between barcode image and print string(pixel)

    int     bar_str_len;        //  the byte length of barcode
    uint8_t bar_str[256];       //  the string of barcode

    char    FontName[32];       //  set fontname for print string
    char    FontCharacter[32];  //  set font charset for print string
    int     FontStyle;          //  set font style for print string
    int     FontSize;           //  set font size for print string
    int     print_style;        //  set print style (BAR_DISP_EXPAND, BAR_DISP_LEFT, etc)
    uint8_t print_str[256];     //  the string to display, set to "" if don't want to print
}barcode_t;



int DrawBarCode(const barcode_t *barinfo, void *barimage);

//=============================================================================
#ifdef __cplusplus
}
#endif

#endif

