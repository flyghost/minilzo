#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLineEdit>
#include <QFormLayout>
#include <QFileDialog>
#include <QDebug>



#include "miniz.h"
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

QString input_file,output_file;

#define malloc malloc
#define free free

#define BLOCK_HEADER_SIZE 4
#define FILE_HEADER_SIZE 4

/* The output buffer can not be smaller than 66 bytes */
// 输出缓冲区不能小于66字节
#define COMPRESS_BUFFER_SIZE 8192
#define DCOMPRESS_BUFFER_SIZE 8192

/*  we must provide a little more output space in case that compression is not possible */
// 如果无法压缩，我们必须提供更多的输出空间
#define MINILZO_BUFFER_PADDING(x) ((x) / 16 + 64 + 3)
#define BUFFER_PADDING MINILZO_BUFFER_PADDING(COMPRESS_BUFFER_SIZE) // ( 8192 / 16 + 64 + 3 ) = 512 + 64 + 3 = 579

static const uint32_t crc32_le_table[256] = {
    0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L, 0x706af48fL, 0xe963a535L, 0x9e6495a3L,
    0x0edb8832L, 0x79dcb8a4L, 0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L, 0x90bf1d91L,
    0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL, 0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L,
    0x136c9856L, 0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L, 0xfa0f3d63L, 0x8d080df5L,
    0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L, 0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
    0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L, 0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L,
    0x26d930acL, 0x51de003aL, 0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L, 0xb8bda50fL,
    0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L, 0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL,
    0x76dc4190L, 0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL, 0x9fbfe4a5L, 0xe8b8d433L,
    0x7807c9a2L, 0x0f00f934L, 0x9609a88eL, 0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
    0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL, 0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L,
    0x65b0d9c6L, 0x12b7e950L, 0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L, 0xfbd44c65L,
    0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L, 0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL,
    0x4369e96aL, 0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L, 0xaa0a4c5fL, 0xdd0d7cc9L,
    0x5005713cL, 0x270241aaL, 0xbe0b1010L, 0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
    0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L, 0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL,

    0xedb88320L, 0x9abfb3b6L, 0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L, 0x73dc1683L,
    0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L, 0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L,
    0xf00f9344L, 0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL, 0x196c3671L, 0x6e6b06e7L,
    0xfed41b76L, 0x89d32be0L, 0x10da7a5aL, 0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
    0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L, 0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL,
    0xd80d2bdaL, 0xaf0a1b4cL, 0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL, 0x4669be79L,
    0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L, 0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL,
    0xc5ba3bbeL, 0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L, 0x2cd99e8bL, 0x5bdeae1dL,
    0x9b64c2b0L, 0xec63f226L, 0x756aa39cL, 0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
    0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL, 0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L,
    0x86d3d2d4L, 0xf1d4e242L, 0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L, 0x18b74777L,
    0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL, 0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L,
    0xa00ae278L, 0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L, 0x4969474dL, 0x3e6e77dbL,
    0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L, 0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
    0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L, 0xcdd70693L, 0x54de5729L, 0x23d967bfL,
    0xb3667a2eL, 0xc4614ab8L, 0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL, 0x2d02ef8dL
};

