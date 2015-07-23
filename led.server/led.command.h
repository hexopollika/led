
#pragma once

#include "parameter.h"

#include <set>

namespace
led {
namespace
command {

using ac_t = std::map <std::string, std::string>;
using parameter_t = parameter::basic_t <ac_t>;
using bind_t = parameter_t::bind_t;

class
set_t :
public parameter_t::arg_t {
public :

    set_t (const std::string& _id, const std::set <std::string>& _id_set) :
    id (_id),
    id_set (_id_set) {}

    bool
    perform (const std::string& data, ac_t& ac) {
        
        auto
        iterator = id_set.find (data);
      
        if (iterator != id_set.end ()) {
            
            ac [id] = * iterator;

            return false;
        }

        return true;
    }

private :

    std::string
    id;
    std::set <std::string>
    id_set;
};

class
basic_t {
public :
    
    basic_t () {
               
        parameter_t::action_t
        set = [this] (ac_t& ac) mutable {

            ac_t::iterator
            iterator = ac.begin ();

            if (iterator != ac.end ()) {

                storage [iterator -> first] = iterator -> second;
                result = "OK\n";
            }
        };
        
        storage = { { "state", "off" }, { "color", "red" }, { "rate", "0" } };

        parameter << bind_t ("set-led-state", set) << set_t ("state", { "on", "off" });
        parameter << bind_t ("set-led-color", set) << set_t ("color", { "red", "green", "blue" });
        parameter << bind_t ("set-led-rate", set) << set_t ("rate", { "0", "1", "2", "3", "4", "5" });
        parameter << bind_t ("get-led-state", [this] (ac_t&) mutable { result = "OK " + storage ["state"] + "\n"; });
        parameter << bind_t ("get-led-color", [this] (ac_t&) mutable { result = "OK " + storage ["color"] + "\n"; });
        parameter << bind_t ("get-led-rate", [this] (ac_t&) mutable { result = "OK " + storage ["rate"] + "\n"; });
    }
    
    std::string
    processing (const std::string& data) {
        
        size_t
        next = parameter.parse (data, 0);

        if (next == std::string::npos or next != data.size ()) {

            return "FAILED\n";
        }
        
        return result;
    }
    
private :
    
    parameter_t
    parameter = { " ", "\n" }; 
    ac_t
    storage;
    std::string
    result;
};
}
}