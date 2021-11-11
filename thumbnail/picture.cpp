/*
Copyright (C) 2017  Paul Kremer

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "picture.h"
//#include <libimagequant.h>
//#include <iostream>
#include "lodepng.h" // Get it from https://raw.githubusercontent.com/lvandeve/lodepng/master/lodepng.h
#include "lodepng.cpp" // Get it from https://raw.githubusercontent.com/lvandeve/lodepng/master/lodepng.cpp

static Byte floatToByte(float v)
{
    if (v > 1.0f)
    {
        v = 1.0f;
    }
    if (v < 0.0f)
    {
        v = 0.0f;
    }
    return Byte(v * 255.0f);
}

Picture::Picture(size_t width, size_t height, int depth)
    : width_(width), height_(height), bg_file_path_(), depth_(depth)
{
    stride_ = width_ * depth_;
    buffer_.resize(height_ * stride_);
}

void Picture::setBg(const char* bg_file_path, const Vec4& bg_color)
{
    bg_color_ = bg_color;

    if (nullptr == bg_file_path)
    {
        fill(bg_color_.x, bg_color_.y, bg_color_.z, bg_color_.w);
        return;
    }

    bg_file_path_.assign(bg_file_path);

    FILE* fp = nullptr;
    png_structp png_ptr = nullptr;
    png_infop info_ptr = nullptr;
    bool success = false;

    do
    {
        fp = fopen(bg_file_path_.c_str(), "rb");
        if (nullptr == fp)
        {
            break;
        }

        png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        if (nullptr == png_ptr)
        {
            break;
        }

        info_ptr = png_create_info_struct( png_ptr );
        if (nullptr == info_ptr)
        {
            break;
        }

        setjmp(png_jmpbuf(png_ptr));

        /* 读取PNG_BYTES_TO_CHECK个字节的数据 */
        const int PNG_BYTES_TO_CHECK = 4;
        char buf[PNG_BYTES_TO_CHECK] = "";

        int ret = fread(buf, 1, PNG_BYTES_TO_CHECK, fp);
        if (ret < PNG_BYTES_TO_CHECK) /* 若读到的数据并没有PNG_BYTES_TO_CHECK个字节 */
        {
            break;
        }

        /* 检测数据是否为PNG的签名 */
        ret = png_sig_cmp((png_bytep) buf, (png_size_t) 0, PNG_BYTES_TO_CHECK);
        if (ret != 0) /* 如果不是PNG的签名，则说明该文件不是PNG文件 */
        {
            break;
        }

        rewind(fp); /* 复位文件指针 */

        png_init_io(png_ptr, fp);
        png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, nullptr); /* 读取PNG图片信息和像素数据 */

        const png_byte color_type = png_get_color_type(png_ptr, info_ptr);
        const png_uint_32 w = png_get_image_width(png_ptr, info_ptr);
        const png_uint_32 h = png_get_image_height(png_ptr, info_ptr);
        if (w != width_ || h != height_)
        {
            break;
        }

        /* 获取图像的所有行像素数据，row_pointers里边就是rgba数据 */
        png_bytep* row_pointers = png_get_rows(png_ptr, info_ptr);
        if (nullptr == row_pointers)
        {
            break;
        }

        setBg(color_type, row_pointers, bg_color_);
        success = true;
    } while (0);

    if (!success)
    {
        fill(bg_color_.x, bg_color_.y, bg_color_.z, bg_color_.w); // 如果设置图片背景失败，则设置背景色//
    }

    if (png_ptr != nullptr)
    {
        if (info_ptr != nullptr)
        {
            png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        }
        else
        {
            png_destroy_read_struct(&png_ptr, nullptr, nullptr);
        }
    }

    if (fp != nullptr)
    {
        fclose(fp);
    }
}

