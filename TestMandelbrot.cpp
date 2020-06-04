//+------------------------------------------------------------------+
//|                                               TestMandelbrot.cpp |
//|                   Copyright 2001-2020, MetaQuotes Software Corp. |
//|                                        http://www.metaquotes.net |
//|                                                                  |
//+------------------------------------------------------------------+
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <ShlObj.h>

double ExtX0=-2.0;
double ExtY0=-2.0;
double ExtX1=-1.0;
double ExtY1=-1.0;

int    ExtResolution=2048;
//+------------------------------------------------------------------+
//| Mandelbrot calculation for pixel                                 |
//+------------------------------------------------------------------+
UINT MFractal(double x0,double y0,double x1,double y1,int gx,int gy,int w,int h)
  {
   double dx = x0 + gx * (x1-x0) / (double)w * 4;
   double dy = y0 + gy * (y1-y0) / (double)h * 4;
   double x  = 0;
   double y  = 0;
   double xx = 0;
   double yy = 0;
   double xy = 0;
   UINT   i=0;
//---
   while((xx+yy)<4 && i<20000)
     {
      xx = x*x;
      yy = y*y;
      xy = x*y;
      y = xy+xy+dy;
      x = xx-yy+dx;
      i++;
     }

   if(i==20000)
      return(0);
//---
   return((UINT)(0xFFFFFF/20000.0)*i);
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void write_bitmap32b(const char *filename,void *data,int width,int height)
  {
#pragma pack(push,1)
   struct BMP
     {
      BITMAPFILEHEADER fh;
      BITMAPINFOHEADER bi;
     } bmp={};
#pragma pack(pop)

   bmp.fh.bfOffBits  =sizeof(bmp);
   bmp.fh.bfSize     =sizeof(bmp)+width*height*sizeof(UINT);
   bmp.fh.bfType     =0x4D42;
   bmp.bi.biBitCount =32;
   bmp.bi.biHeight   =height;
   bmp.bi.biWidth    =width;
   bmp.bi.biPlanes   =1;
   bmp.bi.biSize     =sizeof(bmp.bi);
   bmp.bi.biSizeImage=width*height*sizeof(UINT);

   FILE *out;
   fopen_s(&out,filename,"wb");
   fwrite(&bmp,sizeof(bmp),1,out);
   fwrite(data,sizeof(UINT),width*height,out);
   fclose(out);
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
int main()
  {
   UINT     *uint_array=NULL;
   ULONGLONG t0=::GetTickCount64();
//--- ensure output buffer
   int       image_size=ExtResolution*ExtResolution;
   uint_array=new UINT[image_size];
//--- calculate result
   for(int i=0; i<image_size; i++)
      uint_array[i]=MFractal(ExtX0,ExtY0,ExtX1,ExtY1,i%ExtResolution,i/ExtResolution,ExtResolution,ExtResolution);
   
   ULONGLONG elapsed=::GetTickCount64()-t0;
   printf("%I64u ms elapsed, %u bytes\n",elapsed,UINT(image_size*sizeof(UINT)));
//--- write bitmap
   write_bitmap32b("mandelbrot.bmp",uint_array,ExtResolution,ExtResolution);
   delete[] uint_array;
//--- write results
   __time64_t ctm;
   tm         time_str;
   FILE      *out;
   _time64(&ctm);
   _localtime64_s(&time_str,&ctm);
   fopen_s(&out,"TestMandelbrot.csv","at");
   fseek(out,0,SEEK_END);
   if(ftell(out)==0)
      _fprintf_p(out,"YYYY.MM.DD HH:MI:SS\tELAPSED (ms)\tSIZE (bytes)\n");
   _fprintf_p(out,"%d.%02d.%02d %02d:%02d:%02d\t%I64u\t%u\n",
                   time_str.tm_year+1900,time_str.tm_mon,time_str.tm_mday,time_str.tm_hour,time_str.tm_min,time_str.tm_sec,
                   elapsed,UINT(image_size*sizeof(UINT)));
   fclose(out);
//--- write results
   /*char path[260];
   ::SHGetFolderPathA(NULL,CSIDL_APPDATA,NULL,SHGFP_TYPE_CURRENT,path);
   strcat_s(path,"\\MetaQuotes\\Terminal\\Common\\Files\\BenchResults");
   if(::GetFileAttributesA(path)==INVALID_FILE_ATTRIBUTES)
      ::CreateDirectoryA(path,NULL);
   strcat_s(path,"\\TestMandelbrotCPP.csv");
   fopen_s(&out,path,"at");
   _fprintf_p(out,"%d.%02d.%02d %02d:%02d:%02d\t%I64u\n",
                   time_str.tm_year+1900,time_str.tm_mon,time_str.tm_mday,time_str.tm_hour,time_str.tm_min,time_str.tm_sec,
                   elapsed);
   fclose(out);*/
  }
//+------------------------------------------------------------------+
