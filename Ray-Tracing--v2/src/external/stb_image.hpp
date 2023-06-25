#ifndef STB_IMAGE_HPP
#define STB_IMAGE_HPP

#include <cassert>
#include <climits>
#include <cstdint>
#include <cstdio>
#include <array>
#include <cstring>
#include <iostream>
#include <sys/types.h>

using uchar = unsigned char;
using stbi_uc = unsigned char;
using stbi_us = unsigned short;

using stbi_io_callbacks = struct {
  int (*read)(void *user, char *data, int size); // fill 'data' with 'size' bytes.  return number of bytes actually read
  void (*skip)(void *user, int n);               // skip the next 'n' bytes, or 'unget' the last -n bytes if negative
  int (*eof)(void *user);                        // returns nonzero if we are at end of file/data
};

using stbi__result_info = struct {
  int bits_per_channel;
  int num_channels;
  int channel_order;
};

using stbi__gif_lzw = struct {
  signed short prefix;
  uchar first;
  uchar suffix;
};

using stbi__gif = struct {
  int w, h;
  uchar *out;        // output buffer (always 4 components)
  uchar *background; // The current "background" as far as a gif is concerned
  uchar *history;
  int flags, bgindex, ratio, transparent, eflags;
  uchar pal[256][4];
  uchar lpal[256][4];
  stbi__gif_lzw codes[8192];
  uchar *color_table;
  int parse, step;
  int lflags;
  int start_x, start_y;
  int max_x, max_y;
  int cur_x, cur_y;
  int line_size;
  int delay;
};

enum { STBI_ORDER_RGB, STBI_ORDER_BGR };

static auto stbi__stdio_read(void *user, char *data, int size) -> int {
  return (int)fread(data, 1, size, (FILE *)user);
}

static void stbi__stdio_skip(void *user, int n) {
  int ch;
  fseek((FILE *)user, n, SEEK_CUR);
  ch = fgetc((FILE *)user); /* have to read a byte to reset feof()'s flag */
  if (ch != EOF) {
    ungetc(ch, (FILE *)user); /* push byte back onto stream if valid. */
  }
}

static auto stbi__stdio_eof(void *user) -> int {
  return feof((FILE *)user) || ferror((FILE *)user);
}

static stbi_io_callbacks stbi__stdio_callbacks = {
    stbi__stdio_read,
    stbi__stdio_skip,
    stbi__stdio_eof,
};

using stbi__context = struct {
  uint32_t img_x, img_y;
  int img_n, img_out_n;

  stbi_io_callbacks io;
  void *io_user_data;

  int read_from_callbacks;
  int buflen;
  uchar buffer_start[128];
  int callback_already_read;

  uchar *img_buffer, *img_buffer_end;
  uchar *img_buffer_original, *img_buffer_original_end;
};

static void stbi__refill_buffer(stbi__context *s) {
  int n = (s->io.read)(s->io_user_data, (char *)s->buffer_start, s->buflen);
  s->callback_already_read += (int)(s->img_buffer - s->img_buffer_original);
  if (n == 0) {
    // at end of file, treat same as if from memory, but need to handle case
    // where s->img_buffer isn't pointing to safe memory, e.g. 0-byte file
    s->read_from_callbacks = 0;
    s->img_buffer = s->buffer_start;
    s->img_buffer_end = s->buffer_start + 1;
    *s->img_buffer = 0;
  } else {
    s->img_buffer = s->buffer_start;
    s->img_buffer_end = s->buffer_start + n;
  }
}

inline static auto stbi__get8(stbi__context *s) -> uchar {
  if (s->img_buffer < s->img_buffer_end)
    return *s->img_buffer++;
  if (s->read_from_callbacks) {
    stbi__refill_buffer(s);
    return *s->img_buffer++;
  }
  return 0;
}

static int stbi__get16le(stbi__context *s) {
  int z = stbi__get8(s);
  return z + (stbi__get8(s) << 8);
}

static auto stbi__gif_test_raw(stbi__context *s) -> int {
  int sz;
  if (stbi__get8(s) != 'G' || stbi__get8(s) != 'I' || stbi__get8(s) != 'F' || stbi__get8(s) != '8')
    return 0;
  sz = stbi__get8(s);
  if (sz != '9' && sz != '7')
    return 0;
  if (stbi__get8(s) != 'a')
    return 0;
  return 1;
}

