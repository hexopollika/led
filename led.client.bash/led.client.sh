#!/bin/bash

in=/tmp/led.server.in
out=/tmp/led.server.out
lock=/tmp/led.client.lock

if [[ ! -p $lock ]]; then
    touch $lock
fi

declare -A map

map["on"]="set-led-state on"
map["off"]="set-led-state off"
map["0hz"]="set-led-rate 0"
map["1hz"]="set-led-rate 1"
map["2hz"]="set-led-rate 2"
map["3hz"]="set-led-rate 3"
map["4hz"]="set-led-rate 4"
map["5hz"]="set-led-rate 5"
map["red"]="set-led-color red"
map["green"]="set-led-color green"
map["blue"]="set-led-color blue"

map["state"]="get-led-state"
map["rate"]="get-led-rate"
map["color"]="get-led-color"

echo "(help, exit)"

while [[ 1 ]]; do

    if [[ ! -p $in || ! -p $out ]]; then
        echo "no server"
        exit 1
    fi

    printf "command : "
    read sequence

    for seq_item in $sequence; do

        if [[ $seq_item = "exit" ]]; then
            exit 1
        fi

        if [[ $seq_item = "help" ]]; then
            for map_item in "${!map[@]}"; do
                echo $map_item;
            done | sort -n -k2
            continue
        fi

        map_item=${map[$seq_item]}

        if [[ $map_item = "" ]]; then
            echo "no such command '$seq_item'"
            break
        fi

        {
            if [[ $(flock -x 200)  ]]; then
                echo "flock error"
                exit 1
            fi           

            echo $map_item > $in
            read -t 3 message <> $out

        } 200>$lock

        array=($message)

        if [[ ${#array[@]} > 1 ]]; then
            echo "$seq_item : ${array[1]}"
        else
            if [[ ${array[0]} = "OK" ]]; then
                echo "$seq_item : success"
            else
                echo "$seq_item : failure"
            fi
        fi
    done
done
