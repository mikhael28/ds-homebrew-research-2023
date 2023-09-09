#include <nds.h>
#include <stdio.h>
#include <stdarg.h>

#include <fat.h>
#include <malloc.h>
#include <string.h>
#include "png.h"
#include "draw.h"

u16* fb0;
u16* fb1;

void InitDrawSystem(){
	powerOn(POWER_ALL_2D);
	videoSetMode(MODE_3_2D);
	videoSetModeSub(MODE_3_2D);
	vramSetBankA(VRAM_A_MAIN_BG);
	vramSetBankB(VRAM_B_MAIN_BG);
	vramSetBankC(VRAM_C_SUB_BG);
	bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0,0);
	bgInitSub(3, BgType_Bmp16, BgSize_B16_256x256, 0,0);
	fb1 = (u16*)malloc(S_WIDTH*S_HEIGHT*2);
	fb0 = (u16*)malloc(S_WIDTH*S_HEIGHT*2);
}

void ResetDrawSystem(){
	memset(fb1, 255, 256*192*2);
	memset(fb0, 255, 256*192*2);
}

void DrawSystemUpdate(){
	memmove(BG_GFX, fb1, S_WIDTH*S_HEIGHT*2);
	memmove(BG_GFX_SUB, fb0, S_WIDTH*S_HEIGHT*2);
}

void PutPixelRGB(bool screen, s16 x, s16 y, u16 RGB){
	if(x>=0&&x<S_WIDTH&&y>=0&&y<S_HEIGHT){
	if(RGB>=ARGB16(1, 0, 0, 0)){
	if(screen){
	fb1[x+y*S_WIDTH] = RGB;
	}else{
	fb0[x+y*S_WIDTH] = RGB;
	}
	}
	}
}



u16 GetPixelRGB(bool screen, s16 x, s16 y){
	if(screen)return fb1[x+y*S_WIDTH];
	else return fb0[x+y*S_WIDTH];
}

u8 GetPixelR(bool screen, s16 x, s16 y){
	u16 RGB;
	if(screen)RGB = fb1[x+y*S_WIDTH];
	else RGB = fb0[x+y*S_WIDTH];
	u8 r = RGB&31;
	return r;
}

u8 GetPixelG(bool screen, s16 x, s16 y){
	u16 RGB;
	if(screen)RGB = fb1[x+y*S_WIDTH];
	else RGB = fb0[x+y*S_WIDTH];
	u8 g = (RGB>>5)&31;
	return g;
}

u8 GetPixelB(bool screen, s16 x, s16 y){
	u16 RGB;
	if(screen)RGB = fb1[x+y*S_WIDTH];
	else RGB = fb0[x+y*S_WIDTH];
	u8 b = (RGB>>10)&31;
	return b;
}


void DrawLine(bool screen, s16 x1, s16 y1, s16 x2, s16 y2, u16 RGB){
	int i,dx,dy,sdx,sdy,dxabs,dyabs,x,y,px,py;

	dx=x2-x1;      /* the horizontal distance of the line */
	dy=y2-y1;      /* the vertical distance of the line */
	dxabs = dx;
	sdx = 1;
	if (dx < 0) {
	dxabs = -dx;
	sdx = -1;
	}
	dyabs = dy;
	sdy = 1;
	if (dy < 0) {
	dyabs = -dy;
	sdy = -1;
	}

	x=dyabs>>1;
	y=dxabs>>1;
	px=x1;
	py=y1;
  
	PutPixelRGB(screen, px, py, RGB);


	if (dxabs>=dyabs) {
    for(i=0;i<dxabs;i++)  {
    y+=dyabs;
    if (y>=dxabs)  {
        y-=dxabs;
        py+=sdy;
    }
    px+=sdx;
    PutPixelRGB(screen, px, py, RGB);
    }
	}
	else {
    for(i=0;i<dyabs;i++) {
    x+=dxabs;
    if (x>=dyabs)  {
	x-=dyabs;
	px+=sdx;
    }
    py+=sdy;
    PutPixelRGB(screen, px, py, RGB);
    }
  }
}

void DrawRect(bool screen, s16 x1, s16 y1, s16 x2, s16 y2, u16 RGB){
	DrawLine(screen, x1, y1, x2-1, y1, RGB);
	DrawLine(screen, x1, y2-1, x2-1, y2-1, RGB);
	DrawLine(screen, x1, y1, x1, y2-1, RGB);
	DrawLine(screen, x2-1, y1, x2-1, y2-1, RGB);
	
}

