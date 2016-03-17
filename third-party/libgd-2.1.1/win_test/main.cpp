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

    /* ����100x100��ͼ�������Ҫʹ�����ɫ��
    * ���� gdImageCreateTrueColor �ӿ�
    */
    im = gdImageCreate( 100, 100 );

    /* ��ɫ��Ϊ������������ʹ����RGBAģʽ��Ҳ������͸����ͼ��
    * ʹ�ú� gdAlphaTransparent,������Ϊ͸���ˡ����о���Ĭ�ϴ����ĵ�һ��ͼ��Ϊ������
    */
    black = gdImageColorAllocateAlpha( im, 0, 0, 0, gdAlphaTransparent );
    /* ������ɫ��ǰ���㣬����Ͳ�����Alphaͨ���ˡ� */
    white = gdImageColorAllocate( im, 255, 255, 255 );

    /* ҪдHello.123��ͼ����м䣬���ж��룬���Ծ������������ļ��㷽ʽ�ˣ�
    * gd�ṩ�����¼��ֵ������С���ֱ���
    * ��С         ��ȡ����             ͷ�ļ�
    * Tiny        gdFontGetTiny       gdfontt.h
    * Small       gdFontGetSmall      gdfonts.h
    * MediumBold  gdFontGetMediumBold gdfontmb.h
    * Large       gdFontGetLarge      gdfontl.h
    * Giant       gdFontGetGiant      gdfontg.h
    * ����ʹ�õ���large�����壬�����СΪ 8x16,
    * ����ĺ����������廭��ͼ����
    */
    gdImageString( im, gdFontGetLarge(),
                   im->sx / 2 - ( strlen( s ) * gdFontGetLarge()->w / 2 ),
                   im->sy / 2 - gdFontGetLarge()->h / 2,
                   (unsigned char*)s, white );

    /* ���ļ� */
    pngout = fopen( "test.png", "wb" );

    /* ��ͬ��ͼ���ʽ����Ӧ��ͬ���������
    * PNG -- gdImagePng
    * Gif -- gdImageGif
    * Tiff -- gdImageTiff �ȣ�
    */
    gdImagePngEx( im, pngout, 0 );

    /* Close the files. */
    fclose( pngout );

    /* Destroy the image in memory. */
    gdImageDestroy( im );

    return 0;
}
//------------------------------------------------------------------------------