static void stbi__rewind(stbi__context *s) {
  // conceptually rewind SHOULD rewind to the beginning of the stream,
  // but we just rewind to the beginning of the initial buffer, because
  // we only use it after doing 'test', which only ever looks at at most 92 bytes
  s->img_buffer = s->img_buffer_original;
  s->img_buffer_end = s->img_buffer_original_end;
}

static auto stbi__gif_test(stbi__context *s) -> int {
  int r = stbi__gif_test_raw(s);
  stbi__rewind(s);
  return r;
}

#define STBI_NOTUSED(v) (void)sizeof(v)

static void stbi__gif_parse_colortable(stbi__context *s, stbi_uc pal[256][4], int num_entries, int transp) {
  int i;
  for (i = 0; i < num_entries; ++i) {
    pal[i][2] = stbi__get8(s);
    pal[i][1] = stbi__get8(s);
    pal[i][0] = stbi__get8(s);
    pal[i][3] = transp == i ? 0 : 255;
  }
}

static int stbi__mul2sizes_valid(int a, int b) {
  if (a < 0 || b < 0)
    return 0;
  if (b == 0)
    return 1; // mul-by-0 is always safe
  // portable way to check for no overflows in a*b
  return a <= INT_MAX / b;
}

static int stbi__addsizes_valid(int a, int b) {
  if (b < 0)
    return 0;
  return a <= INT_MAX - b;
}

static int stbi__mad3sizes_valid(int a, int b, int c, int add) {
  return stbi__mul2sizes_valid(a, b) && stbi__mul2sizes_valid(a * b, c) && stbi__addsizes_valid(a * b * c, add);
}

static int stbi__gif_header(stbi__context *s, stbi__gif *g, int *comp, int is_info) {
  stbi_uc version;
  if (stbi__get8(s) != 'G' || stbi__get8(s) != 'I' || stbi__get8(s) != 'F' || stbi__get8(s) != '8') {
    std::cerr << "stbi__err && Corrupt GIF" << std::endl;
    return 0;
  }

  version = stbi__get8(s);
  if (version != '7' && version != '9')
    return 0;
  if (stbi__get8(s) != 'a')
    return 0;

  g->w = stbi__get16le(s);
  g->h = stbi__get16le(s);
  g->flags = stbi__get8(s);
  g->bgindex = stbi__get8(s);
  g->ratio = stbi__get8(s);
  g->transparent = -1;

  if (comp != 0)
    *comp = 4; // can't actually tell whether it's 3 or 4 until we parse the comments

  if (is_info)
    return 1;

  if (g->flags & 0x80)
    stbi__gif_parse_colortable(s, g->pal, 2 << (g->flags & 7), -1);

  return 1;
}

static void stbi__skip(stbi__context *s, int n) {
  if (n == 0)
    return; // already there!
  if (n < 0) {
    s->img_buffer = s->img_buffer_end;
    return;
  }
  if (s->io.read) {
    int blen = (int)(s->img_buffer_end - s->img_buffer);
    if (blen < n) {
      s->img_buffer = s->img_buffer_end;
      (s->io.skip)(s->io_user_data, n - blen);
      return;
    }
  }
  s->img_buffer += n;
}
static void stbi__out_gif_code(stbi__gif *g, u_int16_t code) {
  stbi_uc *p, *c;
  int idx;

  // recurse to decode the prefixes, since the linked-list is backwards,
  // and working backwards through an interleaved image would be nasty
  if (g->codes[code].prefix >= 0)
    stbi__out_gif_code(g, g->codes[code].prefix);

  if (g->cur_y >= g->max_y)
    return;

  idx = g->cur_x + g->cur_y;
  p = &g->out[idx];
  g->history[idx / 4] = 1;

  c = &g->color_table[g->codes[code].suffix * 4];
  if (c[3] > 128) { // don't render transparent pixels;
    p[0] = c[2];
    p[1] = c[1];
    p[2] = c[0];
    p[3] = c[3];
  }
  g->cur_x += 4;

  if (g->cur_x >= g->max_x) {
    g->cur_x = g->start_x;
    g->cur_y += g->step;

    while (g->cur_y >= g->max_y && g->parse > 0) {
      g->step = (1 << g->parse) * g->line_size;
      g->cur_y = g->start_y + (g->step >> 1);
      --g->parse;
    }
  }
}

