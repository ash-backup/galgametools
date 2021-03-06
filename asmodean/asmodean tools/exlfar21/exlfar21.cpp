// exlfar21.cpp, v1.0 2009/12/19
// coded by asmodean

// contact: 
//   web:   http://asmodean.reverse.net
//   email: asmodean [at] hush.com
//   irc:   asmodean on efnet (irc.efnet.net)

// This tool extracts ar21 (*.ar2) archives.

#include "as-util.h"
#include <list>

struct AR21HDR {
  unsigned char signature[4]; // "ar21"
  unsigned long entry_count;
};

struct AR21ENTRY {
  // unsigned char filename_length;
  // wchar_t       filename[filename_length];
  unsigned long offset;
  unsigned long length;
  unsigned long seed;
};

struct AR21ENTRYDATA {
  unsigned char filename_length;
  wchar_t       filename[512];
  AR21ENTRY     entry;
};

typedef std::list<AR21ENTRYDATA> ar21entries_t;

void unobfuscate(unsigned char* buff, unsigned long len, unsigned long seed) {
  static const unsigned char TABLE[512] = {
    0x1D, 0x4F, 0xA2, 0x33, 0x2D, 0x17, 0x56, 0xBB, 0xCE, 0xF2, 0xC6, 0xDD, 0x01, 0x62, 0x88, 0x51, 
    0x21, 0xF2, 0xD3, 0x2C, 0xC7, 0xF3, 0xC4, 0x36, 0x01, 0xC3, 0x43, 0x39, 0x00, 0x91, 0x35, 0x7E, 
    0x9B, 0x00, 0x21, 0xE9, 0x7F, 0x46, 0x61, 0xC8, 0x5B, 0x75, 0x15, 0x55, 0x45, 0xC9, 0x67, 0x98, 
    0xF8, 0x0B, 0x1C, 0x8F, 0x6C, 0x40, 0x4F, 0x4B, 0xA4, 0xF5, 0xB3, 0x5E, 0xEF, 0x08, 0x42, 0x7C, 
    0xDE, 0x9E, 0xCE, 0x13, 0xE2, 0xE2, 0xD4, 0x91, 0x7A, 0xC6, 0x9A, 0x40, 0x52, 0xDB, 0x7A, 0x2E, 
    0x5F, 0xBA, 0x64, 0xC9, 0xB4, 0x61, 0x38, 0xBC, 0x0D, 0xFF, 0x31, 0x56, 0x2D, 0xAA, 0x38, 0xBD, 
    0x44, 0xF8, 0xAD, 0x6A, 0x40, 0xD7, 0x58, 0x23, 0x36, 0xF9, 0x23, 0xFC, 0x07, 0x33, 0xA5, 0xDD, 
    0xA7, 0x10, 0xFA, 0x32, 0x73, 0x23, 0x99, 0x7A, 0xBB, 0x24, 0xEF, 0x92, 0xDE, 0x51, 0x6E, 0x7C, 
    0x91, 0x8B, 0x8E, 0x7A, 0x66, 0x2A, 0x06, 0x39, 0x60, 0x48, 0x9B, 0x6B, 0x03, 0x3D, 0xFF, 0x8F, 
    0xC6, 0x48, 0x8F, 0x70, 0xB1, 0xCA, 0x34, 0x72, 0xF7, 0xF8, 0xFA, 0xB6, 0x49, 0xE3, 0x63, 0x52, 
    0x00, 0x73, 0xFF, 0xE7, 0xD5, 0x90, 0x32, 0x1B, 0x87, 0xF1, 0xA0, 0xF8, 0xF1, 0xE2, 0x76, 0x46, 
    0xFE, 0x32, 0x15, 0x7E, 0x1A, 0x7D, 0x60, 0x75, 0x0A, 0xF6, 0x54, 0xE3, 0x02, 0xCC, 0x48, 0x19, 
    0xF6, 0x5F, 0xDD, 0xB9, 0x56, 0x05, 0xA2, 0xEE, 0x98, 0x50, 0x4C, 0x43, 0x23, 0x48, 0xBD, 0xD8, 
    0xF0, 0xA5, 0x11, 0x7F, 0x52, 0xDE, 0x07, 0xC5, 0x93, 0x82, 0x0D, 0x9A, 0xCC, 0x64, 0x7E, 0x7E, 
    0x63, 0x04, 0x1B, 0x54, 0xE1, 0xCE, 0x59, 0x79, 0x6E, 0xB3, 0x79, 0x31, 0xC9, 0xC6, 0x79, 0x01, 
    0xBD, 0xCE, 0xF4, 0x5F, 0x0E, 0xDE, 0x9D, 0x82, 0x2A, 0x8D, 0xAC, 0x74, 0x35, 0x58, 0x69, 0x5D, 
    0x61, 0x7A, 0x1C, 0x3B, 0x16, 0x3E, 0xE4, 0x8E, 0x83, 0xE6, 0xDB, 0x89, 0x08, 0x1D, 0x93, 0xF6, 
    0x95, 0x39, 0xE0, 0xB7, 0x13, 0x81, 0x17, 0xAB, 0xF8, 0x8D, 0x17, 0x05, 0x4C, 0x4E, 0x1F, 0xC7, 
    0x4D, 0xCC, 0xB1, 0xC1, 0x61, 0x40, 0xB7, 0x69, 0xEF, 0xE7, 0x33, 0x56, 0xF8, 0x0B, 0x71, 0x3F, 
    0xBB, 0x2D, 0xB9, 0xB5, 0xC5, 0x99, 0xE4, 0x78, 0x5C, 0x41, 0xB0, 0x9B, 0x1B, 0xFD, 0xD0, 0x37, 
    0x11, 0xBB, 0xE3, 0x7B, 0x76, 0x81, 0x66, 0xD7, 0x03, 0xC9, 0xCC, 0x59, 0x0E, 0x01, 0x03, 0xF7, 
    0x7C, 0x53, 0xD8, 0x9C, 0x3E, 0xB2, 0xE2, 0x04, 0xDA, 0x69, 0x1D, 0xFE, 0xDD, 0xB0, 0x02, 0x90, 
    0x57, 0x6B, 0x42, 0x35, 0x38, 0xDF, 0x33, 0xA2, 0x10, 0xA5, 0x94, 0x49, 0x2D, 0x3C, 0x33, 0x70, 
    0x33, 0x22, 0x2A, 0x12, 0x3B, 0x29, 0xF6, 0xD2, 0x40, 0x53, 0x09, 0x93, 0x8A, 0x9A, 0x8C, 0x2B, 
    0xAA, 0x5C, 0xB3, 0x3F, 0x4B, 0x3D, 0x85, 0xC5, 0x8D, 0x69, 0xF1, 0x37, 0x8A, 0x64, 0xD5, 0x30, 
    0x94, 0xFD, 0xDF, 0x9E, 0x5B, 0x34, 0x3C, 0x44, 0xD0, 0x74, 0x13, 0xA2, 0x43, 0xEC, 0xF8, 0x6D, 
    0xE0, 0xBE, 0xB7, 0x9E, 0x5E, 0x67, 0x70, 0x76, 0x85, 0x90, 0xFE, 0x66, 0x0E, 0xCC, 0x94, 0x7B, 
    0xE3, 0x07, 0x63, 0xBC, 0x6D, 0x13, 0x27, 0xFA, 0xC1, 0xAB, 0x65, 0x01, 0xA2, 0xB9, 0xD6, 0x40, 
    0x11, 0x95, 0xAB, 0x97, 0xCE, 0x3A, 0x57, 0xC0, 0x82, 0x84, 0xC9, 0xC5, 0x45, 0xE0, 0x82, 0xCB, 
    0x5B, 0xD9, 0xF3, 0x02, 0x4E, 0x99, 0xA0, 0x7F, 0xBF, 0xF1, 0x25, 0x85, 0xEF, 0x88, 0xA7, 0xA9, 
    0x38, 0x25, 0xAC, 0xD0, 0x04, 0x8A, 0x15, 0x9E, 0x82, 0x12, 0x79, 0xD3, 0x20, 0xB2, 0x11, 0x13, 
    0xF3, 0x17, 0x14, 0x34, 0x67, 0x8D, 0xF1, 0xD3, 0x22, 0x1F, 0xC8, 0xC5, 0x51, 0x25, 0xD9, 0x27, 
  };

  unsigned char index1 = (unsigned char) seed;

  for (unsigned long i = 0; i < len; i++) {
    buff[i] ^= TABLE[index1 + (i % 256)];
  }
}

