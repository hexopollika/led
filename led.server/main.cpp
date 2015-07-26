
#include <string>
#include <iostream>

#include <signal.h>
#include <poll.h>
#include <limits.h>

#include "fifo.h"
#include "file_lock.h"
#include "led.command.h"

bool
stop = false;

void sigint (int sig) {

    stop = true;
}
  
int
main (int argc, char** argv) {
    
    try {
        
        led::command::basic_t
        basic;
        fifo_t
        in ("/tmp/led.server.in");
        fifo_t
        out ("/tmp/led.server.out");
        file_lock_t
        file_lock ("/tmp/led.server.lock");
        std::string
        buffer;
        pollfd
        pool_fd [1];
                
        signal (SIGINT, sigint);

        if (file_lock.try_lock () != 0) {

            throw std::runtime_error ("led.server already exists");
        }    

        in.make (0666);
        out.make (0666);
        std::cout << "led.server is created" << std::endl;

        in.open (O_RDONLY | O_NONBLOCK);
        {
            fifo_t
            temp ("/tmp/led.server.out");
            
            temp.open (O_RDONLY | O_NONBLOCK);
            out.open (O_WRONLY | O_NONBLOCK);
        }
        std::cout << "led.server is opened" << std::endl;
        
        pool_fd [0].fd = in.descriptor ();
        pool_fd [0].events = POLLIN;
        pool_fd [0].revents = 0;

        buffer.resize (PIPE_BUF);

        while (stop == false) {

            int
            result = poll (pool_fd, 1, 1000);
            
            if (result > 0) {
                    
                result = in.read (buffer);

                if (result > 0) {
                        
                    result = out.write (basic.processing (buffer));

                    if (result < 0) {

                        std::cout << "led.server : write" << std::endl
                        << "errno : " << errno << std::endl;
                    }

                    buffer.resize (PIPE_BUF);                
                }
                else if (result < 0 and errno != EAGAIN) {

                    throw std::runtime_error ("read");
                }
            }
            else if (result < 0) {
                
                throw std::runtime_error ("poll"); 
            }
        }
        
        std::cout << std::endl << "led.server is stopped" << std::endl;

        return 0;
    }
    catch (const std::exception& exception) {

        std::cout << "led.server : " << exception.what () << std::endl
        << "errno : " << errno << std::endl;
    }
    catch (...) {

        std::cout << "led.server : unknown exception" << std::endl
        << "errno : " << errno << std::endl;
    }
    
    std::cout << "led.server is stopped" << std::endl;

    return 1;
}