void DrawHLine(bool screen, s16 x1, s16 x2, s16 y, u16 RGB){
	for(int i=x1;i<x2;i++)PutPixelRGB(screen, i, y, RGB);
}

void DrawVLine(bool screen, s16 x, s16 y1, s16 y2, u16 RGB){
	for(int i=y1;i<y2;i++)PutPixelRGB(screen, x, i, RGB);
}

void GetJPEGSize(const unsigned char*jpeg, int* width, int* height){
	//REG_EXMEMCNT &= ~0x80;
	JPEG_Decoder decoder;
	JPEG_Decoder_ReadHeaders(&decoder, &jpeg);
	*width = decoder.frame.width;
	*height = decoder.frame.height;
	//REG_EXMEMCNT |= 0x80;
}

u16* LoadJPEG(const char* filename, int *width, int *height){
	FILE* fp = fopen(filename, "rb");
	if(fp!=NULL){
	fseek(fp, 0, SEEK_END);
	u32 size = ftell(fp);
	rewind(fp);
	void* jpeg = malloc(size);
	fread(jpeg, 1, size, fp);
	fclose(fp);
	GetJPEGSize((const unsigned char*)jpeg, width, height);
	u32 pixelsize = *width * *height;
	u16* image = (u16*)malloc(pixelsize*2);
	//REG_IME = 0x00;
	JPEG_DecompressImage((u8*)jpeg, image, *width, *height);
	free(jpeg);
	//REG_IME = 0x01;
	return image;
	}
	return NULL;
}

u16* LoadBMP(const char* filename, int *width, int *height){
	FILE* fp = fopen(filename, "rb");
	if(fp!=NULL){
	fseek(fp, 0, SEEK_END);
	u32 size = ftell(fp);
	rewind(fp);
	BMPHeader0 Bmpinfo0;
	BMP_Headers Bmpinfo;
	fread(&Bmpinfo0, sizeof(BMPHeader0), 1, fp);
	fread(&Bmpinfo, sizeof(BMP_Headers), 1, fp);
	size-=ftell(fp);
	u8* gfx = (u8*)malloc(size);
	fread(gfx, size, 1, fp);
	s32 r, g, b;  s16 tempx, tempy;
	s16 lx = Bmpinfo.Width;   s16 ly = Bmpinfo.Height;
	*width = lx;
	*height = ly;
	u16 Bits = Bmpinfo.BitsperPixel;
	u16* image = (u16*)malloc(lx*ly*2);
	s32 i = 0;
	if (Bits > 16){ // Pour 24 et 32 bits
	for (tempy = ly-1; tempy > -1; tempy--){
	for (tempx = 0; tempx < lx; tempx++){
	b = (gfx[i] >> 3)&31;	i++;
	g = (gfx[i] >> 3)&31;	i++;
	r = (gfx[i] >> 3)&31;	i++;	
	if (Bits == 32) i++;
	image[tempx + (tempy * S_WIDTH)] = ARGB16(1, r, g, b);
	}
	//while(i&3) i++; // Padding....
	}
	}
	free((void*)gfx);
	fclose(fp);
	return image;
	}
	return NULL;
}

struct png_read_data_str
{
void *data;
unsigned long filesize;
unsigned long filepos;
};

static void read_png_data(png_structp png_ptr, png_bytep data, png_size_t length)
{

   void *pngf_data = ((struct png_read_data_str*)png_ptr->io_ptr)->data;
   unsigned long pngf_filesize = ((struct png_read_data_str*)png_ptr->io_ptr)->filesize;
   unsigned long pngf_filepos = ((struct png_read_data_str*)png_ptr->io_ptr)->filepos;

   if((pngf_filepos + length) < pngf_filesize)
   {
	   memmove(data, pngf_data + pngf_filepos, length);
   }
   else
   {
	   memmove(data, pngf_data + pngf_filepos, pngf_filesize - pngf_filepos);
   }

   ((struct png_read_data_str*)png_ptr->io_ptr)->filepos += length;

}

