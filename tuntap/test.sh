#!/bin/bash


host.function-body () {
    echo $1 "()" {
    echo ip netns exec $1 "\$@"
    echo }
}

host.new () {
    local name="$1" ;shift
    ip netns add "$name"
    . <(host.function-body "$name")
}

host.del () {
    local name="$1" ;shift
    ip netns del $name;
    unset -f $name;
}


main () {
    ./tuntap-example 13 &
    local pid="$!"

    ip l set xdpa0 up
    ip a add 10.0.0.1/24 dev xdpa0

    for i in {1..12}; do
        host.new h$i
        ip l set xdpa$i netns h$i
        h$i ip l set xdpa$i up
        h$i ip a add 10.0.0.$((i + 1))/24 dev xdpa$i
    done


    for i in {1..12}; do
        if ! ping 10.0.0.$i -c 1; then
            echo error
            break
        fi
    done

    for i in {1..12}; do
        h$i ip l set xdpa$i netns 1
    done

    for i in {1..12}; do
        host.del h$i
    done

    kill -SIGINT "$pid"
    wait $(jobs -p)
}


main "$@"
