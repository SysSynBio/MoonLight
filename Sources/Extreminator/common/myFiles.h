#ifndef MYFILES_H
#define MYFILES_H

#include <iostream>
#include <string>

namespace myFiles {
  std::istream *openFile(const std::string &fileName);
}

#define MODIF
#define PRINT   1
#endif /* MYFILES_H */