static auto stbi__process_gif_raster(stbi__context *s, stbi__gif *g) -> stbi_uc * {
  stbi_uc lzw_cs;
  int32_t len, init_code;
  uint32_t first;
  int32_t codesize, codemask, avail, oldcode, bits, valid_bits, clear;
  stbi__gif_lzw *p;

  lzw_cs = stbi__get8(s);
  if (lzw_cs > 12)
    return nullptr;
  clear = 1 << lzw_cs;
  first = 1;
  codesize = lzw_cs + 1;
  codemask = (1 << codesize) - 1;
  bits = 0;
  valid_bits = 0;
  for (init_code = 0; init_code < clear; init_code++) {
    g->codes[init_code].prefix = -1;
    g->codes[init_code].first = (stbi_uc)init_code;
    g->codes[init_code].suffix = (stbi_uc)init_code;
  }

  // support no starting clear code
  avail = clear + 2;
  oldcode = -1;

  len = 0;
  for (;;) {
    if (valid_bits < codesize) {
      if (len == 0) {
        len = stbi__get8(s); // start new block
        if (len == 0)
          return g->out;
      }
      --len;
      bits |= (int32_t)stbi__get8(s) << valid_bits;
      valid_bits += 8;
    } else {
      int32_t code = bits & codemask;
      bits >>= codesize;
      valid_bits -= codesize;
      // @OPTIMIZE: is there some way we can accelerate the non-clear path?
      if (code == clear) { // clear code
        codesize = lzw_cs + 1;
        codemask = (1 << codesize) - 1;
        avail = clear + 2;
        oldcode = -1;
        first = 0;
      } else if (code == clear + 1) { // end of stream code
        stbi__skip(s, len);
        while ((len = stbi__get8(s)) > 0)
          stbi__skip(s, len);
        return g->out;
      } else if (code <= avail) {
        if (first) {
          return nullptr;
        }

        if (oldcode >= 0) {
          p = &g->codes[avail++];
          if (avail > 8192) {
            return nullptr;
          }

          p->prefix = (int16_t)oldcode;
          p->first = g->codes[oldcode].first;
          p->suffix = (code == avail) ? p->first : g->codes[code].first;
        } else if (code == avail)
          return nullptr;

        stbi__out_gif_code(g, (int16_t)code);

        if ((avail & codemask) == 0 && avail <= 0x0FFF) {
          codesize++;
          codemask = (1 << codesize) - 1;
        }

        oldcode = code;
      } else {
        return nullptr;
      }
    }
  }
}