int read_png(png_byte **png_out, png_info **png_outinfo, void *png_file, unsigned long png_filesize, int flags)
{
   int y;
   png_structp png_ptr;
   png_infop info_ptr;
   png_bytepp row_pointers;
   png_bytep row_data;
   struct png_read_data_str read_info;


   /* Fill read structure */
   read_info.data = png_file;
   read_info.filesize = png_filesize;
   read_info.filepos = 8;

   /* Check if png_file has a correct PNG signature */
   if (png_sig_cmp((png_bytep)png_file, 0, 8))
   {
      return READPNG_BADSIG;
   }

   /* Create read structure */
   png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

   if (!png_ptr)
   {
      return READPNG_CREATEREADSTRUCT_ERROR;
   }

   /* Create info structure */
   info_ptr = png_create_info_struct(png_ptr);

   if (!info_ptr)
   {
      return READPNG_CREATEINFOSTRUCT_ERROR;
   }

   if (setjmp(png_jmpbuf(png_ptr)))
   {
      return READPNG_INITIO_ERROR;
   }

   /* Set user IO function for reading ... */
   png_set_read_fn(png_ptr, (png_voidp)&read_info, read_png_data);

   /* Set size of signature */
   png_set_sig_bytes(png_ptr, 8);

   /* Read informations about image */
   png_read_info(png_ptr, info_ptr);

   /* Set transformations to do on the image */


   if (info_ptr->bit_depth == 16)   /* 16 bits to 24 */
      png_set_strip_16(png_ptr);

   if((flags & READPNG_WITH_PALETTE) && (info_ptr->color_type == PNG_COLOR_TYPE_PALETTE))
   {
      /* No transformations needed */
   }
   else
   {
      if (info_ptr->color_type == PNG_COLOR_TYPE_PALETTE)    /* If needed, ignore palette */
         png_set_palette_to_rgb(png_ptr);
      if (info_ptr->bit_depth < 8)     /* < 8 bits to 24 */
         png_set_expand(png_ptr);
      if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))  /* ? */
         png_set_expand(png_ptr);
   }
      
   if (info_ptr->color_type == PNG_COLOR_TYPE_GRAY || info_ptr->color_type == PNG_COLOR_TYPE_GRAY_ALPHA)    /* Grayscale to RVB */
      png_set_gray_to_rgb(png_ptr);

   /* Apply transformations */
   png_read_update_info(png_ptr, info_ptr);

   if (setjmp(png_jmpbuf(png_ptr)))
   {
      return READPNG_READIMG_ERROR;
   }

   /* Allocate array for image's pixels */
   row_pointers = (png_bytepp) malloc(sizeof(png_byte*) * info_ptr->height);

   if(*png_out)
   {
      row_data = (png_bytep) *png_out;
   }
   else
   {
      row_data = (png_bytep) malloc(info_ptr->rowbytes * info_ptr->height);
      *png_out = row_data;
   }
   
   for (y=0; y < info_ptr->height; y++)
   {
      row_pointers[y] = (png_bytep) row_data + y * info_ptr->rowbytes;
   }


   /* Read PNG file */
   png_read_image(png_ptr, row_pointers);

   /* Free structures */
   png_destroy_read_struct(&png_ptr, NULL, NULL);
   free(row_pointers);

   /* Give adress to the information structure */
   *png_outinfo = info_ptr;

   /* Success !!! */
   return READPNG_SUCCESS;

}

void read_png_free(png_byte *png_out, png_info *png_outinfo)
{
   png_destroy_read_struct(NULL, &png_outinfo, NULL);
   free(png_out);
}

u16* LoadPNG(const char* filename, int *width, int *height){
	FILE* fp = fopen(filename, "rb");
	if(fp!=NULL){
	fseek(fp, 0, SEEK_END);
	u32 size = ftell(fp);
	rewind(fp);
	void* png = malloc(size);
	fread(png, 1, size, fp);
	fclose(fp);
	png_byte *pngUncompressedData = NULL;
	png_info *pngInfos;
	int width2, height2, channels;
	if(read_png(&pngUncompressedData, &pngInfos, png, size, 0) != 0){
	free(png);
	return NULL;
	}
	width2 = pngInfos->width;
	height2 = pngInfos->height;
	channels = pngInfos->channels;
	*width = width2;
	*height = height2;
	if(!PNG_IS_PALETTE(pngInfos->color_type)){
	char* pixel;
	u16* image = (u16*)malloc(width2*height2*2);
	for(int y = 0; y < height2; y++){
	for(int x = 0; x < width2; x++){
	pixel = GETPIXEL((char*)pngUncompressedData, x,y,width2, channels);
	image[x+y*width2] = ARGB16((*GETALPHA24(pixel))>>7, (*GETRED24(pixel))>>3, (*GETGREEN24(pixel))>>3, (*GETBLUE24(pixel))>>3);
	}
	}
	read_png_free(pngUncompressedData, pngInfos);
	free(png);
	return image;
	}else return NULL;
	}else return NULL;
}

