// Copyright 2017 Space HAUC Command and Data Handling
// This file is part of octopOS which is released under AGPLv3.
// See file LICENSE.txt or go to <http://www.gnu.org/licenses/> for full
// license details.

/*!
 * @file
 */
#ifndef INCLUDE_UTILITY_H_
#define INCLUDE_UTILITY_H_

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>

#include <exception>
#include <system_error>                                                           // NOLINT
#include <cerrno>


#define MEMKEY 0x45861534
#define SEMKEY 0x45861534
#define MSGKEY 0x45861534

#define MSGLEN 126
#define SHMSIZE 4096
/*! The number of modules octopOS will start */
const unsigned NUMMODULES = 20;

/*! The subscriber bit that needs to be set in ids */
const unsigned SUB_BIT = 0x20000000;
/*! The bit in ids that specifies it is a temp id */
const unsigned TEMP_BIT = 0x40000000;

/*! packet type is requesting creation of publisher */
const unsigned CREATE_PUB = 1;
/*! packet type is requesting notification of subscribers */
const unsigned PUBLISH_CODE = 2;
/*! packet type is requesting creation of subscriber */
const unsigned CREATE_SUB = 3;

/*! typedef for generic pointers in shared memory segment */
typedef intptr_t generic_t;
/*! typedef to increase readability */
typedef long octopOS_id_t;                                                        // NOLINT
/*! typedef to make Lukas happy */
typedef uint sem_id_t;

/*! struct to define meta data present at top of allocated spaces in shm */
typedef struct {
    /*! counters to implement reader/writer lock on the shared data */
    unsigned rw_array[2];
} shm_object;

/*! struct to define message bus primative */
typedef struct {
    /*! the id of the message */
    long type;                                                                    // NOLINT
    /*! the text of the message */
    char text[MSGLEN];
} message_buffer;

/*! needed in creation of semaphores */
union semun {
    /*! value to set */
    int val;
    /*! a buffer to hold the semid */
    struct semid_ds *buf;
    /*! access the whole thing as an array */
    unsigned short *array;                                                        // NOLINT
};

/*!
 * function for connecting shared memmory to address space.
 * @return A pointer to the top of the shared memory space
 */
extern intptr_t* connect_shm();

/*!
 * Decraments a semaphore.
 * @param sem The semaphore to access
 * @param counter the counter to decrament
 * @return value of system call. fail if < 0.
 */
extern int p(int sem, int counter);

/*!
 * Incraments a semaphore.
 * @param sem The semaphore to access
 * @param counter the counter to incraments
 * @return value of system call. fail if < 0.
 */
extern int v(int sem, int counter);

/*!
 * Initializes all counters in a semaphore.
 * @param sem_group The semaphore to initializes.
 * @param number_in_group Number of counters in the group.
 * @param value The initial value to set.
 * @return value of system call. fail if < 0.
 */
extern int semsetall(int sem_group, int number_in_group, int value);

#endif  // INCLUDE_UTILITY_H_