static auto stbi__gif_load_next(stbi__context *s, stbi__gif *g, int *comp, int req_comp, uchar *two_back) -> uchar * {
  int dispose;
  int first_frame;
  int pi;
  int pcount;
  STBI_NOTUSED(req_comp);

  // on first frame, any non-written pixels get the background colour (non-transparent)
  first_frame = 0;
  if (g->out == 0) {
    if (!stbi__gif_header(s, g, comp, 0))
      return 0; // stbi__g_failure_reason set by stbi__gif_header

    pcount = g->w * g->h;
    g->out = (stbi_uc *)malloc(4 * pcount);
    g->background = (stbi_uc *)malloc(4 * pcount);
    g->history = (stbi_uc *)malloc(pcount);
    if (!g->out || !g->background || !g->history) {
      std::cerr << "outofmem && Out of memory" << std::endl;
      return nullptr;
    }

    // image is treated as "transparent" at the start - ie, nothing overwrites the current background;
    // background colour is only used for pixels that are not rendered first frame, after that "background"
    // color refers to the color that was there the previous frame.
    memset(g->out, 0x00, 4 * pcount);
    memset(g->background, 0x00, 4 * pcount); // state of the background (starts transparent)
    memset(g->history, 0x00, pcount);        // pixels that were affected previous frame
    first_frame = 1;
  } else {
    // second frame - how do we dispose of the previous one?
    dispose = (g->eflags & 0x1C) >> 2;
    pcount = g->w * g->h;

    if ((dispose == 3) && (two_back == 0)) {
      dispose = 2; // if I don't have an image to revert back to, default to the old background
    }

    if (dispose == 3) { // use previous graphic
      for (pi = 0; pi < pcount; ++pi) {
        if (g->history[pi]) {
          memcpy(&g->out[pi * 4], &two_back[pi * 4], 4);
        }
      }
    } else if (dispose == 2) {
      // restore what was changed last frame to background before that frame;
      for (pi = 0; pi < pcount; ++pi) {
        if (g->history[pi]) {
          memcpy(&g->out[pi * 4], &g->background[pi * 4], 4);
        }
      }
    } else {
      // This is a non-disposal case eithe way, so just
      // leave the pixels as is, and they will become the new background
      // 1: do not dispose
      // 0:  not specified.
    }

    // background is what out is after the undoing of the previou frame;
    memcpy(g->background, g->out, 4 * g->w * g->h);
  }

  // clear my history;
  memset(g->history, 0x00, g->w * g->h); // pixels that were affected previous frame

  for (;;) {
    int tag = stbi__get8(s);
    switch (tag) {
    case 0x2C: /* Image Descriptor */
    {
      int32_t x, y, w, h;
      stbi_uc *o;

      x = stbi__get16le(s);
      y = stbi__get16le(s);
      w = stbi__get16le(s);
      h = stbi__get16le(s);
      if (((x + w) > (g->w)) || ((y + h) > (g->h))) {
        std::cerr << "bad Image Descriptor && Corrupt GIF" << std::endl;
        return nullptr;
      }

      g->line_size = g->w * 4;
      g->start_x = x * 4;
      g->start_y = y * g->line_size;
      g->max_x = g->start_x + w * 4;
      g->max_y = g->start_y + h * g->line_size;
      g->cur_x = g->start_x;
      g->cur_y = g->start_y;

      // if the width of the specified rectangle is 0, that means
      // we may not see *any* pixels or the image is malformed;
      // to make sure this is caught, move the current y down to
      // max_y (which is what out_gif_code checks).
      if (w == 0)
        g->cur_y = g->max_y;

      g->lflags = stbi__get8(s);

      if (g->lflags & 0x40) {
        g->step = 8 * g->line_size; // first interlaced spacing
        g->parse = 3;
      } else {
        g->step = g->line_size;
        g->parse = 0;
      }

      if (g->lflags & 0x80) {
        stbi__gif_parse_colortable(s, g->lpal, 2 << (g->lflags & 7), g->eflags & 0x01 ? g->transparent : -1);
        g->color_table = (stbi_uc *)g->lpal;
      } else if (g->flags & 0x80) {
        g->color_table = (stbi_uc *)g->pal;
      } else
        return nullptr;

      o = stbi__process_gif_raster(s, g);
      if (!o)
        return nullptr;

      // if this was the first frame,
      pcount = g->w * g->h;
      if (first_frame && (g->bgindex > 0)) {
        // if first frame, any pixel not drawn to gets the background color
        for (pi = 0; pi < pcount; ++pi) {
          if (g->history[pi] == 0) {
            g->pal[g->bgindex][3] =
                255; // just in case it was made transparent, undo that; It will be reset next frame if need be;
            memcpy(&g->out[pi * 4], &g->pal[g->bgindex], 4);
          }
        }
      }

      return o;
    }

    case 0x21: // Comment Extension.
    {
      int len;
      int ext = stbi__get8(s);
      if (ext == 0xF9) { // Graphic Control Extension.
        len = stbi__get8(s);
        if (len == 4) {
          g->eflags = stbi__get8(s);
          g->delay = 10 * stbi__get16le(s); // delay - 1/100th of a second, saving as 1/1000ths.

          // unset old transparent
          if (g->transparent >= 0) {
            g->pal[g->transparent][3] = 255;
          }
          if (g->eflags & 0x01) {
            g->transparent = stbi__get8(s);
            if (g->transparent >= 0) {
              g->pal[g->transparent][3] = 0;
            }
          } else {
            // don't need transparent
            stbi__skip(s, 1);
            g->transparent = -1;
          }
        } else {
          stbi__skip(s, len);
          break;
        }
      }
      while ((len = stbi__get8(s)) != 0) {
        stbi__skip(s, len);
      }
      break;
    }

    case 0x3B:             // gif stream termination code
      return (stbi_uc *)s; // using '1' causes warning on some compilers

    default:
      return nullptr;
    }
  }
}

static auto stbi__compute_y(int r, int g, int b) -> stbi_uc {
  return (stbi_uc)(((r * 77) + (g * 150) + (29 * b)) >> 8);
}

static auto stbi__malloc_mad3(int a, int b, int c, int add) -> void * {
  if (!stbi__mad3sizes_valid(a, b, c, add))
    return nullptr;
  return malloc(a * b * c + add);
}

