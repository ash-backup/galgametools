// extensho.cpp, v1.0 2007/01/20
// coded by asmodean

// contact: 
//   web:   http://plaza.rakuten.co.jp/asmodean
//   email: asmodean [at] hush.com
//   irc:   asmodean on efnet (irc.efnet.net)

// This tool extracts data from Asmik Ace's Tenshou Gakuen Gekkoroku (PS2).
// I suggest uema's Susie plugin for the TIM2 (*.tm2) graphics.  Use bmp8to32
// to convert images with transparency.

#include <io.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <direct.h>
#include <cerrno>
#include <cstring>
#include <cstdio>

struct BINHDR {
  unsigned long length; // in 2048-byte blocks
  unsigned long filenames_offset;
  unsigned long entry_count;
};

struct BINENTRY {
  unsigned long filename_offset;
  unsigned long offset;
  unsigned long length; // in bytes
  unsigned long key_seed;
};

static const unsigned long TOC_KEY_SEED    = 0x494F28EA;

static const unsigned long REORDER_TABLE[] = {
  0x0097, 0x0130, 0x0002, 0x01DF, 0x0004, 0x0005, 0x00A8, 0x017A, 
  0x0047, 0x0009, 0x000D, 0x000B, 0x000C, 0x00DA, 0x00C3, 0x0007, 
  0x0010, 0x0011, 0x0012, 0x0013, 0x0089, 0x0015, 0x0016, 0x0017, 
  0x0018, 0x01C2, 0x001A, 0x0186, 0x01E9, 0x00E4, 0x001E, 0x0064, 
  0x0020, 0x009A, 0x014E, 0x0023, 0x01F1, 0x00E9, 0x00B0, 0x0057, 
  0x019B, 0x00F5, 0x00FB, 0x002B, 0x01C9, 0x002D, 0x002E, 0x002F, 
  0x0030, 0x002A, 0x0032, 0x0001, 0x0034, 0x0095, 0x007A, 0x0029, 
  0x0147, 0x0039, 0x008B, 0x003B, 0x003C, 0x003D, 0x006E, 0x0176, 
  0x013B, 0x0155, 0x0080, 0x0100, 0x0096, 0x0045, 0x0046, 0x01BF, 
  0x0048, 0x0124, 0x01B3, 0x00E7, 0x00D7, 0x0137, 0x0199, 0x004F, 
  0x00CE, 0x0060, 0x0052, 0x0075, 0x0054, 0x018D, 0x009F, 0x0027, 
  0x013A, 0x0059, 0x0108, 0x0171, 0x01B5, 0x01C5, 0x01F9, 0x0182, 
  0x01FA, 0x00D3, 0x0062, 0x0063, 0x00E8, 0x0065, 0x0066, 0x00C1, 
  0x0068, 0x0102, 0x0160, 0x01DB, 0x00A7, 0x00A6, 0x0110, 0x0163, 
  0x00DE, 0x0113, 0x0072, 0x0073, 0x0074, 0x01AB, 0x0076, 0x0077, 
  0x0078, 0x0079, 0x00EA, 0x00F4, 0x00D4, 0x00B5, 0x007E, 0x007F, 
  0x0042, 0x008C, 0x013F, 0x0083, 0x013E, 0x0150, 0x0159, 0x0170, 
  0x0172, 0x0061, 0x008A, 0x01C1, 0x0067, 0x00ED, 0x00F8, 0x008F, 
  0x01CA, 0x0091, 0x00F9, 0x01AD, 0x00E5, 0x018E, 0x0192, 0x00EC, 
  0x0098, 0x0033, 0x00E1, 0x0112, 0x01DE, 0x00F7, 0x0154, 0x0056, 
  0x0069, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x0003, 0x006D, 0x006C, 
  0x0151, 0x01B0, 0x00AA, 0x00AB, 0x01D8, 0x0140, 0x0148, 0x00AF, 
  0x0103, 0x00B1, 0x009C, 0x0178, 0x01EE, 0x007D, 0x00B6, 0x005B, 
  0x017D, 0x017F, 0x00BA, 0x01F8, 0x00BC, 0x00BD, 0x00B7, 0x00BF, 
  0x0123, 0x0086, 0x00C2, 0x01A3, 0x00C4, 0x00C5, 0x00B4, 0x00C7, 
  0x01F7, 0x00C9, 0x00CA, 0x010D, 0x00CC, 0x0153, 0x017C, 0x017E, 
  0x00D0, 0x00D1, 0x00D2, 0x01A7, 0x007C, 0x00D5, 0x0198, 0x01EC, 
  0x00D8, 0x00C6, 0x016D, 0x00DB, 0x00DC, 0x00DD, 0x00F3, 0x0055, 
  0x00E0, 0x0085, 0x00E2, 0x01AF, 0x0092, 0x0183, 0x00E6, 0x004B, 
  0x0161, 0x01A9, 0x0036, 0x001D, 0x01FF, 0x01E4, 0x01B2, 0x00CD, 
  0x00F0, 0x00F1, 0x00AC, 0x0070, 0x00AD, 0x01B8, 0x004A, 0x0104, 
  0x010A, 0x0187, 0x01C4, 0x00D9, 0x005E, 0x00FD, 0x00FE, 0x01BD, 
  0x0129, 0x0177, 0x00DF, 0x0094, 0x009D, 0x015F, 0x0106, 0x014B, 
  0x005A, 0x0109, 0x008D, 0x01E2, 0x01FE, 0x0000, 0x010E, 0x010F, 
  0x003E, 0x00BE, 0x01C3, 0x0195, 0x0114, 0x00B8, 0x0116, 0x004E, 
  0x0118, 0x01C0, 0x011A, 0x011B, 0x011C, 0x0131, 0x011E, 0x011F, 
  0x0120, 0x0121, 0x0122, 0x006A, 0x00AE, 0x0125, 0x000A, 0x0127, 
  0x0128, 0x0141, 0x012A, 0x0145, 0x012C, 0x003F, 0x01A8, 0x0158, 
  0x016B, 0x00A0, 0x0132, 0x0133, 0x00A5, 0x001C, 0x0136, 0x00FC, 
  0x0107, 0x0022, 0x0058, 0x0040, 0x013C, 0x0164, 0x011D, 0x014D, 
  0x018F, 0x001B, 0x0142, 0x0143, 0x0144, 0x01F5, 0x0146, 0x010C, 
  0x018B, 0x006F, 0x014A, 0x01C8, 0x01CC, 0x00E3, 0x0081, 0x0156, 
  0x0021, 0x00CF, 0x0152, 0x00EF, 0x01EB, 0x0041, 0x000E, 0x0051, 
  0x012F, 0x0111, 0x015A, 0x015B, 0x015C, 0x015D, 0x015E, 0x0105, 
  0x01BC, 0x01DD, 0x0162, 0x0149, 0x013D, 0x0165, 0x0166, 0x0044, 
  0x0168, 0x0071, 0x016A, 0x0014, 0x016C, 0x01A4, 0x01F6, 0x016F, 
  0x008E, 0x0082, 0x012E, 0x018A, 0x0139, 0x0175, 0x0024, 0x0101, 
  0x00F2, 0x0179, 0x0050, 0x017B, 0x000F, 0x004C, 0x001F, 0x0135, 
  0x0180, 0x0181, 0x005F, 0x0026, 0x016E, 0x0185, 0x0043, 0x01DA, 
  0x0188, 0x01AC, 0x00CB, 0x00FA, 0x01BE, 0x0087, 0x0035, 0x0084, 
  0x0190, 0x0191, 0x0167, 0x01D9, 0x0194, 0x00C0, 0x0196, 0x005C, 
  0x00D6, 0x01CF, 0x019A, 0x0028, 0x019C, 0x019D, 0x019E, 0x0093, 
  0x01A0, 0x01A1, 0x01A2, 0x0049, 0x00F6, 0x01A5, 0x01A6, 0x0031, 
  0x0088, 0x0025, 0x01AA, 0x0053, 0x0099, 0x019F, 0x0138, 0x0189, 
  0x00A9, 0x01B1, 0x00EE, 0x007B, 0x0157, 0x0197, 0x01B6, 0x01B7, 
  0x0037, 0x01B9, 0x01BA, 0x01BB, 0x0169, 0x00FF, 0x0019, 0x0008, 
  0x0119, 0x003A, 0x018C, 0x009B, 0x00B2, 0x005D, 0x01C6, 0x01C7, 
  0x0174, 0x00EB, 0x0090, 0x01CB, 0x0193, 0x01D0, 0x01CE, 0x0117, 
  0x01CD, 0x01D1, 0x01D2, 0x01D3, 0x01D4, 0x01D5, 0x01D6, 0x01D7, 
  0x00B3, 0x014C, 0x002C, 0x006B, 0x01DC, 0x0006, 0x01AE, 0x0134, 
  0x01E0, 0x01E1, 0x010B, 0x01E3, 0x0126, 0x01E5, 0x01E6, 0x01EA, 
  0x01E8, 0x00B9, 0x01E7, 0x009E, 0x0115, 0x01ED, 0x00BB, 0x01EF, 
  0x01F0, 0x012D, 0x01F2, 0x01F3, 0x01F4, 0x012B, 0x0184, 0x00C8, 
  0x014F, 0x004D, 0x01B4, 0x01FB, 0x01FC, 0x01FD, 0x0038, 0x0173
};

