#ifndef SANYA_HPP
#define SANYA_HPP

/**
 * @file sanya.hpp
 * @brief Utilties
 */

#include <stdio.h>

#define FATAL_ERROR(what) \
  fprintf(stderr, "file %s, func %s, line %d -- SANYA::FATAL(%s)\n", \
      __FILE__, __func__, __LINE__, what); \
  exit(1)


#endif /* SANYA_HPP */
