
#include <string>
#include <iostream>

#include <signal.h>

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
        int
        count = 0;
        
        signal (SIGINT, sigint);

        if (file_lock.try_lock () != 0) {

            throw std::runtime_error ("led.server already exists");
        }    

        in.make (0666);
        out.make (0666);
        std::cout << "led.server is created" << std::endl;

        in.open (O_RDONLY | O_NONBLOCK);        
        std::cout << "led.server is opened" << std::endl;            

        buffer.resize (20);

        while (stop == false) {

            int
            length = in.read (buffer);

            if (length > 0) {
                
                buffer = basic.processing (buffer);
                
                try {
                    
                    if (out.descriptor () < 0) {
                        
                        out.open (O_WRONLY | O_NONBLOCK);
                    }

                    length = out.write (buffer);

                    if (length < 0) {

                        throw std::runtime_error ("write");
                    }
                }
                catch (const std::exception& exception) {
                    
                    std::cout << "led.server : " << exception.what () << std::endl
                    << "errno : " << errno << std::endl;
                }

                buffer.resize (20);                
            }
            else if (length < 0 and errno != EAGAIN) {

                throw std::runtime_error ("led.server.read");
            }
            else {
                
                usleep (100000);
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