int Picture::save(const std::string& file_path)
{
    FILE* fp = fopen(file_path.c_str(), "wb");
    if (nullptr == fp)
    {
        return -1;
    }

    png_structp png_ptr  = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (nullptr == png_ptr)
    {
        fclose(fp);
        return -1;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (nullptr == info_ptr)
    {
        png_destroy_write_struct(&png_ptr, nullptr);
        fclose(fp);
        return -1;
    }

    png_init_io(png_ptr, fp);
    png_set_IHDR(png_ptr, info_ptr, width_, height_,
                 8, (4 == depth_) ? PNG_COLOR_TYPE_RGBA : PNG_COLOR_TYPE_RGB,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    png_write_info(png_ptr, info_ptr);

    for (size_t y = 0; y < height_; ++y)
    {
        png_write_row(png_ptr, &buffer_[y * stride_]);
    }

    png_write_end(png_ptr, nullptr);

    png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);

    //compress(file_path);
    return 0;
}

void Picture::setRGB(size_t x, size_t y, float r, float g, float b, float a)
{
    setRGB(x, y, floatToByte(r), floatToByte(g), floatToByte(b), floatToByte(a));
}

void Picture::copyto(unsigned char* data)
{
    memcpy(data, buffer_.data(), width_ * height_ * 4);
}

void Picture::fill(float r, float g, float b, float a)
{
    for (size_t y = 0; y < height_; ++y)
    {
        for (size_t x = 0; x < width_; ++x)
        {
            setRGB(x, y, r, g, b, a);
        }
    }
}

void Picture::setBg(png_byte color_type, png_bytep* row_pointers, const Vec4& bg_color)
{
    /* 根据不同的色彩类型进行相应处理 */
    switch (color_type)
    {
        case PNG_COLOR_TYPE_RGB_ALPHA:
        {
            png_uint_32 pos = 0;

            for (png_uint_32 y = 0; y < height_; ++y)
            {
                for (png_uint_32 x = 0; x < width_ * 4; )
                {
                    /* 以下是RGBA数据，需要自己补充代码，保存RGBA数据 */
                    buffer_[pos++] = row_pointers[y][x++]; // red
                    buffer_[pos++] = row_pointers[y][x++]; // green
                    buffer_[pos++] = row_pointers[y][x++]; // blue

                    if (4 == depth_)
                    {
                        buffer_[pos++] = row_pointers[y][x++]; // alpha
                    }
                }
            }

//            for ( y = 0; y < h; ++y )
//            {
//                for ( x = 0; x < w * 4; )
//                {
//                    /* 以下是RGBA数据，需要自己补充代码，保存RGBA数据 */
//                    /* 目标内存 */ = row_pointers[y][x++]; // red
//                    /* 目标内存 */ = row_pointers[y][x++]; // green
//                    /* 目标内存 */ = row_pointers[y][x++]; // blue
//                    /* 目标内存 */ = row_pointers[y][x++]; // alpha
//                }
//            }
        }
        break;

        case PNG_COLOR_TYPE_RGB:
        {
            png_uint_32 pos = 0;

            for (png_uint_32 y = 0; y < height_; ++y)
            {
                for (png_uint_32 x = 0; x < width_ * 3; )
                {
                    /* 以下是RGBA数据，需要自己补充代码，保存RGBA数据 */
                    buffer_[pos++] = row_pointers[y][x++]; // red
                    buffer_[pos++] = row_pointers[y][x++]; // green
                    buffer_[pos++] = row_pointers[y][x++]; // blue

                    if (4 == depth_)
                    {
                        buffer_[pos++] = 255; // alpha
                    }
                }
            }
//            for ( y = 0; y < h; ++y )
//            {
//                for ( x = 0; x < w * 3; )
//                {
//                    /* 目标内存 */ = row_pointers[y][x++]; // red
//                    /* 目标内存 */ = row_pointers[y][x++]; // green
//                    /* 目标内存 */ = row_pointers[y][x++]; // blue
//                }
//            }
        }
        break;

        /* 其它色彩类型的图像就不读了 */

        default:
        {
            fill(bg_color.x, bg_color.y, bg_color.z, bg_color.w); // 设置背景色 //
        }
        break;
    }
}

void Picture::setRGB(size_t x, size_t y, Byte r, Byte g, Byte b, Byte a)
{
    if (x >= width_ || y >= height_)
    {
        return;
    }

    const size_t i = y * stride_ + x * depth_;

    buffer_[i + 0] = r;
    buffer_[i + 1] = g;
    buffer_[i + 2] = b;

    if (4 == depth_)
    {
        buffer_[i + 3] = a;
    }
}

int Picture::compress(const std::string& file_path)
{
    const char* input_png_file_path = file_path.c_str();
    const char* output_png_file_path = file_path.c_str();

    // Load PNG file and decode it as raw RGBA pixels
    // This uses lodepng library for PNG reading (not part of libimagequant)

    unsigned int width, height;
    unsigned char* raw_rgba_pixels = nullptr;
    unsigned int status = lodepng_decode32_file(&raw_rgba_pixels, &width, &height, input_png_file_path);
    if (status || nullptr == raw_rgba_pixels)
    {
//        fprintf(stderr, "Can't load %s: %s\n", input_png_file_path, lodepng_error_text(status));
        return -1;
    }

    // Use libimagequant to make a palette for the RGBA pixels
#if 0
    liq_attr* handle = nullptr;
    liq_image* input_image = nullptr;
    liq_result* quantization_result = nullptr;
    size_t pixels_size = 0;
    unsigned char* raw_8bit_pixels = nullptr;
    const liq_palette* palette = nullptr;
    unsigned char* output_file_data = nullptr;
    FILE* fp = nullptr;

    handle = liq_attr_create();
    if (nullptr == handle)
    {
        goto cleanup;
    }

    input_image = liq_image_create_rgba(handle, raw_rgba_pixels, width, height, 0);
    if (nullptr == input_image)
    {
        goto cleanup;
    }

    // You could set more options here, like liq_set_quality
    liq_set_quality(handle, 80, 80);

    if (liq_image_quantize(input_image, handle, &quantization_result) != LIQ_OK
            || nullptr == quantization_result)
    {
//        fprintf(stderr, "Quantization failed\n");
        goto cleanup;
    }

    // Use libimagequant to make new image pixels from the palette

    pixels_size = width * height;
    raw_8bit_pixels = (unsigned char*) malloc(pixels_size);
    if (nullptr == raw_8bit_pixels)
    {
        goto cleanup;
    }

    liq_set_dithering_level(quantization_result, 1.0);

    liq_write_remapped_image(quantization_result, input_image, raw_8bit_pixels, pixels_size);
    palette = liq_get_palette(quantization_result);

    // Save converted pixels as a PNG file
    // This uses lodepng library for PNG writing (not part of libimagequant)

    LodePNGState state;
    lodepng_state_init(&state);
    state.info_raw.colortype = LCT_PALETTE;
    state.info_raw.bitdepth = 8;
    state.info_png.color.colortype = LCT_PALETTE;
    state.info_png.color.bitdepth = 8;

    for (int i = 0; i < (int) palette->count; i++)
    {
        lodepng_palette_add(&state.info_png.color, palette->entries[i].r, palette->entries[i].g, palette->entries[i].b, palette->entries[i].a);
        lodepng_palette_add(&state.info_raw, palette->entries[i].r, palette->entries[i].g, palette->entries[i].b, palette->entries[i].a);
    }

    size_t output_file_size;
    if (lodepng_encode(&output_file_data, &output_file_size, raw_8bit_pixels, width, height, &state) != 0)
    {
//        fprintf(stderr, "Can't encode image: %s\n", lodepng_error_text(out_status));
        goto cleanup;
    }

//    const char* output_png_file_path = (file_path + ".ppp").c_str();
    fp = fopen(output_png_file_path, "wb");
    if (fp != nullptr)
    {
        fwrite(output_file_data, 1, output_file_size, fp);
    }

//    printf("Written %s\n", output_png_file_path);

    // Done. Free memory.

cleanup:
    if (fp != nullptr)
    {
        fclose(fp);
        fp = nullptr;
    }

    lodepng_state_cleanup(&state);

    if (output_file_data != nullptr)
    {
        lodepng_free(output_file_data);
        output_file_data = nullptr;
    }

    if (raw_8bit_pixels != nullptr)
    {
        free(raw_8bit_pixels);
        raw_8bit_pixels = nullptr;
    }

    if (quantization_result != nullptr)
    {
        liq_result_destroy(quantization_result);
        quantization_result = nullptr;
    }

    if (input_image != nullptr)
    {
        liq_image_destroy(input_image);
        input_image = nullptr;
    }

    if (handle != nullptr)
    {
        liq_attr_destroy(handle);
        handle = nullptr;
    }

    if (raw_rgba_pixels != nullptr)
    {
        lodepng_free(raw_rgba_pixels);
        raw_rgba_pixels = nullptr;
    }
#endif 
    //
//    liq_result_destroy(quantization_result); // Must be freed only after you're done using the palette
//    liq_image_destroy(input_image);
//    liq_attr_destroy(handle);

//    free(raw_8bit_pixels);
//    lodepng_state_cleanup(&state);

//    rename(output_png_file_path, input_png_file_path);
    return 0;
}
