#include <stdint.h>

struct Subscriber {
  char sub_number_unused[15];
  char hex[5];
  char bytes[10];
  short bits;
  uint32_t msc_location;
  uint32_t vlr_location;
};

struct AccessInfo {};

struct SpecialFacility {};

struct CallForwarding {};