u16* font[256];
void LoadFont(const char* filename){
	int width, height;
	u16* png_font = LoadPNG(filename, &width, &height);
	for(int x1=0;x1<32;x1++){
	for(int y1=0;y1<8;y1++){
	font[x1+y1*32] = (u16*)malloc(128);
	for(int x=0;x<8;x++){
	for(int y=0;y<8;y++){
	font[x1+y1*32][x+y*8] = png_font[(x1*8+x)+(y1*8+y)*256];
	}
	}
	}
	}
	free(png_font);
}

void DrawImage(bool screen, s16 x1, s16 y1, u16* image, u16 width, u16 height){
	for(int x=0;x<width;x++)for(int y=0;y<height;y++)PutPixelRGB(screen, x+x1, y+y1, image[x+y*width]);
}

void PrintChar(bool screen, s16 x1, s16 y1, const char c, u16 color){
	for(int x=0;x<8;x++)for(int y=0;y<8;y++)if(font[(int)c][x+y*8]==32768)PutPixelRGB(screen, x+x1, y+y1, color);
}

void Print(bool screen, s16 x1, s16 y1, u16 color, const char* format, ...){
	char stringa[512];
	va_list args;
	va_start (args, format);
	vsprintf (stringa,format, args);
	va_end (args);
	int len = strlen(stringa);
	s16 x=0, y=0;
	for(int i=0;i<len;i++){
	if(stringa[i] == '\n'){
	x=0;
	y+=8;
	}else if(stringa[i] == '\t'){
	x+=32;
	}else{
	PrintChar(screen, x+x1, y+y1, stringa[i], color);
	x+=8;
	if(x+x1>=256){
	x=0;
	y+=8;
	}
	}
	}
}

void ScreenShot(bool screen, const char* filename){
	FILE* fp = fopen(filename, "wb");
	BMPHeader0 Bmpinfo0;
	BMP_Headers Bmpinfo;
	Bmpinfo0.Id = 0x4D42;
	Bmpinfo0.Length = 256*192*3+sizeof(BMPHeader0)+sizeof(BMP_Headers);
	Bmpinfo0.Nothing1 = 0;
	Bmpinfo0.Nothing2 = 0;
	Bmpinfo0.offset = sizeof(BMPHeader0)+sizeof(BMP_Headers);
	fwrite(&Bmpinfo0, 1, sizeof(BMPHeader0), fp);
	Bmpinfo.SizeofHeader = sizeof(BMP_Headers);
	Bmpinfo.Width = 256;
	Bmpinfo.Height = 192;
	Bmpinfo.Colorplanes = 1;
	Bmpinfo.BitsperPixel = 24;
	Bmpinfo.Compression = 0;
	Bmpinfo.SizeofData = 256*192*3;
	Bmpinfo.WidthperMeter = 0;
	Bmpinfo.HeightperMeter = 0;
	Bmpinfo.NColors = 0;
	Bmpinfo.ImportantColors = 0;
	fwrite(&Bmpinfo, 1, sizeof(BMP_Headers), fp);
	u16 color=0;
	for(int y=0;y<192;y++){
	for(int x=0;x<256;x++){
	if(screen)color=fb1[256*192-y*256+x];
	else color=fb0[256*192-y*256+x];
	u8 b=((color>>10)&31)<<3;
	fwrite(&b, 1, 1, fp);
	u8 g=((color>>5)&31)<<3;
	fwrite(&g, 1, 1, fp);
	u8 r=(color&31)<<3;
	fwrite(&r, 1, 1, fp);
	}
	}
	fclose(fp);
}

u16* ScaleToScreen(u16* src, u16 width, u16 height){
	u16* dest = (u16*)malloc(256*192*2);
	u16 wj = width/256;
	u16 hj = height/192;
	for(int x=0;x<width;x+=wj){
	for(int y=0;y<height;y+=hj){
	dest[(x/wj)+(y/hj)*256] = src[x+y*width];
	}
	}
	return dest;
}