static auto stbi__convert_format(unsigned char *data, int img_n, int req_comp, unsigned int x, unsigned int y)
    -> unsigned char * {
  int i, j;
  unsigned char *good;

  if (req_comp == img_n)
    return data;
  assert(req_comp >= 1 && req_comp <= 4);

  good = (unsigned char *)stbi__malloc_mad3(req_comp, x, y, 0);
  if (good == nullptr) {
    free(data);
    return nullptr;
  }

  for (j = 0; j < (int)y; ++j) {
    unsigned char *src = data + j * x * img_n;
    unsigned char *dest = good + j * x * req_comp;

#define STBI__COMBO(a, b) ((a)*8 + (b))
#define STBI__CASE(a, b)  \
  case STBI__COMBO(a, b): \
    for (i = x - 1; i >= 0; --i, src += a, dest += b)
    // convert source image with img_n components to one with req_comp components;
    // avoid switch per pixel, so use switch per scanline and massive macros
    switch (STBI__COMBO(img_n, req_comp)) {
      STBI__CASE(1, 2) {
        dest[0] = src[0];
        dest[1] = 255;
      }
      break;
      STBI__CASE(1, 3) {
        dest[0] = dest[1] = dest[2] = src[0];
      }
      break;
      STBI__CASE(1, 4) {
        dest[0] = dest[1] = dest[2] = src[0];
        dest[3] = 255;
      }
      break;
      STBI__CASE(2, 1) {
        dest[0] = src[0];
      }
      break;
      STBI__CASE(2, 3) {
        dest[0] = dest[1] = dest[2] = src[0];
      }
      break;
      STBI__CASE(2, 4) {
        dest[0] = dest[1] = dest[2] = src[0];
        dest[3] = src[1];
      }
      break;
      STBI__CASE(3, 4) {
        dest[0] = src[0];
        dest[1] = src[1];
        dest[2] = src[2];
        dest[3] = 255;
      }
      break;
      STBI__CASE(3, 1) {
        dest[0] = stbi__compute_y(src[0], src[1], src[2]);
      }
      break;
      STBI__CASE(3, 2) {
        dest[0] = stbi__compute_y(src[0], src[1], src[2]);
        dest[1] = 255;
      }
      break;
      STBI__CASE(4, 1) {
        dest[0] = stbi__compute_y(src[0], src[1], src[2]);
      }
      break;
      STBI__CASE(4, 2) {
        dest[0] = stbi__compute_y(src[0], src[1], src[2]);
        dest[1] = src[3];
      }
      break;
      STBI__CASE(4, 3) {
        dest[0] = src[0];
        dest[1] = src[1];
        dest[2] = src[2];
      }
      break;
    default:
      assert(0);
      free(data);
      free(good);
      return nullptr;
    }
#undef STBI__CASE
  }

  free(data);
  return good;
}

static auto stbi__gif_load(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri) -> void * {
  uchar *u = nullptr;
  stbi__gif g;
  memset(&g, 0, sizeof(g));
  STBI_NOTUSED(ri);

  u = stbi__gif_load_next(s, &g, comp, req_comp, 0);
  if (u == (stbi_uc *)s)
    u = 0; // end of animated gif marker
  if (u) {
    *x = g.w;
    *y = g.h;

    // moved conversion to after successful load so that the same
    // can be done for multiple frames.
    if (req_comp && req_comp != 4)
      u = stbi__convert_format(u, 4, req_comp, g.w, g.h);
  } else if (g.out) {
    // if there was an error and we allocated an image buffer, free it!
    free(g.out);
  }

  // free buffers needed for multiple frame loading;
  free(g.history);
  free(g.background);

  return u;
}

static auto stbi__load_main(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri, int bpc)
    -> void * {
  memset(ri, 0, sizeof(*ri));         // make sure it's initialized if we add new fields
  ri->bits_per_channel = 8;           // default is 8 so most paths don't have to be changed
  ri->channel_order = STBI_ORDER_RGB; // all current input & output are this, but this is here so we can add BGR order
  ri->num_channels = 0;

  // test the formats with a very explicit header first (at least a FOURCC
  // or distinctive magic number first)

#ifndef STBI_NO_GIF
  if (stbi__gif_test(s))
    return stbi__gif_load(s, x, y, comp, req_comp, ri);
#endif
  std::cerr << "unknown image type && Image not of any known type, or corrupt" << std::endl;
  return nullptr;
}

static auto stbi__convert_16_to_8(uint16_t *orig, int w, int h, int channels) -> stbi_uc * {
  int i;
  int img_len = w * h * channels;
  stbi_uc *reduced;

  reduced = (stbi_uc *)malloc(img_len);
  if (reduced == nullptr)
    return nullptr;

  for (i = 0; i < img_len; ++i)
    reduced[i] = (stbi_uc)((orig[i] >> 8) & 0xFF); // top half of each byte is sufficient approx of 16->8 bit scaling

  free(orig);
  return reduced;
}