void unobfuscate(unsigned char* buff, unsigned long len, unsigned long seed) {
  unsigned long temp[512];  

  for (unsigned char* end = buff + len; buff < end; buff += 2048) {
    unsigned long* source = (unsigned long*) buff;
    unsigned long  key    = seed;

    for (int i = 0; i < 512; i++) {
      temp[i] = source[REORDER_TABLE[i]] ^ key++;
    }

    memcpy(buff, temp, sizeof(temp));
  }
}

int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "extensho v1.0 by asmodean\n\n");
    fprintf(stderr, "usage: %s <input.bin>\n", argv[0]);
    return -1;
  }

  char* in_filename = argv[1];
  
  int fd = open(in_filename, O_RDONLY | O_BINARY);

  if (fd == -1) {
    fprintf(stderr, "Could not open %s (%s)\n", in_filename, strerror(errno));
    return -1;
  }

  // Have to do a preliminary read to find the full toc length
  unsigned char toc_check[2048] = { 0 };
  BINHDR*  hdr                  = (BINHDR*) toc_check;
  
  read(fd, toc_check, sizeof(toc_check));
  unobfuscate(toc_check, sizeof(toc_check), TOC_KEY_SEED);

  // Now read the full toc
  unsigned long  toc_len  = hdr->length * 2048;
  unsigned char* toc_buff = new unsigned char[toc_len];

  lseek(fd, 0, SEEK_SET);
  read(fd, toc_buff, toc_len);
  unobfuscate(toc_buff, toc_len, TOC_KEY_SEED);

  BINENTRY* entries   = (BINENTRY*) (toc_buff + sizeof(*hdr));
  char*     filenames = (char*)     (entries  + hdr->entry_count);

  for (unsigned long i = 0; i < hdr->entry_count; i++) {
    char*          filename   = filenames + entries[i].filename_offset;
    unsigned long  len        = entries[i].length;
    unsigned long  padded_len = len + (len % 2048 ? 2048 - (len % 2048) : 0);
    unsigned char* buff       = new unsigned char[padded_len];

    // Pad data must be read because real data has been mixed with it by the
    // obfuscation algorithm.
    lseek(fd, entries[i].offset, SEEK_SET);
    read(fd, buff, padded_len);
    unobfuscate(buff, len, entries[i].key_seed);

    // There's got to be an easier way...
    {
      int filename_len = strlen(filename);

      for (int j = 0; j < filename_len; j++) {
        if (filename[j] == '/') {
          filename[j] = '\0';
          mkdir(filename);
          filename[j] = '/';
        }
      }
    }

    int out_fd = open(filename, 
                      O_CREAT | O_TRUNC | O_WRONLY | O_BINARY,
                      S_IREAD | S_IWRITE);

    if (out_fd == -1) {
      fprintf(stderr, "Could not open %s (%s)\n", filename, strerror(errno));
      return -1;
    }

    write(out_fd, buff, len);
    close(out_fd);

    delete [] buff;
  }

  delete [] toc_buff;

  close(fd);

  return 0;
}
