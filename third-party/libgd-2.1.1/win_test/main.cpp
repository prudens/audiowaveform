#include <gd.h>
#include <gdfontl.h>
#include <stdio.h>
#include <string.h>

int main( int argc, char **argv )
{
    /* Declare the image */
    gdImagePtr im;
    /* Declare output files */
    FILE *pngout;
    char *s = "Hello, World";
    /* Declare color indexes */
    int black;
    int white;

    /* 创建100x100的图像，如果需要使用真彩色，
    * 换成 gdImageCreateTrueColor 接口
    */
    im = gdImageCreate( 100, 100 );

    /* 黑色作为背景，我这里使用了RGBA模式，也就是有透明的图像，
    * 使用宏 gdAlphaTransparent,背景就为透明了。还有就是默认创建的第一层图像即为背景层
    */
    black = gdImageColorAllocateAlpha( im, 0, 0, 0, gdAlphaTransparent );
    /* 创建白色的前景层，这里就不适用Alpha通道了。 */
    white = gdImageColorAllocate( im, 255, 255, 255 );

    /* 要写Hello.123到图像的中间，居中对齐，所以就是下面这样的计算方式了，
    * gd提供了以下几种的字体大小，分别是
    * 大小         获取函数             头文件
    * Tiny        gdFontGetTiny       gdfontt.h
    * Small       gdFontGetSmall      gdfonts.h
    * MediumBold  gdFontGetMediumBold gdfontmb.h
    * Large       gdFontGetLarge      gdfontl.h
    * Giant       gdFontGetGiant      gdfontg.h
    * 这里使用的是large的字体，字体大小为 8x16,
    * 下面的函数，将字体画到图像中
    */
    gdImageString( im, gdFontGetLarge(),
                   im->sx / 2 - ( strlen( s ) * gdFontGetLarge()->w / 2 ),
                   im->sy / 2 - gdFontGetLarge()->h / 2,
                   (unsigned char*)s, white );

    /* 打开文件 */
    pngout = fopen( "test.png", "wb" );

    /* 不同的图像格式，对应不同的输出函数
    * PNG -- gdImagePng
    * Gif -- gdImageGif
    * Tiff -- gdImageTiff 等，
    */
    gdImagePngEx( im, pngout, 0 );

    /* Close the files. */
    fclose( pngout );

    /* Destroy the image in memory. */
    gdImageDestroy( im );

    return 0;
}
//------------------------------------------------------------------------------