
#pragma once

#include <sys/file.h>
#include <stdio.h>

#include <string>
#include <stdexcept>
#include <functional>

class
file_lock_t {
public :
    
    file_lock_t (const std::string& _path, std::function <int (const char*)> _action = [] (const char*) -> int { return 0; }) :
    action (_action),
    path (_path),
    fd (-1) {}

    ~ file_lock_t () {
        
        if (fd >= 0) {
            
            close (fd);
        }
        
        action (path.c_str ());
    }
    
    int
    try_lock () {
        
        fd = open (path.c_str (), O_CREAT | O_RDWR);

        if (fd < 0) {

            throw std::runtime_error (path + " open");
        }

        if (flock (fd, LOCK_EX | LOCK_NB) == -1) {

            if (errno == EWOULDBLOCK) {

                return 1;
            }
            else {

                throw std::runtime_error (path + " flock");
            }
        }
        
        return 0;
    }
    
private :
    
    std::string
    path;
    int
    fd;
    std::function <int (const char*)>
    action;
};