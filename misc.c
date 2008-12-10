#include <libgimp/gimp.h>

void decode_ycocg_image(gint32 drawableID)
{
   GimpDrawable *drawable;
   GimpPixelRgn srgn, drgn;
   unsigned char *src, *dst;
   int x, y, w, h;
   
   const float offset = 0.5f * 256.0f / 255.0f;
   float Y, Co, Cg, R, G, B;
   
   drawable = gimp_drawable_get(drawableID);
   
   w = drawable->width;
   h = drawable->height;
   
   src = g_malloc(w * 4);
   dst = g_malloc(w * 4);
   
   gimp_pixel_rgn_init(&srgn, drawable, 0, 0, w, h, 0, 0);
   gimp_pixel_rgn_init(&drgn, drawable, 0, 0, w, h, 1, 1);
   
   gimp_progress_init("Decoding YCoCg pixels...");
   
   for(y = 0; y < h; ++y)
   {
      gimp_pixel_rgn_get_row(&srgn, src, 0, y, w);
      
      for(x = 0; x < w; ++x)
      {
         Y  = (float)src[4 * x + 3] / 255.0f;
         Co = (float)src[4 * x + 0] / 255.0f;
         Cg = (float)src[4 * x + 1] / 255.0f;
         
         /* convert YCoCg to RGB */
         Co -= offset;
         Cg -= offset;
         
         R = Y + Co - Cg;
         G = Y + Cg;
         B = Y - Co - Cg;
         
         if(R < 0.0f) R = 0.0f;
         if(R > 1.0f) R = 1.0f;
         if(G < 0.0f) G = 0.0f;
         if(G > 1.0f) G = 1.0f;
         if(B < 0.0f) B = 0.0f;
         if(B > 1.0f) B = 1.0f;
         
         /* copy new alpha from blue */
         dst[4 * x + 3] = src[4 * x + 2];
         
         dst[4 * x + 0] = (unsigned char)(R * 255.0f);
         dst[4 * x + 1] = (unsigned char)(G * 255.0f);
         dst[4 * x + 2] = (unsigned char)(B * 255.0f);
      }
      
      gimp_pixel_rgn_set_row(&drgn, dst, 0, y, w);

      if((y & 31) == 0)
         gimp_progress_update((gdouble)y / (gdouble)h);
   }
   
   gimp_progress_update(1.0);

   gimp_drawable_flush(drawable);
   gimp_drawable_merge_shadow(drawable->drawable_id, 1);
   gimp_drawable_update(drawable->drawable_id, 0, 0, w, h);
   gimp_drawable_detach(drawable);
   
   g_free(src);
   g_free(dst);
}

void decode_ycocg_scaled_image(gint32 drawableID)
{
   GimpDrawable *drawable;
   GimpPixelRgn srgn, drgn;
   unsigned char *src, *dst;
   int x, y, w, h;

   const float offset = 0.5f * 256.0f / 255.0f;
   float Y, Co, Cg, R, G, B, s;
   
   drawable = gimp_drawable_get(drawableID);
   
   w = drawable->width;
   h = drawable->height;

   src = g_malloc(w * 4);
   dst = g_malloc(w * 4);
   
   gimp_pixel_rgn_init(&srgn, drawable, 0, 0, w, h, 0, 0);
   gimp_pixel_rgn_init(&drgn, drawable, 0, 0, w, h, 1, 1);
   
   gimp_progress_init("Decoding YCoCg (scaled) pixels...");
   
   for(y = 0; y < h; ++y)
   {
      gimp_pixel_rgn_get_row(&srgn, src, 0, y, w);
      
      for(x = 0; x < w; ++x)
      {
         Y  = (float)src[4 * x + 3] / 255.0f;
         Co = (float)src[4 * x + 0] / 255.0f;
         Cg = (float)src[4 * x + 1] / 255.0f;
         s  = (float)src[4 * x + 2] / 255.0f;
         
         /* convert YCoCg to RGB */
         s = 1.0f / ((255.0f / 8.0f) * s + 1.0f);
         
         Co = (Co - offset) * s;
         Cg = (Cg - offset) * s;
         
         R = Y + Co - Cg;
         G = Y + Cg;
         B = Y - Co - Cg;

         if(R < 0.0f) R = 0.0f;
         if(R > 1.0f) R = 1.0f;
         if(G < 0.0f) G = 0.0f;
         if(G > 1.0f) G = 1.0f;
         if(B < 0.0f) B = 0.0f;
         if(B > 1.0f) B = 1.0f;
         
         dst[4 * x + 0] = (unsigned char)(R * 255.0f);
         dst[4 * x + 1] = (unsigned char)(G * 255.0f);
         dst[4 * x + 2] = (unsigned char)(B * 255.0f);

         /* set alpha to 1 */
         dst[4 * x + 3] = 255;
      }
      
      gimp_pixel_rgn_set_row(&drgn, dst, 0, y, w);
      
      if((y & 31) == 0)
         gimp_progress_update((gdouble)y / (gdouble)h);
   }
   
   gimp_progress_update(1.0);

   gimp_drawable_flush(drawable);
   gimp_drawable_merge_shadow(drawable->drawable_id, 1);
   gimp_drawable_update(drawable->drawable_id, 0, 0, w, h);
   gimp_drawable_detach(drawable);
   
   g_free(src);
   g_free(dst);
}

void decode_alpha_exp_image(gint32 drawableID)
{
   GimpDrawable *drawable;
   GimpPixelRgn srgn, drgn;
   unsigned char *src, *dst;
   int x, y, w, h;
   float R, G, B, A;
   
   drawable = gimp_drawable_get(drawableID);
   
   w = drawable->width;
   h = drawable->height;
   
   src = g_malloc(w * 4);
   dst = g_malloc(w * 4);
   
   gimp_pixel_rgn_init(&srgn, drawable, 0, 0, w, h, 0, 0);
   gimp_pixel_rgn_init(&drgn, drawable, 0, 0, w, h, 1, 1);
   
   gimp_progress_init("Decoding Alpha-exponent pixels...");
   
   for(y = 0; y < h; ++y)
   {
      gimp_pixel_rgn_get_row(&srgn, src, 0, y, w);
      
      for(x = 0; x < w; ++x)
      {
         R = src[4 * x + 0];
         G = src[4 * x + 1];
         B = src[4 * x + 2];
         A = (float)src[4 * x + 3] / 255.0f;
         
         R *= A;
         G *= A;
         B *= A;
         
         if(R > 255) R = 255;
         if(G > 255) G = 255;
         if(B > 255) B = 255;
         
         dst[4 * x + 0] = (unsigned char)R;
         dst[4 * x + 1] = (unsigned char)G;
         dst[4 * x + 2] = (unsigned char)B;

         /* set alpha to 1 */
         dst[4 * x + 3] = 255;
      }
      
      gimp_pixel_rgn_set_row(&drgn, dst, 0, y, w);
      
      if((y & 31) == 0)
         gimp_progress_update((gdouble)y / (gdouble)h);
   }
   
   gimp_progress_update(1.0);
   
   gimp_drawable_flush(drawable);
   gimp_drawable_merge_shadow(drawable->drawable_id, 1);
   gimp_drawable_update(drawable->drawable_id, 0, 0, w, h);
   gimp_drawable_detach(drawable);
   
   g_free(src);
   g_free(dst);
}
