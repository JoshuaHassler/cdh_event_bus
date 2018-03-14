// Copyright 2017 Space HAUC Command and Data Handling
// This file is part of octopOS which is released under AGPLv3.
// See file LICENSE.txt or go to <http://www.gnu.org/licenses/> for full
// license details.

/*!
 * @file
 */
#include <string>
#include <unordered_map>
#include <tuple>
#include <memory>
#include <utility>
#include <vector>

#include "../include/octopos.h"

// TODO(JoshuaHassler) streamline this
void octopOS::sig_handler(int sig) {
    if (shmctl(shmid, IPC_RMID, 0) < 0) {
        if (sig < 0)
            throw std::system_error(
                errno,
                std::generic_category(),
                "Unable to remove shared memory segment");
    }
    for (auto i : semids) {
        if (semctl(i, 0, IPC_RMID) < 0) {
            if (sig < 0)
                throw std::system_error(
                    errno,
                    std::generic_category(),
                    "Unable to remove semaphores");
        }
    }
    for (unsigned i = 0; i < NUMMODULES; ++i) {
        tentacles.pop_back();
        if (msgctl(tentacle_ids[i], IPC_RMID, NULL) < 0) {
            if (sig < 0)
                throw std::system_error(
                    errno,
                    std::generic_category(),
                    "Unable to remove tentacle");
        }
    }
}

octopOS::octopOS() {
    int signals[] = {SIGHUP, SIGINT, SIGQUIT, SIGBUS, SIGTERM, SIGSEGV, SIGFPE};
    unsigned num_signals = sizeof(signals) / sizeof(int);
    sigset_t signal_mask;
    struct sigaction signal_action;

    sigemptyset(&signal_mask);
    for (unsigned i = 0; i < num_signals; ++i)
        sigaddset(&signal_mask, signals[i]);
    for (unsigned i = 0; i < num_signals; ++i) {
        signal_action.sa_handler = sig_handler;
        signal_action.sa_mask    = signal_mask;
        signal_action.sa_flags   = 0;
        if (sigaction(signals[i], &signal_action, NULL) < 0)
            throw std::system_error(
                errno,
                std::generic_category(),
                "Unable to set signal handler");
    }

    if ((shmid = shmget(MEMKEY, SHMSIZE, IPC_CREAT | 0600)) < 0) {
        throw std::system_error(
            errno,
            std::generic_category(),
            "Unable to create shared memory segment");
    }

    if ((shared_ptr = (intptr_t*)shmat(shmid, NULL, 0)) < (intptr_t*)0) {         // NOLINT
        std::cerr << "Unable to attach shared memory segment: "
            << errno << std::endl;
        sig_handler(-1);
    }

    tentacle::shared_data = shared_ptr;

    for (unsigned i = 0; i < NUMMODULES; ++i) {
        if ((tentacle_ids[i] = msgget(MSGKEY + i, IPC_CREAT | 0600)) < 0) {
            throw std::system_error(
                errno,
                std::generic_category(),
                "Unable to create message queues");
        } else {
            tentacles.push_back(std::make_shared<tentacle>(
                tentacle(MSGKEY + i)));
        }
    }

    shared_end_ptr = shared_ptr;
}

std::pair<unsigned, key_t> octopOS::create_new_topic
    (std::string name, unsigned size) {
    std::pair<unsigned, key_t> return_value(0, -1);

    topic_writer_in();
    auto tmp = topic_data.find(name);
    if (tmp == topic_data.end()) {
        key_t my_sem;
        if ((my_sem = semget(SEMKEY+semids.size(), 4, IPC_CREAT | 0600)) >= 0) {
            semsetall(my_sem, 4, 1);

            topic_data.emplace(name, std::tuple<unsigned, key_t,
                std::vector<std::pair<octopOS_id_t, unsigned>>>(
                    shared_end_ptr - shared_ptr,
                    SEMKEY+semids.size(),
                    std::vector<std::pair<octopOS_id_t, unsigned>>()));

                return_value.first = shared_end_ptr - shared_ptr;
                return_value.second = SEMKEY+semids.size();
                semids.push_back(my_sem);
                shm_object* init = (shm_object*)shared_end_ptr;                   // NOLINT
                init->rw_array[0] = init->rw_array[1] = 0;
                shared_end_ptr +=
                    (size + sizeof(shm_object)) / sizeof(intptr_t) + 1;
                // This size calculation has extra padding
        }
    } else {
        return_value.first = std::get<0>(tmp->second);
        return_value.second = std::get<1>(tmp->second);
    }
    topic_writer_out();

    return return_value;
}