static const uint16_t crc16_le_table[256] = {
    0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
    0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
    0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
    0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
    0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
    0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
    0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
    0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
    0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
    0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
    0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
    0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
    0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
    0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
    0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
    0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
    0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
    0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
    0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
    0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
    0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
    0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
    0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
    0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
    0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
    0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
    0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
    0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
    0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
    0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
    0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
    0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

static const uint8_t crc8_le_table[] = {
    0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83, 0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41,
    0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e, 0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc,
    0x23, 0x7d, 0x9f, 0xc1, 0x42, 0x1c, 0xfe, 0xa0, 0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62,
    0xbe, 0xe0, 0x02, 0x5c, 0xdf, 0x81, 0x63, 0x3d, 0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff,
    0x46, 0x18, 0xfa, 0xa4, 0x27, 0x79, 0x9b, 0xc5, 0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07,
    0xdb, 0x85, 0x67, 0x39, 0xba, 0xe4, 0x06, 0x58, 0x19, 0x47, 0xa5, 0xfb, 0x78, 0x26, 0xc4, 0x9a,
    0x65, 0x3b, 0xd9, 0x87, 0x04, 0x5a, 0xb8, 0xe6, 0xa7, 0xf9, 0x1b, 0x45, 0xc6, 0x98, 0x7a, 0x24,
    0xf8, 0xa6, 0x44, 0x1a, 0x99, 0xc7, 0x25, 0x7b, 0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9,
    0x8c, 0xd2, 0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f, 0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd,
    0x11, 0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92, 0xd3, 0x8d, 0x6f, 0x31, 0xb2, 0xec, 0x0e, 0x50,
    0xaf, 0xf1, 0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c, 0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee,
    0x32, 0x6c, 0x8e, 0xd0, 0x53, 0x0d, 0xef, 0xb1, 0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73,
    0xca, 0x94, 0x76, 0x28, 0xab, 0xf5, 0x17, 0x49, 0x08, 0x56, 0xb4, 0xea, 0x69, 0x37, 0xd5, 0x8b,
    0x57, 0x09, 0xeb, 0xb5, 0x36, 0x68, 0x8a, 0xd4, 0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa, 0x48, 0x16,
    0xe9, 0xb7, 0x55, 0x0b, 0x88, 0xd6, 0x34, 0x6a, 0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8,
    0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7, 0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35
};

uint16_t crc16_le(uint16_t crc, const uint8_t *buf, uint32_t len)
{
    uint32_t i;

    crc = ~crc;

    for (i = 0; i < len; i++)
    {
        crc = ((const uint16_t *)crc16_le_table)[(crc ^ buf[i]) & 0xff] ^ (crc >> 8);
    }

    return ~crc;
}

uint32_t crc32_le(uint32_t crc, const uint8_t *buf, uint32_t len)
{
    uint32_t i;

    crc = ~crc;

    for (i = 0; i < len; i++)
    {
        crc = crc32_le_table[(crc ^ buf[i]) & 0xff] ^ (crc >> 8);
    }

    return ~crc;
}

uint8_t crc8_le(uint8_t crc, uint8_t const *p, uint32_t len)
{
    while (len--)
    {
        crc = ((const uint8_t *)crc8_le_table)[crc ^ *p++];
    }

    return crc;
}

static int miniz_compress_file(int fd_in, int fd_out)
{
    /* Start to compress file  */
    unsigned char *cmprs_buffer = NULL;
    unsigned char *buffer = NULL;
    uint8_t buffer_hdr[BLOCK_HEADER_SIZE] = { 0 };
    uint8_t image_buffer_hdr[FILE_HEADER_SIZE] = { 0 };
    mz_ulong cmprs_size = 0, total_cmprs_size = 0;
    mz_ulong block_size = 0;
    size_t file_size = 0, i = 0;
    int ret = 0;

    uint8_t crc_cal_buffer[4096] = { 0 };
    int nread = 0;
    uint32_t result = UINT32_MAX;

    uint32_t init_filesize = 0;

    file_size = lseek(fd_in, 0, SEEK_END); // 返回的是文件的长度
    lseek(fd_in, 0, SEEK_SET);             // 将文件指针指向开头

    cmprs_buffer = (uint8_t *)malloc(COMPRESS_BUFFER_SIZE + BUFFER_PADDING); // 8192 +  ( 8192 / 16 + 64 + 3 ) = 8192 + 512 + 64 + 3 =  8192 + 579
    buffer = (unsigned char *)malloc(COMPRESS_BUFFER_SIZE);                        // 8192
    if (!cmprs_buffer || !buffer)
    {
        //printf("[minilzo] No memory for cmprs_buffer or buffer!\n");
        qDebug()<<"[minilzo] No memory for cmprs_buffer or buffer!";
        ret = -1;
        goto _exit;
    }

    //printf("[minilzo]compress start : \n");
    qDebug()<<"[minilzo]compress start : \n";

//    uint32_t init_filesize = 0;
    write(fd_out, &init_filesize, FILE_HEADER_SIZE); // 初始化输出文件帧头大小为0

    for (i = 0; i < file_size; i += COMPRESS_BUFFER_SIZE)   // 8192  8k  只能8K增加
    {
        if ((file_size - i) < COMPRESS_BUFFER_SIZE)
        {
            block_size = file_size - i;
        }
        else
        {
            block_size = COMPRESS_BUFFER_SIZE;
        }

        memset(buffer, 0x00, COMPRESS_BUFFER_SIZE);
        memset(cmprs_buffer, 0x00, COMPRESS_BUFFER_SIZE + BUFFER_PADDING);

        cmprs_size = COMPRESS_BUFFER_SIZE + BUFFER_PADDING;

        read(fd_in, buffer, block_size);        // 读取待压缩的文件

        /* The destination buffer must be at least size + 400 bytes large because incompressible data may increase in size. */
        //ret = lzo1x_1_compress(buffer, block_size, cmprs_buffer, &cmprs_size, wrkmem);
        ret = compress(static_cast<unsigned char *>(cmprs_buffer),
                       &cmprs_size,
                       buffer,
                       block_size);
        if (ret != Z_OK)
        {
            //printf("compress return error[%d]\n", ret);
            qDebug()<<"compress return error[%d]\n";
            ret = -1;
            goto _exit;
        }

        /* Store compress block size to the block header (4 byte). */
        buffer_hdr[3] = cmprs_size % (1 << 8);
        buffer_hdr[2] = (cmprs_size % (1 << 16)) / (1 << 8);
        buffer_hdr[1] = (cmprs_size % (1 << 24)) / (1 << 16);
        buffer_hdr[0] = cmprs_size / (1 << 24);

        // printf("---%02x %02x %02x %02x\n", buffer_hdr[0], buffer_hdr[1], buffer_hdr[2], buffer_hdr[3]);
        write(fd_out, buffer_hdr, BLOCK_HEADER_SIZE);
        write(fd_out, cmprs_buffer, cmprs_size);
        //printf("cmprs_size %d\n", cmprs_size);
        qDebug()<<"cmprs_size "<< cmprs_size;
        total_cmprs_size += cmprs_size + BLOCK_HEADER_SIZE;
    }
    lseek(fd_out, 0, SEEK_SET);
    image_buffer_hdr[3] = total_cmprs_size % (1 << 8);
    image_buffer_hdr[2] = (total_cmprs_size % (1 << 16)) / (1 << 8);
    image_buffer_hdr[1] = (total_cmprs_size % (1 << 24)) / (1 << 16);
    image_buffer_hdr[0] = total_cmprs_size / (1 << 24);
    write(fd_out, image_buffer_hdr, FILE_HEADER_SIZE);

    //caculate crc32
    lseek(fd_out, 0, SEEK_SET);
//    uint8_t crc_cal_buffer[4096] = { 0 };
//    int nread = 0;
//    uint32_t result = UINT32_MAX;
    while ((nread = read(fd_out, crc_cal_buffer, 4096)) > 0)
    {
        result = crc32_le(result, crc_cal_buffer, nread);
    }
    lseek(fd_out, 0, SEEK_END);
    write(fd_out, &result, sizeof(result));
    //printf("\n");
//    printf("[minilzo]compressed %d bytes into %d bytes , compression ratio is %d%, crc is %08x!\n", file_size, total_cmprs_size,
//           (total_cmprs_size * 100) / file_size, result);
//    qDebug()<<"[minilzo]compressed" <<file_size
//           << "bytes into" << total_cmprs_size
//           << "bytes , compression ratio is "
//          << total_cmprs_size <<"%," <<" crc is "%08x!\n", , ,
//            (total_cmprs_size * 100) / file_size, result);
_exit:
    if (cmprs_buffer)
    {
        free(cmprs_buffer);
    }

    if (buffer)
    {
        free(buffer);
    }

    return ret;
}

static int miniz_decompress_file(int fd_in, int fd_out)
{
    /* Start to decompress file  */
    uint8_t *dcmprs_buffer = NULL, *buffer = NULL;
    uint8_t buffer_hdr[BLOCK_HEADER_SIZE] = { 0 };
    uint8_t image_buffer_hdr[FILE_HEADER_SIZE] = { 0 };
    size_t dcmprs_size = 0, block_size = 0, total_dcmprs_size = 0;
    size_t file_size = 0, i = 0, head_file_size = 0;
    int ret = 0;

    file_size = lseek(fd_in, 0, SEEK_END);
    lseek(fd_in, 0, SEEK_SET);

    //file size is in the compressd package head and crc32 checksum
    file_size -= 8;

    read(fd_in, image_buffer_hdr, FILE_HEADER_SIZE);
    head_file_size = image_buffer_hdr[0] * (1 << 24) + image_buffer_hdr[1] * (1 << 16) + image_buffer_hdr[2] * (1 << 8) + image_buffer_hdr[3];
    if (file_size <= BLOCK_HEADER_SIZE || file_size != head_file_size)
    {
        printf("[minilzo] decomprssion file size : %d error, head_file_size is %d !\n", file_size, head_file_size);
        ret = -1;
        goto _dcmprs_exit;
    }

    dcmprs_buffer = (uint8_t *)malloc(DCOMPRESS_BUFFER_SIZE);
    dcmprs_size = DCOMPRESS_BUFFER_SIZE;
    buffer = (uint8_t *)malloc(DCOMPRESS_BUFFER_SIZE + BUFFER_PADDING);
    if (!dcmprs_buffer || !buffer)
    {
        printf("[minilzo] No memory for dcmprs_buffer or buffer!\n");
        ret = -1;
        goto _dcmprs_exit;
    }

    printf("[minilzo]decompress start : ");
    for (i = 0; i < file_size; i += BLOCK_HEADER_SIZE + block_size)
    {
        /* Get the decompress block size from the block header. */
        read(fd_in, buffer_hdr, BLOCK_HEADER_SIZE);
        block_size = buffer_hdr[0] * (1 << 24) + buffer_hdr[1] * (1 << 16) + buffer_hdr[2] * (1 << 8) + buffer_hdr[3];

        memset(buffer, 0x00, COMPRESS_BUFFER_SIZE + BUFFER_PADDING);
        memset(dcmprs_buffer, 0x00, DCOMPRESS_BUFFER_SIZE);

        read(fd_in, buffer, block_size);

        //ret = lzo1x_decompress(buffer, block_size, dcmprs_buffer, &dcmprs_size, NULL);
        ret = uncompress(dcmprs_buffer, &dcmprs_size, buffer, block_size);
        if (ret != Z_OK)
        {
            ret = -1;
            goto _dcmprs_exit;
        }

        write(fd_out, dcmprs_buffer, dcmprs_size);

        total_dcmprs_size += dcmprs_size;
        printf(">");
    }
    printf("\n");
    printf("decompressed %d bytes into %d bytes !\n", file_size, total_dcmprs_size);

_dcmprs_exit:
    if (dcmprs_buffer)
    {
        free(dcmprs_buffer);
    }

    if (buffer)
    {
        free(buffer);
    }

    return ret;
}

static int file_compress(int mode)
{
    int fd_in = -1, fd_out = -1;
    int ret = 0;

//    if (argc != 4)
//    {
//        printf("Usage:\n");
//        printf("minilzo_test -c [file] [cmprs_file]          -compress \"file\" to \"cmprs_file\" \n");
//        printf("minilzo_test -d [cmprs_file] [dcmprs_file]   -dcompress \"cmprs_file\" to \"dcmprs_file\" \n");

//        ret = -1;
//        goto _exit;
//    }

    printf("miniz.c version: %s\n", MZ_VERSION);

    //fd_in = open(argv[2], O_RDONLY, 0);/home/wzy/miniz/aaa.txt
    //fd_in = open("/home/wzy/miniz/aaa.txt", O_RDONLY, 0);
    char*  ch;
    QByteArray ba = input_file.toLatin1();
    ch=ba.data();
    fd_in = open(ch, O_RDONLY, 0);
    if (fd_in < 0)
    {
        printf("[minilzo] open the input file : %s error!\n");
        ret = -1;
        goto _exit;
    }

    //fd_out = open(argv[3], O_CREAT | O_RDWR | O_TRUNC, 0666);

    ba = output_file.toLatin1();
    ch=ba.data();
    //fd_out = open("/home/wzy/miniz/aaab.txt", O_CREAT | O_RDWR | O_TRUNC, 0666);
    fd_out = open(ch, O_CREAT | O_RDWR | O_TRUNC, 0666);
    if (fd_out < 0)
    {
        printf("[minilzo] open the output file : error %d!\n", errno);
        ret = -1;
        goto _exit;
    }

//    if (miniz_compress_file(fd_in, fd_out) < 0)
//    {
//        printf("[minilzo] minilzo compress file error!\n");
//    }
    // 压缩
    if (mode == 0)
    {

        if (miniz_compress_file(fd_in, fd_out) < 0)
        {
            printf("[minilzo] minilzo compress file error!\n");
        }
    }
    // 解压
    else if (mode == 1)
    {

        if (miniz_decompress_file(fd_in, fd_out) < 0)
        {
            printf("[minilzo] minilzo decompress file error!\n");
        }
    }
    else
    {
        printf("Usage:\n");
        printf("param = 0          -compress \"file\" to \"cmprs_file\" \n");
        printf("param = 1   -dcompress \"cmprs_file\" to \"dcmprs_file\" \n");

        ret = -1;
        goto _exit;
    }

_exit:
    if (fd_in >= 0)
    {
        close(fd_in);
    }

    if (fd_out >= 0)
    {
        close(fd_out);
    }
}


void MainWindow::compress_file()
{
    input_file = buttonEdit1->text();
    output_file = input_file + ".compress";
    file_compress(0);
}

void MainWindow::decompress_file()
{
    input_file = buttonEdit1->text();
    output_file = input_file + ".decompress";
    file_compress(1);
}






MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    buttonEditInit();

//    file = new QFile("/home/wzy/qt/minilzo/minilzo/raw");
//    //QFile file("/home/wzy/qt/minilzo/minilzo/raw");

//    if(!file->open(QIODevice::ReadWrite | QIODevice::Text)) {
//            qDebug()<<"Can't open the file!"<<endl;
//    }

//    //QTextStream stream(&file);

//    filestream = new QTextStream(file);

    QObject::connect(ui->compressButton, SIGNAL(clicked()), this, SLOT(compressButton_Clicked()));
//    file_compress(0);
    QObject::connect(ui->decompressButton, SIGNAL(clicked()), this, SLOT(decompressButton_clicked()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

#include <QApplication>

void MainWindow::buttonEditInit()
{
    // 按钮显示文字
    buttonEdit1 = new ButtonEdit("打开");
    // 按钮显示图标
    //buttonEdit1 = new ButtonEdit(QApplication::style()->standardIcon(QStyle::SP_DirOpenIcon));

//    QObject::connect(buttonEdit1,  &ButtonEdit::buttonClicked, this, path_lineEdit1_Clicked());
    QObject::connect(buttonEdit1,  &ButtonEdit::buttonClicked, this, &MainWindow::path_lineEdit1_Clicked);
    auto layout = new QFormLayout;
    layout->addRow("", buttonEdit1);

    ui->buttonEdit->setLayout(layout);
}

void MainWindow::path_lineEdit1_Clicked()
{
    // 方法一
//    bin_path = get_path();
    // 方法二
    QString bin_path=QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("选择bin文件"),
                                                  qApp->applicationDirPath(),
                                                  QString::fromLocal8Bit("bin File(*)"));
    buttonEdit1->setText(bin_path);
    ui->outputlineEdit->setText(bin_path);
}

void MainWindow::compressButton_Clicked()
{
    if(buttonEdit1->text() == nullptr || ui->outputlineEdit->text() == nullptr)
        //printf("ddddd");
        //qDebug()<<"dddd";
        return;
    compress_file();
}

void MainWindow::decompressButton_clicked()
{qDebug()<<"dddd";
    if(buttonEdit1->text() == nullptr || ui->outputlineEdit->text() == nullptr)
        //printf("ddddd");
        //qDebug()<<"dddd";
        return;
    decompress_file();
}
