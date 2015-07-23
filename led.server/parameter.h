
#pragma once

#include <string>
#include <map>
#include <list>
#include <functional>

namespace
parameter {
    
template <class ac_t>
class
basic_t {    
public :
    
    using action_t = std::function <void (ac_t&)>;
    
    class
    arg_t {
    public :

        virtual
        bool
        perform (const std::string& data, ac_t&) = 0;
    };
    
    class
    line_t {
    public :
        
        ~ line_t () {
            
            for (auto arg : arg_list) {
                
                delete arg;
            }
        }
        
        template <class final_t>
        line_t&
        operator << (const final_t& arg)  {

            arg_list.push_back (new final_t (arg));

            return * this;
        }

        std::list <arg_t*>
        arg_list;
        action_t
        action;        
    };
    
    class
    bind_t {
    public :

        bind_t (const std::string& _id, action_t _action) :
        id (_id),
        action (_action) {}

        std::string
        id;
        action_t
        action;
    };
    
    basic_t (const std::string& _delimiter, const std::string& _end) :
    delimiter (_delimiter),
    end (_end) {}

    size_t
    parse (const std::string& data, size_t prev) {
        
        size_t
        next = data.find (end, prev);

        if (next != std::string::npos and next > 0) {
            
            ac_t
            ac;
            std::string
            part = data.substr (prev, next - prev);
            size_t
            start = part.find (delimiter);
            
            if (start != std::string::npos and start != next) {
                
                std::string
                id = part.substr (0, start);
                auto
                iterator = line_map.find (id);
                size_t
                stop;

                if (iterator == line_map.end ()) {

                    return std::string::npos;
                }
                
                ++ start;

                for (auto& arg : iterator -> second.arg_list) {
                
                    stop = part.find (delimiter, start);
                    
                    if (stop != std::string::npos) {
                        
                        id = part.substr (start, stop - start);
                        start = stop + 1;
                    }
                    else {
                        
                        id = part.substr (start, next - start);
                        start = next;
                    }

                    if (arg -> perform (id, ac) == true) {

                        return std::string::npos;
                    }
                }

                if (stop != std::string::npos) {
                
                    return std::string::npos;
                }
                
                iterator -> second.action (ac);
            }
            else {
                
                auto
                iterator = line_map.find (part);

                if (iterator == line_map.end ()) {

                    return std::string::npos;
                }
                
                iterator -> second.action (ac);
            }
        }
        else {
            
            return std::string::npos;
        }
        
        return next + 1;
    }
    
    line_t&
    operator << (const bind_t& bind)  {

        auto&
        line = line_map [bind.id];

        line.action = bind.action;

        return line;
    }

private :    

    std::string
    delimiter,
    end;
    std::map <std::string, line_t>
    line_map;
};
}