static int stbi__vertically_flip_on_load_global = 0;
#define stbi__vertically_flip_on_load stbi__vertically_flip_on_load_global

static void stbi__vertical_flip(void *image, int w, int h, int bytes_per_pixel) {
  int row;
  size_t bytes_per_row = (size_t)w * bytes_per_pixel;
  stbi_uc temp[2048];
  stbi_uc *bytes = (stbi_uc *)image;

  for (row = 0; row < (h >> 1); row++) {
    stbi_uc *row0 = bytes + row * bytes_per_row;
    stbi_uc *row1 = bytes + (h - row - 1) * bytes_per_row;
    // swap row0 with row1
    size_t bytes_left = bytes_per_row;
    while (bytes_left) {
      size_t bytes_copy = (bytes_left < sizeof(temp)) ? bytes_left : sizeof(temp);
      memcpy(temp, row0, bytes_copy);
      memcpy(row0, row1, bytes_copy);
      memcpy(row1, temp, bytes_copy);
      row0 += bytes_copy;
      row1 += bytes_copy;
      bytes_left -= bytes_copy;
    }
  }
}

static auto stbi__load_and_postprocess_8bit(stbi__context *s, int *x, int *y, int *comp, int req_comp)
    -> unsigned char * {
  stbi__result_info ri;
  void *result = stbi__load_main(s, x, y, comp, req_comp, &ri, 8);

  if (result == nullptr)
    return nullptr;

  // it is the responsibility of the loaders to make sure we get either 8 or 16 bit.
  assert(ri.bits_per_channel == 8 || ri.bits_per_channel == 16);

  if (ri.bits_per_channel != 8) {
    result = stbi__convert_16_to_8((uint16_t *)result, *x, *y, req_comp == 0 ? *comp : req_comp);
    ri.bits_per_channel = 8;
  }

  // @TODO: move stbi__convert_format to here

  if (stbi__vertically_flip_on_load) {
    int channels = req_comp ? req_comp : *comp;
    stbi__vertical_flip(result, *x, *y, channels * sizeof(stbi_uc));
  }

  return (unsigned char *)result;
}

static void stbi__start_callbacks(stbi__context *s, stbi_io_callbacks *c, void *user) {
  s->io = *c;
  s->io_user_data = user;
  s->buflen = sizeof(s->buffer_start);
  s->read_from_callbacks = 1;
  s->callback_already_read = 0;
  s->img_buffer = s->img_buffer_original = s->buffer_start;
  stbi__refill_buffer(s);
  s->img_buffer_original_end = s->img_buffer_end;
}

static void stbi__start_file(stbi__context *s, FILE *f) {
  stbi__start_callbacks(s, &stbi__stdio_callbacks, (void *)f);
}

extern auto stbi_load_from_file(FILE *f, int *x, int *y, int *comp, int req_comp) -> uchar * {
  unsigned char *result;
  stbi__context s;
  stbi__start_file(&s, f);
  result = stbi__load_and_postprocess_8bit(&s, x, y, comp, req_comp);
  if (result) {
    // need to 'unget' all the characters in the IO buffer
    fseek(f, -(int)(s.img_buffer_end - s.img_buffer), SEEK_CUR);
  }
  return result;
}

#include <sys/stat.h>
// 通过stat结构体 获得文件大小，单位字节
auto getFileSize1(const char *fileName) -> size_t {

  if (fileName == nullptr) {
    return 0;
  }

  // 这是一个存储文件(夹)信息的结构体，其中有文件大小和创建时间、访问时间、修改时间等
  struct stat statbuf;
  statbuf.st_size = 0;
  // 提供文件名字符串，获得文件属性结构体
  stat(fileName, &statbuf);

  // 获取文件大小
  size_t filesize = statbuf.st_size;

  return filesize;
}

extern auto stbi_load(char const *filename, int *x, int *y, int *comp, int req_comp) -> uchar * {
  std::cout << "image size : " << getFileSize1(filename) << std::endl;
  system("pwd");
  FILE *f = fopen(filename, "rb");
  std::cout << filename << std::endl;
  unsigned char *result;
  if (!f) {
    std::cerr << "can't fopen && Unable to open file" << std::endl;
    return nullptr;
  }

  result = stbi_load_from_file(f, x, y, comp, req_comp);
  fclose(f);
  return result;
}

#endif