// TENTACLE_INDEX_DYNAMIC is expected to be freed by this function
void* octopOS::listen_for_child(void* tentacle_index_dynamic) {
    int tentacle_index = *(int*)tentacle_index_dynamic;                   // NOLINT
    free((int*)tentacle_index_dynamic);                   // NOLINT
    std::shared_ptr<tentacle> t = tentacles[tentacle_index];

    std::pair<long, std::string> data;                                            // NOLINT
    for (;;) {
        data = t->read(-5);
        std::istringstream iss(data.second);
        std::vector<std::string> tokens {
            std::istream_iterator<std::string> { iss },
            std::istream_iterator<std::string>{}
        };

        switch (data.first) {
            case CREATE_PUB: {
                std::pair<unsigned, key_t> return_data;

                return_data = octopOS::getInstance().create_new_topic(tokens[2],
                    std::stoi(tokens[1]));
                std::stringstream ss;
                ss << return_data.first << " " << return_data.second << " "
                   << std::to_string(get_id(tentacle::role_t::PUBLISHER));
                t->write(std::stoi(tokens[0]), ss.str());
                break;
            }
            case PUBLISH_CODE: {
                octopOS::getInstance().propagate_to_subscribers(tokens[0]);
                break;
            }
            case CREATE_SUB: {
                octopOS_id_t id = get_id(tentacle::role_t::SUBSCRIBER);
                std::pair<unsigned, key_t> response =
                    octopOS::getInstance().subscribe_to_topic(
                        tokens[2],
                        tentacle_index,
                        id,
                        std::stoi(tokens[1]));

                t->write(std::stoi(tokens[0]),
                        std::to_string(response.first)+" "+
                        std::to_string(response.second)+" "+
                        std::to_string(id));
                break;
            }
            default:
                if (data.first != 0) {
                    throw std::runtime_error("Unexpected message on tenticale");
                }
        }
    }

    return NULL;
}

bool octopOS::propagate_to_subscribers(std::string name) {
    bool return_value = false;

    topic_reader_in();

    auto tmp = topic_data.find(name);
    if (tmp != topic_data.end()) {
        message_buffer my_buffer;
        strncpy(my_buffer.text, name.c_str(), name.size());
        my_buffer.text[name.size()] = '\0';
        for (auto i : std::get<2>(tmp->second)) {
            tentacles[i.second]->write(i.first, "ping");
        }
        return_value = true;
    }

    topic_reader_out();

    return return_value;
}

std::pair<unsigned, key_t> octopOS::subscribe_to_topic(std::string name,
    unsigned tentacle, octopOS_id_t subscriber_id, long size) {                   // NOLINT
    std::pair<unsigned, key_t> return_value(0, 0);

    topic_reader_in();

    auto current_topic_data = topic_data.find(name);

    topic_reader_out();
    if (current_topic_data != topic_data.end()) {
        topic_writer_in();
        std::get<2>(current_topic_data->second).push_back(
            std::pair<octopOS_id_t, unsigned>(subscriber_id, tentacle));
        topic_writer_out();
        return_value.first = std::get<0>(current_topic_data->second);
        return_value.second = std::get<1>(current_topic_data->second);
    } else if (size >= 0) {
        create_new_topic(name, size).first;
        return_value = subscribe_to_topic(name, tentacle, subscriber_id, size);
    }

    return return_value;
}

void octopOS::topic_reader_in() {
    topic_data_rdtry.lock();
    topic_data_rdlock.lock();
    topic_data_readers += 1;
    if (topic_data_readers == 1)
        topic_data_lock.lock();
    topic_data_rdlock.unlock();
    topic_data_rdtry.unlock();
}

void octopOS::topic_reader_out() {
    topic_data_rdlock.lock();
    topic_data_readers -= 1;
    if (topic_data_readers == 0)
        topic_data_lock.unlock();
    topic_data_rdlock.unlock();
}

void octopOS::topic_writer_in() {
    topic_data_wrlock.lock();
    topic_data_writers += 1;
    if (topic_data_writers == 1)
        topic_data_rdtry.lock();
    topic_data_wrlock.unlock();
    topic_data_wrlock.lock();
}

void octopOS::topic_writer_out() {
    topic_data_wrlock.unlock();
    topic_data_wrlock.lock();
    topic_data_writers -= 1;
    if (topic_data_writers == 0)
        topic_data_rdtry.unlock();
    topic_data_wrlock.unlock();
}

long octopOS::get_id(tentacle::role_t role) {                                     // NOLINT
    static long id_count = 100;                                                   // NOLINT
    static std::mutex id_lock;
    long return_value;                                                            // NOLINT

    id_lock.lock();
    if ((return_value = id_count++) > 0x1fffffff) {
        id_lock.unlock();
        throw std::overflow_error("Id space has been exhausted!");
        // If this happens all hope is lost. Kill everything and start over.
    }
    id_lock.unlock();

    switch (role) {
      default:
        break;
      case tentacle::role_t::SUBSCRIBER:
        return_value |= SUB_BIT;
        break;
    }

    return return_value;
}

octopOS::~octopOS() {
    try  {
        sig_handler(0);
    } catch (const std::system_error& ex) {
        std::cerr << ex.code().message() << std::endl;
        std::cerr << ex.what() << std::endl;
    }
}

int octopOS::shmid = 0;
int octopOS::tentacle_ids[NUMMODULES];
std::vector<std::shared_ptr<tentacle>> octopOS::tentacles;
std::vector<int> octopOS::semids;
intptr_t *octopOS::shared_ptr, *octopOS::shared_end_ptr;
std::unordered_map<std::string, std::tuple<unsigned, key_t,
    std::vector<std::pair<octopOS_id_t, unsigned>>>> octopOS::topic_data;
std::mutex octopOS::topic_data_rdlock, octopOS::topic_data_rdtry,
    octopOS::topic_data_wrlock, octopOS::topic_data_lock;
unsigned octopOS::topic_data_readers = 0, octopOS::topic_data_writers = 0;
