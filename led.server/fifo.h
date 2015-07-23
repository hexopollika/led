
#pragma once

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#include <string>
#include <vector>
#include <stdexcept>
#include <set>
#include <functional>

class
fifo_t {
public :
    
    fifo_t (const std::string& _path) :
    path (_path),
    fd (-1) {}
    
    ~ fifo_t () {
        
        if (fd >= 0) {
            
            close (fd);
        }
        
        action (path.c_str ());
    }
    
    void
    make (__mode_t mode) {
        
        remove (path.c_str ());
    
        if (mkfifo (path.c_str (), mode) != 0) {

            throw std::runtime_error (path + " mkfifo");
        }
        
        action = ::remove;
    }
    
    void
    open (int flag) {
        
        fd = ::open (path.c_str (), flag);

        if (fd < 0) {
        
            throw std::runtime_error (path + " open");
        }
    }
    
    ssize_t
    read (std::string& buffer) {
        
        ssize_t
        length = ::read (fd, & buffer [0], buffer.size ());
        
        if (length > 0) {
            
            buffer.resize (length);
        }
        
        return length;
    }
    
    ssize_t
    write (const std::string& buffer) {
        
        return ::write (fd, & buffer [0], buffer.size ());
    }
    
    int
    descriptor () {
        
        return fd;
    }
    
private :
    
    std::string
    path;
    int
    fd;
    std::function <int (const char*)>
    action = [] (const char*) -> int { return 0; };
};