int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "exlfar21 v1.0 by asmodean\n\n");
    fprintf(stderr, "usage: %s <input.ar2>\n", argv[0]);
    return -1;
  }

  string in_filename(argv[1]);

  int fd = as::open_or_die(in_filename, O_RDONLY | O_BINARY);

  AR21HDR hdr;
  read(fd, &hdr, sizeof(hdr));

  ar21entries_t entries;

  for (unsigned long i = 0; i < hdr.entry_count; i++) {
    AR21ENTRYDATA data = { 0 };
    
    read(fd, &data.filename_length, sizeof(data.filename_length));
    read(fd, data.filename, data.filename_length * sizeof(wchar_t));
    read(fd, &data.entry, sizeof(data.entry));

    for (unsigned long j = 0; j < data.filename_length; j++) {
      data.filename[j] ^= data.filename_length;
    }

    entries.push_back(data);
  }

  unsigned long data_offset = tell(fd);

  for (ar21entries_t::iterator i = entries.begin(); i != entries.end(); ++i) {
    unsigned long  len  = i->entry.length;
    unsigned char* buff = new unsigned char[len];
    lseek(fd, data_offset + i->entry.offset, SEEK_SET);
    read(fd, buff, len);
    unobfuscate(buff, len, i->entry.seed);

    string out_filename = as::convert_wchar(i->filename);

    as::make_path(out_filename);
    as::write_file(out_filename, buff, len);

    delete [] buff;
  }

  return 0;
}
