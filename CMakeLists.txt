cmake_minimum_required(VERSION 3.10)
project(noshiro LANGUAGES C)

add_library(noshiro STATIC
    base64.c
    bitset.c
    bytebuffer.c
    hash.c
    heap.c
    log.c
    md5.c
    queue.c
    rbtree.c
    sda.c
    sds.c
    stack.c
    utf8.c
    uuid.c

    base64.h
    bitset.h
    bytebuffer.h
    byteswap.h
    hash.h
    heap.h
    log.h
    md5.h
    queue.h
    rbtree.h
    sda.h
    sds.h
    stack.h
    stf.h
    utf8.h
    uuid.h
)
